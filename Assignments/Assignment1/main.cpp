// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "Complex.h"

//#define MAX_USER_INPUT_LENGTH 50

int main()
{
    std::string user_input;

    Complex z1 = Complex(1, 2);
    Complex z2 = Complex(3, 4);
    Complex z3 = z1.divide(z2);

    z3.print();

    while (user_input.empty() || user_input[0] != 'Q') {
        std::cout << "Type a letter to choose the desired math operation (A, S, M, D)" << std::endl;
        std::cout << "followed by two complex number pairs. Type Q to quit." << std::endl;
        getline(std::cin, user_input);
    }
}