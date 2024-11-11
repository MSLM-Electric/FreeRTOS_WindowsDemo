```
To build project by CMake do: (initially)

x86 (for using on 32bit): (32bit build highly recommended! CMSIS OS API/osMutex/osMessage/osPool/osMail worx good!)

mkdir build32
cd build32
cmake -G "Visual Studio 15 2017" ..
cd ..
#cmake --build build32 --config Release  #for exporting the .exe

OR

x64 (for using on 64bit platform - not recommended!):

mkdir build
cd build
cmake ..
```