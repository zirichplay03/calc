-- Создание таблицы пользователей
CREATE TABLE IF NOT EXISTS users (
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     username TEXT NOT NULL UNIQUE,
                                     password TEXT NOT NULL,
                                     balance REAL DEFAULT 0.0
);

-- Создание таблицы журнала действий
CREATE TABLE IF NOT EXISTS logs (
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    username TEXT NOT NULL,
                                    action TEXT NOT NULL,
                                    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Пример добавления пользователей
INSERT OR IGNORE INTO users (username, password, balance) VALUES ('user1', 'password1', 100.0);
INSERT OR IGNORE INTO users (username, password, balance) VALUES ('user2', 'password2', 50.0);