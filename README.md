# Simple Threadpool

A simple header-only threadpool lib implemented by C++.  
Just include it.

# How to use

To use this lib, just include it.
```c++
#include "Threadpool.h"
```

To construct a new threadpool
```c++
Threadpool tp;
```

Add new tasks in the pool
```c++
// Suppose you have a function with a int argument which returns a int
int task(int n); 
...

tp.push(task,num);

// Also, you can push a lambda expression
tp.push([num](){return num+1;})
```
