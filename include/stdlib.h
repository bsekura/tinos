#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#ifndef NULL
#define NULL 0
#endif

#define RAND_MAX  2147483647

typedef struct {
   int quot;
   int rem;
} div_t;

typedef struct {
   long quot;
   long rem;
} ldiv_t;

void        abort(void) __attribute__((noreturn));
int         abs(int _i);
int         atexit(void (*_func)(void));
double      atof(const char* p);
int         atoi(const char* p);
long        atol(const char* p);
long double atold(const char* p);
void*       malloc(size_t size);
void* 	    calloc(size_t size, size_t nelem);
void* 	    realloc(void* p, size_t size);
void        free(void* ptr);
void	    cfree(void* ptr);
div_t       div(int num, int denom);
void        exit(int status) __attribute__((noreturn));
void*       bsearch(const void* key, const void* base, 
                    size_t nmemb, size_t size,
                    int (*compar)(const void *, const void *));
int         exec(const char* filename, char* args);
int         execl(const char* filename, const char* argv0, ...);
int         execle(const char* filename, const char* argv0, ...);
int         execlp(const char* filename, const char* argv0, ...);
int         execlpe(const char* filename, const char* argv0, ...);
int         execv(const char* filename, const char* const* argv);
int         execve(const char* filename, const char* const* argv, 
                   const char* const* env);
int         execvp(const char* filename, const char* const* argv);
int         execvpe(const char* filename, const char* const* argv, 
                    const char* const* env);
int         rand(void);
void        srand(unsigned seed);
int         system(const char* cmdline);

char*	    initstate(unsigned _seed, char* _arg_state, int _n);
char*	    setstate(char* _arg_state);
long	    random(void);
int	    srandom(int _seed);

char*	    getpass(const char*);
int	    getlongpass(const char*, char*, int);

#ifdef __cplusplus
}
#endif

#endif /* _STDLIB_H_ */


