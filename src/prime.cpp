#include "prime.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex mut;
static void cut_non_primes(std::vector<uint_fast64_t> &lp,
                           const std::vector<uint_fast64_t> &pr,
                           const std::size_t &i, const uint_fast64_t &n) {
  for (auto p = pr.begin(); *p <= lp[i] && *p * i <= n; p++) {
    std::lock_guard<std::mutex> m(mut);
    lp[*p * i] = *p;
  }
}

void prime::sieve_linear(uint_fast64_t n) {
  std::size_t thr_count = std::thread::hardware_concurrency();
  std::vector<std::thread> threads(thr_count);
  std::vector<uint_fast64_t> pr;
  std::vector<uint_fast64_t> lp(n + 1, 0);
  for (std::size_t i = 2; i <= n; i++) {
    if (!lp[i]) {
      lp[i] = i;
      pr.push_back(i);
    }
    for (auto p = pr.begin(); *p <= lp[i] && *p * i <= n; p++) {
      lp[*p * i] = *p;
    }
  }
  for (auto i : pr) {
    std::cout << i << std::endl;
  }
}
