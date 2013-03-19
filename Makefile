

BUILD_DIR = ./bin
SRC_DIR = ./src
INC = ./include
CC = g++ -std=c++0x -I$(INC)

DEPS = $(INC)/Graph.h $(INC)/PartitionedGraph.h
OBJ = $(BUILD_DIR)/Graph.o $(BUILD_DIR)/PartitionedGraph.o $(BUILD_DIR)/Main.o
SRC = $(SRC_DIR)/Graph.cc $(SRC_DIR)/PartitionedGraph.cc $(SRC_DIR)/Main.cc

all: binary binary.debug

bin/%.o : src/%.cc $(DEPS)
	$(CC) -c -o $@ $<

binary: $(OBJ)
	$(CC) -o $@ $(OBJ)

binary.debug: $(SRC) $(DEPS)
	$(CC) -o $@ -g $(SRC)

clean:
	\rm -f binary binary.debug bin/*

.PHONY: clean

