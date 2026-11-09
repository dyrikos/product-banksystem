# 🏦 BankSystem — консольна банківська система

- **Що це:** навчальна консольна програма на C++, яка зберігає рахунки клієнтів і виконує банківські операції: вхід за номером рахунку й PIN, баланс, поповнення, зняття, переказ, відкриття нового рахунку.
- **Проблема й для кого:** студенти бачать банк лише ззовні; тут — маленька чесна модель рахунків, переказів і PIN, яку можна прочитати й протестувати за вечір ([vision](0-product/vision.md)).
- **Команда:** Юра Кампов, спеціальність F2 «Інженерія програмного забезпечення» — працює сам.
- **Прототип:** робоча програма, знімки ключових екранів — [prototype](2-architecture/prototype/README.md).
- **Стан:** v0.2.0 — модулі, збереження даних між запусками, база SQLite, 33 модульні тести, CI ([CHANGELOG](CHANGELOG.md)).
- **Запуск за хвилину:** `cmake -S . -B build && cmake --build build`, далі `build/bank` (Windows: `build\bank.exe`) — увійти як `UA1223456789`, PIN `1234`.
- **Важливо:** це навчальний проєкт, не для справжніх грошей — PIN зберігаються відкрито ([privacy](5-management/privacy.md)).
- **Як працювати в проєкті:** [CONTRIBUTING.md](CONTRIBUTING.md) · обслуговування — [runbook](5-management/runbook.md).

## Документація

| Тема | Документи |
|---|---|
| [0-product](0-product/) — ідея | [vision](0-product/vision.md) · [competitors](0-product/competitors.md) · [personas](0-product/personas.md) · [interviews](0-product/interviews.md) (план) |
| [1-lifecycle](1-lifecycle/) — ЖЦ і вимоги | [process](1-lifecycle/process.md) · [srs](1-lifecycle/srs.md) · [mvp](1-lifecycle/mvp.md) · [backlog](1-lifecycle/backlog.md) · [sprint-plan](1-lifecycle/sprint-plan.md) · [changes](1-lifecycle/changes.md) · [traceability](1-lifecycle/traceability.md) |
| [2-architecture](2-architecture/) — архітектура | [architecture](2-architecture/architecture.md) · [modules](2-architecture/modules.md) · [use-cases](2-architecture/use-cases.md) · [class-diagram](2-architecture/class-diagram.md) · [api](2-architecture/api.md) · [integration-plan](2-architecture/integration-plan.md) · [prototype](2-architecture/prototype/README.md) · [ADR](2-architecture/adr/) |
| [3-implementation](3-implementation/) — реалізація | [data-model](3-implementation/data-model.md) · [validation-rules](3-implementation/validation-rules.md) · [git-workflow](3-implementation/git-workflow.md) |
| [4-quality](4-quality/) — якість | [test-plan](4-quality/test-plan.md) · [test-cases](4-quality/test-cases.md) · [test-report](4-quality/test-report.md) · [bug-reports](4-quality/bug-reports.md) · [review-report](4-quality/review-report.md) · [not-tested](4-quality/not-tested.md) |
| [5-management](5-management/) — управління | [plan](5-management/plan.md) · [risks](5-management/risks.md) · [release-plan](5-management/release-plan.md) · [roadmap](5-management/roadmap.md) · [user-guide](5-management/user-guide.md) · [support](5-management/support.md) · [privacy](5-management/privacy.md) · [runbook](5-management/runbook.md) |
| [practicals](practicals/) — практичні 1–6 | [список](practicals/README.md) |
| Корінь | [CHANGELOG](CHANGELOG.md) · [CONTRIBUTING](CONTRIBUTING.md) · [LICENSE](LICENSE) · [ci.yml](.github/workflows/ci.yml) · [docs.yml](.github/workflows/docs.yml) |

## Збірка й запуск

Потрібні компілятор C++17 (GCC 9+, Clang 10+ або MSVC 2019+) і CMake 3.20+. На Windows найпростіше:

```text
winget install --id BrechtSanders.WinLibs.POSIX.UCRT -e
winget install --id Kitware.CMake -e
```

Збірка й тести:

```bash
cmake -S . -B build                 # на Windows з MinGW: додати -G "MinGW Makefiles"
cmake --build build
ctest --test-dir build --output-on-failure
```

Запуск:

```bash
build/bank                          # дані — у базі data/accounts.db
build/bank path/to/accounts.db      # інше місце для даних
```

Під час першого запуску програма створює базу SQLite `data/accounts.db` із трьома демо-рахунками:

| Номер | PIN | Власник | Баланс |
|---|---|---|---|
| `UA1223456789` | `1234` | Yra Kampov | 5500.50 |
| `UA987654321` | `5678` | Polina Katruch | 1200.00 |
| `UA1122334455` | `1111` | Ivanka Katruch | 3400.00 |

Як користуватися — [user-guide](5-management/user-guide.md).

## Структура коду

| Модуль | Файли | Що робить |
|---|---|---|
| `money` | `src/money.h/.cpp` | гроші в центах: розбір «12.34» і виведення |
| `bank` | `src/bank.h/.cpp` | вхід, поповнення, зняття, переказ, відкриття рахунку — без вводу-виводу |
| `storage` | `src/storage.h/.cpp` | збереження рахунків між запусками в базі SQLite |
| SQLite | `third_party/sqlite/` | вбудована база даних 3.50.4 — нічого не треба встановлювати ([ADR-0004](2-architecture/adr/0004-sqlite-storage.md)) |
| `cli` | `src/main.cpp` | меню й діалог з користувачем |
| тести | `tests/` | 33 модульні тести, власний `check.h` без фреймворків |
| оригінал | `legacy/12main.cpp` | перша версія в одному файлі — для порівняння, не збирається |

Контракт модулів — [modules.md](2-architecture/modules.md).

## Як викласти на GitHub (GitHub Desktop)

Історія git уже є: гілка `main` (v0.1.0 → v0.2.0 → сховище SQLite) і гілка `docs/currency-dollars` для першого pull request.

1. Розпакувати архів. Тека `BankSystem` містить приховану теку `.git` — її не видаляти.
2. GitHub Desktop → **File → Add local repository** → обрати теку `BankSystem`.
3. Вгорі **Current branch: main** → **Publish repository**. Назва — `product-banksystem`.
4. **Current branch** → `docs/currency-dollars` → **Publish branch**.
5. **Preview Pull Request → Create pull request** — відкриється GitHub.
6. На GitHub: **Files changed** → коментарі до рядків → **Review changes → Comment → Submit review**. Кнопку Approve GitHub не дає для власного PR — це нормально.
7. Дочекатися зелених перевірок CI → **Merge pull request → Create a merge commit**.
8. У GitHub Desktop: гілка `main` → **Fetch origin → Pull**.
9. **Settings → Collaborators** → додати викладача.

## Ліцензія

[MIT](LICENSE).
