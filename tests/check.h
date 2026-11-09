#pragma once

// Мінімальний набір для модульних тестів без сторонніх бібліотек.
// TEST(name) реєструє тест, CHECK(умова) фіксує падіння, TEST_MAIN() запускає всі.

#include <iostream>
#include <vector>

namespace check {

struct TestCase {
    const char* name;
    void (*fn)();
};

inline std::vector<TestCase>& registry() {
    static std::vector<TestCase> tests;
    return tests;
}

inline int& failures() {
    static int count = 0;
    return count;
}

struct Registrar {
    Registrar(const char* name, void (*fn)()) { registry().push_back({name, fn}); }
};

inline int run_all() {
    int failed_tests = 0;
    for (const auto& test : registry()) {
        const int before = failures();
        test.fn();
        const bool ok = failures() == before;
        std::cout << (ok ? "[PASS] " : "[FAIL] ") << test.name << "\n";
        if (!ok) {
            ++failed_tests;
        }
    }
    std::cout << registry().size() - failed_tests << " passed, " << failed_tests << " failed\n";
    return failed_tests == 0 ? 0 : 1;
}

}  // namespace check

#define CHECK(cond)                                                                    \
    do {                                                                               \
        if (!(cond)) {                                                                 \
            ++check::failures();                                                       \
            std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond "\n"; \
        }                                                                              \
    } while (0)

#define TEST(name)                                                  \
    static void name();                                             \
    static const check::Registrar name##_registrar(#name, name);   \
    static void name()

#define TEST_MAIN() \
    int main() { return check::run_all(); }
