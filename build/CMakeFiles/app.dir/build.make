# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:\GitStore\MyEngine\MyEngine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\GitStore\MyEngine\MyEngine\build

# Include any dependencies generated for this target.
include CMakeFiles/app.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/app.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/app.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/app.dir/flags.make

CMakeFiles/app.dir/codegen:
.PHONY : CMakeFiles/app.dir/codegen

CMakeFiles/app.dir/source/Auxiliary.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/Auxiliary.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/Auxiliary.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/Auxiliary.cpp
CMakeFiles/app.dir/source/Auxiliary.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/app.dir/source/Auxiliary.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/Auxiliary.cpp.obj -MF CMakeFiles\app.dir\source\Auxiliary.cpp.obj.d -o CMakeFiles\app.dir\source\Auxiliary.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\Auxiliary.cpp

CMakeFiles/app.dir/source/Auxiliary.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/Auxiliary.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\Auxiliary.cpp > CMakeFiles\app.dir\source\Auxiliary.cpp.i

CMakeFiles/app.dir/source/Auxiliary.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/Auxiliary.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\Auxiliary.cpp -o CMakeFiles\app.dir\source\Auxiliary.cpp.s

CMakeFiles/app.dir/source/base.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/base.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/base.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/base.cpp
CMakeFiles/app.dir/source/base.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/app.dir/source/base.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/base.cpp.obj -MF CMakeFiles\app.dir\source\base.cpp.obj.d -o CMakeFiles\app.dir\source\base.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\base.cpp

CMakeFiles/app.dir/source/base.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/base.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\base.cpp > CMakeFiles\app.dir\source\base.cpp.i

CMakeFiles/app.dir/source/base.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/base.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\base.cpp -o CMakeFiles\app.dir\source\base.cpp.s

CMakeFiles/app.dir/source/imgui.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/imgui.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/imgui.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/imgui.cpp
CMakeFiles/app.dir/source/imgui.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/app.dir/source/imgui.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/imgui.cpp.obj -MF CMakeFiles\app.dir\source\imgui.cpp.obj.d -o CMakeFiles\app.dir\source\imgui.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\imgui.cpp

CMakeFiles/app.dir/source/imgui.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/imgui.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\imgui.cpp > CMakeFiles\app.dir\source\imgui.cpp.i

CMakeFiles/app.dir/source/imgui.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/imgui.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\imgui.cpp -o CMakeFiles\app.dir\source\imgui.cpp.s

CMakeFiles/app.dir/source/imgui_draw.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/imgui_draw.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/imgui_draw.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/imgui_draw.cpp
CMakeFiles/app.dir/source/imgui_draw.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/app.dir/source/imgui_draw.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/imgui_draw.cpp.obj -MF CMakeFiles\app.dir\source\imgui_draw.cpp.obj.d -o CMakeFiles\app.dir\source\imgui_draw.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\imgui_draw.cpp

CMakeFiles/app.dir/source/imgui_draw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/imgui_draw.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\imgui_draw.cpp > CMakeFiles\app.dir\source\imgui_draw.cpp.i

CMakeFiles/app.dir/source/imgui_draw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/imgui_draw.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\imgui_draw.cpp -o CMakeFiles\app.dir\source\imgui_draw.cpp.s

CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/imgui_impl_glfw.cpp
CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.obj -MF CMakeFiles\app.dir\source\imgui_impl_glfw.cpp.obj.d -o CMakeFiles\app.dir\source\imgui_impl_glfw.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\imgui_impl_glfw.cpp

CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\imgui_impl_glfw.cpp > CMakeFiles\app.dir\source\imgui_impl_glfw.cpp.i

CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\imgui_impl_glfw.cpp -o CMakeFiles\app.dir\source\imgui_impl_glfw.cpp.s

CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/imgui_impl_opengl3.cpp
CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.obj -MF CMakeFiles\app.dir\source\imgui_impl_opengl3.cpp.obj.d -o CMakeFiles\app.dir\source\imgui_impl_opengl3.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\imgui_impl_opengl3.cpp

CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\imgui_impl_opengl3.cpp > CMakeFiles\app.dir\source\imgui_impl_opengl3.cpp.i

CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\imgui_impl_opengl3.cpp -o CMakeFiles\app.dir\source\imgui_impl_opengl3.cpp.s

CMakeFiles/app.dir/source/imgui_tables.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/imgui_tables.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/imgui_tables.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/imgui_tables.cpp
CMakeFiles/app.dir/source/imgui_tables.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/app.dir/source/imgui_tables.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/imgui_tables.cpp.obj -MF CMakeFiles\app.dir\source\imgui_tables.cpp.obj.d -o CMakeFiles\app.dir\source\imgui_tables.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\imgui_tables.cpp

CMakeFiles/app.dir/source/imgui_tables.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/imgui_tables.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\imgui_tables.cpp > CMakeFiles\app.dir\source\imgui_tables.cpp.i

CMakeFiles/app.dir/source/imgui_tables.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/imgui_tables.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\imgui_tables.cpp -o CMakeFiles\app.dir\source\imgui_tables.cpp.s

CMakeFiles/app.dir/source/imgui_widgets.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/imgui_widgets.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/imgui_widgets.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/imgui_widgets.cpp
CMakeFiles/app.dir/source/imgui_widgets.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/app.dir/source/imgui_widgets.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/imgui_widgets.cpp.obj -MF CMakeFiles\app.dir\source\imgui_widgets.cpp.obj.d -o CMakeFiles\app.dir\source\imgui_widgets.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\imgui_widgets.cpp

