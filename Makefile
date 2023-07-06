CXX := g++
CXXFLAGS := -std=c++17


ifeq ($(OS),Windows_NT)
	TARGET := main.exe
else
	TARGET := main
endif


SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)


$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
