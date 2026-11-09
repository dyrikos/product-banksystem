#pragma once

#include <optional>
#include <string>
#include <vector>

#include "bank.h"

namespace bank {

// Рахунки зберігаються між запусками в базі SQLite (ADR-0004). Схема — у 3-implementation/data-model.md.

// nullopt і порожній error — сховища ще немає (перший запуск; файл не створюється);
// nullopt і непорожній error — сховище пошкоджене, error пояснює, де саме.
std::optional<std::vector<Account>> load_accounts(const std::string& path, std::string& error);

// Записує всі рахунки однією транзакцією: або все, або нічого — старі дані лишаються.
bool save_accounts(const std::string& path, const std::vector<Account>& accounts);

}  // namespace bank
