builddir = build
mode = debug
asan = true
args =
O =

EXEC = ${builddir}/vatc

CC = gcc
CPPFLAGS = -MMD -MP -Isrc
CCFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -O$O
LDFLAGS = -O$O

SRC = ${wildcard src/*.c src/*/*.c}
OBJ = ${SRC:%.c=${builddir}/%.o}
DEP = ${OBJ:.o=.d}

ifeq (${mode},debug)
O = 0
CPPFLAGS += -DBOOTSTRAP_DEBUG
CCFLAGS += -ggdb3
LDFLAGS += -ggdb3
else ifeq (${mode},release)
O = 2
else
${error invalid value for `mode`: ${mode}, should be debug or release}
endif

ifeq (${asan},true)
CCFLAGS += -fsanitize=leak,address,undefined
LDFLAGS += -fsanitize=leak,address,undefined
else ifneq (${asan},false)
${error invalid value for `asan`: ${asan}, should be true or false}
endif

all: ${EXEC}

obj: ${OBJ}

bear: clean
	mkdir -p ${builddir}
	bear --output ${builddir}/compile_commands.json -- make obj

run: ${EXEC}
	$< ${args}

clean:
	${RM} ${EXEC} ${OBJ}

mrproper: clean
	${RM} -r ${builddir}

.PHONY = all obj bear run clean mrproper

${EXEC}: ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

${builddir}/%.o: %.c
	mkdir -p ${dir $@}
	${CC} ${CPPFLAGS} ${CCFLAGS} -o $@ -c $<

-include ${DEP}
