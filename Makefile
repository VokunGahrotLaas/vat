
args =
tests_args =
gdb_args = -ex=r -ex=q
build = build
std = gnu++20

PREFIX ?= /usr/local

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
BISONFLAGS = -v -Wcounterexamples
BISON_SRC = lib/src/parser/parser.yy
BISON_HEADER = ${build}/lib/include/vat/parser/parser.yy.hh
BISON_OUTPUT = ${build}/lib/src/parser/parser.yy.output
BISON_LOCATION = ${build}/lib/include/vat/parser/location.hh
BISON_SRC_LOCATION = ${build}/lib/src/parser/location.hh
BISON_OUT = ${build}/lib/src/parser/parser.yy.cc
BISON_OBJ = ${BISON_OUT:.cc=.o}

LIB_DIRS = lib/src/bind lib/src/eval lib/src/ast lib/src/parser lib/src
LIB_SRC = ${wildcard ${addsuffix /*.cc,${LIB_DIRS}}}
LIB_OBJ = ${addprefix ${build}/,${LIB_SRC:.cc=.o}}
LIB_HEADERS = ${FLEX_HEADER} ${BISON_HEADER} ${BISON_LOCATION} ${BISON_SRC_LOCATION}
LIB = ${build}/libvat.so

DIRS = src/args src
SRC = ${wildcard ${addsuffix /*.cc,${DIRS}}}
OBJ = ${addprefix ${build}/,${SRC:.cc=.o}}
EXEC = ${build}/vat

TESTS_DIRS = tests
TESTS_SRC = ${wildcard ${addsuffix /test_*.cc,${TESTS_DIRS}}}
TESTS_EXEC = ${addprefix ${build}/,${TESTS_SRC:.cc=}}
TESTS_CHECK = ${addprefix check_,${TESTS_EXEC}}

BUILD_DIRS = ${addprefix ${build}/,${DIRS} ${LIB_DIRS} ${TESTS_DIRS} lib/include/vat/parser}
CLEAN_DIRS = ${addprefix clean_,${BUILD_DIRS}}

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

all: $(EXEC)

phony_explicit:
.PHONY = all phony_explicit lib run gdb check install uninstall clean_files clean
.WAIT:

lib: ${LIB}

${BUILD_DIRS}: %:
	@mkdir -p $@

${CLEAN_DIRS}: clean_%: phony_explicit
	@if [ -d $* ]; then rmdir --ignore-fail-on-non-empty -p $*; fi

${FLEX_OUT}: ${FLEX_SRC} | ${BUILD_DIRS}
	${FLEX} ${FLEXFLAGS} --header-file=${FLEX_HEADER} -o ${FLEX_OUT} $<

${BISON_OUT}: ${BISON_SRC} | ${BUILD_DIRS}
	${BISON} ${BISONFLAGS} --defines=${BISON_HEADER} -o ${BISON_OUT} $<
	mv ${BISON_SRC_LOCATION} ${BISON_LOCATION}

${FLEX_OBJ}: ${FLEX_OUT} ${BISON_OUT}
	+${CXX} ${CXXFLAGS} -fPIC -o $@ -c ${FLEX_OUT}

${BISON_OBJ}: ${FLEX_OUT} ${BISON_OUT}
	+${CXX} ${CXXFLAGS} -I${build}/lib/include/vat/parser -fPIC -o $@ -c ${BISON_OUT}

${LIB_OBJ}: ${build}/lib/src/%.o: lib/src/%.cc ${FLEX_OUT} ${BISON_OUT}
	+${CXX} ${CXXFLAGS} -Wold-style-cast -fPIC -o $@ -c $<

${build}/src/%.o: src/%.cc ${FLEX_OUT} ${BISON_OUT}
	+${CXX} ${CXXFLAGS} -Wold-style-cast -o $@ -c $<

${LIB}: ${LIB_OBJ} ${FLEX_OBJ} ${BISON_OBJ}
	+${CXX} -shared -o $@ $^ ${LDFLAGS}

$(EXEC): ${OBJ} | ${LIB}
	+${CXX} -o $@ $^ -L${build} -lvat ${LDFLAGS}

${build}/tests/%: tests/%.cc | ${LIB}
	+${CXX} ${CXXFLAGS} -o $@ $< ${LIB} ${LDFLAGS}

run: $(EXEC)
	LD_LIBRARY_PATH=${build} ./$< ${args}

gdb: ${EXEC}
	LD_LIBRARY_PATH=${build} gdb -q $(gdb_args) --args $(EXEC) $(args)

check_${build}/tests/%: ${build}/tests/% ${LIB} phony_explicit
	./$< ${tests_args}

check: ${TESTS_EXEC} .WAIT ${TESTS_CHECK}

install: ${LIB} ${EXEC}
	install -d ${PREFIX}/include
	cp -r lib/include/vat ${PREFIX}/include
	cp -r ${build}/lib/include/vat ${PREFIX}/include
	install -Dm755 ${LIB} -t ${PREFIX}/lib
	install -Dm755 ${EXEC} -t ${PREFIX}/bin

uninstall:
	${RM} -r ${PREFIX}/include/vat
	${RM} ${PREFIX}/lib/libvat.so
	${RM} ${PREFIX}/bin/vat

clean_files:
	${RM} ${EXEC} ${LIB} ${TESTS_EXEC} ${OBJ} ${LIB_OBJ} ${FLEX_OBJ} ${BISON_OBJ} ${FLEX_OUT} ${BISON_OUT} ${BISON_OUTPUT} ${LIB_HEADERS}

clean: clean_files .WAIT ${CLEAN_DIRS}
