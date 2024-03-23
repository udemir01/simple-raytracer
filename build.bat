@echo off

set CompilerCommands= -Zi -nologo -O2

IF NOT EXIST ..\..\build\ray mkdir ..\..\build\ray
pushd ..\..\build\ray

cl %CompilerCommands% ..\..\ray\code\ray.cpp
popd

pushd ..\data

..\..\build\ray\ray.exe input2.txt
start output.ppm
popd
