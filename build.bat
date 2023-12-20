@echo off

del bin/doc.exe
cd build/make
make
cd ../../bin
:: adding for malloc as test
doc malloc
cd ..
