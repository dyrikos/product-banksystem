#include "bank.h"

#include <cctype>
#include <cstdio>
#include <utility>

namespace bank {

bool valid_pin(const std::string& pin) {
    if (pin.size() != 4) {
        return false;
    }
    for (char c : pin) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

bool valid_owner(const std::string& owner) {
    if (owner.size() < 2 || owner.size() > 50) {
        return false;
    }
    return owner.find_first_of(";\r\n") == std::string::npos;
}

Account make_account(const std::string& owner, const std::string& number, const std::string& pin,
                     const std::string& balance) {
    return Account{owner, number, pin, parse_money(balance).value_or(0)};
}

Bank::Bank(std::vector<Account> accounts) : accounts_(std::move(accounts)) {}

Account* Bank::authenticate(const std::string& number, const std::string& pin) {
    Account* acc = find(number);
    // PIN порівнюємо як рядок: «123» ≠ «0123», «1234x» ≠ «1234»
    return acc != nullptr && acc->pin == pin ? acc : nullptr;
}

Account* Bank::find(const std::string& number) {
    for (auto& acc : accounts_) {
        if (acc.number == number) {
            return &acc;
        }
    }
    return nullptr;
}

Result Bank::deposit(Account& account, Money amount) {
    if (amount <= 0 || account.balance + amount > kMaxAmount) {
        return Result::InvalidAmount;
    }
    account.balance += amount;
    return Result::Ok;
}

Result Bank::withdraw(Account& account, Money amount) {
    if (amount <= 0) {
        return Result::InvalidAmount;
    }
    if (amount > account.balance) {
        return Result::InsufficientFunds;
    }
    account.balance -= amount;
    return Result::Ok;
}

Result Bank::transfer(Account& from, const std::string& to_number, Money amount) {
    if (to_number == from.number) {
        return Result::SameAccount;
    }
    Account* to = find(to_number);
    if (to == nullptr) {
        return Result::NotFound;
    }
    if (amount <= 0 || to->balance + amount > kMaxAmount) {
        return Result::InvalidAmount;
    }
    if (amount > from.balance) {
        return Result::InsufficientFunds;
    }
    from.balance -= amount;
    to->balance += amount;
    return Result::Ok;
}

std::optional<std::string> Bank::open_account(const std::string& owner, const std::string& pin) {
    if (!valid_owner(owner) || !valid_pin(pin)) {
        return std::nullopt;
    }
    // Номер: «UA» + 10 цифр; беремо перший вільний, починаючи з кількості рахунків
    char number[16];
    for (unsigned long n = accounts_.size() + 1;; ++n) {
        std::snprintf(number, sizeof number, "UA%010lu", n);
        if (find(number) == nullptr) {
            break;
        }
    }
    accounts_.push_back(Account{owner, number, pin, 0});
    return std::string(number);
}

}  // namespace bank
