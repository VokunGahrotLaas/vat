
args =
tests_args =
gdb_args = -ex=r -ex=q
build = build
std = gnu++20

CXX ?= g++
CXXFLAGS = -O$O -Ilib/include -I${build}/lib/include -std=${std} -Wall -Wextra -Wpedantic
LDFLAGS = -O$O

FLEX ?= flex
FLEXFLAGS =
FLEX_SRC = lib/src/parser/lexer.ll
FLEX_HEADER = ${build}/lib/include/vat/parser/lexer.ll.hh
FLEX_OUT = ${build}/lib/src/parser/lexer.ll.cc
FLEX_OBJ = ${FLEX_OUT:.cc=.o}

BISON ?= bison
BISONFLAGS = -v #-Wcounterexamples
BISON_SRC = lib/src/parser/parser.yy
BISON_HEADER = ${build}/lib/include/vat/parser/parser.yy.hh
BISON_OUTPUT = ${build}/lib/src/parser/parser.yy.output
BISON_LOCATION = ${build}/lib/include/vat/parser/location.hh
BISON_SRC_LOCATION = ${build}/lib/src/parser/location.hh
BISON_OUT = ${build}/lib/src/parser/parser.yy.cc
BISON_OBJ = ${BISON_OUT:.cc=.o}

LIB_SRC = ${wildcard lib/src/*.cc lib/src/*/*.cc}
LIB_OBJ = ${addprefix ${build}/,${LIB_SRC:.cc=.o}}
LIB_HEADERS = ${FLEX_HEADER} ${BISON_HEADER} ${BISON_LOCATION} ${BISON_SRC_LOCATION}
LIB = ${build}/libvat.so

SRC = ${wildcard src/*.cc src/*/*.cc}
OBJ = ${addprefix ${build}/,${SRC:.cc=.o}}
EXEC = ${build}/vat

TESTS_SRC = ${wildcard tests/test_*.cc}
TESTS_EXEC = ${addprefix ${build}/,${TESTS_SRC:.cc=}}
TESTS_CHECK = ${addprefix check_,${TESTS_EXEC}}

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

.PHONY = all lib run gdb check clean phony_explicit
phony_explicit:
.WAIT:

lib: ${LIB}

${build}/:
	${MKDIR} $@lib/include/vat/ast
	${MKDIR} $@lib/include/vat/parser
	${MKDIR} $@lib/src/ast
	${MKDIR} $@lib/src/parser
	${MKDIR} $@src
	${MKDIR} $@tests
	${MKDIR} $@test_assets

${BISON_LOCATION}: ${BISON_SRC_LOCATION}
	cp $< $@

${FLEX_OUT} ${FLEX_HEADER}: ${FLEX_SRC} | ${build}/
	${FLEX} ${FLEXFLAGS} --header-file=${FLEX_HEADER} -o ${FLEX_OUT} $<

${BISON_OUT} ${BISON_HEADER} ${BISON_SRC_LOCATION} ${BISON_OUTPUT}: ${BISON_SRC} | ${build}/
	${BISON} ${BISONFLAGS} --defines=${BISON_HEADER} -o ${BISON_OUT} $<

${FLEX_OBJ}: ${FLEX_OUT} | ${BISON_HEADER}
	+${CXX} ${CXXFLAGS} -fPIC -o $@ -c $<

${BISON_OBJ}: ${BISON_OUT} | ${FLEX_HEADER}
	+${CXX} ${CXXFLAGS} -I${build}/lib/include/vat/parser -fPIC -o $@ -c $<

${LIB_OBJ}: ${build}/lib/src/%.o: lib/src/%.cc | ${LIB_HEADERS}
	+${CXX} ${CXXFLAGS} -Wold-style-cast -fPIC -o $@ -c $<

${build}/src/%.o: src/%.cc | ${LIB_HEADERS}
	+${CXX} ${CXXFLAGS} -Wold-style-cast -o $@ -c $<

${LIB}: ${LIB_OBJ} ${FLEX_OBJ} ${BISON_OBJ}
	+${CXX} -shared -o $@ $^ ${LDFLAGS}

$(EXEC): ${OBJ} | ${LIB}
	+${CXX} -o $@ $^ -L${build} -lvat ${LDFLAGS}

${build}/tests/%: tests/%.cc | ${LIB}
	+${CXX} ${CXXFLAGS} -o $@ $< ${LIB} ${LDFLAGS}

run: $(EXEC)
	LD_LIBRARY_PATH=${build} ${prefix} ./$< ${args}

gdb: ${EXEC}
	${prefix} gdb -q $(gdb_args) --args $(EXEC) $(args)

check_${build}/tests/%: ${build}/tests/% | ${LIB} phony_explicit
	${prefix} ./$< ${tests_args}

check: ${TESTS_EXEC} .WAIT ${TESTS_CHECK}

clean:
	${RM} ${EXEC} ${LIB} ${TESTS_EXEC} ${OBJ} ${LIB_OBJ} ${FLEX_OBJ} ${BISON_OBJ} ${FLEX_OUT} ${BISON_OUT} ${BISON_OUTPUT} ${LIB_HEADERS}
ifneq (${realpath ${build}},${realpath .})
	${RM} -r ${build}/test_assets/
ifneq (${wildcard ${build}},)
	${RMDIR} ${build}
endif
ifneq (${wildcard ${build}/lib/include/vat/ast},)
	${RMDIR} -p ${build}/lib/include/vat/ast
endif
ifneq (${wildcard ${build}/lib/include/vat/parser},)
	${RMDIR} -p ${build}/lib/include/vat/parser
endif
ifneq (${wildcard ${build}/lib/src/ast},)
	${RMDIR} -p ${build}/lib/src/ast
endif
ifneq (${wildcard ${build}/lib/src/parser},)
	${RMDIR} -p ${build}/lib/src/parser
endif
ifneq (${wildcard ${build}/src},)
	${RMDIR} -p ${build}/src
endif
ifneq (${wildcard ${build}/tests},)
	${RMDIR} -p ${build}/tests
endif
endif
