#ifndef COMPLEX_H
#define COMPLEX_H

#include <string>
#include <iostream>

class Complex {
private:
    double real;
    double imaginary;

public:
    Complex(double r, double i);
    double getReal();
    double getImaginary();
    Complex getConjugate();
    Complex add(Complex other);
    Complex subtract(Complex other);
    Complex multiply(Complex other);
    Complex divide(Complex other);
    std::string toString();
    void print();
};

#endif