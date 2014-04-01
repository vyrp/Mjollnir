BIN=$(MAKEFILE_DIR)/bin
SRC=$(MAKEFILE_DIR)/src

FULL_SOURCES=$(shell ls $(SRC)/*.cpp)
SOURCES=$(foreach file, $(FULL_SOURCES), $(shell basename $(file)))
BINARIES=$(SOURCES:%.cpp=$(BIN)/%)

all: $(BIN) $(BINARIES)

$(BIN):
	mkdir -p $(BIN)

$(BINARIES): $(BIN)/% : $(SRC)/%.cpp
	g++ -Wall -std=c++11 $< -o $@ -lpthread
	
clean:
	rm -rf $(BIN)
    
