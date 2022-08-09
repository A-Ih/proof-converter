CC = clang++-10

CFLAGS = -std=c++17 -stdlib=libstdc++ -g -Wall -I./
TEST_CFLAGS = $(CFLAGS) -fno-omit-frame-pointer -fsanitize=address,leak,undefined

all: b b_debug test_parser test_semantic

b:
	$(CC) $(CFLAGS) b.cc expression_calculus/expression.cc -o b.out

b_debug:
	$(CC) $(TEST_CFLAGS) b.cc expression_calculus/expression.cc -o b_debug.out

test_parser:
	$(CC) $(TEST_CFLAGS) test_parser.cc expression_calculus/expression.cc -o test_parser.out

test_semantic:
	$(CC) $(TEST_CFLAGS) test_semantic.cc expression_calculus/expression.cc -o test_semantic.out

