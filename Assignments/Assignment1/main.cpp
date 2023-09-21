#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include "Complex.h"

// Function to get user input and return the operation and values
bool getUserInput(char& operation, double values[4]) {
    std::string user_input;
    fprintf(stderr, "\nType a letter to choose the desired math operation (A, S, M, D)\n");
    fprintf(stderr, "followed by two complex number pairs. Type Q to quit.\n");
    getline(std::cin, user_input);

    if (std::toupper(user_input[0]) == 'Q') { return false; }

    int numberOfParses = sscanf(user_input.c_str(), " %c %lf %lf %lf %lf %lf", &operation, &values[0], &values[1], &values[2], &values[3], &values[3]);

    if (numberOfParses != 5) {
        std::cout << (numberOfParses < 5 ? "error code: 2: missing arguments" : "error code: 3: extra arguments") << std::flush;
        // Setting operation to 'e' indicates an error for the main function to handle.
        operation = 'e';
    }

    return true;
}

// Function to perform the complex operation and handle exceptions
void performOperation(char operation, const double values[4]) {
    Complex z1 = Complex(values[0], values[1]);
    Complex z2 = Complex(values[2], values[3]);
    Complex result;

    switch (std::tolower(operation)) {
    case 'a':
        result = z1.add(z2);
        result.print();
        break;
    case 's':
        result = z1.subtract(z2);
        result.print();
        break;
    case 'm':
        result = z1.multiply(z2);
        result.print();
        break;
    case 'd':
        try {
            result = z1.divide(z2);
            result.print();
        }
        catch (const std::runtime_error& e) {
            std::cout << e.what() << std::flush;
        }
        break;
    default:
        std::cout << "error code: 1: illegal command" << std::flush;
    }
}

int main() {
    char operation;
    double values[4];

    while (getUserInput(operation, values)) {
        if (operation == 'e') continue;
        performOperation(operation, values);
    }

    return 0;
}