# Діаграма класів

```mermaid
classDiagram
    class Account {
        +string owner
        +string number
        +string pin
        +Money balance
    }
    class Result {
        <<enumeration>>
        Ok
        InvalidAmount
        InsufficientFunds
        SameAccount
        NotFound
    }
    class Bank {
        -vector~Account~ accounts_
        +authenticate(number, pin) Account*
        +find(number) Account*
        +deposit(account, amount) Result
        +withdraw(account, amount) Result
        +transfer(from, to_number, amount) Result
        +open_account(owner, pin) optional~string~
        +accounts() vector~Account~
    }
    class App {
        -string path_
        -Bank bank_
        +run() int
    }
    class money {
        <<module>>
        +parse_money(text) optional~Money~
        +format_money(amount) string
    }
    class storage {
        <<module>>
        +load_accounts(path, error) optional~vector~Account~~
        +save_accounts(path, accounts) bool
    }
    class InputClosed {
        <<exception>>
    }
    Bank "1" *-- "*" Account
    Bank ..> Result
    Bank ..> money
    App *-- Bank
    App ..> storage
    App ..> InputClosed : кидає й ловить
    storage ..> Account
```

`Money` — псевдонім `std::int64_t` (центи). `App` — клас у `main.cpp`, що веде діалог (модуль `cli`). `InputClosed` — виняток, яким `cli` завершує програму, коли ввід закрито (BUG-04).
