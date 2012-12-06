CC = gcc
CFLAGS += -g -Wall 

A2_OBJ = a2_conf.o a2_error.o a2_io.o a2_lex.o a2_map.o a2_string.o
TEST_OBJ = ./test/test_io.o ./test/test_string.o
OBJ = $(A2_OBJ) $(TEST_OBJ)

TEST =  $(foreach s, $(TEST_OBJ), $(basename $(s)))

all: $(OBJ) $(TEST)

$(TEST): $(A2_OBJ) 
	$(CC) -o $@ $? $(basename $@).o

$(OBJ): 
	$(CC) $(CFLAGS) -c -o $@ $(basename $@).c

.PHONY : clean
clean:
	rm -rf *.o
	rm -rf ./test/*.o
	rm -rf $(TEST)