CMakeFiles/app.dir/source/imgui_widgets.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/imgui_widgets.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\imgui_widgets.cpp > CMakeFiles\app.dir\source\imgui_widgets.cpp.i

CMakeFiles/app.dir/source/imgui_widgets.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/imgui_widgets.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\imgui_widgets.cpp -o CMakeFiles\app.dir\source\imgui_widgets.cpp.s

CMakeFiles/app.dir/source/main.cpp.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/source/main.cpp.obj: CMakeFiles/app.dir/includes_CXX.rsp
CMakeFiles/app.dir/source/main.cpp.obj: E:/GitStore/MyEngine/MyEngine/source/main.cpp
CMakeFiles/app.dir/source/main.cpp.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/app.dir/source/main.cpp.obj"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/app.dir/source/main.cpp.obj -MF CMakeFiles\app.dir\source\main.cpp.obj.d -o CMakeFiles\app.dir\source\main.cpp.obj -c E:\GitStore\MyEngine\MyEngine\source\main.cpp

CMakeFiles/app.dir/source/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/app.dir/source/main.cpp.i"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\GitStore\MyEngine\MyEngine\source\main.cpp > CMakeFiles\app.dir\source\main.cpp.i

CMakeFiles/app.dir/source/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/app.dir/source/main.cpp.s"
	E:\Compiler\mingw64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\GitStore\MyEngine\MyEngine\source\main.cpp -o CMakeFiles\app.dir\source\main.cpp.s

CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.obj: CMakeFiles/app.dir/flags.make
CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.obj: CMakeFiles/app.dir/includes_C.rsp
CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.obj: E:/lib_c/glad/src/glad.c
CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.obj: CMakeFiles/app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.obj"
	E:\Compiler\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.obj -MF CMakeFiles\app.dir\E_\lib_c\glad\src\glad.c.obj.d -o CMakeFiles\app.dir\E_\lib_c\glad\src\glad.c.obj -c E:\lib_c\glad\src\glad.c

CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.i"
	E:\Compiler\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:\lib_c\glad\src\glad.c > CMakeFiles\app.dir\E_\lib_c\glad\src\glad.c.i

CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.s"
	E:\Compiler\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:\lib_c\glad\src\glad.c -o CMakeFiles\app.dir\E_\lib_c\glad\src\glad.c.s

# Object files for target app
app_OBJECTS = \
"CMakeFiles/app.dir/source/Auxiliary.cpp.obj" \
"CMakeFiles/app.dir/source/base.cpp.obj" \
"CMakeFiles/app.dir/source/imgui.cpp.obj" \
"CMakeFiles/app.dir/source/imgui_draw.cpp.obj" \
"CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.obj" \
"CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.obj" \
"CMakeFiles/app.dir/source/imgui_tables.cpp.obj" \
"CMakeFiles/app.dir/source/imgui_widgets.cpp.obj" \
"CMakeFiles/app.dir/source/main.cpp.obj" \
"CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.obj"

# External object files for target app
app_EXTERNAL_OBJECTS =

bin/app.exe: CMakeFiles/app.dir/source/Auxiliary.cpp.obj
bin/app.exe: CMakeFiles/app.dir/source/base.cpp.obj
bin/app.exe: CMakeFiles/app.dir/source/imgui.cpp.obj
bin/app.exe: CMakeFiles/app.dir/source/imgui_draw.cpp.obj
bin/app.exe: CMakeFiles/app.dir/source/imgui_impl_glfw.cpp.obj
bin/app.exe: CMakeFiles/app.dir/source/imgui_impl_opengl3.cpp.obj
bin/app.exe: CMakeFiles/app.dir/source/imgui_tables.cpp.obj
bin/app.exe: CMakeFiles/app.dir/source/imgui_widgets.cpp.obj
bin/app.exe: CMakeFiles/app.dir/source/main.cpp.obj
bin/app.exe: CMakeFiles/app.dir/E_/lib_c/glad/src/glad.c.obj
bin/app.exe: CMakeFiles/app.dir/build.make
bin/app.exe: E:/lib_c/glfw-3.4/build_mingw/src/libglfw3.a
bin/app.exe: E:/lib_c/assimp-master/build_mingw64/lib/libassimp.dll.a
bin/app.exe: CMakeFiles/app.dir/linkLibs.rsp
bin/app.exe: CMakeFiles/app.dir/objects1.rsp
bin/app.exe: CMakeFiles/app.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=E:\GitStore\MyEngine\MyEngine\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX executable bin\app.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\app.dir\link.txt --verbose=$(VERBOSE)
	"C:\Program Files\CMake\bin\cmake.exe" -E copy_if_different E:/lib_c/assimp-master/build_mingw64/bin/libassimp-5.dll E:/GitStore/MyEngine/MyEngine/build/bin

# Rule to build all files generated by this target.
CMakeFiles/app.dir/build: bin/app.exe
.PHONY : CMakeFiles/app.dir/build

CMakeFiles/app.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\app.dir\cmake_clean.cmake
.PHONY : CMakeFiles/app.dir/clean

CMakeFiles/app.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\GitStore\MyEngine\MyEngine E:\GitStore\MyEngine\MyEngine E:\GitStore\MyEngine\MyEngine\build E:\GitStore\MyEngine\MyEngine\build E:\GitStore\MyEngine\MyEngine\build\CMakeFiles\app.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/app.dir/depend

