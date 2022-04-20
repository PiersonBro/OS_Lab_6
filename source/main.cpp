
#include <iostream>

void bufferoverrun() {
    int *p = new int[10];
    p[-1] = 10; //Buffer overrun to the negative side
    p[10] = 10; //Buffer overrun to the positive side
}

void memoryleak() {
    int *p = new int[100];

    //No call to delete, so this pointer is a memory leak
}

void invaliddelete_1() {
    int *p;
    delete p;
}

void invaliddelete_2() {
    int *p = new int;
    delete p;

    //invalid delete
    delete p;
}

int main() {
    bufferoverrun();
    memoryleak();
    invaliddelete_1();
    invaliddelete_2();
}

// optional_ops.cpp

void * operator new(std::size_t n) throw(std::bad_alloc)
{
  //...
}
void operator delete(void * p) throw()
{
  //...
}
