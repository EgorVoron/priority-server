#! /bin/sh
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ..
Release/priorityServerCrow