# Runbook: збірка, запуск і обслуговування

Для того, хто бачить проєкт уперше: як зібрати, де лежать дані, як зробити резервну копію й що робити при збої.

## 1. Встановити інструменти

| ОС | Команди |
|---|---|
| Windows | `winget install --id BrechtSanders.WinLibs.POSIX.UCRT -e` і `winget install --id Kitware.CMake -e`, потім перезапустити термінал |
| Ubuntu | `sudo apt install g++ cmake` |
| macOS | `xcode-select --install` і `brew install cmake` |

Перевірка: `g++ --version` (потрібен C++17, GCC 9+), `cmake --version` (3.20+).

## 2. Зібрати й перевірити

```bash
cmake -S . -B build                 # Windows з MinGW: cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
ctest --test-dir build --output-on-failure    # очікується: 100% tests passed out of 3
```

На Windows `bank.exe` збирається статично — його можна скопіювати на інший компʼютер без MinGW.

## 3. Запустити

```bash
build/bank                          # сховище: data/accounts.db відносно поточної теки
build/bank D:/bank/accounts.db      # інше місце
```

`data/accounts.db` створюється **відносно теки, з якої запущено програму**. Запустили з іншої теки — побачите демо-рахунки замість своїх: вкажіть шлях до сховища явно.

## 4. Дані й резервна копія

- Сховище — база SQLite, схема — [data-model](../3-implementation/data-model.md). Переглянути: DB Browser for SQLite або `sqlite3 data/accounts.db "SELECT * FROM accounts;"`.
- **Резервна копія:** закрити програму й скопіювати файл:

```bash
cp data/accounts.db "backup/accounts-$(date +%F).db"
```

- **Відновлення:** закрити програму, скопіювати копію на місце `data/accounts.db`.
- Файл `data/accounts.db-journal` — журнал SQLite, лишається лише після збою під час збереження. **Не видаляти**: під час наступного запуску SQLite сама відкотить незавершену транзакцію.

## 5. Типові проблеми

| Що бачимо | Причина | Що робити |
|---|---|---|
| `Error: cannot read saved data: …, account UA…: …` або `…: file is not a database` | сховище пошкоджене (ручне редагування, збій диска, підмінений файл) | виправити названий рахунок у DB Browser for SQLite за [схемою](../3-implementation/data-model.md) або відновити з копії. Програма свідомо не запускається, щоб не затерти дані |
| `Warning: could not save data to …` | немає прав на запис або тека лише для читання | запустити з теки, де є право запису, або вказати інший шлях аргументом |
| Замість своїх рахунків — демо-рахунки | програму запущено з іншої теки | вказати шлях до сховища явно: `bank path/to/accounts.db` |
| На Windows `bank.exe` не запускається: «не знайдено libstdc++-6.dll» | зібрано без статичної лінковки (старий `CMakeLists.txt`) | перезібрати: `cmake --build build --clean-first` |
| `cmake` не знаходить компілятор на Windows | термінал відкрито до встановлення | перезапустити термінал; перевірити `g++ --version` |
| Імʼя кирилицею зберіглось «кракозябрами» | консоль Windows не в UTF-8 | використовувати латиницю ([not-tested](../4-quality/not-tested.md)) |
| Тести падають лише в CI на Windows | MSVC відрізняється від GCC | відкрити лог CI; виправлення — окремим PR з тестом |

## 6. Доступи

| Ресурс | Хто | Як видати / відкликати |
|---|---|---|
| Репозиторій GitHub | Юра Кампов (власник) | Settings → Collaborators |
| `data/accounts.db` | той, хто має доступ до компʼютера | файл не потрапляє в git; PIN у ньому відкритим текстом — не давати доступ стороннім |
