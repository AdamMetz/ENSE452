#include "Complex.h"

Complex::Complex(double r, double i) {
	real = r;
	imaginary = i;
}

double Complex::getReal() {return real;}
double Complex::getImaginary() { return imaginary; }
Complex Complex::getConjugate() { return Complex(real, imaginary * -1); }

Complex Complex::add(Complex other) {
	return Complex(real + other.getReal(), imaginary + other.getImaginary());
}

Complex Complex::subtract(Complex other) {
	return Complex(real - other.getReal(), imaginary - other.getImaginary());
}

Complex Complex::multiply(Complex other) {
	return Complex(
		real * other.getReal() + (imaginary * other.getImaginary()) * (-1),
		real * other.getImaginary() + other.real * imaginary
	);
}

Complex Complex::divide(Complex other) {
	Complex numerator = this->multiply(other.getConjugate());
	Complex denominator = other.multiply(other.getConjugate());
	return Complex(
		numerator.getReal() / denominator.getReal(),
		numerator.getImaginary() / denominator.getReal()
	);
}

std::string Complex::toString() {
	return 
		std::to_string(real) 
		+ (imaginary > 0 ? " + j " : " - j ") 
		+ std::to_string(imaginary);
}

void Complex::print() {
	printf("%g", real);
	std::cout << (imaginary > 0 ? " + j " : " - j ");
	printf("%g", imaginary > 0 ? imaginary : -imaginary);
	std::cout << std::endl;
}