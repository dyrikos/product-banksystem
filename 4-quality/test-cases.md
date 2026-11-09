# Тест-кейси

TC-01…TC-29, TC-38, TC-39 — автотести (`ctest`), статус за прогоном 09.11.2026. TC-30…TC-37 — наскрізні сценарії: ввід подається в `build/bank <тимчасове сховище>` через `stdin`. Тип: **+** позитивний, **−** негативний, **М** межове значення.

## Модуль `money` (`tests/test_money.cpp`)

| ID | Вимога | Тип | Вхід | Очікувано | Тест | Статус |
|---|---|---|---|---|---|---|
| TC-01 | FR-03 | +, М | `12`, `12.5`, `0.01` | `12.00`, `12.50`, `0.01` | `parses_whole_and_cents` | ✅ |
| TC-02 | NFR-3 | − | `abc`, порожньо | відхилено | `rejects_not_a_number` | ✅ |
| TC-03 | NFR-1 | −, М | `10.999` | відхилено: копійок лише дві | `rejects_more_than_two_decimals` | ✅ (було 🔴, BUG-03) |
| TC-04 | NFR-3 | − | `100abc` | відхилено | `rejects_trailing_garbage` | ✅ (було 🔴, BUG-03) |
| TC-05 | NFR-1 | + | `0.10 + 0.20` | рівно `0.30` | `adding_cents_is_exact` | ✅ (було 🔴, BUG-01) |

## Модуль `bank` (`tests/test_bank.cpp`)

| ID | Вимога | Тип | Передумова й дія | Очікувано | Тест | Статус |
|---|---|---|---|---|---|---|
| TC-06 | FR-01 | + | `UA1223456789` + `1234` | вхід | `authenticate_with_correct_pin` | ✅ |
| TC-07 | FR-01 | − | неправильний PIN; невідомий номер | відмова | `authenticate_rejects_wrong_pin_and_unknown_number` | ✅ |
| TC-08 | FR-01 | −, М | PIN рахунку `0123`; ввести `123` | відмова | `pin_with_leading_zero_needs_all_four_digits` | ✅ (було 🔴, BUG-02) |
| TC-09 | FR-01 | − | PIN `1234`; ввести `1234x` | відмова | `pin_with_trailing_garbage_rejected` | ✅ (було 🔴, BUG-02) |
| TC-10 | FR-03 | + | 1200.00 + 100.10 | 1300.10 | `deposit_adds_to_balance` | ✅ |
| TC-11 | FR-03 | −, М | поповнити на 0 | `InvalidAmount` | `deposit_zero_is_invalid` | ✅ |
| TC-12 | FR-04 | +, М | зняти рівно 1200.00 з 1200.00 | 0.00 | `withdraw_exact_balance_leaves_zero` | ✅ |
| TC-13 | FR-04 | −, М | зняти 1200.01 з 1200.00 | `InsufficientFunds`, баланс без змін | `withdraw_one_cent_more_than_balance_fails` | ✅ |
| TC-14 | NFR-1 | + | 0.30 − 0.10 − 0.20 | обидва зняття `Ok` | `withdraw_after_fractional_operations` | ✅ (було 🔴, BUG-01) |
| TC-15 | FR-05 | + | переказ 500.50 | 5000.00 / 1700.50 | `transfer_moves_money` | ✅ |
| TC-16 | FR-05 | − | переказ на власний рахунок | `SameAccount` | `transfer_to_own_account_rejected` | ✅ |
| TC-17 | FR-05 | − | переказ на невідомий рахунок | `NotFound`, баланс без змін | `transfer_to_unknown_account_changes_nothing` | ✅ |
| TC-18 | FR-05 | −, М | переказ 1200.01 з 1200.00 | `InsufficientFunds` | `transfer_more_than_balance_rejected` | ✅ |
| TC-19 | FR-07 | + | відкрити «New Client», PIN `0042` | номер видано, вхід працює, баланс 0.00 | `open_account_starts_at_zero_and_can_log_in` | ✅ |
| TC-20 | FR-07 | + | відкрити 2 рахунки | різні номери | `open_account_gives_unique_numbers` | ✅ |
| TC-21 | FR-07 | −, М | PIN `123`, `12345`, `12a4` | відмова | `open_account_rejects_bad_pin` | ✅ |
| TC-22 | FR-07 | −, М | імʼя `A`, `Ann;Bob` | відмова | `open_account_rejects_bad_owner` | ✅ |

