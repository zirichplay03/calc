#include "calculator.h"

/**
 * @brief Выполняет сложение двух чисел.
 * @tparam T Тип чисел (int, double и т. д.).
 * @param a Первое число.
 * @param b Второе число.
 * @return Результат сложения.
 */
template <class T>
T add(T a, T b) {
    return a + b;
}

/**
 * @brief Выполняет вычитание двух чисел.
 * @tparam T Тип чисел (int, double и т. д.).
 * @param a Уменьшаемое число.
 * @param b Вычитаемое число.
 * @return Результат вычитания.
 */
template <class T>
T subtract(T a, T b) {
    return a - b;
}

/**
 * @brief Выполняет умножение двух чисел.
 * @tparam T Тип чисел (int, double и т. д.).
 * @param a Первый множитель.
 * @param b Второй множитель.
 * @return Результат умножения.
 */
template <class T>
T multiply(T a, T b) {
    return a * b;
}

/**
 * @brief Выполняет деление двух чисел.
 * @tparam T Тип чисел (int, double и т. д.).
 * @param a Делимое.
 * @param b Делитель.
 * @return Результат деления или 0 в случае деления на ноль.
 */
template <class T>
T divide(T a, T b) {
    if (b == 0) {
        std::cout << "Error: Division by zero!" << std::endl;
        return 0;
    }
    return a / b;
}

/**
 * @brief Выполняет математическую операцию между двумя числами.
 * @tparam T Тип чисел (int, double и т. д.).
 * @param a Первое число.
 * @param b Второе число.
 * @param op Оператор операции (+, -, *, /).
 * @return Строка с результатом вычисления или сообщение об ошибке.
 */
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

template std::string calculate<double>(double a, double b, char op);
