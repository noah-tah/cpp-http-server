# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

# Libraries
LIBS = -lws2_32

# Sources
SRC = src/Networking.cpp \
      src/HTTPRequest.cpp \
      src/HTTPResponse.cpp \
      src/Utils.cpp \
      src/Server.cpp \
      main.cpp

# Object Files
OBJ = $(SRC:.cpp=.o)

# Executable
EXEC = server.exe

all: $(EXEC)

$(EXEC): $(OBJ)
    $(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
    del /Q *.o $(EXEC)