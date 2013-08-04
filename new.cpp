#include <new.h>
#include <Arduino.h>

#define INFO(x) {}
//#define INFO(x) Serial.println(x)

void * operator new(size_t size)
{
  INFO("new");
  return malloc(size);
}

void * operator new[](size_t size)
{
  INFO("new[]");
  return malloc(size);
}

void operator delete(void * ptr)
{
  INFO("del");
  free(ptr);
}

void operator delete[](void * ptr)
{
  INFO("del[]");
  free(ptr);
}

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {}; 

void __cxa_pure_virtual(void) {};

