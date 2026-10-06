#!/bin/bash
echo "Building tests..."
gcc -I src/kyber -o build/test_kem tests/test_kem.c src/kyber/*.c
./build/test_kem