CC=g++
CFLAGS=-c --std=c++20
LDFLAGS=

OUTPUT_DIR=build
SRC_DIR=src
TEST_DIR=test

COMPILE=$(CC) $(CFLAGS)
LINK = $(CC) $(LDFLAGS)

run_tests: link_tests
	$(OUTPUT_DIR)/test_build

link_tests: build_tests build_lib build_main $(OUTPUT_DIR)
	$(LINK) $(OUTPUT_DIR)/constructor.o $(OUTPUT_DIR)/addition.o $(OUTPUT_DIR)/subtraction.o $(OUTPUT_DIR)/division.o $(OUTPUT_DIR)/multiplication.o $(OUTPUT_DIR)/BigDecimal.o $(OUTPUT_DIR)/main.o -o $(OUTPUT_DIR)/test_build

build_tests: $(OUTPUT_DIR)
	$(COMPILE) $(TEST_DIR)/constructor.cpp -o $(OUTPUT_DIR)/constructor.o && \
	$(COMPILE) $(TEST_DIR)/addition.cpp -o $(OUTPUT_DIR)/addition.o && \
	$(COMPILE) $(TEST_DIR)/subtraction.cpp -o $(OUTPUT_DIR)/subtraction.o && \
	$(COMPILE) $(TEST_DIR)/division.cpp -o $(OUTPUT_DIR)/division.o && \
	$(COMPILE) $(TEST_DIR)/multiplication.cpp -o $(OUTPUT_DIR)/multiplication.o \

build_lib: $(OUTPUT_DIR)
	$(COMPILE) $(SRC_DIR)/BigDecimal.cpp -o $(OUTPUT_DIR)/BigDecimal.o

build_main: $(OUTPUT_DIR)
	$(COMPILE) main.cpp -o $(OUTPUT_DIR)/main.o

$(OUTPUT_DIR):
	mkdir $(OUTPUT_DIR)