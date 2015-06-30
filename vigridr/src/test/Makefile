CC = g++ -std=c++0x -DGTEST_USE_OWN_TR1_TUPLE=0
INCLUDES = -I ../../third-parties/cpp/include
LFLAGS = -L../../third-parties/cpp/lib
LIBRARIES = -lpthread -lgtest
TARGETS = GameTest
OBJECTS_GameTest = ../../obj/server/GameLogic.o
BIN := ./bin

all: $(BIN) $(BIN)/$(TARGETS)

$(BIN):
	@mkdir -p $(BIN)
	@echo "Created test bin directory"

$(BIN)/$(TARGETS): $(BIN)/% : %.cpp
	$(CC) $< $(OBJECTS_$(<:%.cpp=%)) $(INCLUDES) $(LFLAGS) -o $@ $(LIBRARIES)

remove:
	rm -rf bin

clean: remove

test: $(BIN)/$(TARGETS)
	$(foreach target, $(TARGETS), ./$(BIN)/$(target);)
