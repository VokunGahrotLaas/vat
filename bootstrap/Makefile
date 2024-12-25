mode = debug
builddir =
asan =
args =
O =

CC = gcc
CPPFLAGS = -MMD -MP -Isrc -D_GNU_SOURCE
CCFLAGS = -Wall -Wextra -Wpedantic -Werror -std=c99 -O$O
LDFLAGS = -O$O

SRC = ${wildcard src/*.c src/*/*.c}
OBJ = ${SRC:%.c=${builddir}/%.o}
DEP = ${OBJ:.o=.d}
VATC = ${builddir}/vatc

VATCFLAGS = --backend C
VATCPPFLAGS = -MMD -MP -D_GNU_SOURCE
VATCCFLAGS = -std=c99 -O${O}
VATLDFLAGS = -O$O

EXPS_SRC = ${wildcard exemples/*.vat}
EXPS_OBJ = ${EXPS_SRC:%.vat=${builddir}/%.c}
EXPS_DEP = ${EXPS_OBJ:.c=.d}
EXPS = ${EXPS_OBJ:.c=}

ifeq (${mode},debug)
O = 0
asan = true
builddir = build
CPPFLAGS += -DBOOTSTRAP_DEBUG
CCFLAGS += -ggdb3
LDFLAGS += -ggdb3
VATCPPFLAGS += -DBOOTSTRAP_DEBUG
VATCCFLAGS += -ggdb3
VATLDFLAGS += -ggdb3
else ifeq (${mode},release)
O = 2
asan = false
builddir = build-release
else
${error invalid value for `mode`: ${mode}, should be debug or release}
endif

ifeq (${asan},true)
CCFLAGS += -fsanitize=leak,address,undefined
LDFLAGS += -fsanitize=leak,address,undefined
VATCCFLAGS += -fsanitize=leak,address,undefined
VATLDFLAGS += -fsanitize=leak,address,undefined
else ifneq (${asan},false)
${error invalid value for `asan`: ${asan}, should be true or false}
endif

all: vatc

vatc: ${VATC}

exps: ${EXPS}

exps_c: ${EXPS_OBJ}

obj: ${OBJ}

bear: clean
	mkdir -p ${builddir}
	bear --output ${builddir}/compile_commands.json -- make obj

bear_exps: clean_exps
	mkdir -p ${builddir}/exemples
	make exps_c
	bear --output ${builddir}/exemples/compile_commands.json -- make exps

run: ${VATC}
	$< ${args}

clean_exps:
	${RM} ${EXPS} ${EXPS_OBJ} ${EXPS_DEP}

clean: clean_exps
	${RM} ${VATC} ${OBJ} ${DEP}

mrproper: clean
	${RM} -r ${builddir}

.PHONY = all vatc exps exps_c obj bear bear_exps run clean_exps clean mrproper

${VATC}: ${OBJ}
	${CC} -o $@ $^ ${LDFLAGS}

${builddir}/src/%.o: src/%.c
	mkdir -p ${dir $@}
	${CC} ${CPPFLAGS} ${CCFLAGS} -o $@ -c $<

${EXPS}: ${builddir}/exemples/%: ${builddir}/exemples/%.c
	${CC} ${VATCPPFLAGS} ${VATCCFLAGS} -o $@ $< ${VATLDFLAGS}

${builddir}/exemples/%.c: exemples/%.vat ${VATC}
	mkdir -p ${dir $@}
	${VATC} ${VATCFLAGS} -o $@ -t $<

-include ${DEP} ${EXPS_DEPS}
