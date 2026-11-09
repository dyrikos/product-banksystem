#include "storage.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>

namespace bank {

namespace {

const char* const kHeader = "# BankSystem accounts v1";

bool all_digits(const std::string& text) {
    if (text.empty()) {
        return false;
    }
    for (char c : text) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

bool valid_number(const std::string& number) {
    return number.size() > 2 && number.compare(0, 2, "UA") == 0 && all_digits(number.substr(2));
}

// «UA1223456789;1234;550050;Yra Kampov» → рахунок; імʼя останнє, бо в ньому можуть бути пробіли
std::optional<Account> parse_line(const std::string& line, std::string& problem) {
    std::istringstream in(line);
    std::string number, pin, cents, owner;
    if (!std::getline(in, number, ';') || !std::getline(in, pin, ';') ||
        !std::getline(in, cents, ';') || !std::getline(in, owner)) {
        problem = "expected 4 fields: number;pin;balance_in_cents;owner";
        return std::nullopt;
    }
    if (!valid_number(number)) {
        problem = "bad account number '" + number + "'";
    } else if (!valid_pin(pin)) {
        problem = "PIN must be 4 digits";
    } else if (!all_digits(cents) || cents.size() > 11 || std::stoll(cents) > kMaxAmount) {
        problem = "bad balance '" + cents + "'";
    } else if (!valid_owner(owner)) {
        problem = "bad owner name";
    } else {
        return Account{owner, number, pin, std::stoll(cents)};
    }
    return std::nullopt;
}

}  // namespace

std::optional<std::vector<Account>> load_accounts(const std::string& path, std::string& error) {
    error.clear();
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return std::nullopt;  // перший запуск
    }

    std::vector<Account> accounts;
    std::set<std::string> numbers;
    std::string line;
    for (int line_no = 1; std::getline(in, line); ++line_no) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();  // файл, відредагований у Windows
        }
        if (line.empty() || line[0] == '#') {
            continue;
        }
        std::string problem;
        auto account = parse_line(line, problem);
        if (account && !numbers.insert(account->number).second) {
            problem = "duplicate account number " + account->number;
            account.reset();
        }
        if (!account) {
            error = path + ", line " + std::to_string(line_no) + ": " + problem;
            return std::nullopt;
        }
        accounts.push_back(*account);
    }
    return accounts;
}

bool save_accounts(const std::string& path, const std::vector<Account>& accounts) {
    // Спершу пишемо поруч, потім підміняємо: збій посеред запису не зіпсує старі дані
    const std::string tmp = path + ".tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out) {
            return false;
        }
        out << kHeader << "\n";
        for (const auto& acc : accounts) {
            out << acc.number << ';' << acc.pin << ';' << acc.balance << ';' << acc.owner << "\n";
        }
        if (!out.flush()) {
            return false;
        }
    }
    std::error_code ec;
    std::filesystem::rename(tmp, path, ec);
    return !ec;
}

}  // namespace bank
