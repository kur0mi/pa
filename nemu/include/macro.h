#ifndef __MACRO_H__
#define __MACRO_H__

#define str_temp(x) #x
#define str(x) str_temp(x)

#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define concat3(x, y, z) concat(concat(x, y), z)
#define concat4(x, y, z, w) concat3(concat(x, y), z, w)
#define concat5(x, y, z, v, w) concat4(concat(x, y), z, v, w)

#define GreenText(cond, ...) \
  do { \
      fflush(stdout); \
      fprintf(stdout, "\x1b[1;32m"); \
      fprintf(stdout, __VA_ARGS__); \
      fprintf(stdout, "\x1b[0m\n"); \
  } while (0)

#endif
