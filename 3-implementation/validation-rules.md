# Правила валідації вводу

Перевірка на одному рівні — у модулях `money` і `bank`; `cli` лише показує повідомлення й просить ввести ще раз.

| Поле | Правило | Валідне | Невалідне | Що бачить користувач | Де | Тести |
|---|---|---|---|---|---|---|
| Сума | цифри, необовʼязково `.` і 1–2 цифри; > 0 | `12`, `12.5`, `0.01` | `0`, `-5`, `abc`, `100abc`, `10.999`, `12,50`, `12.` | «Invalid input. Amount must be a positive number with up to 2 decimals.» і повтор | `parse_money` | TC-01…TC-04, TC-11 |
| Сума | баланс після операції ≤ 999 999 999.99 | — | поповнення, що переповнить рахунок | «amount is too large. Operation cancelled.» | `Bank::deposit`, `transfer` | — (див. [not-tested](../4-quality/not-tested.md)) |
| PIN при вході | рівно той самий рядок, що в рахунку | `0123` | `123` для PIN `0123`, `1234x` | «Access denied. Attempts left: N» | `Bank::authenticate` | TC-07…TC-09 |
| PIN нового рахунку | рівно 4 цифри | `0042` | `123`, `12345`, `12a4` | «PIN must be 4 digits» | `valid_pin` | TC-21 |
| Імʼя нового рахунку | 2–50 символів, без `;` і переносів | `Olena Ko` | `A`, `Ann;Bob` | «name must be 2-50 characters without ';'» | `valid_owner` | TC-22 |
| Рахунок отримувача | існує і не власний | `UA987654321` | власний номер, `UA000` | «cannot transfer funds to your own account» / «Target account not found» — до запиту суми | `cli`, `Bank::transfer` | TC-16, TC-17, TC-33 |
| Пункт меню | `1`…`5` (екран входу — `1`…`3`) | `2` | `9`, `abc`, порожньо | «Invalid option. Please choose from 1 to 5.» | `cli` | — (див. [not-tested](../4-quality/not-tested.md)) |
| Кінець вводу (EOF) | — | — | Ctrl+Z / Ctrl+D, кінець файлу | «Input closed. Goodbye!», вихід з кодом 0 | `cli` | TC-32 |
| Рядок сховища | 4 поля за [форматом](data-model.md), номер унікальний | `UA1;1111;100;Name` | `UA2;12a4;100;Name`, дубль номера | «cannot read saved data: …, line N: …», вихід з кодом 1 | `load_accounts` | TC-27…TC-29, TC-37 |
