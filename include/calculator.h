#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <iostream>
#include <string>
#include <sstream>

class Calculator {
public:
    template <typename T>
    static T add(T a, T b) { return a + b; }

    template <typename T>
    static T subtract(T a, T b) { return a - b; }

    template <typename T>
    static T multiply(T a, T b) { return a * b; }

    template <typename T>
    static T divide(T a, T b) {
        if (b == 0) {
            std::cerr << "Error: Division by zero!" << std::endl;
            return 0;
        }
        return a / b;
    }

    // Метод для выполнения операции калькулятора
    template <typename T>
    static T executeOperation(char op, T a, T b) {
        switch (op) {
            case '+': return add(a, b);
            case '-': return subtract(a, b);
            case '*': return multiply(a, b);
            case '/': return divide(a, b);
            default:
                std::cerr << "Invalid operation!" << std::endl;
                return 0;
        }
    }
};

#endif // CALCULATOR_H
