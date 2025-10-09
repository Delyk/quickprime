#include "prime.h"
#include <bitset>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

void prime::sieve_linear_print(uint_fast64_t n) {
  std::vector<uint_fast64_t> pr = sieve_segmented_wheel(n);

  std::cout << "\n\n";
  for (auto i : pr) {
    std::cout << i << std::endl;
  }
}

prime::bitmap::boolean::boolean() {
  this->bit = 0;
  this->offset = 0;
  this->byte = 0;
}

prime::bitmap::boolean::boolean(char &byte, unsigned offset) : offset(offset) {
  this->byte = &byte;
  bit = byte & (1 << offset);
  // std::cout << "Byte: " << std::bitset<8>(byte) << std::endl;
  // std::cout << "Offset: " << offset << std::endl;
  // std::cout << "Bit: " << bit << std::endl;
}

prime::bitmap::boolean::operator bool() const { return bit; }

prime::bitmap::boolean &prime::bitmap::boolean::operator=(bool bit) {
  this->bit = bit;
  if (bit) {
    // std::cout << "Equation:\n " << std::bitset<8>(*byte) << " | "
    //           << std::bitset<8>(1 << offset) << " = ";
    *byte |= 1 << offset;
  } else {
    // std::cout << "Equation:\n " << std::bitset<8>(*byte) << " & "
    //           << std::bitset<8>(~(1 << offset)) << " = ";
    *byte &= ~(1 << offset);
  }
  // std::cout << std::bitset<8>(*byte) << std::endl;
  return *this;
}

prime::bitmap::bitmap(uint_fast64_t size, bool fill) {
  this->size = size / 8 + 1;
  bits = new char[this->size];

  char num;
  if (fill) {
    num = -1;
  } else {
    num = 0;
  }

  // std::cout << "Init array: ";
  for (uint_fast64_t i = 0; i < this->size; i++) {
    bits[i] = num;
    // std::cout << std::bitset<8>(bits[i]) << " ";
  }
  // std::cout << "\n";
}

prime::bitmap::boolean &prime::bitmap::operator[](uint_fast64_t index) {
  uint_fast64_t true_index = index / 8;
  // std::cout << "\nIndex: " << index << "\nTrue index: " << true_index
  // << std::endl;
  unsigned offset = index % 8;
  bit = boolean(bits[true_index], offset);
  return bit;
}

void prime::bitmap::reset(bool num) {
  char number = num ? -1 : 0;
  for (size_t i = 0; i < size; i++) {
    bits[i] = number;
  }
}

prime::bitmap::~bitmap() {
  // std::cout << "Final array: ";
  // for (uint_fast64_t i = 0; i < size; i++) {
  //   std::cout << std::bitset<8>(bits[i]) << " ";
  // }
  delete[] bits;
}

static bool isNotDivide(uint_fast64_t &i,
                        const std::vector<uint_fast64_t> &bazis) {
  for (unsigned long long j : bazis) {
    if (!(i % j)) {
      return false;
    }
  }
  return true;
}

static void startConsequence(std::vector<uint_fast64_t> &wheel,
                             const std::vector<uint_fast64_t> &bazis,
                             const uint_fast64_t &primorial) {
  for (std::size_t i = 1; i <= primorial; i++) {
    if (isNotDivide(i, bazis)) {
      wheel.push_back(i);
    }
  }
}

static inline uint_fast64_t
getStartSize(const std::vector<uint_fast64_t> &bazis) {
  unsigned long long size = 1;
  for (unsigned long long i : bazis) {
    size *= (i - 1);
  }
  return size;
}

