#pragma once

#pragma clang assume_nonnull begin

#define EXIT_FAILURE 1
#define BUFSIZ 4096

typedef struct {
	void (*read)(const char *buf, int count);
	void (*write)(const char *buf, int count);
} FILE;

typedef struct {
	int errno;
	FILE *stdin, *stdout, *stderr;
} PCB;
extern PCB *_get_pcb();

#define errno 	(_get_pcb()->errno)
#define stdin 	(_get_pcb()->stdin)
#define stdout 	(_get_pcb()->stdout)
#define stderr 	(_get_pcb()->stderr)

#define INT_MAX 0x7fffffff

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

FILE* tmpfile(void);
void clearerr(FILE* fd);

#define EOF (-1)

char* getenv(const char* name);

_Noreturn void exit(int code);

double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
double cos(double x);
double cosh(double x);
double exp(double x);
double log(double x);
double log10(double x);
double log2(double x);
double sin(double x);
double sinh(double x);
double tan(double x);
double tanh(double x);
double frexp(double value, int *exp);

double fmod(double x, double y);
double ldexp(double x, int y);
double modf(double x, double* y);
double pow(double x, double y);

// fclose
// feof
// ferror
// fflush
// fgets
// fopen
// fputc
// fputs
// fread
// free realloc
// fscanf
// fseek
// ftell
// fwrite
// getc
// getenv
// luaopen_os
// memchr memcmp memcpy memmove memset
// putchar setvbuf strchr strcmp strcpy strerror strlen strncpy strrchr strstr strtoul  tmpfile ungetc

#pragma clang assume_nonnull end
