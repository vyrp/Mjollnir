CC = g++ -std=c++0x
INCLUDES = -I/usr/local/include/gtest
LFLAGS = -L/usr/local/lib
LIBRARIES = -lpthread -lgtest
TARGETS = TicTacToeTest
OBJECTS_TicTacToeTest = ../../obj/server/GameLogic.o
BIN := ./bin

all: $(BIN) $(BIN)/$(TARGETS)

$(BIN):
	@mkdir -p $(BIN)
	@echo "Created test bin directory"

$(BIN)/$(TARGETS): $(BIN)/% : %.cpp
	$(CC) $< $(OBJECTS_$(<:%.cpp=%)) $(INCLUDES) $(LFLAGS) -o $@ $(LIBRARIES)



test: $(BIN)/$(TARGETS)	
	$(foreach target, $(TARGETS), ./$(BIN)/$(target);)
