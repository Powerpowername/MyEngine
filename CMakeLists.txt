cmake_minimum_required(VERSION 3.30)

project(APP)

file(GLOB SOR_LIST 
                    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
                    E:/lib_c/glad/src/*.c)
message(STATUS ${SOR_LIST})
file(GLOB HEADER ${PROJECT_SOURCE_DIR}/include/*.hpp ${PROJECT_SOURCE_DIR}/include/*.h)
message(STATUS ${HEADER})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)


add_executable(app ${SOR_LIST})
# find_package用法
find_library(MY_LIB
             NAMES glfw3
             PATHS E:/lib_c/glfw-3.4/build_mingw/src)   
if(MY_LIB)
    message(STATUS "Found mylib: ${MY_LIB}")
    target_link_libraries(app PRIVATE ${MY_LIB})
else()
    message(FATAL_ERROR "mylib not found")
endif()
# find_package用法
find_library(ASSIMP
             NAMES assimp
             PATHS E:/lib_c/assimp-master/build_mingw64/lib)   
if(ASSIMP)
    message(STATUS "Found mylib: ${ASSIMP}")
    target_link_libraries(app PRIVATE ${ASSIMP})
else()
    message(FATAL_ERROR "mylib not found")
endif()

set(ASSIMP_DLL_DIR "E:/lib_c/assimp-master/build_mingw64/bin")
find_file(ASSIMP_DLL
  NAMES libassimp-5.dll assimp.dll
  PATHS "E:/lib_c/assimp-master/build_mingw64/bin"
  NO_DEFAULT_PATH
)
if(NOT ASSIMP_DLL)
  message(FATAL_ERROR "libassimp-5.dll not found!")
endif()

add_custom_command(TARGET app POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${ASSIMP_DLL}"
  $<TARGET_FILE_DIR:app>
)

# message(FATAL_ERROR ${PROJECT_SOURCE_DIR}/include)
# 指定头文件路径
target_include_directories(app PUBLIC 
                            E:/GitStore/MyEngine/MyEngine/include/
                            E:/lib_c/glfw-3.4/include
                            E:/lib_c/glad/include
                            E:/lib_c/assimp-master/include
                            E:/lib_c/assimp-master/build_mingw64/include)


