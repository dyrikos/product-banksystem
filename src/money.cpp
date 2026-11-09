#include "money.h"

#include <cctype>

namespace bank {

std::optional<Money> parse_money(const std::string& text) {
    const auto dot = text.find('.');
    const std::string whole = text.substr(0, dot);
    const std::string cents = dot == std::string::npos ? "" : text.substr(dot + 1);

    if (whole.empty() || whole.size() > 9 || cents.size() > 2) {
        return std::nullopt;
    }
    if (dot != std::string::npos && cents.empty()) {
        return std::nullopt;  // «12.» — незакінчена сума
    }
    for (char c : whole + cents) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return std::nullopt;  // літери, знак «-», друга крапка
        }
    }

    Money amount = std::stoll(whole) * 100;
    if (!cents.empty()) {
        amount += std::stoll(cents) * (cents.size() == 1 ? 10 : 1);
    }
    return amount;
}

std::string format_money(Money amount) {
    std::string cents = std::to_string(amount % 100);
    if (cents.size() == 1) {
        cents = "0" + cents;
    }
    return std::to_string(amount / 100) + "." + cents;
}

}  // namespace bank
