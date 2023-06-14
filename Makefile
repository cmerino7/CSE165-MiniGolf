CC = gcc
CXX = g++

COMPILE_FLAGS = -Wall -pthread -ggdb -O3
LINK_FLAGS = -lglfw3 -lsoil -lGL -lm -ldl
WARNINGS = -Wno-delete-non-virtual-dtor -Wno-unused-value -Wno-reorder -Wno-catch-value -Wno-maybe-uninitialized

glfw = library/glfw
glfw_inc = $(glfw)/include
glfw_lib = $(glfw)/build/src

glad = library/GLAD
glad_inc = $(glad)/include

glm = library/glm-master
glm_inc = $(glm)

soil = library/soil
soil_inc = $(soil)/inc
soil_lib = $(soil)/build

INCLUDES = -I$(glfw_inc) -I$(glad_inc) -I$(glm_inc) -I$(soil_inc)
LIBRARIES = -L$(glfw_lib) -L$(soil_lib)

cpp_files = main.cpp game.cpp ball_object.cpp game_level.cpp game_object.cpp resource_manager.cpp shader.cpp sprite_renderer.cpp texture.cpp

headers = game.h ball_object.h game_level.h game_object.h resource_manager.h shader.h

all:
	$(CXX) $(LIBRARIES) $(INCLUDES) -o MiniGolf.exe $(cpp_files) library/GLAD/src/glad.c $(COMPILE_FLAGS) $(LINK_FLAGS) $(WARNINGS)
	
run:
	./MiniGolf.exe
	
clean:
	rm -f MiniGolf.exe
