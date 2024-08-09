FreeRTOS Windows Port
```
It is for simulating on Visual Studio.
```
Buildable CMakeLists.txt located on:
[**BasicTemplate project** here](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/BasicTemplate)

[**and here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/RTOSdebuggingTips-Tricks/FindingBugWithBitLoggerList)


How To:
-------

- If you want create yourself project you can refer to BasicTemplate project located 
[**here**](https://github.com/MSLM-Electric/FreeRTOS_WindowsDemo/tree/master/ExamplesNExperiments/BasicTemplate):
- First of all clone this repo (FreeRTOS_WindowsDemo)
- And then create your projects folder on inside ExamplesNExperiments folder.
- Copy the mentioned project BasicTemplate files to your created folder.
- Go to CMakeLists.txt file on your project folder.
- Open file and edit the next lines by specifying your projects name i.e. *MyProjectForExample*:
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
