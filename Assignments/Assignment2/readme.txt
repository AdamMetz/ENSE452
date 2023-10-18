Student Name: Adam Metz

Additions I made:
queue.h:
  - Function prototype for: void insert(Data d, unsigned position);
queue.cpp:
  - Added implementation for function: void insert(Data d, unsigned position);
testq.cpp:
  - Removed the old tests and replaced them with new tests for my insert function. Tests include:
    - Testing the sample queue provided within the assignment 2 document
    - Inserting a new element at the tail position
    - Inserting a new element at the head position
    - Inserting a new element at an invalid position

Code can be compiled using the makefile
  - Run the command 'make'
  - Run the command './testq'