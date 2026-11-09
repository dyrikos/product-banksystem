# Робота з git

## Правила

- `main` — завжди робочий стан: збирається, `ctest` зелений.
- Гілка на задачу: `feat/history`, `fix/bug-07-pin-lock`, `docs/srs`.
- Коміт: `тип: що змінено` — [CONTRIBUTING](../CONTRIBUTING.md).
- У `main` — через pull request.

## Pull request, коли автор один

```bash
git switch -c feat/history
# … код і тести …
git add src/ tests/
git commit -m "feat: історія останніх 10 операцій (FR-09)"
git push -u origin feat/history
# GitHub → Compare & pull request → шаблон «Навіщо / Як перевірити / Що не входить»
```

Ревʼюера немає, тож перед злиттям — **самоперевірка за чек-листом**:

1. PR робить те, що в описі, і тільки це?
2. Межа модуля витримана: `bank`, `money`, `storage` не читають консоль?
3. Що буде на порожньому й некоректному вводі?
4. Є тест, що падав до правки (для `fix:`)?
5. CI зелений на Ubuntu і Windows?

## Виправлення дефекту

```bash
git switch -c fix/bug-02-pin-string
git add tests/test_bank.cpp && git commit -m "test: PIN 0123 не пускає з 123 (BUG-02)"   # червоний
git add src/bank.h src/bank.cpp && git commit -m "fix: PIN порівнюється як рядок (BUG-02)"  # зелений
```

## Конфлікт злиття (для практичної 4)

Конфлікт можна отримати й самому: дві гілки від одного `main` змінюють **той самий рядок** (наприклад, у README), першу зливаємо, другу оновлюємо `git merge main` — git покаже маркери `<<<<<<<`, `=======`, `>>>>>>>`. Лишаємо потрібне, прибираємо маркери, `git add`, `git commit`.

## Перевірка

```bash
git log --graph --oneline --all
```
