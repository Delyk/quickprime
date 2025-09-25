#include "prime.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

static std::mutex mut;
static std::size_t thr_count = std::thread::hardware_concurrency();
static void cut_non_primes(std::vector<uint_fast64_t> &lp,
                           std::vector<uint_fast64_t> &pr,
                           const uint_fast64_t &n, const std::size_t &i) {
  for (auto p = pr.begin(); *p <= lp[i] && *p * i <= n; p++) {
    std::lock_guard<std::mutex> m(mut);
    lp[*p * i] = *p;
  }
}
static void cut_classic_sieve(std::vector<uint_fast64_t> &lp,
                              const uint_fast64_t &start,
                              const uint_fast64_t &low,
                              const uint_fast64_t &high,
                              const uint_fast64_t &i) {
  for (std::size_t j = start; j <= high; j += i) {
    std::lock_guard<std::mutex> m(mut);
    lp[j - low] = j;
  }
}

static void wait_threads(std::vector<std::thread> &threads) {
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
  threads.clear();
}

void prime::sieve_linear_print(uint_fast64_t n) {
  std::vector<uint_fast64_t> pr = sieve_linear_skip(n);

  for (auto i : pr) {
    std::cout << i << std::endl;
  }
}

std::vector<uint_fast64_t> prime::sieve_linear(uint_fast64_t n) {
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
  return pr;
}

std::vector<uint_fast64_t> prime::sieve_linear_skip(uint_fast64_t n) {
  std::vector<uint_fast64_t> pr{2};
  std::vector<uint_fast64_t> lp(n / 2 + 1, 0);

  for (std::size_t i = 0; i <= n / 2; i++) {
    uint_fast64_t num = 2 * i + 3;
    if (!lp[i]) {
      lp[i] = num;
      pr.push_back(num);
    }

    for (auto p = pr.begin() + 1; *p <= lp[i] + 3 && *p * num <= n / 2; p++) {
      uint_fast64_t pit = (std::pow(*p, 2) - 3) / 2;
      lp[*p * num] = *p;
    }
  }

  return pr;
}

std::vector<uint_fast64_t> prime::sieve_segmented_parallel(uint_fast64_t n) {

  uint_fast64_t lim = std::sqrt(n) + 1;
  std::vector<std::thread> threads;
  std::vector<uint_fast64_t> pr;
  std::vector<uint_fast64_t> lp(lim + 1, 0);
  pr = sieve_linear(lim);

  uint_fast64_t low = lim;
  uint_fast64_t high = 2 * lim - 1;
  if (high > n) {
    high = n;
  }

  while (low <= n) {
    for (auto i : pr) {

      uint_fast64_t start = (low + i - 1) / i * i;
      if (start < low) {
        start = i * i;
      }
      if (start < low) {
        start = low;
      }

      if (threads.size() >= thr_count) {
        for (auto it = threads.begin(); it != threads.end(); it++) {
          if (it->joinable()) {
            it->join();
            threads.erase(it);
            break;
          }
        }
      }
      threads.emplace_back(cut_classic_sieve, std::ref(lp), std::ref(start),
                           std::ref(low), std::ref(high), std::ref(i));
    }

    wait_threads(threads);
    for (std::size_t i = low; i <= high; i++) {
      if (!lp[i - low]) {
        pr.push_back(i);
      }
    }

    low += lim;
    high += lim;
    if (high > n) {
      high = n;
    }
    std::fill(lp.begin(), lp.end(), 0);
  }

  return pr;
}

std::vector<uint_fast64_t> prime::sieve_segmented(uint_fast64_t n) {

  uint_fast64_t lim = std::sqrt(n);
  std::vector<uint_fast64_t> pr;
  std::vector<bool> lp(lim + 1, true);
  pr = sieve_linear(lim);

  uint_fast64_t low = lim;
  uint_fast64_t high = 2 * lim - 1;
  if (high > n) {
    high = n;
  }

  while (low <= n) {
    for (auto i : pr) {

      uint_fast64_t start = (low + i - 1) / i * i;
      if (start < low) {
        start = i * i;
      }
      if (start < low) {
        start = low;
      }

      for (std::size_t j = start; j <= high; j += i) {
        lp[j - low] = false;
      }
    }

    for (std::size_t i = low; i <= high; i++) {
      if (lp[i - low]) {
        pr.push_back(i);
      }
    }

    low += lim;
    high += lim;
    if (high > n) {
      high = n;
    }
    std::fill(lp.begin(), lp.end(), true);
  }

  return pr;
}

std::vector<uint_fast64_t> prime::sieve_linear_parallel(uint_fast64_t n) {

  std::vector<std::thread> threads;
  std::vector<uint_fast64_t> pr;
  std::vector<uint_fast64_t> lp(n + 1, 0);

  for (std::size_t i = 2; i <= n; i++) {
    if (!lp[i]) {
      lp[i] = i;
      pr.push_back(i);
    }

    threads.emplace_back(cut_non_primes, std ::ref(lp), std::ref(pr),
                         std::ref(n), std ::ref(i));

    if (threads.size() >= thr_count) {
      for (auto it = threads.begin(); it != threads.end(); it++) {
        if (it->joinable()) {
          it->join();
          threads.erase(it);
          break;
        }
      }
    }
    wait_threads(threads);
  }
  return pr;
}
