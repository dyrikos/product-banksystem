#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace bank {

// Гроші — ціле число центів: 0.10 + 0.20 рівно 0.30 (ADR-0002).
using Money = std::int64_t;

// Найбільша сума, яку приймаємо: 999 999 999.99 — запас від переповнення int64.
constexpr Money kMaxAmount = 99'999'999'999;

// «12», «12.5», «12.50» → центи. Порожньо, «abc», «100abc», «10.999», «-5» → nullopt.
std::optional<Money> parse_money(const std::string& text);

// 123456 → «1234.56»
std::string format_money(Money amount);

}  // namespace bank
