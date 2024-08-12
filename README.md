

FreeRTOS Windows Port
```
It is for simulating on Visual Studio.
```
Buildable CMakeLists.txt located on:
[**BasicTemplate project** here](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/BasicTemplate)
and another example [**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/RTOSdebuggingTips-Tricks/FindingBugWithBitLoggerList)


How To:
-------

- If you want create yourself project you can refer to BasicTemplate project located 
[**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/BasicTemplate):
- First of all clone this repo (FreeRTOS_WindowsDemo)
- And then create your projects folder on inside ExamplesNExperiments folder.
- Copy the mentioned project BasicTemplate files to your created folder.
- Go to CMakeLists.txt file on your project folder.
- Open file and edit the next lines (*line* **2** and **3**) by specifying your projects name i.e. *MyProjectForExample*:
```
project(MyProjectForExample)
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
<sup>**_Note:_** _Unfortunatly I've found some buggy moment on VS 2019 Debug Launch. It doesn't shows the consoles processes/writings after Debug start so to avoid it just do a few restarts._</sup><br />
<sup>**_Note:_** _CMSIS OS functions simulations testing in progress._</sup>