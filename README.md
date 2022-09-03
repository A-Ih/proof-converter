# How to compile and run
```
make b
./b
<input in format of task B from pdf>
```
# How to make a debug build
```
make b_debug
./b_debug
<input in format of task B from pdf>
```
# How to launch unit tests
```
make ut
./test_tokenizer # check that the toknes are parsed correctly
./test_parser # check whether the parser creates correct AST
./test_semantic # check whether the expression is correctly converted to prefix notation
```
# How to launch all tests
```
./test.sh # launches unit tests as well as all sample inputs
```
All sample inputs can be found at `positive/` (the program should output a
converted proof that can be checked manually), and `negative/` (the program is
expected to print an error message at incorrect line of proof)
