-- Создание таблицы пользователей
CREATE TABLE IF NOT EXISTS users (
                                     id INTEGER PRIMARY KEY AUTOINCREMENT,
                                     username TEXT NOT NULL UNIQUE,
                                     password TEXT NOT NULL,
                                     balance REAL DEFAULT 0.0
);

-- Создание таблицы журнала
CREATE TABLE IF NOT EXISTS logs (
                                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                                    username TEXT NOT NULL,
                                    expression TEXT NOT NULL,
                                    result REAL NOT NULL,
                                    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Создание таблицы сессий
CREATE TABLE IF NOT EXISTS sessions (
                                        session_id INTEGER PRIMARY KEY AUTOINCREMENT,
                                        user_id INTEGER NOT NULL,
                                        active BOOLEAN DEFAULT TRUE,
                                        FOREIGN KEY (user_id) REFERENCES users(id)
);

-- Пример добавления пользователей (с использованием INSERT OR IGNORE)
INSERT OR IGNORE INTO users (username, password, balance) VALUES ('user1', 'password1', 100.0);
INSERT OR IGNORE INTO users (username, password, balance) VALUES ('user2', 'password2', 50.0);

-- Пример добавления записей в журнал
INSERT INTO logs (username, expression, result) VALUES ('user1', '5 + 3', 8.0);
INSERT INTO logs (username, expression, result) VALUES ('user2', '10 - 2', 8.0);

-- Пример добавления сессии
INSERT INTO sessions (user_id, active) VALUES (1, TRUE);
INSERT INTO sessions (user_id, active) VALUES (2, TRUE);