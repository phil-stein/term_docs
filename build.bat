@echo off

del bin\doc.exe
cd build\make
make
cd ..\..\bin
doc
cd ..
