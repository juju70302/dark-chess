ONum:=-O3
CompilerVer:=-std=c++11
CFlag:=
mainFileName:=main
exeFileName:=main

all: clean pre make
	rm -rf *.o
make: agent

nopre: agent
	
exe:
	./$(exeFileName)
clean:
	rm -rf *.o *.exe $(exeFileName) src/hash_value.cpp

agent: src/agent.cpp board_state trans_table AI
	g++ -std=c++17 -O2 src/$(mainFileName).cpp src/agent.cpp AI.o board_state.o hash_value.o -o $(exeFileName)
random: src/agent.cpp board_state trans_table
	g++ -c $(CFlag) $(ONum) $(CompilerVer) -D RANDOM_MOVE src/AI.cpp
	g++ -std=c++17 -O2 src/$(mainFileName).cpp src/agent.cpp AI.o board_state.o hash_value.o -o random
pre: preprocess
	g++ preprocess.o -o preprocess
	./preprocess
	rm -rf preprocess.o preprocess
test: board_state trans_table AI test.o
	g++ test.o AI.o board_state.o hash_value.o -o $(exeFileName)


preprocess: src/preprocess.cpp
	rm -rf preprocess.o
	g++ -c -D _PRE_PROCESS_ $(CFlag) $(ONum) $(CompilerVer) src/preprocess.cpp
board_state: src/board_state.cpp src/board_state.h
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/board_state.cpp
trans_table: src/hash_value.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/hash_value.cpp
AI: src/AI.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/AI.cpp
test.o: src/test.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/test.cpp
main.o: src/$(mainFileName).cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/$(mainFileName).cpp