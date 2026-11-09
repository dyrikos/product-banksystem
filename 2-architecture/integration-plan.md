# План інтеграції

Знизу вгору: кожен крок перевіряється тестами без консолі.

| Крок | Що інтегруємо | Перевірка |
|---|---|---|
| 1 | `money` | TC-01…TC-05 |
| 2 | `bank` + `money` | TC-06…TC-22 |
| 3 | `storage` + структура рахунку | TC-23…TC-29 |
| 4 | `cli` + усі модулі | наскрізні сценарії TC-30…TC-37 |
| 5 | CI: збірка й тести на Ubuntu і Windows | зелений `ci.yml` |

## Послідовність: вхід і поповнення (UC-01, UC-02)

```mermaid
sequenceDiagram
    actor C as Клієнт
    participant cli
    participant storage
    participant bank
    participant money
    cli->>storage: load_accounts(data/accounts.db)
    storage-->>cli: рахунки (або «немає» / «пошкоджене»)
    C->>cli: номер, PIN
    cli->>bank: authenticate(номер, pin)
    bank-->>cli: рахунок або nullptr
    C->>cli: «2», сума «100.10»
    cli->>money: parse_money("100.10")
    money-->>cli: 10010 копійок
    cli->>bank: deposit(рахунок, 10010)
    bank-->>cli: Ok
    cli->>storage: save_accounts(рахунки)
    storage-->>cli: true (одна транзакція SQLite)
    cli-->>C: New balance: $5600.60
```

## Послідовність: переказ (UC-02)

```mermaid
sequenceDiagram
    actor C as Клієнт
    participant cli
    participant bank
    participant money
    participant storage
    C->>cli: «4», номер отримувача
    cli->>bank: find(номер)
    alt отримувача немає або це власний рахунок
        bank-->>cli: nullptr / той самий рахунок
        cli-->>C: помилка — сума не питається (BUG-07)
    else отримувач є
        C->>cli: сума
        cli->>money: parse_money(сума)
        money-->>cli: копійки
        cli->>bank: transfer(від, номер, сума)
        bank-->>cli: Ok / InsufficientFunds / InvalidAmount
        cli->>storage: save_accounts(рахунки) — лише після Ok
        cli-->>C: результат і залишок
    end
```

## Стики й ризики

| Стик | Що може не зійтися | Як захищено |
|---|---|---|
| `cli` ↔ `money` | користувач вводить «12,50» з комою | `parse_money` відхиляє, `cli` просить ввести ще раз; кома — у беклозі як можлива зміна |
| `bank` ↔ `storage` | PIN «0123» збережеться як 123 | PIN — рядок у обох модулях; TC-23 перевіряє round-trip «0123» |
| `storage` ↔ `cli` | пошкоджене сховище перезапишеться демо-даними | `load_accounts` розрізняє «немає» і «пошкоджене»; `cli` при пошкодженні виходить з кодом 1 (TC-37) |
| `cli` ↔ `bank` | вказівник на рахунок «протух» після відкриття нового | `cli` шукає рахунок за номером на кожному кроці меню |
