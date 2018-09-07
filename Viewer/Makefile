# Desired program name
PROG := main

# Project specific libraries and compiler flags
LD_FLAGS := -lsfml-graphics -lsfml-network -lsfml-audio -lsfml-window -lsfml-system -lstdc++ -framework OpenGL
C_FLAGS  := -std=c++14 -O0 -Wall -Wextra -Wshadow -pedantic -g -Wformat-security

# Desired compiler
CC := clang

# Location of our libraries and headers
C_LIB := -L/usr/local/lib
C_INC := -I/usr/local/include -I./imgui -I./imgui-sfml

# Location of our source files and object files
C_FILES   := $(wildcard src/*.cpp) imgui/imgui.cpp imgui/imgui_draw.cpp imgui-sfml/imgui-SFML.cpp
OBJ_FILES := $(addprefix obj/,$(notdir $(C_FILES:.cpp=.o)))

# Generic commands for make and make clean
bin/$(PROG): $(OBJ_FILES)
	$(CC) $(C_LIB) $(LD_FLAGS) -o $@ $^

obj/%.o: src/%.cpp src/%.hpp
	$(CC) $(C_FLAGS) $(C_INC) -c -o $@ $<

obj/imgui.o: imgui/imgui.cpp
	$(CC) $(C_FLAGS) $(C_INC) -c -o $@ $<

obj/imgui_draw.o: imgui/imgui_draw.cpp
	$(CC) $(C_FLAGS) $(C_INC) -c -o $@ $<

obj/imgui-SFML.o: imgui-sfml/imgui-SFML.cpp
	$(CC) $(C_FLAGS) $(C_INC) -c -o $@ $<

obj/main.o: src/main.cpp
	$(CC) $(C_FLAGS) $(C_INC) -c -o $@ $<

clean:
	rm obj/*.o
	rm tests/*.o
	rm bin/$(PROG)

# Testing

tests/%.o: tests/%.cpp src/%.h
	$(CC) $(C_FLAGS) $(C_INC) -I./src/ -c -o $@ $<

test_runner/test: tests/*.o obj/*.o
	$(CC) $(C_LIB) $(LD_FLAGS) -o $@ $(filter-out obj/main.o, $^)

test: test_runner/test
	./test_runner/test
