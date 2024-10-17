```
To build project by CMake do: (initially)

run the following commands:

mkdir build32 & pushd build32
cmake -G "Visual Studio 15 2017" \build32\
popd
mkdir build64 & pushd build64
cmake -G "Visual Studio 15 2017 Win64" \build32\
popd
cmake --build build32 --config Release
cmake --build build64 --config Release
```