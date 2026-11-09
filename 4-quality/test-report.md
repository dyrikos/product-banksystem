# Звіт про тестування v0.2.0

| | |
|---|---|
| Обʼєкт | BankSystem v0.2.0 |
| Автор і тестувальник | Юра Кампов, спеціальність F2 «Інженерія програмного забезпечення» |
| Дата | 09.11.2026 |
| Середовище | Windows 11, GCC 16.2 (MinGW-w64 UCRT), CMake 4.4 |

## 1. Підсумок

| Набір | Тестів | Пройдено | Не пройдено |
|---|---|---|---|
| `money` | 6 | 6 | 0 |
| `bank` | 18 | 18 | 0 |
| `storage` | 7 | 7 | 0 |
| Наскрізні сценарії | 8 | 8 | 0 |
| Попередження компілятора (`-Wall -Wextra -Wpedantic`) | — | 0 | — |

```text
1: 6 passed, 0 failed
2: 18 passed, 0 failed
3: 7 passed, 0 failed
100% tests passed out of 3
```

CI (Ubuntu + Windows/MSVC) запуститься після першого push на GitHub — **ще не перевірено** (NFR-6).

## 2. Червоний перед зеленим: дефекти оригіналу

Код v0.1.0 спершу перенесено в модулі **без зміни поведінки** (`double`, PIN як `int`), потім написано тести з правильними очікуваннями. Прогін на перенесеному коді — 6 падінь, кожне — дефект оригіналу:

```text
test_money.cpp:20: CHECK failed: !parse_money("10.999")
[FAIL] rejects_more_than_two_decimals
test_money.cpp:25: CHECK failed: !parse_money("100abc")
[FAIL] rejects_trailing_garbage
test_money.cpp:32: CHECK failed: sum == *parse_money("0.30")
[FAIL] adding_cents_is_exact
2 passed, 3 failed
test_bank.cpp:37: CHECK failed: b.authenticate("UA0000000001", "123") == nullptr
[FAIL] pin_with_leading_zero_needs_all_four_digits
test_bank.cpp:43: CHECK failed: b.authenticate("UA1223456789", "1234x") == nullptr
[FAIL] pin_with_trailing_garbage_rejected
test_bank.cpp:81: CHECK failed: b.withdraw(*acc, m("0.20")) == Result::Ok
[FAIL] withdraw_after_fractional_operations
10 passed, 3 failed
```

Після правок (копійки, PIN-рядок, строгий розбір) — усі зелені.

**Перевірка на самому оригіналі.** Щоб переконатися, що дефекти не внесено при перенесенні, `legacy/12main.cpp` зібрано без змін:

- PIN `1234x` для рахунку `UA1223456789` → «Hello, Yra Kampov!» — вхід із неправильним PIN (BUG-02);
- закритий ввід посеред суми → за 3 с програма надрукувала **97 МБ** рядків «Invalid input…» і була зупинена таймаутом (BUG-04);
- поповнення `1e3` → `New balance: $6500.50`, потім `1e20` → `New balance: $100000000000000000000.00` — 6500.50 зникли (BUG-08). Тести TC-38 і TC-39 дописано після статичного ревʼю: на перенесеному коді вони вже зелені, червоний стан видно лише на оригіналі.

**Сховище — через TDD.** Тести `test_storage.cpp` написано до реалізації, проти заглушки: `1 passed, 6 failed` (пройшов лише «сховища немає» — заглушка й так нічого не знаходить). Після реалізації — 7 з 7.

## 3. Наскрізні сценарії

| Сценарій | Ключовий вивід |
|---|---|
| TC-30 вхід і поповнення | `Deposit successful! New balance: $5600.60` |
| TC-31 перезапуск | `Current balance: $5600.60` |
| TC-32 EOF | `Input closed. Goodbye!`, код 0 |
| TC-33 переказ на невідомий | `Error: Target account not found.` — суму не питає |
| TC-34 новий рахунок | `Account opened! Your account number: UA0000000004`, `Hello, Olena Ko!` |
| TC-35 3 спроби | `Access denied … Attempts left: 2 / 1 / 0`, `Too many failed attempts. Goodbye!` |
| TC-36 копійки | `Remaining balance: $0.20`, потім `$0.00` |
| TC-37 пошкоджене сховище | `Error: cannot read saved data: …, line 6: PIN must be 4 digits`, код 1, рядок у файлі лишився |

## 4. Відкриті дефекти й обмеження

Відкритих дефектів «Критична»/«Висока» немає. Обмеження — у [not-tested.md](not-tested.md) і [risks.md](../5-management/risks.md): PIN відкритим текстом (R-03), CI ще не запускався, кирилиця в консолі Windows не перевірена.

## 5. Висновок

v0.2.0 відповідає Must-вимогам FR-01…FR-06, FR-15 і NFR-1…NFR-3. Готова для демо на парі. Для v1.0.0 потрібні: зелений CI на обох ОС, хеш PIN, інтервʼю з користувачами.
