#!/bin/bash

make clean
make test
cd ./build
./tester
cd .. 
