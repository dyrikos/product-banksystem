// Спільний шар запуску (cli): меню, ввід і вивід. Логіка — у модулі bank, збереження — у storage.

#include <filesystem>
#include <iostream>
#include <string>

#include "bank.h"
#include "money.h"
#include "storage.h"

using bank::Account;
using bank::Bank;
using bank::Money;
using bank::Result;

namespace {

constexpr int kLoginAttempts = 3;

struct InputClosed {};  // ввід закінчився (Ctrl+Z / Ctrl+D або кінець файлу) — виходимо без зациклення

std::string read_line(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    if (!std::getline(std::cin, line)) {
        throw InputClosed{};
    }
    const auto first = line.find_first_not_of(" \t\r");
    const auto last = line.find_last_not_of(" \t\r");
    return first == std::string::npos ? "" : line.substr(first, last - first + 1);
}

Money read_positive_amount(const std::string& prompt) {
    while (true) {
        auto amount = bank::parse_money(read_line(prompt));
        if (amount && *amount > 0) {
            return *amount;
        }
        std::cout << "Invalid input. Amount must be a positive number with up to 2 decimals.\n";
    }
}

std::vector<Account> demo_accounts() {
    return {
        bank::make_account("Yra Kampov", "UA1223456789", "1234", "5500.50"),
        bank::make_account("Polina Katruch", "UA987654321", "5678", "1200.00"),
        bank::make_account("Ivanka Katruch", "UA1122334455", "1111", "3400.00"),
    };
}

class App {
public:
    explicit App(std::string path) : path_(std::move(path)), bank_({}) {}

    int run() {
        if (!load()) {
            return 1;
        }
        std::cout << "=== Welcome to the Banking System ===\n";
        try {
            std::string number;
            if (!login_screen(number)) {
                return 0;
            }
            main_menu(number);
        } catch (const InputClosed&) {
            std::cout << "\nInput closed. Goodbye!\n";
        }
        return 0;
    }

private:
    bool load() {
        std::string error;
        auto accounts = bank::load_accounts(path_, error);
        if (!accounts && !error.empty()) {
            // Не перезаписуємо пошкоджені дані — людина має їх побачити й виправити
            std::cerr << "Error: cannot read saved data: " << error << "\n";
            return false;
        }
        if (!accounts) {
            accounts = demo_accounts();
            bank_ = Bank(*accounts);
            std::filesystem::path dir = std::filesystem::path(path_).parent_path();
            if (!dir.empty()) {
                std::error_code ec;
                std::filesystem::create_directories(dir, ec);
            }
            save();
            std::cout << "No saved data found. Created " << path_ << " with demo accounts.\n";
            return true;
        }
        bank_ = Bank(*accounts);
        return true;
    }

    void save() {
        if (!bank::save_accounts(path_, bank_.accounts())) {
            std::cout << "Warning: could not save data to " << path_ << ".\n";
        }
    }

    // true — користувач увійшов, number — його рахунок; false — вихід
    bool login_screen(std::string& number) {
        while (true) {
            std::cout << "\n1. Log in\n2. Open new account\n3. Exit\n";
            const std::string choice = read_line("Enter your choice: ");
            if (choice == "1") {
                return log_in(number);
            }
            if (choice == "2") {
                open_account();
            } else if (choice == "3") {
                std::cout << "\nThank you for using our service. Goodbye!\n";
                return false;
            } else {
                std::cout << "\nInvalid option. Please choose from 1 to 3.\n";
            }
        }
    }

    bool log_in(std::string& number) {
        for (int attempt = 1; attempt <= kLoginAttempts; ++attempt) {
            number = read_line("Enter account number: ");
            const std::string pin = read_line("Enter PIN code: ");
            if (Account* user = bank_.authenticate(number, pin)) {
                std::cout << "\nHello, " << user->owner << "!\n";
                return true;
            }
            std::cout << "\nAccess denied. Invalid credentials. Attempts left: "
                      << kLoginAttempts - attempt << "\n";
        }
        std::cout << "Too many failed attempts. Goodbye!\n";
        return false;
    }

    void open_account() {
        const std::string owner = read_line("Enter your full name: ");
        const std::string pin = read_line("Choose a 4-digit PIN code: ");
        auto number = bank_.open_account(owner, pin);
        if (!number) {
            std::cout << "Error: name must be 2-50 characters without ';', PIN must be 4 digits.\n";
            return;
        }
        save();
        std::cout << "Account opened! Your account number: " << *number << "\n";
    }

    void main_menu(const std::string& number) {
        while (true) {
            std::cout << "\n------------------------------------\n"
                      << "          Main Menu\n"
                      << "------------------------------------\n"
                      << "1. Check balance\n"
                      << "2. Deposit money\n"
                      << "3. Withdraw money\n"
                      << "4. Transfer money\n"
                      << "5. Exit\n"
                      << "------------------------------------\n";
            const std::string choice = read_line("Enter your choice: ");
            Account& user = *bank_.find(number);  // шукаємо щоразу: вказівник не «протухає»
            if (choice == "1") {
                std::cout << "\n[Account: " << user.number << "]\n"
                          << "Current balance: $" << bank::format_money(user.balance) << "\n";
            } else if (choice == "2") {
                deposit(user);
            } else if (choice == "3") {
                withdraw(user);
            } else if (choice == "4") {
                transfer(user);
            } else if (choice == "5") {
                std::cout << "\nThank you for using our service. Goodbye!\n";
                return;
            } else {
                std::cout << "\nInvalid option. Please choose from 1 to 5.\n";
            }
        }
    }

    void deposit(Account& user) {
        const Money amount = read_positive_amount("Enter amount to deposit: $");
        if (bank_.deposit(user, amount) != Result::Ok) {
            std::cout << "Error: amount is too large. Operation cancelled.\n";
            return;
        }
        save();
        std::cout << "Deposit successful! New balance: $" << bank::format_money(user.balance) << "\n";
    }

    void withdraw(Account& user) {
        const Money amount = read_positive_amount("Enter amount to withdraw: $");
        if (bank_.withdraw(user, amount) != Result::Ok) {
            std::cout << "Error: Insufficient funds. Operation cancelled.\n";
            return;
        }
        save();
        std::cout << "Withdrawal successful! Remaining balance: $"
                  << bank::format_money(user.balance) << "\n";
    }

    void transfer(Account& sender) {
        const std::string target = read_line("Enter target account number: ");
        // Отримувача перевіряємо до запиту суми, щоб не питати даремно
        if (target == sender.number) {
            std::cout << "Error: You cannot transfer funds to your own account.\n";
            return;
        }
        Account* receiver = bank_.find(target);
        if (receiver == nullptr) {
            std::cout << "Error: Target account not found.\n";
            return;
        }
        const Money amount = read_positive_amount("Enter amount to transfer: $");
        switch (bank_.transfer(sender, target, amount)) {
        case Result::Ok:
            save();
            std::cout << "Successfully transferred $" << bank::format_money(amount) << " to "
                      << receiver->owner << ".\n"
                      << "Your remaining balance: $" << bank::format_money(sender.balance) << "\n";
            break;
        case Result::InsufficientFunds:
            std::cout << "Error: Insufficient funds for this transfer.\n";
            break;
        default:
            std::cout << "Error: amount is too large. Operation cancelled.\n";
            break;
        }
    }

    std::string path_;
    Bank bank_;
};

}  // namespace

int main(int argc, char* argv[]) {
    // Шлях до сховища можна передати аргументом; за замовчуванням — data/accounts.db
    return App(argc > 1 ? argv[1] : "data/accounts.db").run();
}
