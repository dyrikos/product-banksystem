#pragma once

#include <optional>
#include <string>
#include <vector>

#include "bank.h"

namespace bank {

// Рахунки зберігаються між запусками (ADR-0003). Формат — у 3-implementation/data-model.md.

// nullopt і порожній error — сховища ще немає (перший запуск);
// nullopt і непорожній error — сховище пошкоджене, error пояснює, де саме.
std::optional<std::vector<Account>> load_accounts(const std::string& path, std::string& error);

// Записує всі рахунки. Старий вміст замінюється лише після повного запису нового.
bool save_accounts(const std::string& path, const std::vector<Account>& accounts);

}  // namespace bank
