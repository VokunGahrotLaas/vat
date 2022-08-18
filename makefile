
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

INCLUDE_DIR = include
SRC_DIR = src
TMP_DIR = tmp
EXEMPLE_DIR = exemple

CC = gcc
CFLAGS = $(MODEFLAGS) -O$O -I$(INCLUDE_DIR) -std=$(std) -Wall -Wextra

SRC = $(SRC_DIR)/main.c
EXEC = vat

all: $(TMP_DIR)/$(EXEC)

$(TMP_DIR):
	mkdir -p $(TMP_DIR)

$(TMP_DIR)/$(EXEC): $(SRC) | $(TMP_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

run: $(TMP_DIR)/$(EXEC)
	cp $(EXEMPLE_DIR)/* $(TMP_DIR)/
	cd $(TMP_DIR) && ./$(EXEC) $(args)

gdb: $(TMP_DIR)/$(EXEC)
	cd $(TMP_DIR) && gdb -q $(gdb_args) --args $(EXEC) $(args)

gprof:
	cd $(TMP_DIR) && gprof $(gprof_args) $(EXEC) gmon.out

clean:
	$(RM) -r $(TMP_DIR)

.PHONY = all run gdb gprof clean
