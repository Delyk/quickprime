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

static void wait_threads(std::vector<std::thread> &threads) {
  for (auto &t : threads) {
    if (t.joinable()) {
      t.join();
    }
  }
  threads.clear();
}

void prime::sieve_linear_print(uint_fast64_t n) {
  std::vector<uint_fast64_t> pr = sieve_linear_segmented(n);

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

    cut_non_primes(lp, pr, n, i);
  }
  return pr;
}

std::vector<uint_fast64_t> prime::sieve_linear_segmented(uint_fast64_t n) {

  uint_fast64_t lim = std::sqrt(n);
  std::vector<uint_fast64_t> pr;
  std::vector<uint_fast64_t> lp(lim + 1, 0);
  pr = sieve_linear(lim);

  uint_fast64_t low = lim;
  uint_fast64_t high = 2 * lim;
  if (high > n) {
    high = n;
  }

  while (low <= n) {
    for (auto i : pr) {

      uint_fast64_t start = (low + i);
      if (start < low) {
        start = i * i;
      }
      if (start < low) {
        start = low;
      }

      std::size_t ja = start;
      std::size_t jb = start - low;
      if (!lp[jb]) {
        lp[jb] = ja;
        pr.push_back(ja);
      }

      for (auto p = pr.begin(); *p <= lp[jb] && *p * jb <= n; p++) {
        lp.at((*p * jb) - lim) = *p;
      }

      low += lim;
      high += lim;
      if (high > n) {
        high = n;
      }
      std::fill(lp.begin(), lp.end(), 0);
    }
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
