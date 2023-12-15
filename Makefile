
args =
gdb_args = -ex=r -ex=q
build = build

std = gnu++20

CXX ?= g++
CXXFLAGS = -O$O -Isrc -I${build}/src -std=${std} -Wall -Wextra -Wpedantic
LDFLAGS = -O$O

FLEX ?= flex
FLEXFLAGS =
FLEX_SRC = src/parser/lexer.ll
FLEX_HEADER = ${build}/src/parser/lexer.ll.hh
FLEX_OBJ = ${build}/src/parser/lexer.ll.cc

BISON ?= bison
BISONFLAGS = -v #-Wcounterexamples
BISON_SRC = src/parser/parser.yy
BISON_HEADER = ${build}/src/parser/parser.yy.hh
BISON_OBJ = ${build}/src/parser/parser.yy.cc
BISON_OUTPUT = ${build}/src/parser/parser.yy.output
BISON_LOCATION = ${build}/src/parser/location.hh

SRC = ${wildcard src/*.cc src/parser/*.cc src/ast/*.cc}
OBJ = ${addprefix ${build}/,${SRC:.cc=.o}} ${FLEX_OBJ:.cc=.o} ${BISON_OBJ:.cc=.o}
EXEC = ${build}/vat

mode = release
ifeq (${mode},debug)
CXXFLAGS += -ggdb3 -DVAT_DEBUG
LDFLAGS += -ggdb3
O = 0
else ifeq (${mode},release)
CXXFLAGS +=
LDFLAGS +=
O = 2
else
$(error "Unknown mode: ${mode}")
endif

lto = false
ifeq (${lto},true)
CXXFLAGS += -flto=auto -fuse-linker-plugin
LDFLAGS += -flto=auto
else ifneq (${lto},false)
${error "lto should be true or false"}
endif

sanitize = false
ifeq (${sanitize},true)
CXXFLAGS += -fsanitize=address,leak,undefined
LDFLAGS += -fsanitize=address,leak,undefined
else ifneq (${sanitize},false)
${error "sanitize should be true or false"}
endif

MKDIR = mkdir -p
RMDIR = rmdir --ignore-fail-on-non-empty

all: $(EXEC)

${build}/:
	${MKDIR} $@src/parser
	${MKDIR} $@src/ast

${build}/src/%.o: src/%.cc | ${FLEX_HEADER} ${BISON_HEADER} ${build}/
	+${CXX} ${CXXFLAGS} -Wold-style-cast -o $@ -c $<

${FLEX_OBJ:.cpp=}.o: ${FLEX_OBJ} | ${BISON_HEADER} ${build}/
	+${CXX} ${CXXFLAGS} -o $@ -c $<

${BISON_OBJ:.cpp=}.o: ${BISON_OBJ} | ${FLEX_HEADER} ${build}/
	+${CXX} ${CXXFLAGS} -I${build}/src/parser -o $@ -c $<

${FLEX_OBJ} ${FLEX_HEADER}: ${FLEX_SRC} | ${build}/
	${FLEX} ${FLEXFLAGS} --header-file=${FLEX_HEADER} -o ${FLEX_OBJ} $<

${BISON_OBJ} ${BISON_HEADER}: ${BISON_SRC} | ${build}/
	${BISON} ${BISONFLAGS} --defines=${BISON_HEADER} -o ${BISON_OBJ} $<

$(EXEC): $(OBJ) | ${build}/
	+${CXX} -o $@ $^ ${LDFLAGS}

run: $(EXEC)
	${prefix} ./$< ${args}

gdb: ${EXEC}
	${prefix} gdb -q $(gdb_args) --args $(EXEC) $(args)

clean:
	${RM} ${EXEC} ${OBJ} ${FLEX_HEADER} ${FLEX_OBJ} ${BISON_HEADER} ${BISON_OBJ} ${BISON_OUTPUT} ${BISON_LOCATION}
ifneq (${realpath ${build}},${realpath .})
ifneq (${wildcard ${build}},)
	${RMDIR} ${build}
endif
ifneq (${wildcard ${build}/src/ast},)
	${RMDIR} -p ${build}/src/ast
endif
ifneq (${wildcard ${build}/src/parser},)
	${RMDIR} -p ${build}/src/parser
endif
endif

.PHONY = all run gdb clean
