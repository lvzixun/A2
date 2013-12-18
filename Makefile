CC = gcc
CFLAGS += -O2  -g -Wall
AR = ar rcu

UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
	ifeq ($(UNAME_S), CYGWIN_NT-6.1)
		N = //
		RM = rm -rf
		A2_STATIC_LIB = liba2.a
		A2 = a2
	else
		N = \\
		RM = del
		CFLAGS += -D __USE_MINGW_ANSI_STDIO -D _MINGW32_
		A2_STATIC_LIB = a2.lib
		A2 = a2.exe 
	endif
else 
	ifeq ($(UNAME_S), Darwin)
		N = //
		RM = rm -rf
		A2_STATIC_LIB = liba2.a
		CFLAGS += -std=gnu89
		A2 = a2
	endif 
	ifeq ($(UNAME_S), Linux)
		N = //
		RM = rm -rf
		A2_STATIC_LIB = liba2.a
		A2 = a2
	endif
endif

_A2_OBJ = a2_mem.o a2_error.o a2_io.o a2_lex.o a2_map.o a2_string.o a2_env.o  a2_closure.o \
a2_obj.o a2_gc.o a2_parse.o a2_ir.o a2_array.o a2_state.o a2_xclosure.o a2_vm.o\
a2_libutil.o 
A2_OBJ = $(foreach s, $(_A2_OBJ), .$(N)src$(N)$(s))

_A2_T_OBJ = a2.o
A2_T_OBJ = $(foreach s, $(_A2_T_OBJ), .$(N)src$(N)$(s))

#TEST_OBJ = ./test/test_io.o ./test/test_string.o ./test/test_lex.o ./test/test_map.o ./test/test_parse.o ./test/test_ir.o
TEST_OBJ = .$(N)test$(N)test_a2.o
TEST =  $(foreach s, $(TEST_OBJ), $(basename $(s)))

OBJ = $(A2_OBJ) $(TEST_OBJ) $(A2_T_OBJ)
OBJ_C = $(foreach s, $(OBJ), $(basename $(s)).c)

all: test2 $(OBJ) $(A2_STATIC_LIB) $(TEST) $(A2)

test2:
	@echo $(OS)


install:
	cp a2 /usr/local/bin/

$(A2): $(A2_T_OBJ)
	$(CC) $(CFLAGS) -o $@ $? -lreadline $(A2_STATIC_LIB)

$(TEST): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $?  $(A2_STATIC_LIB)

$(A2_STATIC_LIB): $(A2_OBJ)
	$(AR) $(A2_STATIC_LIB) $?

$(OBJ): $(OBJ_C)
	$(CC) $(CFLAGS) -c -o $@ $(basename $@).c

.PHONY : clean
clean:
	$(RM) $(A2_OBJ)
	$(RM) $(TEST_OBJ)
	$(RM) $(TEST)
	$(RM) $(A2_STATIC_LIB)
	$(RM) $(A2) $(A2_T_OBJ)
