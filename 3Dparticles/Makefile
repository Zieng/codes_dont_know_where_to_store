GLFW = `pkg-config --cflags glfw3` `pkg-config --libs --static glfw3`
GLEW = `pkg-config --cflags glew` `pkg-config --libs glew`
LIB = -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa

COMMON = load_funcs.cpp
TARGET = 3Dparticles

all: $(TARGET)

$(TARGET): $(TARGET).cpp 
	clang++ $(TARGET).cpp -o $(TARGET) $(COMMON) $(GLFW) $(GLEW) $(LIB)
clear:
	rm *.o