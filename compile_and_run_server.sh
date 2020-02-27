#!/bin/sh
gcc server.c -lpthread
mv a.out server.out
clear
./server.out