PROG = ka-scheme
SRCS = eval.cpp scheme.cpp tokenizer.cpp main.cpp

CPPFLAGS ?= -g -std=c++20 -Wall

all: $(PROG)

$(PROG): $(SRCS:.cpp=.o)
	$(LINK.cpp) $(LDFLAGS) -o $@ $^ $(LDADD)
