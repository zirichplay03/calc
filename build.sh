#!/bin/bash

set -e  # Останавливать выполнение при ошибках

# Указываем корневую директорию проекта (замени, если путь другой)
PROJECT_ROOT="$HOME/calc"
BUILD_DIR="$PROJECT_ROOT/build"

echo "Конфигурация проекта..."

# Удаляем старые файлы сборки (если нужно)
rm -rf "$BUILD_DIR"

# Создаём папку для сборки
mkdir -p "$BUILD_DIR"

# Переходим в папку сборки
cd "$BUILD_DIR"

# Запускаем CMake
cmake "$PROJECT_ROOT"

echo "Сборка проекта..."
cmake --build .

echo "✅ Сборка завершена!"

