#ifndef _A2_IO_H_
#define _A2_IO_H_
#include "a2_conf.h"
#include "a2_env.h"

struct a2_io;

// for file
struct a2_io*  a2_io_open(struct a2_env* env_p, const char* file_name);
void a2_io_close(struct a2_io* io_p);

// for string
struct a2_io* a2_io_openS(const char*  str, size_t len);
void a2_io_closeS(struct a2_io* io_p);

inline char a2_io_readchar(struct a2_io* io_p);
inline char a2_io_atchar(struct a2_io* io_p);
inline char a2_io_matchchar(struct a2_io* io_p, int n);
inline int a2_io_end(struct a2_io* io_p);

#endif

