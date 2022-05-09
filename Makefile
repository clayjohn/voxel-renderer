#OBJS specifies which files to compile as part of the project
OBJS = main.cpp src/glad.c

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I include -I/usr/local/include

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L/usr/local/lib -L/usr/lib -L/usr/lib/x86_64-linux-gnu

#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -std=c++11 

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS =  -lXi -lm -lGL -lm -lpthread -ldl -lglfw -lrt -lm -ldl -lXrandr -lXinerama -lXext -lXcursor -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp
#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = main

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
