include Makefile.conf

all: test app

dirs:
	mkdir -p $(BIN_DIR) $(OBJ_DIR)

test: dirs $(BIN_DIR)/board_tests

app: dirs $(BIN_DIR)/game

$(BIN_DIR)/board_tests: $(OBJ_DIR)/Board_t.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o Board.h
	$(CXX) $(CFLAGS) -o $(BIN_DIR)/board_tests $(OBJ_DIR)/Board_t.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o

$(BIN_DIR)/game: $(OBJ_DIR)/Game.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/CommandLineParser.o Board.h 
	$(CXX) $(CFLAGS) -o $(BIN_DIR)/game $(OBJ_DIR)/Game.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/CommandLineParser.o

$(OBJ_DIR)/Game.o: Game.cc Board.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Game.o Game.cc

$(OBJ_DIR)/Board_t.o: Board_t.cc Board.h utils/Test.h utils/Mock.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Board_t.o Board_t.cc

$(OBJ_DIR)/Board.o: Board.cc Board.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Board.o Board.cc

$(OBJ_DIR)/Test.o: utils/Test.cc utils/Test.h utils/CommandLineParser.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Test.o utils/Test.cc

$(OBJ_DIR)/CommandLineParser.o: utils/CommandLineParser.cc utils/CommandLineParser.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/CommandLineParser.o utils/CommandLineParser.cc

clean:
	rm -f $(BIN_DIR)/*
	rm -f $(OBJ_DIR)/*
