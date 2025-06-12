


FreeRTOS Windows Port
```
It is for simulating on Visual Studio.
```
Buildable CMakeLists.txt located on:<br />
- here [**BasicTemplate project**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/BasicTemplate)<br />
- ~~and another example [**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/RTOSdebuggingTips-Tricks/FindingBugWithBitLoggerList)~~ (should be upgrade to FreeRTOS v10.0.1 and change the build instructions to 32-bit platform)

Also you can use the updated sources of **FreeRTOS v10.0.1** as template by example on [**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/TemplateFreeRTOSv10test)

How To:
-------

- If you want create yourself project you can refer to BasicTemplate project located
[**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/BasicTemplate).
- First of all clone this repo (FreeRTOS_WindowsDemo)
- And then create your projects folder on inside ExamplesNExperiments folder ('FreeRTOS_WindowsDemo/ExamplesNExperiments').
- Copy the mentioned BasicTemplate project files to your created folder.
- Go to CMakeLists.txt file on your project folder.
- Open file and edit the *line* **2** by specifying your projects name i.e. *MyProjectForExample*:
```
set(PROJ_NAME MyProjectForExample)
```
- Save your edits and close file.
- Then do next steps:
```
mkdir build32
cd build32
cmake -G "Visual Studio 15 2017" ..
cd ..
```
- If the entered command *cmake -G "Visual Studio 15 2017" ..* has fault then setup to your PC the Visual Studio Build Tools 2017 (*Visual C++ Build Tools* option in setup menu) and try again. Required space for *Build Tools* is ~4GB. Link for installer: *https://aka.ms/vs/15/release/vs_buildtools.exe*
- After successfully built project go to **_/build32_** folder and open the  **"MyProjectForExample.sln"** file.
- After opening it on VS (*not ~~VS Code!~~*) set the main project ("_MyProjectForExample_") as startup project. To do it open **_Solution Explorer_** window on VS and
chose **_Set as Startup Project_** by clicking right mouse on _MyProjectForExample_ in list. Now you can launch the VS debug.<br />
<sup>**_Note:_** _Unfortunatly I've found some buggy feature on VS 2019 Debug Launch. It doesn't shows the consoles processes/writings after Debug started so to avoid it just do a few restarts while debugging. In high performanced PC/CPU & big RAM this bug not occure!_</sup><br />
<sup>**_Note:_** _CMSIS OS functions simulations testing in progress. Many functions works good. See the `BasicTemplate/README.md` file._ </sup><br />



Buildable and worky projects:
```
1. BasicTemplate
2. RTOSdebuggingTips-Tricks/FindingBugWithBitLoggerList (not brought into compatibiling with 32-bit platform! Soon)
3. TasksArrayTests (not 32-bit buildable. Soon)
4. TimerID_using (not 32-bit buildable)
5. TemplateFreeRTOSv10test  (upgraded to FreeRTOS v10.0.1 and compiled.
              Instructions for build and cmsis_os files not brought into
              compatibiling with 32-bit platform! Soon)
```


<sup>**_Note:_** For using cmsis_os functions on ISR section to simulate use **_simulatePROCESSOR_HANDLER_MODE()_** function inside. Then before returning/exiting from ISR section execute **_simulatePROCESSOR_THREAD_MODE()_**. And don't forget use `init_simulatePROCESSOR_MODES()` before launching the `vTaskStartScheduler()` on main code.</sup><br />

**And again one last Note!**
Unfortunately there is some limit in simulation of queue processes.
For properly using osMessage()/osPool()/osMail() and s.o. features the max length of structure for putting to it's input queue should be 8 bytes or less.
If structure variable put to the queue is more 8bytes, then simulation crashes!
Good example:
```cpp
typedef struct {
	uint8_t BUFFER; //1..4bytes for 32bit platform
	InterfacePortHandle_t* Port; //4bytes for 32bit platform
}portsBuffer_t;

//..

portsBuffer_t* ifsPtr;
ifsPtr = osPoolAlloc(mpool);
ifsPtr->Port = PortHandle;
ifsPtr->BUFFER = BUFFER;
osMessagePut(MsgBox, (uint32_t)ifsPtr, 10);
```
Bad example:
```cpp
typedef struct { // Message object structure
	uint32_t/*float and*//*uint32_t gets error*/    voltage;
	uint32_t/*float*/    current;
	uint32_t      counter;
}T_MEAS;  //the length of structure is more 8 bytes!

//..

T_MEAS* mptr;
mptr = osPoolAlloc(mpool);
mptr->voltage = 227;
mptr->current = 12;
mptr->counter = 170823;
osMessagePut(MsgBox, (uint32_t)mptr, osWaitForever);
```