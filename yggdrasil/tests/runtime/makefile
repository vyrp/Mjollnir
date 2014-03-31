BIN=$(MAKEFILE_DIR)/bin
SRC=$(MAKEFILE_DIR)/src
FILES=$(MAKEFILE_DIR)/files

all: $(BIN) $(BIN)/file_read $(BIN)/file_write $(BIN)/sleep $(BIN)/while $(BIN)/mem $(BIN)/fork $(BIN)/incs

$(BIN):
	mkdir $(BIN)

$(BIN)/file_read: $(SRC)/file_read.cpp
	g++ -Wall $(SRC)/file_read.cpp -o $(BIN)/file_read

$(BIN)/file_write: $(SRC)/file_write.cpp
	g++ -Wall $(SRC)/file_write.cpp -o $(BIN)/file_write

$(BIN)/sleep: $(SRC)/sleep.cpp
	g++ -Wall $(SRC)/sleep.cpp -o $(BIN)/sleep

$(BIN)/while: $(SRC)/while.cpp
	g++ -Wall $(SRC)/while.cpp -o $(BIN)/while

$(BIN)/mem: $(SRC)/mem.cpp
	g++ -Wall $(SRC)/mem.cpp -o $(BIN)/mem

$(BIN)/fork: $(SRC)/fork.cpp
	g++ -Wall $(SRC)/fork.cpp -o $(BIN)/fork

$(BIN)/incs: $(SRC)/includes.cpp
	g++ -Wall -std=c++11 $(SRC)/includes.cpp -o $(BIN)/incs -lpthread
	
clean:
	rm -rf $(BIN)
	rm -rf $(FILES)/created.txt
    
