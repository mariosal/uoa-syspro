#include <arpa/inet.h>
#include <inttypes.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ms/queue.h"
#include "misc.h"
#include "str.h"

static size_t num_cs;
static int* sock_cs;
static char** path_cs;
static pthread_mutex_t* sock_lock;
static size_t count_active_mirrors;
static pthread_mutex_t counter_lock;
static size_t bytes;
static size_t bytes_avg;
static size_t bytes_var;
static pthread_mutex_t bytes_lock;
static size_t files;
static pthread_mutex_t files_lock;

struct MirrorArg {
  struct Queue* q;
  size_t id;
  char host[BUFSIZE];
  uint16_t port;
  char path[BUFSIZE];
  unsigned delay;
};

struct WorkerArg {
  struct Queue* q;
};

static bool Prefix(const char* pr, const char* s) {
  for (size_t i = 0; pr[i]; ++i) {
    if (pr[i] != s[i]) {
      return false;
    }
  }
  return true;
}

static void* MirrorManager(void* arg_void) {
  struct MirrorArg arg = *(struct MirrorArg*)arg_void;
  free(arg_void);
  pthread_mutex_lock(&sock_lock[arg.id]);
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  sock_cs[arg.id] = sock;
  snprintf(path_cs[arg.id], BUFSIZE, "%s_%" PRIu16, arg.host, arg.port);
  if (sock < 0) {
    Error("socket");
  }

  struct hostent* hp = gethostbyname(arg.host);
  if (hp == NULL) {
    Error("gethostbyname");
  }

  struct sockaddr_in addr;
  memcpy(&addr.sin_addr, hp->h_addr_list[0], (size_t)hp->h_length);
  addr.sin_port = htons(arg.port);
  addr.sin_family = AF_INET;
  if (connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
    Error("connect");
  }

  char cmd[BUFSIZE];
  snprintf(cmd, sizeof(cmd), "LIST 0 %d\n", arg.delay);
  WriteAll(sock, cmd, strlen(cmd));

  struct Str* buf = StrInit();
  StrRead(buf, sock);
  size_t num_dirs;
  sscanf(StrS(buf), "%zu", &num_dirs);

  for (size_t i = 0; i < num_dirs; ++i) {
    StrReadNl(buf, sock);
    if (Prefix(arg.path + 2, StrS(buf) + 2)) {
      snprintf(cmd, sizeof(cmd), "mkdir -p %s/%s\n", path_cs[arg.id], StrS(buf));
      system(cmd);
    }
  }

  StrRead(buf, sock);
  size_t num_files;
  sscanf(StrS(buf), "%zu", &num_files);

  for (size_t i = 0; i < num_files; ++i) {
    StrReadNl(buf, sock);
    if (Prefix(arg.path + 2, StrS(buf) + 2)) {
      QueuePush(arg.q, StrS(buf), sock);
    }
  }

  StrReset(&buf);

  pthread_mutex_unlock(&sock_lock[arg.id]);
  return NULL;
}

static void* Worker(void* arg_void) {
  struct WorkerArg arg = *(struct WorkerArg*)arg_void;
  free(arg_void);

  while (1) {
    char path[BUFSIZE];
    int sock;
    pthread_mutex_lock(&counter_lock);
    if (QueueLen(arg.q) == 0 && count_active_mirrors == 0) {
      pthread_mutex_unlock(&counter_lock);
      break;
    }
    QueuePop(arg.q, path, &sock);
    pthread_mutex_unlock(&counter_lock);
    size_t id = 0;
    for (size_t i = 0; i < num_cs; ++i) {
      if (sock_cs[i] == sock) {
        id = i;
      }
    }
    pthread_mutex_lock(&sock_lock[id]);

    char cmd[BUFSIZE];
    snprintf(cmd, sizeof(cmd), "FETCH %s\n", path);
    WriteAll(sock, cmd, strlen(cmd));

    struct Str* buf = StrInit();
    StrRead(buf, sock);
    size_t num_bytes;
    sscanf(StrS(buf), "%zu", &num_bytes);

    char real_path[BUFSIZE];
    snprintf(real_path, sizeof(real_path), "%s/%s", path_cs[id], path);
    FILE* file = fopen(real_path, "w");
    int total = 0;
    for (size_t i = 0; i < num_bytes; i += StrLen(buf) + 1) {
      StrReadAllNl(buf, sock);
      total += StrLen(buf) + 1;
      fprintf(file, "%s\n", StrS(buf));
    }
    fclose(file);
    StrReset(&buf);
    pthread_mutex_unlock(&sock_lock[id]);
  }
  return NULL;
}

