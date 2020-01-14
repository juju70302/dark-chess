ONum:=-O3
CompilerVer:=-std=c++11
CFlag:=
mainFileName:=test
exeFileName:=main

make: nopre
	
all: clean pre make
	rm -rf *.o
	./$(exeFileName)
exe: make
	./$(exeFileName)
clean:
	rm -rf *.o *.exe $(exeFileName)


pre: preprocess.o
	g++ preprocess.o -o preprocess
	./preprocess
	rm -rf preprocess.o preprocess
nopre: board_state.o trans_table.o AI.o Protocol.o ClientSocket.o MyAI.o main.o
	g++ $(mainFileName).o Protocol.o AI.o MyAI.o ClientSocket.o board_state.o hash_value.o -o $(exeFileName)

preprocess.o: src/preprocess.cpp
	rm -rf preprocess.o
	g++ -c -D _PRE_PROCESS_ $(CFlag) $(ONum) $(CompilerVer) src/preprocess.cpp
board_state.o: src/board_state.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/board_state.cpp
trans_table.o: src/hash_value.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/hash_value.cpp
AI.o: src/AI.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/AI.cpp
ClientSocket.o: src/ClientSocket.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/ClientSocket.cpp
Protocol.o: src/Protocol.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/Protocol.cpp
MyAI.o: src/MyAI.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/MyAI.cpp
main.o: src/$(mainFileName).cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/$(mainFileName).cpp