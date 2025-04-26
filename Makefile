CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
OBJ = main.o lexer.o
TARGET = lexer

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

main.o: main.cpp lexer.h
	$(CXX) $(CXXFLAGS) -c main.cpp

lexer.o: lexer.cpp lexer.h
	$(CXX) $(CXXFLAGS) -c lexer.cpp

clean:
	rm -f $(OBJ) $(TARGET)