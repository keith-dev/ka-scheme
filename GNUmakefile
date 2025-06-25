LIB = lib/libka-scheme.a
LIB_SRCS = src/eval.cpp src/scheme.cpp src/tokenizer.cpp

PROG = bin/ka-scheme
SRCS = src/main.cpp

CPPFLAGS ?= -g -std=c++20 -Iinclude -Wall
LDADD ?= -lka-scheme

.PROXY: clean clean-all

all: preamble $(PROG)

preamble:
	- mkdir bin lib

clean-all: clean
	- rm $(LIB)
	- rm $(PROG)

clean:
	- rm $(LIB_SRCS:.cpp=.o)
	- rm $(SRCS:.cpp=.o)

$(PROG): $(LIB) $(SRCS:.cpp=.o)
	$(LINK.cpp) $(LDFLAGS) -o $@ $(SRCS:.cpp=.o) $(LIB)

$(LIB): $(LIB_SRCS:.cpp=.o)
	$(AR) -crsD $@ $(LIB_SRCS:.cpp=.o)
