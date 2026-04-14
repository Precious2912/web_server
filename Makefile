CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2

SRC = src/main.cpp \
      src/socket.cpp \
      src/request.cpp \
      src/response.cpp \
      src/router.cpp \
      src/file_handler.cpp \
	  src/form_handler.cpp \
	  src/security.cpp \
	  src/utils.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = sss_server

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)