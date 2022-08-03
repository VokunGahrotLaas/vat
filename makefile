
args =
gdb_args = -r

mode = release
ifeq ($(mode),debug)
MODEFLAGS = -g -DVAT_DEBUG
O = 0
else
MODEFLAGS = -flto -s
O = 2
endif

sanitize = false
ifeq ($(sanitize),true)
MODEFLAGS += -fsanitize=address
endif

analyzer = false
ifeq ($(analyzer),true)
MODEFLAGS += -fanalyzer
endif

error = false
ifeq ($(error),true)
MODEFLAGS += -Werror
endif

pedantic = false
ifeq ($(pedantic),true)
MODEFLAGS += -pedantic
endif

std = gnu2x

CC = gcc
CFLAGS = $(MODEFLAGS) -O$O -std=$(std) -Wall -Wextra

SRC_DIR = src
SRC = $(SRC_DIR)/main.c
EXEC = vat

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS)

run: $(EXEC)
	./$(EXEC) $(args)

gdb: $(EXEC)
	gdb -q $(gdb_args) --args $(EXEC) $(args)

clean:
	$(RM) $(EXEC)

.PHONY = run gdb clean
