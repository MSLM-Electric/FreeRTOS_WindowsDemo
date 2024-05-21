##
get_filename_component(GLOB_DEFINES_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)  #TODO: test it 
#set(ROOT_PATH "${GLOB_DEFINES_DIR}/" CACHE PATH "Path to Root") #FORCE)
#message(STATUS "Root path: ${ROOT_PATH}") 

add_definitions(-DDEBUG_ON_VS)
#add_definitions(-DCMSIS_OS_ENABLE)