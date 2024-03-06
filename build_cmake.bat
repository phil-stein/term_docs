@echo off

:: --- build  ---
cd build/make
cmake -G "MinGW Makefiles" . -DCMAKE_EXPORT_COMPILE_COMMANDS=1
:: copy compile_commands.json to root for clangd lsp
copy /y compile_commands.json ..\..\compile_commands.json

:: --- build vs19 ---
copy /y CMakeLists.txt ../vs19/CMakeLists.txt
cd ../vs19
cmake -G "Visual Studio 16 2019" ../vs19 

cd ../..
