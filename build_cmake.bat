@echo off

:: --- build  ---
cd build/make
cmake -G "MinGW Makefiles" .

:: --- build vs19 ---
copy /y CMakeLists.txt ../vs19/CMakeLists.txt
cd ../vs19
cmake -G "Visual Studio 16 2019" ../vs19 

cd ../..
