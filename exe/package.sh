#!/bin/sh
set -e -x

cd ../build
make clean
rm -rf fstl.exe
make -j8

cp ../README.md README.txt
cp release/fstl.exe .
/c/Program\ Files/7-Zip/7z.exe a fstl.zip fstl.exe README.txt
cp fstl.zip ..
rm README.txt fstl.exe
