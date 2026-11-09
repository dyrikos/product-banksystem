#pragma once

#include <optional>
#include <string>
#include <vector>

#include "money.h"

namespace bank {

struct Account {
    std::string owner;
    std::string number;  // «UA» + цифри
    std::string pin;     // рівно 4 цифри; рядок, щоб «0123» не ставало 123
    Money balance;       // копійки
};

enum class Result { Ok, InvalidAmount, InsufficientFunds, SameAccount, NotFound };

bool valid_pin(const std::string& pin);
// Імʼя власника: 2–50 символів, без «;» (роздільник у сховищі) і без переносів рядка.
bool valid_owner(const std::string& owner);

// Рахунок із текстових полів — так тести не залежать від того, як зберігаються PIN і гроші.
Account make_account(const std::string& owner, const std::string& number, const std::string& pin,
                     const std::string& balance);

class Bank {
public:
    explicit Bank(std::vector<Account> accounts);

    Account* authenticate(const std::string& number, const std::string& pin);
    Account* find(const std::string& number);

    Result deposit(Account& account, Money amount);
    Result withdraw(Account& account, Money amount);
    Result transfer(Account& from, const std::string& to_number, Money amount);

    // Новий рахунок із нульовим балансом; повертає його номер або nullopt, якщо імʼя чи PIN
    // некоректні. Увага: вказівники Account*, отримані раніше, після цього можуть стати недійсними.
    std::optional<std::string> open_account(const std::string& owner, const std::string& pin);

    const std::vector<Account>& accounts() const { return accounts_; }

private:
    std::vector<Account> accounts_;
};

}  // namespace bank
