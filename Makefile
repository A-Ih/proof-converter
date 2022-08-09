CC = clang++-10

CFLAGS = -std=c++17 -stdlib=libstdc++ -g -Wall -I./
TEST_CFLAGS = $(CFLAGS) -fno-omit-frame-pointer -fsanitize=address,leak,undefined

all: a test_parser test_semantic

a:
	$(CC) $(CFLAGS) a.cc expression_calculus/expression.cc -o a.out

test_parser:
	$(CC) $(TEST_CFLAGS) test_parser.cc expression_calculus/expression.cc -o test_parser.out

test_semantic:
	$(CC) $(TEST_CFLAGS) test_semantic.cc expression_calculus/expression.cc -o test_semantic.out

