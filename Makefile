# Compiler settings
CC = gcc
CXX = g++
AR = ar
CFLAGS = -Wall -Iinclude -static -g
CXXFLAGS = -Wall -Iinclude -static -g
LDFLAGS = -lenet

# Directories
SRC_DIR = src
EXAMPLES_DIR = examples
BUILD_DIR = build
LIB_NAME = libsmpn.a

# Find all source files
C_SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJ_DIR = $(BUILD_DIR)/obj
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SRCS))

CPP_SRCS = $(wildcard $(EXAMPLES_DIR)/*.cpp)
CPP_OBJS = $(patsubst $(EXAMPLES_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SRCS))
EXE_FILES = $(patsubst $(EXAMPLES_DIR)/%.cpp,$(EXAMPLES_DIR)/%,$(CPP_SRCS))

# Default target
all: $(BUILD_DIR)/$(LIB_NAME) $(EXE_FILES)

# Create build directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build static library
$(BUILD_DIR)/$(LIB_NAME): $(OBJS) | $(BUILD_DIR)
	$(AR) rcs $@ $(OBJS)

# Compile C++ examples
$(EXAMPLES_DIR)/%: $(EXAMPLES_DIR)/%.cpp $(BUILD_DIR)/$(LIB_NAME)
	$(CXX) $(CXXFLAGS) $< -L$(BUILD_DIR) -lmpn $(LDFLAGS) -o $@

# Clean
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*.a $(OBJ_DIR)/*.o $(EXAMPLES_DIR)/*


