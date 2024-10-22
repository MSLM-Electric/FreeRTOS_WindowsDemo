We simulate the communication process between Master and Slave which have not RTOS. At first we immitate the interrupt sections of hardware peripheral (timer & interface). The main background processes would be in the another created task! Only one task named backgroundProcesses!

The main used libs should be also compatible for Arduino platforms!

Initially to launch the project build it once! To do it follow the next steps:

x64 (for using on 64bit platform):
```
mkdir build
cd build
cmake ..
```

OR

x86 (for using on 32bit):
```
mkdir build32
cd build32
cmake -G "Visual Studio 15 2017" ..
cd ..
#cmake --build build32 --config Release  #for exporting the .exe
```