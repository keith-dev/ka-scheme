PROG = lisp-cpp
SRCS = eval.cpp lisp-cpp.cpp tokenizer.cpp

CPPFLAGS ?= -g -std=c++20 -Wall

all: $(PROG)

$(PROG): $(SRCS:.cpp=.o)
	$(LINK.cpp) $(LDFLAGS) -o $@ $^ $(LDADD)
