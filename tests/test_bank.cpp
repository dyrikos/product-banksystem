#include "bank.h"
#include "check.h"

using namespace bank;

namespace {

Bank demo() {
    return Bank({
        make_account("Yra Kampov", "UA1223456789", "1234", "5500.50"),
        make_account("Polina Katruch", "UA987654321", "5678", "1200.00"),
        make_account("Pin Zero", "UA0000000001", "0123", "10.00"),
    });
}

Money m(const char* text) { return *parse_money(text); }

}  // namespace

// --- вхід ---

TEST(authenticate_with_correct_pin) {
    Bank b = demo();
    CHECK(b.authenticate("UA1223456789", "1234") != nullptr);
}

TEST(authenticate_rejects_wrong_pin_and_unknown_number) {
    Bank b = demo();
    CHECK(b.authenticate("UA1223456789", "9999") == nullptr);
    CHECK(b.authenticate("UA0000000000", "1234") == nullptr);
}

TEST(pin_with_leading_zero_needs_all_four_digits) {
    // PIN «0123»: «123» — інший PIN, вхід не має пускати
    Bank b = demo();
    CHECK(b.authenticate("UA0000000001", "0123") != nullptr);
    CHECK(b.authenticate("UA0000000001", "123") == nullptr);
}

TEST(pin_with_trailing_garbage_rejected) {
    // «1234x» — не PIN 1234
    Bank b = demo();
    CHECK(b.authenticate("UA1223456789", "1234x") == nullptr);
}

// --- поповнення й зняття ---

TEST(deposit_adds_to_balance) {
    Bank b = demo();
    Account* acc = b.find("UA987654321");
    CHECK(b.deposit(*acc, m("100.10")) == Result::Ok);
    CHECK(format_money(acc->balance) == "1300.10");
}

TEST(deposit_zero_is_invalid) {
    Bank b = demo();
    Account* acc = b.find("UA987654321");
    CHECK(b.deposit(*acc, m("0")) == Result::InvalidAmount);
}

TEST(withdraw_exact_balance_leaves_zero) {
    Bank b = demo();
    Account* acc = b.find("UA987654321");
    CHECK(b.withdraw(*acc, m("1200.00")) == Result::Ok);  // межа: рівно весь баланс
    CHECK(format_money(acc->balance) == "0.00");
}

TEST(withdraw_one_cent_more_than_balance_fails) {
    Bank b = demo();
    Account* acc = b.find("UA987654321");
    CHECK(b.withdraw(*acc, m("1200.01")) == Result::InsufficientFunds);
    CHECK(format_money(acc->balance) == "1200.00");
}

TEST(withdraw_after_fractional_operations) {
    // дано: на рахунку 0.30; знімаємо 0.10, потім 0.20 — баланс рівно покриває суму
    Bank b({make_account("Test", "UA1", "1111", "0.30")});
    Account* acc = b.find("UA1");
    CHECK(b.withdraw(*acc, m("0.10")) == Result::Ok);
    // перевірка: банк не відмовляє «недостатньо коштів», коли на екрані 0.20 і знімаємо 0.20
    CHECK(b.withdraw(*acc, m("0.20")) == Result::Ok);
}

// --- переказ ---

TEST(transfer_moves_money) {
    Bank b = demo();
    Account* from = b.find("UA1223456789");
    CHECK(b.transfer(*from, "UA987654321", m("500.50")) == Result::Ok);
    CHECK(format_money(from->balance) == "5000.00");
    CHECK(format_money(b.find("UA987654321")->balance) == "1700.50");
}

TEST(transfer_to_own_account_rejected) {
    Bank b = demo();
    Account* from = b.find("UA1223456789");
    CHECK(b.transfer(*from, "UA1223456789", m("1")) == Result::SameAccount);
}

TEST(transfer_to_unknown_account_changes_nothing) {
    Bank b = demo();
    Account* from = b.find("UA1223456789");
    CHECK(b.transfer(*from, "UA0000000000", m("1")) == Result::NotFound);
    CHECK(format_money(from->balance) == "5500.50");
}

TEST(transfer_more_than_balance_rejected) {
    Bank b = demo();
    Account* from = b.find("UA987654321");
    CHECK(b.transfer(*from, "UA1223456789", m("1200.01")) == Result::InsufficientFunds);
}

// --- відкриття рахунку ---

TEST(open_account_starts_at_zero_and_can_log_in) {
    Bank b = demo();
    auto number = b.open_account("New Client", "0042");
    CHECK(number.has_value());
    Account* acc = number ? b.authenticate(*number, "0042") : nullptr;
    CHECK(acc != nullptr);
    CHECK(acc && format_money(acc->balance) == "0.00");
}

TEST(open_account_gives_unique_numbers) {
    Bank b = demo();
    auto first = b.open_account("First Client", "1111");
    auto second = b.open_account("Second Client", "2222");
    CHECK(first && second && *first != *second);
}

TEST(open_account_rejects_bad_pin) {
    Bank b = demo();
    CHECK(!b.open_account("Client", "123"));    // межа: 3 цифри
    CHECK(!b.open_account("Client", "12345"));  // межа: 5 цифр
    CHECK(!b.open_account("Client", "12a4"));
}

TEST(open_account_rejects_bad_owner) {
    Bank b = demo();
    CHECK(!b.open_account("A", "1111"));        // межа: 1 символ
    CHECK(!b.open_account("Ann;Bob", "1111"));  // роздільник сховища
}

// --- верхня межа ---

TEST(deposit_above_max_is_invalid) {
    // дано: 5500.50 на рахунку; оригінал приймав 1e20 і «губив» копійки (BUG-08)
    Bank b = demo();
    Account* acc = b.authenticate("UA1223456789", "1234");
    CHECK(acc && b.deposit(*acc, kMaxAmount) == Result::InvalidAmount);
    // перевірка: баланс не змінився
    CHECK(acc && format_money(acc->balance) == "5500.50");
}

TEST_MAIN()
