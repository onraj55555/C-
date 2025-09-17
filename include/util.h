#ifndef UTIL_H_
#define UTIL_H_

#define debug(fmt, ...) do { if(DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while(0)

void terminate(const char * fmt, ...);
#endif