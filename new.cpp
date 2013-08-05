#include "new.h"

void * operator new(size_t size)
{
  MOREINFO("new", (unsigned int) size);
  void * ret = malloc(size);
  MOREINFO("ptr", (unsigned int) ret);
  return ret;
}

void * operator new[](size_t size)
{
  MOREINFO("new[]", (unsigned int) size);
  void * ret = malloc(size);
  MOREINFO("ptr", (unsigned int) ret);
  return ret;
}

void operator delete(void * ptr)
{
  MOREINFO("del", (unsigned int) ptr);
  free(ptr);
}

void operator delete[](void * ptr)
{
  MOREINFO("del[]", (unsigned int) ptr);
  free(ptr);
}

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {}; 

void __cxa_pure_virtual(void) {};

