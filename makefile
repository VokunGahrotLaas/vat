
args =
gdb_args = -ex=r -ex=q
gprof_args = -b -p

mode = release
ifeq ($(mode),debug)
MODEFLAGS = -g -DVAT_DEBUG
O = g
else ifeq ($(mode),gprof)
MODEFLAGS = -g -pg -DVAT_DEBUG
O = g
else ifeq ($(mode),release)
MODEFLAGS = -flto -s
O = 2
else
$(error "Unknown mode: $(mode)")
endif

sanitize = false
ifeq ($(sanitize),true)
MODEFLAGS += -fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

error = false
ifeq ($(error),true)
MODEFLAGS += -Werror
endif

pedantic = false
ifeq ($(pedantic),true)
MODEFLAGS += -pedantic
endif

native = false
ifeq ($(native),true)
MODEFLAGS += -march=native
endif

std = gnu2x

CC = gcc
CFLAGS = $(MODEFLAGS) -O$O -std=$(std) -Wall -Wextra

SRC_DIR = src
SRC = $(SRC_DIR)/main.c
OBJ = gmon.out
EXEC = vat

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

run: $(EXEC)
	./$(EXEC) $(args)

gdb: $(EXEC)
	gdb -q $(gdb_args) --args $(EXEC) $(args)

gprof:
	gprof $(gprof_args) $(EXEC) gmon.out

clean:
	$(RM) $(EXEC) $(OBJ)

.PHONY = run gdb gprof clean
