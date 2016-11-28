
#include "../src/a2.h"
#include "../src/a2_libutil.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>


#define A2_COM_HEAD "A2 power by zixun.\n"


static inline void _load(struct a2_state* state, const char* filename){
  if(a2_loadfile(state, filename)){
    printf("%s\n", a2_tostring(state, a2_top(state)-1));
  }
}

static void _command(struct a2_state* state){
  for(;;){
    char* str = NULL;
    if( NULL != (str = readline(">")) ){ 
      if(str[0]=='='){
        size_t sz = strlen(str)+7;
        char tmp [sz];
        sprintf(tmp , "print(%s)", str+1);
        tmp[sz-1] = '\0';
        if(a2_dostring(state, tmp, strlen(tmp)))
          printf("%s", a2_tostring(state, a2_top(state)-1)); 
      }else{
      if(a2_dostring(state, str, strlen(str)))
        printf("%s", a2_tostring(state, a2_top(state)-1));
      }
      add_history(str);
      free(str);
    }
  }
}

int main(int argc, char const *argv[]){
  struct a2_state* as = a2_open();
  a2_openutil(as);

  if(argc ==2){ // load file modle
    _load(as, argv[1]);
  }else if (argc == 1){ // command modle
    printf(A2_COM_HEAD);
    _command(as);
  }else{
    printf("use:\na2 [file]\n");
  }

  a2_close(as);
  return 0;
}