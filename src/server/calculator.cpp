#include "calculator.h"
#include <sstream>
#include <iostream>

template <class T>
T add(T a, T b) {
    return a + b;
}

template <class T>
T subtract(T a, T b) {
    return a - b;
}

template <class T>
T multiply(T a, T b) {
    return a * b;
}

template <class T>
T divide(T a, T b) {
    if (b == 0) {
        std::cout << "Error: Division by zero!" << std::endl;
        return 0;
    }
    return a / b;
}

template<class T>
std::string calculate(T a, T b, char op) {
    std::ostringstream resultStream;
    switch(op) {
        case '+':
            resultStream << "Addition result: " << add(a, b);
            break;
        case '-':
            resultStream << "Subtraction result: " << subtract(a, b);
            break;
        case '*':
            resultStream << "Multiplication result: " << multiply(a, b);
            break;
        case '/':
            resultStream << "Division result: " << divide(a, b);
            break;
        default:
            resultStream << "Invalid operation!";
    }
    return resultStream.str();
}


// Явная инстанциация для double
template std::string calculate<double>(double a, double b, char op);