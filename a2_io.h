#ifndef _A2_IO_H_
#define _A2_IO_H_

struct a2_io;

struct a2_io*  a2_io_open(const char* file_name);
void a2_io_close(struct a2_io* io_p);
char a2_io_readchar(struct a2_io* io_p);
int a2_io_end(struct a2_io* io_p);

#endif