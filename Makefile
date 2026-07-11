# ALGOL 26 Compiler Core Build Core Automation Engine
CXX      := g++
CXXFLAGS := -std=c++23 -fmodules-ts -O3
TARGET   := a126c
SRC_DIR  := src
BUILD_DIR:= build

# Track explicit C++23 Module Object Dependencies
MODS     := tokens.o ast_nodes.o lexer.o parser.o codegen.o
MAIN     := $(SRC_DIR)/main.cc

.PHONY: all clean examples

all: check_env $(TARGET)

# Mandate compiler framework environment verification
check_env:
	@echo "Checking system dependency configurations..."
	@$(CXX) -v 2>&1 | grep -q "gcc version 1[4-6]" || (echo "Error: GCC version 14+ with C++23 support is required." && exit 1)

$(TARGET): $(MODS)
	$(CXX) $(CXXFLAGS) $(MODS) $(MAIN) -o $(TARGET)
	@mkdir -p $(BUILD_DIR)

tokens.o: $(SRC_DIR)/tokens.cppm
	$(CXX) $(CXXFLAGS) --compile-std-module -c $(SRC_DIR)/tokens.cppm

ast_nodes.o: $(SRC_DIR)/ast_nodes.cppm
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/ast_nodes.cppm

lexer.o: $(SRC_DIR)/lexer.cppm
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/lexer.cppm

parser.o: $(SRC_DIR)/parser.cppm
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/parser.cppm

codegen.o: $(SRC_DIR)/codegen.cppm
	$(CXX) $(CXXFLAGS) -c $(SRC_DIR)/codegen.cppm

clean:
	rm -rf gcm.cache/ *.o $(TARGET) $(BUILD_DIR)/
	@echo "Workspace compilation artifacts completely purged."
