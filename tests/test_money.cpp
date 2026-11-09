#include "check.h"
#include "money.h"

using bank::format_money;
using bank::parse_money;

TEST(parses_whole_and_cents) {
    CHECK(format_money(*parse_money("12")) == "12.00");
    CHECK(format_money(*parse_money("12.5")) == "12.50");
    CHECK(format_money(*parse_money("0.01")) == "0.01");  // межа: найменша сума
}

TEST(rejects_not_a_number) {
    CHECK(!parse_money("abc"));
    CHECK(!parse_money(""));
}

TEST(rejects_more_than_two_decimals) {
    // 10.999 — не сума в гривнях: копійок лише дві
    CHECK(!parse_money("10.999"));
}

TEST(rejects_trailing_garbage) {
    // «100abc» — помилка вводу, а не 100
    CHECK(!parse_money("100abc"));
}

TEST(rejects_exponent_notation) {
    // «1e3» і «1e20» оригінал приймав як 1000 і 10^20 (BUG-08)
    CHECK(!parse_money("1e3"));
    CHECK(!parse_money("1e20"));
}

TEST(adding_cents_is_exact) {
    // дано: 0.10 + 0.20 — класичне місце похибки double
    auto sum = *parse_money("0.10") + *parse_money("0.20");
    // перевірка: рівно 0.30, а не 0.30000000000000004
    CHECK(sum == *parse_money("0.30"));
}

TEST_MAIN()
