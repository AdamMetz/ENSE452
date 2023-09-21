ENSE 452 - Assignment 1 - Complex Calculator

This program performs the following complex number operations:
	- Addition
	- Subtraction
	- Multiplication
	- Division

The operands must be two complex numbers. 
For example to add: 1 + j1 and 3 + j3
Calculator input: A 1 1 3 3

There are four primary errors that are handled:
	- error code: 1: illegal command
	- error code: 2: missing arguments
	- error code: 3: extra arguments
	- error code: 4: divide by zero

The application will run until 'Q' is entered as input,
or the application is forcibly closed.

To build the application using GCC:

g++ main.cpp Complex.cpp -o ComplexCalculator.exe

Optionally with compiler flags:
g++ -Wall -Wextra -Wfloat-equal -Wconversion -Wparentheses -pedantic \
-Wunused-parameter -Wunused-variable -Wreturn-type -Wunused-function \
-Wredundant-decls -Wreturn-type -Wunused-value -Wswitch-default \
-Wuninitialized -Winit-self -Werror main.cpp Complex.cpp -o ComplexCalculator.exe

Then run ./ComplexCalculator.exe