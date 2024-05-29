##
get_filename_component(GLOB_DEFINES_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)  #TODO: test it 
set(MAIN_PROJECT_AND_CONFIGS_PATH "${GLOB_DEFINES_DIR}/" CACHE PATH "Path to main projects location") #FORCE)
message(STATUS "Main Projects location path: ${MAIN_PROJECT_AND_CONFIGS_PATH}") 

add_definitions(-DDEBUG_ON_VS)
add_definitions(-DCMSIS_OS_ENABLE)

#get_directory_property(DirDefs PARENT_DIRECTORY "${FREERTOS_EXPERIMENTS}/Configs.cmake" COMPILE_DEFINITIONS)
get_directory_property(ConfigDirDefs COMPILE_DEFINITIONS)
message(STATUS "ConfigDirDefs: " ${ConfigDirDefs})
foreach(d ${ConfigDirDefs})
    message(STATUS "Found define: " ${d})
    if(${d} STREQUAL CMSIS_OS_ENABLE)
        message(STATUS "SET: => " ${d})
        set(CMSIS_OS_ENABLE_FLAG TRUE)
    endif()
endforeach()