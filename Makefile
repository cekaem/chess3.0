include Makefile.conf

all: test app

dirs:
	mkdir -p $(BIN_DIR) $(OBJ_DIR)

test: dirs $(BIN_DIR)/board_tests $(BIN_DIR)/move_calculator_tests $(BIN_DIR)/engine_tests $(BIN_DIR)/pgn_creator_tests

app: dirs $(BIN_DIR)/game

$(BIN_DIR)/board_tests: $(OBJ_DIR)/Board_t.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o
	$(CXX) $(CFLAGS) -o $(BIN_DIR)/board_tests $(OBJ_DIR)/Board_t.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o

$(BIN_DIR)/move_calculator_tests: $(OBJ_DIR)/MoveCalculator_t.o $(OBJ_DIR)/MoveCalculator.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o
	$(CXX) $(CFLAGS) -o $(BIN_DIR)/move_calculator_tests $(OBJ_DIR)/MoveCalculator_t.o $(OBJ_DIR)/MoveCalculator.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o

$(BIN_DIR)/engine_tests: $(OBJ_DIR)/Engine_t.o $(OBJ_DIR)/Engine.o $(OBJ_DIR)/MoveCalculator.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o
	$(CXX) $(CFLAGS) -o $(BIN_DIR)/engine_tests $(OBJ_DIR)/Engine_t.o $(OBJ_DIR)/Engine.o $(OBJ_DIR)/MoveCalculator.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o

$(BIN_DIR)/pgn_creator_tests: $(OBJ_DIR)/PGNCreator_t.o $(OBJ_DIR)/PGNCreator.o $(OBJ_DIR)/MoveCalculator.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o
	$(CXX) $(CFLAGS) -o $(BIN_DIR)/pgn_creator_tests $(OBJ_DIR)/PGNCreator_t.o $(OBJ_DIR)/PGNCreator.o $(OBJ_DIR)/MoveCalculator.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Test.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o

$(BIN_DIR)/game: $(OBJ_DIR)/Game.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Engine.o $(OBJ_DIR)/MoveCalculator.o $(OBJ_DIR)/PGNCreator.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o
	$(CXX) $(CFLAGS) -o $(BIN_DIR)/game $(OBJ_DIR)/Game.o $(OBJ_DIR)/Board.o $(OBJ_DIR)/Engine.o $(OBJ_DIR)/MoveCalculator.o $(OBJ_DIR)/PGNCreator.o $(OBJ_DIR)/CommandLineParser.o $(OBJ_DIR)/Utils.o

$(OBJ_DIR)/PGNCreator.o: PGNCreator.cc PGNCreator.h Board.h MoveCalculator.h Types.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/PGNCreator.o PGNCreator.cc

$(OBJ_DIR)/PGNCreator_t.o: PGNCreator_t.cc PGNCreator.h MoveCalculator.h Board.h Types.h utils/Test.h utils/Mock.h utils/Utils.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/PGNCreator_t.o PGNCreator_t.cc

$(OBJ_DIR)/Game.o: Game.cc Board.h Engine.h MoveCalculator.h PGNCreator.h Types.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Game.o Game.cc

$(OBJ_DIR)/Board_t.o: Board_t.cc Board.h utils/Test.h utils/Mock.h utils/Utils.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Board_t.o Board_t.cc

$(OBJ_DIR)/Board.o: Board.cc Board.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Board.o Board.cc

$(OBJ_DIR)/Engine.o: Engine.cc Engine.h MoveCalculator.h Board.h Types.h utils/Timer.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Engine.o Engine.cc

$(OBJ_DIR)/Engine_t.o: Engine_t.cc Engine.h MoveCalculator.h Board.h utils/Test.h utils/Mock.h utils/Utils.h Types.h utils/Timer.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Engine_t.o Engine_t.cc

$(OBJ_DIR)/MoveCalculator.o: MoveCalculator.cc MoveCalculator.h Board.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/MoveCalculator.o MoveCalculator.cc

$(OBJ_DIR)/MoveCalculator_t.o: MoveCalculator_t.cc MoveCalculator.h Board.h utils/Test.h utils/Mock.h utils/Utils.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/MoveCalculator_t.o MoveCalculator_t.cc

$(OBJ_DIR)/Test.o: utils/Test.cc utils/Test.h utils/CommandLineParser.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Test.o utils/Test.cc

$(OBJ_DIR)/CommandLineParser.o: utils/CommandLineParser.cc utils/CommandLineParser.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/CommandLineParser.o utils/CommandLineParser.cc

$(OBJ_DIR)/Utils.o: utils/Utils.cc utils/Utils.h
	$(CXX) $(CFLAGS) -c -o $(OBJ_DIR)/Utils.o utils/Utils.cc

clean:
	rm -f $(BIN_DIR)/*
	rm -f $(OBJ_DIR)/*
