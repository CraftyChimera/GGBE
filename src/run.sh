#!/bin/bash

cmake -S . -B build/
cd build/
make
cd ..