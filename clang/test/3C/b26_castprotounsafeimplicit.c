// RUN: 3c -alltypes -addcr %s -- | FileCheck -match-full-lines -check-prefixes="CHECK_ALL","CHECK" %s
// RUN: 3c -addcr %s -- | FileCheck -match-full-lines -check-prefixes="CHECK_NOALL","CHECK" %s
// RUN: 3c -addcr %s -- | %clang -c -fcheckedc-extension -x c -o /dev/null -
// RUN: 3c -output-postfix=checked -alltypes %s
// RUN: 3c -alltypes %S/b26_castprotounsafeimplicit.checked.c -- | count 0
// RUN: rm %S/b26_castprotounsafeimplicit.checked.c
#include <stddef.h>
extern _Itype_for_any(T) void *calloc(size_t nmemb, size_t size) : itype(_Array_ptr<T>) byte_count(nmemb * size);
extern _Itype_for_any(T) void free(void *pointer : itype(_Array_ptr<T>) byte_count(0));
extern _Itype_for_any(T) void *malloc(size_t size) : itype(_Array_ptr<T>) byte_count(size);
extern _Itype_for_any(T) void *realloc(void *pointer : itype(_Array_ptr<T>) byte_count(1), size_t size) : itype(_Array_ptr<T>) byte_count(size);
extern int printf(const char * restrict format : itype(restrict _Nt_array_ptr<const char>), ...);
extern _Unchecked char *strcpy(char * restrict dest, const char * restrict src : itype(restrict _Nt_array_ptr<const char>));

int *sus(int *, int *);
	//CHECK_NOALL: int *sus(int *x, _Ptr<int> y) : itype(_Ptr<int>);
	//CHECK_ALL: int *sus(int *x : itype(_Array_ptr<int>), _Ptr<int> y) : itype(_Ptr<int>);

int* foo() {
	//CHECK: _Ptr<int> foo(void) {
  int sx = 3, sy = 4; 
  int *x = &sx;
	//CHECK: int *x = &sx;
  int *y = &sy;
	//CHECK: _Ptr<int> y =  &sy;
  int *z = (int *) sus(x, y);
	//CHECK: _Ptr<int> z =  (_Ptr<int>) sus(x, y);
  *z = *z + 1;
  return z;
}

char* bar() {
	//CHECK: char* bar(void) {
  int sx = 3, sy = 4; 
  int *x = &sx;
	//CHECK: int *x = &sx;
  int *y = &sy;
	//CHECK: _Ptr<int> y =  &sy;
  char *z = (sus(x, y));
	//CHECK: char *z = (sus(x, y));
  return z;
}

int *sus(int *x, int*y) {
	//CHECK_NOALL: int *sus(int *x, _Ptr<int> y) : itype(_Ptr<int>) {
	//CHECK_ALL: int *sus(int *x : itype(_Array_ptr<int>), _Ptr<int> y) : itype(_Ptr<int>) {
  int *z = malloc(sizeof(int));
	//CHECK: _Ptr<int> z =  malloc<int>(sizeof(int));
  *z = 1;
  x++;
  *x = 2;
  return z;
}
