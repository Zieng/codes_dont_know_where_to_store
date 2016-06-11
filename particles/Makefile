GLM = -I /usr/local/include
GLFW = `pkg-config --cflags glfw3` `pkg-config --libs --static glfw3`
GLEW = `pkg-config --cflags glew` `pkg-config --libs glew`
LIB = -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa

COMMON = load_funcs.cpp
TARGET = particles

all: $(TARGET)

$(TARGET): $(TARGET).cpp
	clang++ $(TARGET).cpp -o $(TARGET) $(COMMON) $(GLFW) $(GLEW) $(LIB) $(GLM)
clear:
	rm *.o