## Модуль `storage` (`tests/test_storage.cpp`)

| ID | Вимога | Тип | Дія | Очікувано | Тест | Статус |
|---|---|---|---|---|---|---|
| TC-23 | FR-06 | + | зберегти й прочитати 2 рахунки (апостроф в імені, PIN `0123`, 0.01) | усі поля однакові | `round_trip_keeps_every_field` | ✅ (TDD, було 🔴) |
| TC-24 | FR-06 | + | прочитати, коли сховища немає | «перший запуск», без помилки | `missing_storage_means_first_run` | ✅ |
| TC-25 | FR-06 | М | зберегти 0 рахунків | прочитано порожній список | `storage_without_accounts_is_valid` | ✅ (TDD, було 🔴) |
| TC-26 | NFR-2 | + | зберегти двічі | лишився другий стан | `second_save_replaces_first` | ✅ (TDD, було 🔴) |
| TC-27 | NFR-2 | − | баланс `abc` у 3-му рядку | помилка з «line 3» | `corrupted_balance_is_reported_with_line` | ✅ (TDD, було 🔴) |
| TC-28 | NFR-2 | − | PIN `12a4` у сховищі | помилка | `corrupted_pin_is_reported` | ✅ (TDD, було 🔴) |
| TC-29 | NFR-2 | − | два рахунки з одним номером | помилка | `duplicate_account_number_is_reported` | ✅ (TDD, було 🔴) |

## Додано після ревʼю (BUG-08)

| ID | Вимога | Тип | Вхід / дія | Очікувано | Тест | Статус |
|---|---|---|---|---|---|---|
| TC-38 | FR-15 | − | `1e3`, `1e20` (`money`) | відхилено | `rejects_exponent_notation` | ✅ |
| TC-39 | FR-03, FR-15 | −, М | 5500.50 + `kMaxAmount` (`bank`) | `InvalidAmount`, баланс 5500.50 | `deposit_above_max_is_invalid` | ✅ |

## Наскрізні сценарії (`build/bank`)

| ID | Вимога | Ввід | Очікувано | Статус |
|---|---|---|---|---|
| TC-30 | FR-01…FR-03 | вхід `UA1223456789`/`1234`, поповнити 100.10, баланс | «Deposit successful! New balance: $5600.60», баланс 5600.60 | ✅ |
| TC-31 | FR-06 | повторний запуск з тим самим сховищем, баланс | 5600.60 — збережено | ✅ |
| TC-32 | NFR-3 | вхід, «Deposit», ввід закінчується | «Input closed. Goodbye!», код 0, без зациклення | ✅ (було 🔴, BUG-04) |
| TC-33 | FR-05 | переказ на `UA000` | «Target account not found» **без** запиту суми | ✅ |
| TC-34 | FR-07 | відкрити «Olena Ko»/`0042`, увійти | номер `UA0000000004`, «Hello, Olena Ko!», 0.00 | ✅ |
| TC-35 | FR-01, FR-08 | PIN `1234x`, `123`, `0000` | 3 × «Access denied», «Too many failed attempts» | ✅ |
| TC-36 | NFR-1 | поповнити 0.30, зняти 0.10, зняти 0.20 | обидва зняття успішні, баланс 0.00 | ✅ |
| TC-37 | NFR-2 | дописати в сховище рядок `UA1;12a4;100;Bad`, запустити | «cannot read saved data: …, line 6: PIN must be 4 digits», код 1, сховище не змінене | ✅ |

**Разом:** 39 кейсів (31 автотест + 8 наскрізних), з них 24 негативні або межові.
