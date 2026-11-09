#include "storage.h"

#include <cctype>
#include <filesystem>
#include <memory>
#include <set>

#include "sqlite3.h"

namespace bank {

namespace {

// Версія схеми в PRAGMA user_version: зміна схеми = нова версія + міграція
constexpr int kSchemaVersion = 1;

const char* const kCreateTable =
    "CREATE TABLE IF NOT EXISTS accounts ("
    " number  TEXT PRIMARY KEY,"
    " pin     TEXT NOT NULL CHECK (length(pin) = 4 AND pin NOT GLOB '*[^0-9]*'),"
    " balance INTEGER NOT NULL CHECK (balance BETWEEN 0 AND 99999999999),"
    " owner   TEXT NOT NULL"
    ");";

struct DbCloser {
    void operator()(sqlite3* db) const { sqlite3_close(db); }
};
struct StmtFinalizer {
    void operator()(sqlite3_stmt* stmt) const { sqlite3_finalize(stmt); }
};
using Db = std::unique_ptr<sqlite3, DbCloser>;
using Stmt = std::unique_ptr<sqlite3_stmt, StmtFinalizer>;

Db open_db(const std::string& path, int flags) {
    sqlite3* raw = nullptr;
    const int rc = sqlite3_open_v2(path.c_str(), &raw, flags, nullptr);
    Db db(raw);
    if (rc != SQLITE_OK) {
        return nullptr;
    }
    sqlite3_busy_timeout(db.get(), 2000);  // друга копія програми чекає, а не падає
    return db;
}

Stmt prepare(sqlite3* db, const char* sql) {
    sqlite3_stmt* raw = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &raw, nullptr) != SQLITE_OK) {
        return nullptr;
    }
    return Stmt(raw);
}

std::string column_text(sqlite3_stmt* stmt, int col) {
    const unsigned char* text = sqlite3_column_text(stmt, col);
    return text ? reinterpret_cast<const char*>(text) : "";
}

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

// Рядок таблиці → рахунок. База могла бути змінена стороннім інструментом, тому перевіряємо все.
std::optional<Account> read_row(sqlite3_stmt* stmt, std::string& problem) {
    const std::string number = column_text(stmt, 0);
    const std::string pin = column_text(stmt, 1);
    const std::string owner = column_text(stmt, 3);
    if (!valid_number(number)) {
        problem = "bad account number '" + number + "'";
    } else if (!valid_pin(pin)) {
        problem = "account " + number + ": PIN must be 4 digits";
    } else if (sqlite3_column_type(stmt, 2) != SQLITE_INTEGER || sqlite3_column_int64(stmt, 2) < 0 ||
               sqlite3_column_int64(stmt, 2) > kMaxAmount) {
        problem = "account " + number + ": bad balance '" + column_text(stmt, 2) + "'";
    } else if (!valid_owner(owner)) {
        problem = "account " + number + ": bad owner name";
    } else {
        return Account{owner, number, pin, sqlite3_column_int64(stmt, 2)};
    }
    return std::nullopt;
}

}  // namespace

std::optional<std::vector<Account>> load_accounts(const std::string& path, std::string& error) {
    error.clear();
    if (!std::filesystem::exists(path)) {
        return std::nullopt;  // перший запуск; READONLY нижче теж не створить файл
    }
    Db db = open_db(path, SQLITE_OPEN_READONLY);
    if (!db) {
        error = path + ": cannot open database";
        return std::nullopt;
    }

    Stmt version = prepare(db.get(), "PRAGMA user_version;");
    if (!version || sqlite3_step(version.get()) != SQLITE_ROW) {
        error = path + ": " + sqlite3_errmsg(db.get());
        return std::nullopt;
    }
    if (sqlite3_column_int(version.get(), 0) > kSchemaVersion) {
        error = path + ": data was saved by a newer version of BankSystem";
        return std::nullopt;
    }

    Stmt select = prepare(db.get(), "SELECT number, pin, balance, owner FROM accounts ORDER BY rowid;");
    if (!select) {
        error = path + ": " + sqlite3_errmsg(db.get());  // «file is not a database», «no such table»
        return std::nullopt;
    }

    std::vector<Account> accounts;
    std::set<std::string> numbers;
    int rc = SQLITE_ROW;
    while ((rc = sqlite3_step(select.get())) == SQLITE_ROW) {
        std::string problem;
        auto account = read_row(select.get(), problem);
        if (account && !numbers.insert(account->number).second) {
            problem = "duplicate account number " + account->number;
            account.reset();
        }
        if (!account) {
            error = path + ", " + problem;
            return std::nullopt;
        }
        accounts.push_back(*account);
    }
    if (rc != SQLITE_DONE) {
        error = path + ": " + sqlite3_errmsg(db.get());
        return std::nullopt;
    }
    return accounts;
}

bool save_accounts(const std::string& path, const std::vector<Account>& accounts) {
    Db db = open_db(path, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    if (!db) {
        return false;
    }
    auto exec = [&](const char* sql) { return sqlite3_exec(db.get(), sql, nullptr, nullptr, nullptr) == SQLITE_OK; };

    // Одна транзакція: збій посеред запису відкочується, старі дані лишаються
    if (!exec("BEGIN IMMEDIATE;")) {
        return false;
    }
    bool ok = exec(kCreateTable) && exec("DELETE FROM accounts;");
    Stmt insert = ok ? prepare(db.get(), "INSERT INTO accounts (number, pin, balance, owner) VALUES (?, ?, ?, ?);")
                     : nullptr;
    ok = ok && insert;
    for (const auto& acc : accounts) {
        if (!ok) {
            break;
        }
        sqlite3_bind_text(insert.get(), 1, acc.number.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insert.get(), 2, acc.pin.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int64(insert.get(), 3, acc.balance);
        sqlite3_bind_text(insert.get(), 4, acc.owner.c_str(), -1, SQLITE_STATIC);
        ok = sqlite3_step(insert.get()) == SQLITE_DONE;  // дублікат номера → помилка PRIMARY KEY
        sqlite3_reset(insert.get());
    }
    insert.reset();
    ok = ok && exec(("PRAGMA user_version = " + std::to_string(kSchemaVersion) + ";").c_str());
    if (ok && exec("COMMIT;")) {
        return true;
    }
    exec("ROLLBACK;");
    return false;
}

}  // namespace bank
