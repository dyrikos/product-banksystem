#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

using namespace std;

struct BankAccount {
    string ownerName;
    string accountNumber;
    int pinCode;
    double balance;
};

void showMenu();
BankAccount* authenticate(vector<BankAccount>& accounts);
void checkBalance(const BankAccount& account);
double getPositiveAmount(const string& prompt);
void depositMoney(BankAccount& account);
void withdrawMoney(BankAccount& account);
void doWork() {}

void* ptr;

void transferMoney(BankAccount& sender, std::vector<BankAccount>& accounts);

int main() {
    vector<BankAccount> bankAccounts{
        {"Yra Kampov", "UA1223456789", 1234, 5500.50},
        {"Polina Katruch", "UA987654321", 5678, 1200.00},
        {"Ivanka Katruch", "UA1122334455", 1111, 3400.00}
    };

    cout << "=== Welcome to the Banking System ===\n";

    BankAccount* currentUser = authenticate(bankAccounts);
    if (currentUser == nullptr) {
        cout << "\nAccess denied. Invalid credentials.\n";
        return 0;
    }

    cout << "\nHello, " << currentUser->ownerName << "!\n";

    int choice = 0;
    while (true) {
        showMenu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter a valid menu number.\n";
            continue;
        }

        switch (choice) {
        case 1:
            checkBalance(*currentUser);
            break;
        case 2:
            depositMoney(*currentUser);
            break;
        case 3:
            withdrawMoney(*currentUser);
            break;
        case 4:
            transferMoney(*currentUser, bankAccounts);
            break;
        case 5:
            cout << "\nThank you for using our service. Goodbye!\n";
            return 0;
        default:
            cout << "\nInvalid option. Please choose from 1 to 5.\n";
            break;
        }
    }
}

void showMenu() {
    cout << "\n------------------------------------\n"
         << "          Main Menu\n"
         << "------------------------------------\n"
         << "1. Check balance\n"
         << "2. Deposit money\n"
         << "3. Withdraw money\n"
         << "4. Transfer money\n"
         << "5. Exit\n"
         << "------------------------------------\n"
         << "Enter your choice: ";
}

double getPositiveAmount(const string& prompt) {
    double amount = 0.0;

    while (true) {
        cout << prompt;
        if (cin >> amount && amount > 0.0) {
            return amount;
        }

        cout << "Invalid input. Amount must be a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

BankAccount* authenticate(vector<BankAccount>& accounts) {
    string accNum;
    int pin = 0;

    cout << "Enter account number: ";
    cin >> accNum;
    cout << "Enter PIN code: ";

    if (!(cin >> pin)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return nullptr;
    }

    for (auto& acc : accounts) {
        if (acc.accountNumber == accNum && acc.pinCode == pin) {
            return &acc;
        }
    }

    return nullptr;
}

void checkBalance(const BankAccount& account) {
    cout << fixed << setprecision(2);
    cout << "\n[Account: " << account.accountNumber << "]\n";
    cout << "Current balance: $" << account.balance << "\n";
}

void depositMoney(BankAccount& account) {
    double amount = getPositiveAmount("Enter amount to deposit: $");
    account.balance += amount;

    cout << fixed << setprecision(2);
    cout << "Deposit successful! New balance: $" << account.balance << '\n';
}

void withdrawMoney(BankAccount& account) {
    double amount = getPositiveAmount("Enter amount to withdraw: $");

    if (amount > account.balance) {
        cout << "Error: Insufficient funds. Operation cancelled.\n";
        return;
    }

    account.balance -= amount;

    cout << fixed << setprecision(2);
    cout << "Withdrawal successful! Remaining balance: $"
         << account.balance << "\n";
}

void transferMoney(BankAccount& sender, vector<BankAccount>& accounts) {
    string targetAcc;
    cout << "Enter target account number: ";
    cin >> targetAcc;

    if (targetAcc == sender.accountNumber) {
        cout << "Error: You cannot transfer funds to your own account.\n";
        return;
    }

    double amount = getPositiveAmount("Enter amount to transfer: $");

    if (amount > sender.balance) {
        cout << "Error: Insufficient funds for this transfer.\n";
        return;
    }

    for (auto& acc : accounts) {
        if (acc.accountNumber == targetAcc) {
            sender.balance -= amount;
            acc.balance += amount;

            cout << fixed << setprecision(2);
            cout << "Successfully transferred $" << amount
                 << " to " << acc.ownerName << ".\n";
            cout << "Your remaining balance: $" << sender.balance << "\n";
            return;
        }
    }

    cout << "Error: Target account not found.\n";
}
