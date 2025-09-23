#include "prime.h"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

static std::mutex mut;
static void cut_non_primes(std::vector<uint_fast64_t> &lp, uint_fast64_t p,
                           const std::size_t &i, const uint_fast64_t &n) {
  std::lock_guard<std::mutex> m(mut);
  lp[p * i] = p;
}

void prime::sieve_linear(uint_fast64_t n) {
  std::size_t thr_count = std::thread::hardware_concurrency();
  if (!thr_count) {
    thr_count = 2;
  }
  std::vector<std::thread> threads;
  std::vector<uint_fast64_t> pr;
  std::vector<uint_fast64_t> lp(n + 1, 0);
  for (std::size_t i = 2; i <= n; i++) {
    if (!lp[i]) {
      lp[i] = i;
      pr.push_back(i);
    }
    for (auto p = pr.begin(); *p <= lp[i] && *p * i <= n; p++) {
      threads.emplace_back(cut_non_primes, std ::ref(lp), *p, std ::ref(i),
                           std ::ref(n));
      if (threads.size() >= thr_count) {
        for (auto it = threads.begin(); it != threads.end(); it++) {
          if (it->joinable()) {
            it->join();
            threads.erase(it);
            break;
          }
        }
      }
    }
    for (auto &t : threads) {
      if (t.joinable()) {
        t.join();
      }
    }
    threads.clear();
  }
  for (auto &t : threads) {
    if (t.joinable())
      t.join();
  }
  threads.clear();
  for (auto i : pr) {
    std::cout << i << std::endl;
  }
}
