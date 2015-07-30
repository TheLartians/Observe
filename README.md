# lars::event

A c++11 event-listener system template. Usage example:

```
$ g++ -std=c++11 example.cpp 
$ ./a.out 
l1 : A clicked at 1, 0
tmp: A clicked at 1, 0
l2 : B clicked at 0, 1
   : B clicked at 0, 1
l1 : A clicked at 2, 0
   : B clicked at 0, 2
```
