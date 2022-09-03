# What is this program
This program is a solution to task B of contest from our Math Logic course.
All the task statements are located at `contest-mathlog-2021-ru.pdf` (russian).

A brief description: given a proof in hilbert style, check if it's correct and
if so, convert it to natural deduction. For instance, given the input
```
|-A->B->A&B
A->B->A&B
```
The program may yield the output:
```
[3] A,B|-A [Ax]
[3] A,B|-B [Ax]
[2] A,B|-(A)&(B) [I&]
[1] A|-(B)->((A)&(B)) [I->]
[0] |-(A)->((B)->((A)&(B))) [I->]
```
Or, in case of incorrect proof:
```
|-A->A->A
A->A->B
A->A->A
```
The program must print:
```
Proof is incorrect at line 2
```
The program was tested with gcc and clang on linux.
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

