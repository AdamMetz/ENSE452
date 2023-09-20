#define _CRT_SECURE_NO_WARNINGS
// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "Complex.h"

int main()
{
    std::string user_input;
    double values[4];
    char operation;

    while (true) {
        std::cout << "Type a letter to choose the desired math operation (A, S, M, D)" << std::endl;
        std::cout << "followed by two complex number pairs. Type Q to quit." << std::endl;
        getline(std::cin, user_input);

        int numberOfParses = sscanf(user_input.c_str(), "%c %lf %lf %lf %lf", &operation, &values[0], &values[1], &values[2], &values[3]);

        char operationLowercase = static_cast<char>(std::tolower(operation));

        if (operationLowercase == 'Q') break;

        if (numberOfParses == 5) {
            Complex z1 = Complex(values[0], values[1]);
            Complex z2 = Complex(values[2], values[3]);
            Complex result;

            switch (operationLowercase) {
                case 'a':
                   result = z1.add(z2);
                   break;
                case 's':
                   result = z1.subtract(z2);
                   break;
                case 'm':
                   result = z1.multiply(z2);
                   break;
                case 'd':
                   try { result = z1.divide(z2); }
                   catch (const std::runtime_error& e) {
                       std::cerr << e.what() << std::endl;
                       continue;
                   }
                   break;
                default:
                    std::cout << "error code: 1: illegal command" << std::endl;
                    continue;
            }

            result.print();

        } else if (numberOfParses < 5) {
            std::cout << "error code: 2: missing arguments" << std::endl;
        } else {
            std::cout << "error code: 3: extra arguments" << std::endl;
        }
    }
}