CC = gcc
CFLAGS += -g -Wall 

A2_OBJ = a2_conf.o a2_error.o a2_io.o a2_lex.o a2_map.o
TEST_OBJ = ./test/test_io.o 
OBJ = $(A2_OBJ) $(TEST_OBJ)

TEST =  $(foreach s, $(TEST_OBJ), $(basename $(s)))

$(TEST): $(OBJ) 
	$(CC) -o $@ $?

$(OBJ): 
	$(CC) $(CFLAGS) -c -o $@ $(basename $@).c

.PHONY : clean
clean:
	rm -rf *.o
	rm -rf ./test/*.o
	rm -rf $(TEST)