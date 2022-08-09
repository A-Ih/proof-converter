#!/bin/sh

make all
for i in test_parser.out test_semantic.out; do
    echo Running $i
    if ./$i; then
        echo ====SUCCESS====
    else
        echo ====FAILURE====
        exit 1
    fi
done
