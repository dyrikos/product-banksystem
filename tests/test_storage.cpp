#include <cstdio>
#include <fstream>
#include <string>

#include "check.h"
#include "storage.h"

using namespace bank;

namespace {

const std::string kPath = "test_accounts.txt";

void write_raw(const std::string& content) {
    std::ofstream out(kPath, std::ios::binary);
    out << content;
}

}  // namespace

TEST(round_trip_keeps_every_field) {
    std::remove(kPath.c_str());
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
    std::remove(kPath.c_str());
    std::string error = "old";
    CHECK(!load_accounts(kPath, error).has_value());
    CHECK(error.empty());
}

TEST(storage_without_accounts_is_valid) {
    // межа: сховище є, рахунків нуль
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

TEST(corrupted_balance_is_reported_with_line) {
    write_raw("# BankSystem accounts v1\nUA1;1111;100;Ok Name\nUA2;2222;abc;Bad Balance\n");
    std::string error;
    CHECK(!load_accounts(kPath, error).has_value());
    CHECK(error.find("line 3") != std::string::npos);
}

TEST(corrupted_pin_is_reported) {
    write_raw("# BankSystem accounts v1\nUA1;12a4;100;Name\n");
    std::string error;
    CHECK(!load_accounts(kPath, error).has_value());
    CHECK(!error.empty());
}

TEST(duplicate_account_number_is_reported) {
    write_raw("# BankSystem accounts v1\nUA1;1111;100;One\nUA1;2222;200;Two\n");
    std::string error;
    CHECK(!load_accounts(kPath, error).has_value());
    CHECK(!error.empty());
}

TEST_MAIN()
