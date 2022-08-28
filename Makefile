CC = clang++

CFLAGS = -O2 -std=c++17 -stdlib=libstdc++ -g -Wall -I./
TEST_CFLAGS = $(CFLAGS) -fno-omit-frame-pointer -fsanitize=address,leak,undefined

SOURCES = expression_calculus/expression.cc expression_calculus/rules.cc

all: b

ut: test_parser test_semantic

b:
	$(CC) $(CFLAGS) b.cc $(SOURCES) -o b

b_debug:
	$(CC) $(TEST_CFLAGS) b.cc $(SOURCES) -o b_debug

test_parser:
	$(CC) $(TEST_CFLAGS) test_parser.cc $(SOURCES) -o test_parser

test_semantic:
	$(CC) $(TEST_CFLAGS) test_semantic.cc $(SOURCES) -o test_semantic

archive:
	git archive --format zip -o tdir/ibrahim-ml.zip HEAD

.PHONY: clean

clean:
	rm -f b_debug b test_parser test_semantic
