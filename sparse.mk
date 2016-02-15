TARGET = sparse
$(info BUILDING: $(TARGET))

# gcc or clang should be interchangable if needed
CC = gcc
CFLAGS = -O3 -std=c99 -Wall
LFLAGS =

# all source files in src
SRC = src/sparse_entry.c src/spartan.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

clean :
	rm $(OBJ); rm $(TARGET)
