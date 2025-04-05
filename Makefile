# Just uncomment whichever it is on your machine
CPP=g++
#CPP=/opt/homebrew/bin/g++-13
CPPFLAGS=-std=c++20 -I.

main: Huffman.o bitmap.o
	$(CPP) $(CPPFLAGS) -o main Huffman.o bitmap.o

Huffman.o : Huffman.cpp
	$(CPP) $(CPPFLAGS) -c Huffman.cpp

bitmap.o : bitmap-cpp/bitmap.cpp
	$(CPP) $(CPPFLAGS) -c bitmap-cpp/bitmap.cpp

clean :
	rm main bitmap.o Huffman.o
