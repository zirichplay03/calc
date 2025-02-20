#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <iostream>
#include <sstream>

template <class T>
T add(T a, T b);

template <class T>
T subtract(T a, T b);

template <class T>
T multiply(T a, T b);

template <class T>
T divide(T a, T b);

template<class T>
std::string calculate(T a, T b, char op);

void displayMenu(char* buffer);

#endif // CALCULATOR_H