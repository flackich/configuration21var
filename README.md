# Конфигурационное управление — вариант 21

Эмулятор командной оболочки UNIX-подобной ОС на C++17.

## Возможности

Проект реализует пять этапов практической работы:

1. REPL: приглашение командной строки, разбор аргументов в кавычках,
   обработка ошибок, `exit`.
2. Конфигурация: параметры `--vfs` и `--script`, стартовый сценарий.
3. VFS: виртуальная файловая система из JSON и команда `vfs-save`.
4. Основные команды: `ls`, `cd`, `whoami`, `uptime`, `wc`.
5. Дополнительные команды: `mkdir`, `chown`.

Все операции с VFS выполняются в памяти. Изменённое состояние записывается
на диск только по команде `vfs-save`.

## Структура

- `src/` — исходный код C++.
- `tests/` — shell-тесты.
- `config/` — основная VFS и стартовый сценарий.
- `test_data/` — варианты VFS для проверки.
- `run.sh` — сборка и запуск.
- `Makefile` — сборка и тестирование.

## Сборка

На macOS:

```bash
make
```

Или:

```bash
./run.sh
```

## Запуск с параметрами

```bash
./run.sh --vfs config/vfs.json --script config/startup.txt
```

## Тесты

```bash
make test
```

## Примеры

```text
user@Mac:~$ ls
Documents/

user@Mac:~$ cd Documents
user@Mac:~/Documents$ ls
study/

user@Mac:~/Documents/study$ wc info.txt
2 5 45 info.txt

user@Mac:~$ mkdir new_folder
user@Mac:~$ chown student new_folder
user@Mac:~$ vfs-save vfs_saved.json
```

## Git

Этапы работы рекомендуется фиксировать отдельными Conventional/Scoped
Commits, например:

```text
feat(repl): implement shell REPL and parser
feat(config): add command line configuration
feat(vfs): implement virtual file system
feat(commands): implement main shell commands
feat(commands): add mkdir and chown
```
