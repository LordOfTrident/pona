SRC  = $(wildcard src/*.cc)
DEPS = $(wildcard src/*.hh)
OBJ  = $(addsuffix .o,$(subst src/,bin/,$(basename $(SRC))))

OUT = ./bin/app

CXX = g++
CXX_VER = c++17
CXX_FLAGS = -static -static-libstdc++ -O3 -std=$(CXX_VER) -Wall -Wextra -Werror \
            -pedantic -Wno-deprecated-declarations
CXX_LIBS = -lncursesw -ltinfo

compile: ./bin $(OBJ) $(SRC)
	$(CXX) $(CXX_FLAGS) -o $(OUT) $(OBJ) $(CXX_LIBS)

bin/%.o: src/%.cc $(DEPS)
	$(CXX) -c $< $(CXX_FLAGS) -o $@

./bin:
	mkdir -p bin

clean:
	rm -r ./bin/*

all:
	@echo compile, clean
