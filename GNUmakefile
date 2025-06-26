LIB = lib/libka-scheme.a
LIB_SRCS = src/eval.cpp src/scheme.cpp src/tokenizer.cpp

PROG = bin/ka-scheme
SRCS = src/main.cpp

TEST = bin/utest-ka-scheme
TEST_SRCS = test/scheme.cpp

CPPFLAGS ?= -g -std=c++20 -Iinclude -Wall
LDADD ?= -lka-scheme

.PROXY: clean clean-all

all: preamble $(PROG) $(TEST)

preamble:
	- mkdir bin lib

clean-all: clean
	- rm $(LIB)
	- rm $(TEST)
	- rm $(PROG)
	- rmdir lib bin

clean:
	- rm $(LIB_SRCS:.cpp=.o)
	- rm $(SRCS:.cpp=.o)

$(TEST): $(LIB) $(TEST_SRCS:.cpp=.o)
	$(LINK.cpp) -o $@ $(TEST_SRCS:.cpp=.o) $(LIB) -lgtest -lgtest_main

$(PROG): $(LIB) $(SRCS:.cpp=.o)
	$(LINK.cpp) -o $@ $(SRCS:.cpp=.o) $(LIB)

$(LIB): $(LIB_SRCS:.cpp=.o)
	$(AR) -crsD $@ $(LIB_SRCS:.cpp=.o)
