ONum:=-O3
CompilerVer:=-std=c++11
CFlag:=
mainFileName:=test
exeFileName:=main

make: board_state.o Protocol.o ClientSocket.o MyAI.o main.o
	g++ $(mainFileName).o Protocol.o MyAI.o ClientSocket.o board_state.o -o $(exeFileName)
board_state.o: src/board_state.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/board_state.cpp
ClientSocket.o: src/ClientSocket.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/ClientSocket.cpp
Protocol.o: src/Protocol.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/Protocol.cpp
MyAI.o: src/MyAI.cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/MyAI.cpp
main.o: src/$(mainFileName).cpp
	g++ -c $(CFlag) $(ONum) $(CompilerVer) src/$(mainFileName).cpp
all: clean make
	rm -rf *.o
	./$(exeFileName)
exe: make
	./$(exeFileName)
clean:
	rm -rf *.o *.exe $(exeFileName)