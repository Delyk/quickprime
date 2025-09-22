#include "prime.h"
#include <cstdint>
#include <iostream>
#include <vector>

static void

    void
    prime::sieve_linear(uint_fast64_t n) {
  std::vector<unsigned long long> pr;
  std::vector<unsigned long long> lp(n + 1, 0); // Минимальные простые делители
  // Для каждого числа от 2 до n
  for (std::size_t i = 2; i <= n; i++) {
    // Если lp пуст
    if (!lp[i]) {
      // Устанавливаем пустоту в i
      lp[i] = i;
      pr.push_back(i); // ПЖцомещаем индекс в массив простых чисел
    }
    // Начинаем итерацию в массиве простых чисел
    // пока не достигнем добавленного числа или числа до
    // которого ищем простые
    for (auto p = pr.begin(); *p <= lp[i] && *p * i <= n; p++) {
      lp[*p * i] = *p; // Устанавливаем все кратные в ненулевые значения
    }
  }
  for (auto i : pr) {
    std::cout << i << std::endl;
  }
}
