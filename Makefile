CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra -march=native

SRC = src/main.cpp src/tracer_core.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = kronos_tracer

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o include/*.gch $(TARGET) *.krn

