TARGET_EXEC := lang
TEST_EXEC := lang_test 

CXX = clang
CPPFLAGS = -std=c++17 -stdlib=libc++ -O0 -g -fobjc-arc
LIBRARIES := -lstdc++
LDFLAGS = $(LIBRARIES)

BUILD_DIR := ./build
SRC_DIRS := ./src
TEST_DIR := ./test

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
TEST_SRC := $(shell find $(TEST_DIR) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
TEST_OBJS := $(filter-out ./build/./src/main.cpp.o, $(OBJS)) ./build/./test/main.cpp.o

all: $(BUILD_DIR)/$(TARGET_EXEC) $(BUILD_DIR)/$(TEST_EXEC)

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/$(TEST_EXEC): $(TEST_OBJS)
	$(CXX) $(TEST_OBJS) -o $@ $(LDFLAGS)

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

test: $(BUILD_DIR)/$(TEST_EXEC)
	$^

run: $(BUILD_DIR)/$(TARGET_EXEC)
	$^

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)