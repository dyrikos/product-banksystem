# Матриця трасування

Вимога → use case ([use-cases](../2-architecture/use-cases.md)) → модуль ([modules](../2-architecture/modules.md)) → тест-кейс ([test-cases](../4-quality/test-cases.md)). TC-01…TC-29, TC-38, TC-39 — автотести (`ctest`), TC-30…TC-37 — наскрізні сценарії.

| Вимога | MoSCoW | Use case | Модулі | Тест-кейси | Стан |
|---|---|---|---|---|---|
| FR-01 Вхід | Must | UC-01 | bank, cli | TC-06…TC-09, TC-30, TC-35 | ✅ |
| FR-02 Баланс | Must | UC-02 | cli | TC-30, TC-31 | ✅ |
| FR-03 Поповнення | Must | UC-02 | bank, money | TC-01, TC-10, TC-11, TC-30 | ✅ |
| FR-04 Зняття | Must | UC-02 | bank, money | TC-12…TC-14, TC-36 | ✅ |
| FR-05 Переказ | Must | UC-02 | bank, cli | TC-15…TC-18, TC-33 | ✅ |
| FR-06 Збереження | Must | UC-01, UC-02 | storage, cli | TC-23…TC-29, TC-31, TC-37 | ✅ |
| FR-07 Відкриття рахунку | Should | UC-03 | bank, cli | TC-19…TC-22, TC-34 | ✅ |
| FR-08 3 спроби | Should | UC-01 | cli | TC-35 | ✅ |
| FR-15 Некоректна сума | Must | UC-02 | money, bank, cli | TC-02…TC-04, TC-38, TC-39 | ✅ |
| FR-09…FR-12 | Should/Could | — | — | v0.3+ | ⏳ |
| NFR-1 Точні суми | — | — | money, bank | TC-03, TC-05, TC-14, TC-36 | ✅ |
| NFR-2 Надійність збереження | — | — | storage | TC-26…TC-29, TC-37 | ✅ |
| NFR-3 Стійкість вводу | — | — | cli, money | TC-02, TC-04, TC-32 | ✅ |
| NFR-4 Безпека PIN | — | — | bank | TC-08, TC-09, TC-35 | ⚠️ PIN відкритим текстом до v0.3 |
| NFR-5 Зручність | — | — | — | інтервʼю (план) | ⏳ |
| NFR-6 Переносимість | — | — | усі | CI Ubuntu + Windows | ⚠️ локально перевірено лише Windows (MinGW) |
| NFR-7 Супроводжуваність | — | — | усі | 31 автотест | ✅ |
| NFR-8 Швидкодія | — | — | bank | замір (план) | ⏳ |

**Перевірка:** кожна Must-вимога (FR-01…FR-06, FR-15) має щонайменше один автотест або наскрізний сценарій.
