#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

#include "check.h"
#include "sqlite3.h"
#include "storage.h"

using namespace bank;

namespace {

const std::string kPath = "test_accounts.db";

void remove_storage() {
    std::remove(kPath.c_str());
    std::remove((kPath + "-journal").c_str());
}

// Сховище, «зіпсоване» стороннім інструментом: таблиця без обмежень, у ній погані рядки
void write_raw_table(const char* rows_sql) {
    remove_storage();
    sqlite3* db = nullptr;
    sqlite3_open(kPath.c_str(), &db);
    const std::string sql =
        "CREATE TABLE accounts(number TEXT, pin TEXT, balance, owner TEXT);" + std::string(rows_sql);
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
    sqlite3_close(db);
}

}  // namespace

TEST(round_trip_keeps_every_field) {
    remove_storage();
    std::vector<Account> saved{
        make_account("Yra Kampov", "UA1223456789", "1234", "5500.50"),
        make_account("Marʼyana O'Neil", "UA0000000002", "0123", "0.01"),  // апостроф, PIN з нулем
    };
    CHECK(save_accounts(kPath, saved));
    std::string error;
    auto loaded = load_accounts(kPath, error);
    CHECK(loaded.has_value());
    CHECK(loaded && loaded->size() == 2);
    CHECK(loaded && (*loaded)[1].owner == "Marʼyana O'Neil");
    CHECK(loaded && (*loaded)[1].pin == "0123");
    CHECK(loaded && format_money((*loaded)[1].balance) == "0.01");
}

TEST(missing_storage_means_first_run) {
    remove_storage();
    std::string error = "old";
    CHECK(!load_accounts(kPath, error).has_value());
    CHECK(error.empty());
    CHECK(!std::filesystem::exists(kPath));  // читання не створює порожню базу
}

TEST(storage_without_accounts_is_valid) {
    // межа: сховище є, рахунків нуль
    remove_storage();
    CHECK(save_accounts(kPath, {}));
    std::string error;
    auto loaded = load_accounts(kPath, error);
    CHECK(loaded && loaded->empty());
}

TEST(second_save_replaces_first) {
    CHECK(save_accounts(kPath, {make_account("A A", "UA1", "1111", "1")}));
    CHECK(save_accounts(kPath, {make_account("B B", "UA2", "2222", "2")}));
    std::string error;
    auto loaded = load_accounts(kPath, error);
    CHECK(loaded && loaded->size() == 1 && (*loaded)[0].number == "UA2");
}

TEST(corrupted_balance_is_reported_with_account) {
    write_raw_table("INSERT INTO accounts VALUES('UA1','1111',100,'Ok Name'),('UA2','2222','abc','Bad Balance');");
    std::string error;
    CHECK(!load_accounts(kPath, error).has_value());
    CHECK(error.find("UA2") != std::string::npos);
}

TEST(corrupted_pin_is_reported) {
    write_raw_table("INSERT INTO accounts VALUES('UA1','12a4',100,'Name');");
    std::string error;
    CHECK(!load_accounts(kPath, error).has_value());
    CHECK(error.find("PIN") != std::string::npos);
}

TEST(failed_save_keeps_previous_data) {
    // дано: збережено один рахунок; новий запис містить два рахунки з одним номером
    remove_storage();
    CHECK(save_accounts(kPath, {make_account("Old Data", "UA1", "1111", "10")}));
    CHECK(!save_accounts(kPath, {make_account("One", "UA7", "1111", "1"),
                                 make_account("Two", "UA7", "2222", "2")}));
    // перевірка: транзакція відкотилась — лежать старі дані
    std::string error;
    auto loaded = load_accounts(kPath, error);
    CHECK(loaded && loaded->size() == 1 && (*loaded)[0].owner == "Old Data");
}

TEST(not_a_database_is_reported) {
    remove_storage();
    std::ofstream(kPath, std::ios::binary) << "UA1223456789;1234;550050;Yra Kampov\n";
    std::string error;
    CHECK(!load_accounts(kPath, error).has_value());
    CHECK(error.find("not a database") != std::string::npos);
}

TEST(storage_is_sqlite_database) {
    remove_storage();
    CHECK(save_accounts(kPath, {make_account("A A", "UA1", "1111", "1")}));
    std::ifstream in(kPath, std::ios::binary);
    std::string magic(15, '\0');
    in.read(&magic[0], 15);
    CHECK(magic == "SQLite format 3");
}

TEST_MAIN()
