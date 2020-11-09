INC=-I./include

default: main
main: src/main.cpp
	@mkdir -p build
	@g++ -fno-inline-small-functions -fno-split-wide-types -fno-tree-scev-cprop -std=c++11 $(INC) "./src/main.cpp" -o "./build/interpreter"
clean:
	@rm -rf build

.PHONY: clean