static std::vector<uint_fast64_t> wheel_factorization(uint_fast64_t n) {
  std::vector<uint_fast64_t> wheel;
  std::vector<uint_fast64_t> bazis{2, 3, 5, 7};
  uint_fast64_t primorial = 1;
  for (auto i : bazis) {
    primorial *= i;
  }

  startConsequence(wheel, bazis, primorial);
  uint_fast64_t size = getStartSize(bazis);
  for (uint_fast64_t i = 1; i <= primorial; i++) {
    for (std::size_t j = 0; j < size; j++) {
      uint_fast64_t val = wheel[j] + i * primorial;
      if (val > n) {
        return wheel;
      }
      wheel.push_back(val);
    }
  }

  return wheel;
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
  std::vector<uint_fast64_t> lp(n / 2, 0);

  for (std::size_t i = 0; i < n / 2 - 1; i++) {
    uint_fast64_t num = 2 * i + 3;
    if (!lp[i]) {
      lp[i] = num;
      pr.push_back(num);
    }
    for (auto p = pr.begin() + 1; *p <= lp[i] && *p * num <= n && p != pr.end();
         p++) {
      lp[(*p * num) / 2 - 1] = num;
    }
  }

  return pr;
}

std::vector<uint_fast64_t> prime::sieve_segmented(uint_fast64_t n) {

  uint_fast64_t lim = std::sqrt(n);
  std::vector<uint_fast64_t> pr;
  bitmap lp(lim + 1, true);
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
    lp.reset(true);
  }

  return pr;
}

std::vector<uint_fast64_t> prime::sieve_segmented_wheel(uint_fast64_t n) {
  uint_fast64_t lim = std::sqrt(n);
  std::vector<uint_fast64_t> pr;
  bitmap lp(lim + 1, true);
  std::vector<uint_fast64_t> wheel = wheel_factorization(n);
  pr = sieve_linear_skip(lim);

  uint_fast64_t low = lim;
  uint_fast64_t high = 2 * lim - 1;
  auto it = wheel.begin();
  if (high > n) {
    high = n;
  }
  while (*it < low) {
    if (it == wheel.end()) {
      it--;
      break;
    }
    it++;
  }

  while (low <= n) {
    for (auto i : pr) {
      uint_fast64_t start = (low + i - 1) / i * i;
      if (start < low)
        start = i * i;
      if (start < low)
        start = low;

      for (std::size_t j = start; j <= high; j += i) {
        lp[j - low] = false;
      }

      while (*it < high) {
        if (it == wheel.end()) {
          break;
        }
        if (lp[*it - low]) {
          for (uint_fast64_t j = *it + *it; j <= high; j += *it) {
            lp[j - low] = false;
          }
        }
        it++;
      }
    }

    for (std::size_t i = low; i <= high; i++) {
      if (lp[i - low]) {
        pr.push_back(i);
      }
    }

    low += lim;
    if (*it < low) {
      while (*it < low) {
        if (it == wheel.end()) {
          it--;
          break;
        }
        it++;
      }
    }

    high += lim;
    if (high > n) {
      high = n;
    }

    lp.reset(true);
  }

  return pr;
}

std::vector<uint_fast64_t> prime::sieve_atkhin(uint_fast64_t n) {
  std::vector<uint_fast64_t> primes{2, 3, 5};
  if (n <= 5) {
    return primes;
  }
  bitmap nums(n, false);
  uint_fast64_t lim = std::sqrt(n);
  if (lim >= 3)
    nums[2] = nums[3] = true;

  for (uint_fast64_t x = 1; x <= lim; x++) {
    uint_fast64_t x2 = x * x;
    for (uint_fast64_t y = 1; y <= lim; y++) {
      uint_fast64_t y2 = y * y;
      uint_fast64_t eq = 4 * x2 + y2;
      if (eq <= n && (eq % 12 == 1 || eq % 12 == 5)) {
        nums[eq] = !nums[eq];
      }

      eq = 3 * x2 + y2;
      if (eq <= n && (eq % 12 == 7)) {
        nums[eq] = !nums[eq];
      }
      if (x > y) {
        eq = 3 * x2 - y2;
        if (eq <= n && (eq % 12 == 11)) {
          nums[eq] = !nums[eq];
        }
      }
    }
  }

  for (uint_fast64_t i = 5; i <= lim; i++) {
    if (nums[i]) {
      uint_fast64_t i2 = i * i;
      for (uint_fast64_t j = i2; j < n; j += i2) {
        nums[j] = false;
      }
    }
  }

  for (std::size_t i = 6; i <= n; i++) {
    if (nums[i]) {
      primes.push_back(i);
    }
  }
  return primes;
}
