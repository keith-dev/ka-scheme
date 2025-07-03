#include "../contrib/linenoise/linenoise.h"

#include <gtest/gtest.h>

TEST(linenoise, TestFree) {
	char* line = strdup("");
	linenoiseFree(line);
}

#ifdef UNHIDE_TEST
TEST(linenoise, TestBlockingRead) {
	char* line = linenoise("Please enter some text> ");
	linenoiseFree(line);
}
#endif // UNHIDE_TEST
