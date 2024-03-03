CC = g++
CFLAGS = -std=c++11 -Wall

SRC = main.cpp
HEADER = editor.hpp
OBJ = $(SRC:.cpp=.o)
EXEC = ed

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

%.o: %.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
