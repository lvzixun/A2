CC = gcc
CFLAGS += -D _DEBUG_ -g -Wall
AR = ar rcu

ifeq ($(OS), Windows_NT)
 N = \\
 RM = del
 CFLAGS += -D __USE_MINGW_ANSI_STDIO -D _MINGW32_
 A2 = a2.lib
else
 N = //
 RM = rm -rf
 A2 = liba2.a
endif

A2_OBJ = a2_mem.o a2_error.o a2_io.o a2_lex.o a2_map.o a2_string.o a2_env.o  a2_closure.o \
a2_obj.o a2_gc.o a2_parse.o a2_ir.o a2_array.o a2_state.o a2_xclosure.o a2_vm.o\
a2_libutil.o

#TEST_OBJ = ./test/test_io.o ./test/test_string.o ./test/test_lex.o ./test/test_map.o ./test/test_parse.o ./test/test_ir.o
TEST_OBJ = .$(N)test$(N)test_a2.o
OBJ = $(A2_OBJ) $(TEST_OBJ)

TEST =  $(foreach s, $(TEST_OBJ), $(basename $(s)))

all: $(A2) $(TEST)

$(TEST):
	$(CC) $(CFLAGS) -o $@ $? $(basename $@).o  $(A2)

$(A2): $(OBJ)
	$(AR) $(A2) $?

$(OBJ): 
	$(CC) $(CFLAGS) -c -o $@ $(basename $@).c

.PHONY : clean
clean:
	$(RM) $(A2_OBJ)
	$(RM) $(TEST_OBJ)
	$(RM) $(TEST)
	$(RM) $(A2)
