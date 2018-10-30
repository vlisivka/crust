#ifndef CRUST_UNITTEST_H_
#define CRUST_UNITTEST_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* For assert_abort() */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>


static inline void _assert_true(bool condition, const char * expr, const char * message, const char * file, const char * func, int lineno) {
  if(!condition) {
    fprintf(stderr, "ASSERT FAILED at %s:%d, %s(): %s. Condition failed \"%s\".\n",
      file, lineno, func, message, expr);
    abort();
  }
}
#define assert_true(condition, message) _assert_true(condition, #condition, message, __FILE__, __func__, __LINE__)

static inline void _assert_equal_int(int expected, int actual, const char * actual_expr, const char * message, const char * file, const char * func, int lineno) {
  if(expected != actual) {
    fprintf(stderr, "ASSERT FAILED at %s:%d, %s(): %s. Expected value: %d, actual value of \"%s\": %d.\n",
      file, lineno, func, message, expected, actual_expr, actual);
    abort();
  }
}
#define assert_equal_int(expected, actual, message) _assert_equal_int(expected, actual, #actual, message, __FILE__, __func__, __LINE__)

static inline void _assert_equal_charp(const char * expected, const char * actual, const char * actual_expr, const char * message, const char * file, const char * func, int lineno) {
  if(strcmp(expected, actual) != 0) {
    fprintf(stderr, "ASSERT FAILED at %s:%d, %s(): %s. Expected value: \"%s\", actual value of \"%s\": \"%s\".\n",
      file, lineno, func, message, expected, actual_expr, actual);
    abort();
  }
}
#define assert_equal_charp(expected, actual, message) _assert_equal_charp(expected, actual, #actual, message, __FILE__, __func__, __LINE__)

/**
 * Generator for test function name to run at start of the program.
 */
#define it(name, message) __attribute__((constructor(200))) void test_##name()


/* assert_abort() */

void _crust_unittest_exit_on_abort();

/** Fork, execute code, and exit from child process with one or another exit code. */
#define assert_abort(code, message) \
do { \
 fflush(stdout); \
 fflush(stderr); \
 pid_t _pid = fork(); \
\
 if (_pid == -1) { \
    perror("Fork failed."); \
    abort(); \
 } else if (_pid == 0) { \
    signal(SIGABRT, &_crust_unittest_exit_on_abort); \
    fclose(stderr); /* close stderr to avoid confusing error messages */ \
    code; \
    _exit(53); \
    return; \
 } else { \
    int _child_status; \
    (void)waitpid(_pid, &_child_status, 0); \
    if(WIFEXITED(_child_status)) { \
      if(WEXITSTATUS(_child_status) == 42) { \
	  /* OK */ \
      } else if (WEXITSTATUS(_child_status) == 53) { \
        fprintf(stderr, "ASSERT FAILED at %s:%d, %s(): %s. Expected abort() in code: \"%s\", but code executed without call to abort().\n", \
        __FILE__, __LINE__, __func__, message, #code); \
      } \
    }else { \
        fprintf(stderr, "ASSERT FAILED at %s:%d, %s(): %s. Expected abort() in code: \"%s\", but exit status is different: %d.\n", \
        __FILE__, __LINE__, __func__, message, #code, _child_status); \
    } \
 } \
} while(0)



#endif /* CRUST_UNITTEST_H_ */
