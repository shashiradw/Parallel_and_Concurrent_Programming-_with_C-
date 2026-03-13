CXX = g++
CXXFLAGS = -std=c++20 -O2 -I /usr/include
LDFLAGS = -lboost_system -lpthread

TARGET = matrix
SRC = matric_multiplication.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)