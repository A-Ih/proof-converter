#!/bin/sh

make clean

make ut
echo Running unit tests
for i in test_parser test_semantic test_tokenizer; do
    echo Running $i
    if ./$i; then
        echo ====SUCCESS====
    else
        echo ====FAILURE====
        exit 1
    fi
done

touch temp

make b_debug
echo Running positive tests
for i in positive/*; do
    echo Running positive test $i
    if ./b_debug <$i | tee temp; then
        if grep -q 'Proof is incorrect at line' temp || grep -q "The proof does not prove the required expression" temp; then
            echo "====FAILURE====(error in proof)"
            exit 1
        else
            echo ====SUCCESS====
        fi
    else
        echo ====FAILURE====
        exit 1
    fi
done

echo Running negative tests
for i in negative/*.in; do
    correct_answer="negative/$(basename $i .in).out"
    echo Running negative test $i
    if ./b_debug <$i | tee temp; then
        if diff -w $correct_answer temp; then
            echo ====SUCCESS====
        else
            echo "====FAILURE====(errors don't match)"
            exit 1
        fi
    else
        echo "====FAILURE====(program exited with error code $?)"
        exit 1
    fi
done
rm -f temp
