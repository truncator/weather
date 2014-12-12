CXX=g++
FLAGS=-std=c++11
BIN=weather
INCLUDE_DIR=./Libraries
SOURCE=main.cpp Config.cpp $(INCLUDE_DIR)/json/jsoncpp.cpp

weather:
	$(CXX) $(FLAGS) -o $(BIN) $(SOURCE) -I$(INCLUDE_DIR)

all: weather

clean:
	rm -f weather *.o
