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
OBJS = $(patsubst src/%.cpp, obj/%.o, $(SRC))
TARGET = sss_server

.PHONY: all clean

all: obj $(TARGET)

obj:
	mkdir -p obj

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf obj $(TARGET)