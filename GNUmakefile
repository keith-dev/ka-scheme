LIB = lib/libka-scheme.a
LIB_SRCS = src/eval.cpp src/scheme.cpp src/tokenizer.cpp
LIB_SRCS_C = contrib/linenoise/linenoise.c

PROG = bin/ka-scheme
SRCS = src/main.cpp

TEST = bin/utest-ka-scheme
TEST_SRCS = test/scheme.cpp test/linenoise.cpp

CPPFLAGS ?= -fsanitize=address -g -std=c++20 -Iinclude -Wall
CFLAGS ?= -fsanitize=address -g -std=c11 -Iinclude

.PROXY: clean clean-all

all: preamble $(PROG) $(TEST)

preamble:
	- mkdir bin lib

clean-all: clean
	- rm $(LIB)
	- rm $(PROG)
	- rm $(TEST)
	- rmdir lib bin

clean:
	- rm $(LIB_SRCS:.cpp=.o) $(LIB_SRCS_C:.c=.o)
	- rm $(SRCS:.cpp=.o)
	- rm $(TEST_SRCS:.cpp=.o)

$(TEST): $(LIB) $(TEST_SRCS:.cpp=.o)
	$(LINK.cpp) -o $@ $(TEST_SRCS:.cpp=.o) $(LIB) $(LDADD) -lgtest -lgtest_main

$(PROG): $(LIB) $(SRCS:.cpp=.o)
	$(LINK.cpp) -o $@ $(SRCS:.cpp=.o) $(LIB) $(LDADD)

$(LIB): $(LIB_SRCS:.cpp=.o) $(LIB_SRCS_C:.c=.o)
	$(AR) -crsD $@ $(LIB_SRCS:.cpp=.o) $(LIB_SRCS_C:.c=.o)

# fix default
COMPILE.c = $(CC) $(CFLAGS) $(TARGET_ARCH) -c
