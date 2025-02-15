#include "calculator.h"
#include <cstring>

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
        cout << "Error: Division by zero!" << endl;
        return 0;
    }
    return a / b;
}

template<class T>
void calculate(T a, T b, char op) {
    switch(op) {
        case '+':
            cout << "Addition result: " << add(a, b) << endl;
            break;
        case '-':
            cout << "Subtraction result: " << subtract(a, b) << endl;
            break;
        case '*':
            cout << "Multiplication result: " << multiply(a, b) << endl;
            break;
        case '/':
            cout << "Division result: " << divide(a, b) << endl;
            break;
        default:
            cout << "Invalid operation!" << endl;
    }
}

// Реализация функции для отображения меню
template <class T>
void showMenu() {
    cout << "Welcome to the Calculator!" << endl;
    cout << "Choose an operation:" << endl;
    cout << "1. Addition (+)" << endl;
    cout << "2. Subtraction (-)" << endl;
    cout << "3. Multiplication (*)" << endl;
    cout << "4. Division (/)" << endl;
    cout << "Enter operation (+, -, *, /): ";
}

void displayMenu(char* buffer) {
    const char* menu = "Welcome to the Calculator!\nChoose an operation:\n1. Addition (+)\n2. Subtraction (-)\n3. Multiplication (*)\n4. Division (/)\nEnter operation (+, -, *, /): ";
    strcpy(buffer, menu);
}

template void calculate<double>(double a, double b, char op);

