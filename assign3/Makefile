# Exercise configuration

GL_OBJS = build/inta.o \
          build/misc.o \
          build/str.o

MI_OBJS = build/mi_main.o
MI_BIN = MirrorInitiator

CS_OBJS = build/cs_main.o \
          build/cs_handler.o
CS_BIN = ContentServer

MS_OBJS = build/ms_main.o \
          build/ms_queue.o
MS_BIN = MirrorServer

# Flags passed to preprocessor, compiler, and linker

CPPFLAGS += -iquoteinclude -pthread -D_POSIX_C_SOURCE=200809L
CFLAGS += -std=c99 -pedantic-errors
ifeq ($(CC), clang)
	CFLAGS += -Weverything -g
else
	CFLAGS += -Wall -Wextra -g3
endif
LDFLAGS += -pthread

# Build targets

.PHONY = all clean
all : $(MI_BIN) $(CS_BIN) $(MS_BIN)
clean :
	$(RM) -r $(MI_BIN) $(CS_BIN) $(MS_BIN) build/* output/*

# Exercise builds

build/inta.o : src/inta.c include/inta.h include/misc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/misc.o : src/misc.c include/misc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/str.o : src/str.c include/str.h include/misc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/mi_main.o : src/mi/main.c include/misc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(MI_BIN) : $(MI_OBJS) $(GL_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

build/cs_main.o : src/cs/main.c include/cs/handler.h include/misc.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/cs_handler.o : src/cs/handler.c include/cs/handler.h include/misc.h include/str.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(CS_BIN) : $(CS_OBJS) $(GL_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

build/ms_main.o : src/ms/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/ms_queue.o : src/ms/queue.c include/ms/queue.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(MS_BIN) : $(MS_OBJS) $(GL_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)
