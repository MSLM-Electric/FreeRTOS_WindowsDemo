

FreeRTOS Windows Port
```
It is for simulating on Visual Studio.
```
Buildable CMakeLists.txt located on:
[**BasicTemplate project**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/BasicTemplate) here
and another example [**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/RTOSdebuggingTips-Tricks/FindingBugWithBitLoggerList)

Also you can use the updated sources of **FreeRTOS v10.0.1** as template by example on [**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/TemplateFreeRTOSv10test)

How To:
-------

- If you want create yourself project you can refer to BasicTemplate project located 
[**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/BasicTemplate):
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
mkdir build
cd build
cmake ..
```
- After successfully built project go to **_/build_** folder and open the  **"MyProjectForExample.sln"** file.
- After opening it on VS (*not ~~VS Code!~~*) set the main project ("_MyProjectForExample_") as startup project. To do it open **_Solution Explorer_** window on VS and 
chose **_Set as Startup Project_** by clicking right mouse on _MyProjectForExample_ in list. Now you can launch the VS debug.<br />
<sup>**_Note:_** _Unfortunatly I've found some buggy moment on VS 2019 Debug Launch. It doesn't shows the consoles processes/writings after Debug started so to avoid it just do a few restarts while debugging._</sup><br />
<sup>**_Note:_** _CMSIS OS functions simulations testing in progress._</sup><br />



Buildable and worky projects:
```
1. BasicTemplate
2. RTOSdebuggingTips-Tricks/FindingBugWithBitLoggerList
3. TasksArrayTests
4. TimerID_using
5. TemplateFreeRTOSv10test  (upgraded to FreeRTOS v10.0.1 and compiled. 
              CMSIS OS compalible too but simulations of it not tested!)
```


In progress:
```
6. ProtocolMasterSlaveDev
```
