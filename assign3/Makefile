# Exercise configuration

MI_OBJS = build/mi_main.o
MI_BIN = MirrorInitiator

CS_OBJS = build/cs_main.o
CS_BIN = ContentServer

MS_OBJS = build/ms_main.o
MS_BIN = MirrorServer

# Flags passed to preprocessor, compiler, and linker

CPPFLAGS += -iquoteinclude -pthread
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
	$(RM) -r $(MI_BIN) $(CS_BIN) $(MS_BIN) build/*

# Exercise builds

build/mi_main.o : src/mi/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(MI_BIN) : $(MI_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

build/cs_main.o : src/cs/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(CS_BIN) : $(CS_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)

build/ms_main.o : src/ms/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(MS_BIN) : $(MS_OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)
