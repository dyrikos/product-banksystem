# SQLite

Вихідний код SQLite у вигляді одного файлу («amalgamation»), щоб збірка не залежала від того, чи встановлено SQLite в системі ([ADR-0004](../../2-architecture/adr/0004-sqlite-storage.md)).

| | |
|---|---|
| Версія | 3.50.4 (`2025-07-30`) |
| Звідки | `https://www.sqlite.org/2025/sqlite-amalgamation-3500400.zip` |
| SHA-256 архіву | `1d3049dd0f830a025a53105fc79fd2ab9431aea99e137809d064d8ee8356b032` |
| Файли | `sqlite3.c`, `sqlite3.h` — без змін |
| Ліцензія | суспільне надбання (public domain) |

Опції збірки — у `CMakeLists.txt`: `SQLITE_THREADSAFE=0` (програма однопотокова), `SQLITE_OMIT_LOAD_EXTENSION` (розширення не потрібні).

**Оновлення:** завантажити новий архів amalgamation з sqlite.org, замінити два файли, оновити цю таблицю, прогнати `ctest`.
