# Exercise configuration

OBJS = build/main.o \
       build/date.o

BIN = werhauz

# Flags passed to preprocessor, compiler, and linker

CPPFLAGS += -iquoteinclude
CFLAGS += -std=c99 -pedantic-errors
ifeq ($(CC), clang)
	CFLAGS += -Weverything -g
else
	CFLAGS += -Wall -Wextra -g3
endif

# Build targets

.PHONY = all clean
all : $(BIN)
clean :
	$(RM) -r build/*

# Exercise builds

build/main.o : src/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

build/date.o : src/date.c include/date.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN) : $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@ $(LDFLAGS)