int main(int argc, char** argv) {
  setbuf(stdout, NULL);
  uint16_t port = 6000;
  char root[BUFSIZE];
  size_t num_threads = 2;
  strcpy(root, "output");
  for (int i = 1; i < argc; ++i) {
    if (Equals(argv[i], "p")) {
      ++i;
      sscanf(argv[i], "%" SCNu16, &port);
    } else if (Equals(argv[i], "m")) {
      ++i;
      strcpy(root, argv[i]);
    } else if (Equals(argv[i], "w")) {
      ++i;
      sscanf(argv[i], "%zu", &num_threads);
    }
  }
  chdir(root);

  int lsock = socket(AF_INET, SOCK_STREAM, 0);
  if (lsock < 0) {
    Error("socket");
  }

  struct sockaddr_in addr;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;
  setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
  if (bind(lsock, (struct sockaddr*)&addr, sizeof(addr))) {
    Error("bind");
  }
  if (listen(lsock, 5)) {
    Error("listen");
  }

  while (true) {
    int csock = accept(lsock, NULL,  NULL);
    if (csock < 0) {
      Error("accept");
    }

    struct Queue* q = QueueInit();

    struct Str* buf = StrInit();
    StrRead(buf, csock);
    sscanf(StrS(buf), "%zu", &num_cs);

    bytes = 0;
    bytes_avg = 0;
    bytes_var = 0;
    files = 0;

    sock_cs = malloc(sizeof(*sock_cs) * num_cs);
    path_cs = malloc(sizeof(*path_cs) * num_cs);
    sock_lock = malloc(sizeof(*sock_lock) * num_cs);
    for (size_t i = 0; i < num_cs; ++i) {
      path_cs[i] = malloc(sizeof(*path_cs[i]) * BUFSIZE);
      pthread_mutex_init(&sock_lock[i], NULL);
    }
    count_active_mirrors = num_cs;
    pthread_mutex_init(&counter_lock, NULL);

    pthread_t* worker_t = malloc(num_threads * sizeof(*worker_t));
    for (size_t i = 0; i < num_threads; ++i) {
      struct WorkerArg* arg = malloc(sizeof(*arg));
      arg->q = q;
      if (pthread_create(&worker_t[i], NULL, Worker, arg)) {
        Error("create");
      }
    }

    pthread_t* mirror_t = malloc(sizeof(*mirror_t) * num_cs);
    for (size_t i = 0; i < num_cs; ++i) {
      struct MirrorArg* arg = malloc(sizeof(*arg));
      arg->q = q;
      arg->id = i;

      StrRead(buf, csock);
      strcpy(arg->host, StrS(buf));

      StrRead(buf, csock);
      sscanf(StrS(buf), "%" SCNu16, &arg->port);

      StrReadNl(buf, csock);
      strcpy(arg->path, StrS(buf));

      StrRead(buf, csock);
      sscanf(StrS(buf), "%u", &arg->delay);

      if (pthread_create(&mirror_t[i], NULL, MirrorManager, arg)) {
        Error("create");
      }
    }
    for (size_t i = 0; i < num_cs; ++i) {
      if (pthread_join(mirror_t[i], NULL)) {
        Error("join");
      }
      pthread_mutex_lock(&counter_lock);
      --count_active_mirrors;
      pthread_mutex_unlock(&counter_lock);
    }
    for (size_t i = 0; i < num_threads; ++i) {
      if (pthread_join(worker_t[i], NULL)) {
        Error("join");
      }
    }

    char st[BUFSIZE];
    snprintf(st, sizeof(st), "4\n");
    WriteAll(csock, st, strlen(st));

    snprintf(st, sizeof(st), "Count bytes: %d\n", bytes);
    WriteAll(csock, st, strlen(st));

    snprintf(st, sizeof(st), "Count files: %d\n", files);
    WriteAll(csock, st, strlen(st));

    snprintf(st, sizeof(st), "Average bytes: 5\n", bytes_avg);
    WriteAll(csock, st, strlen(st));

    snprintf(st, sizeof(st), "Bytes variance: %d\n", bytes_var);
    WriteAll(csock, st, strlen(st));

    for (size_t i = 0; i < num_cs; ++i) {
      pthread_mutex_destroy(&sock_lock[i]);
      close(sock_cs[i]);
      free(path_cs[i]);
    }
    free(sock_cs);
    free(path_cs);
    free(sock_lock);
    free(mirror_t);
    free(worker_t);
    StrReset(&buf);
    QueueReset(&q);
    close(csock);
  }
}
