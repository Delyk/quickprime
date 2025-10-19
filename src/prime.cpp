#include "prime.h"
#include <atomic>
#include <bitset>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <mutex>
#include <ostream>
#include <thread>
#include <vector>

std::atomic<bool> finished = false;

void prime::primes_queque::push(uint_fast64_t val) {
  std::lock_guard<std::mutex> lock(mtx);
  q.push(val);
  v.notify_one();
}

uint_fast64_t prime::primes_queque::pop() {
  std::unique_lock<std::mutex> lock(mtx);
  v.wait(lock, [&] { return !q.empty() || finished; });
  if (q.empty() && finished) {
    return 0;
  }
  uint_fast64_t val = q.front();
  q.pop();
  return val;
}

void prime::output(primes_queque &buffer) {
  while (true) {
    uint_fast64_t val = buffer.pop();
    if (val == 0) {
      break;
    }
    std::cout << val << std::endl;
  }
}

static inline uint_fast64_t to_num(uint_fast64_t byte, uint_fast64_t offset) {
  return byte * 8 + offset;
}

prime::bitmap::boolean::boolean() {
  this->bit = 0;
  this->offset = 0;
  this->byte = 0;
}

prime::bitmap::boolean::boolean(char &byte, unsigned offset) : offset(offset) {
  this->byte = &byte;
  bit = byte & (1 << offset);
}

prime::bitmap::boolean::~boolean() {
  byte = nullptr;
  bit = 0;
  offset = 0;
}

prime::bitmap::boolean::operator bool() const { return bit; }

prime::bitmap::boolean &prime::bitmap::boolean::operator=(bool bit) {
  this->bit = bit;

  if (bit) {
    *byte |= 1 << offset;
  } else {
    *byte &= ~(1 << offset);
  }

  return *this;
}

prime::bitmap::bitmap(uint_fast64_t size, bool fill) : bits(nullptr) {
  if (size) {
    this->size = size / 8 + 1;
    init_bits(this->size, fill);
  } else {
    this->size = 0;
  }
}

prime::bitmap::boolean &prime::bitmap::operator[](uint_fast64_t index) {
  uint_fast64_t true_index = index / 8;
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

void prime::bitmap::init_bits(uint_fast64_t size, bool fill) {
  if (bits) {
    delete[] bits;
  }
  bits = new char[size];

  char num;
  if (fill) {
    num = -1;
  } else {
    num = 0;
  }

  for (uint_fast64_t i = 0; i < size; i++) {
    bits[i] = num;
  }
}

prime::bitmap::~bitmap() { delete[] bits; }

prime::primes::primes(uint_fast64_t n) : size(n), has_num(false) {
  true_size = size / 8 + 1;
  init_bits(true_size, has_num);
}

prime::primes::primes(const primes &pr)
    : size(pr.size), true_size(pr.true_size), has_num(pr.has_num) {
  bits = new char[true_size];
  for (uint_fast64_t i = 0; i < true_size; i++) {
    this->bits[i] = pr.bits[i];
  }
}

prime::primes::primes(std::initializer_list<uint_fast64_t> list,
                      uint_fast64_t n)
    : primes(n) {
  for (uint_fast64_t i : list) {
    this->push_back(i);
  }
}

prime::primes::Iterator::Iterator(prime::primes::Iterator::pointer ptr,
                                  uint_fast64_t size, uint_fast64_t have_nums)
    : byte(0), offset(0), have_num(have_nums) {
  if (have_nums) {
    m_ptr = ptr;
    this->size = size;
    ++(*this);
  } else {
    m_ptr = nullptr;
    this->size = 0;
  }
}

prime::primes::Iterator::reference prime::primes::Iterator::operator*() {
  return byte * 8 + offset;
}

prime::primes::Iterator &prime::primes::Iterator::operator++() {
  if (m_ptr) {
    while (byte < size && to_num(byte, offset) != have_num) {
      if (offset < 7) {
        offset++;
      } else {
        byte++;
        offset = 0;
      }
      if (m_ptr[byte] & (1 << offset)) {
        return *this;
      }
    }
    byte = 0;
    offset = 0;
  }
  return *this;
}

prime::primes::Iterator prime::primes::Iterator::operator++(int t) {
  Iterator tmp = *this;
  ++(*this);
  return tmp;
}

bool prime::primes::Iterator::operator!=(Iterator &it) const {
  return to_num(byte, offset) != 0;
}

void prime::primes::push_back(uint_fast64_t n) {
  if (n > size) {
    std::cerr << "Error. Array overflow. " << n << " > " << size << std::endl;
    exit(1);
  }
  uint_fast64_t b = n / 8;
  unsigned char offset = n % 8;
  bits[b] |= (1 << offset);
  has_num = n;
  // std::cout << n << std::endl;
  // debug_print();
}

prime::primes::Iterator prime::primes::begin() const {
  return Iterator(bits, true_size, has_num);
}

prime::primes::Iterator prime::primes::end() const {
  return Iterator(nullptr, 0, false);
}

prime::primes &prime::primes::operator=(std::vector<uint_fast64_t> pr) {
  for (uint_fast64_t i : pr) {
    this->push_back(i);
  }
  return *this;
}

prime::primes &prime::primes::operator=(const primes &pr) {
  this->has_num = pr.has_num;
  if (this->size < pr.size) {
    this->size = pr.size;
    this->true_size = pr.true_size;
    delete[] bits;
    bits = new char[true_size];
  }
  for (uint_fast64_t i = 0; i < pr.true_size; i++) {
    bits[i] = pr.bits[i];
  }
  return *this;
}

prime::primes &prime::primes::operator=(primes &&pr) noexcept {
  if (this->size < pr.size) {
    this->size = pr.size;
    this->true_size = pr.true_size;
  }
  this->has_num = pr.has_num;
  for (uint_fast64_t i = 0; i < pr.true_size; i++) {
    this->bits[i] = pr.bits[i];
  }

  pr.bits = nullptr;
  pr.size = 0;
  pr.true_size = 0;
  pr.has_num = 0;
  return *this;
}

void prime::primes::print() const {
  for (uint_fast64_t i : *this) {
    std::cout << i << std::endl;
  }
}

void prime::primes::debug_print() {
  std::cout << "Array: ";
  for (size_t i = 0; i < true_size; i++) {
    std::cout << std::bitset<8>(bits[i]) << " ";
  }
  std::cout << std::endl;
}

prime::primes::~primes() {

  std::ofstream cache =
      std::ofstream(".cache.bin", std::ios::out | std::ios::binary);

  if (cache.is_open()) {
    cache.write(bits, true_size);
    cache.close();
  } else {
    std::cerr << "Cannot create cache file. Check permissions." << std::endl;
  }
}

static bool isNotDivide(uint_fast64_t &i,
                        const std::vector<uint_fast64_t> &bazis) {
  for (uint_fast64_t j : bazis) {
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
  uint_fast64_t size = 1;
  for (uint_fast64_t i : bazis) {
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

prime::primes prime::sieve_linear(uint_fast64_t n) {
  if (n < 2) {
    return primes{0};
  }
  primes pr{n};
  std::vector<uint_fast64_t> lp(n + 1, 0);

  for (std::size_t i = 2; i <= n; i++) {
    if (!lp[i]) {
      lp[i] = i;
      pr.push_back(i);
    }

    for (auto p = pr.begin(); *p <= lp[i] && *p * i <= n && p != pr.end();
         p++) {
      lp[*p * i] = *p;
    }
  }

  pr.print();
  return pr;
}

prime::primes prime::sieve_linear_skip(uint_fast64_t n) {
  if (n < 2) {
    return primes{0};
  }
  primes pr{{2}, n};
  std::vector<uint_fast64_t> lp(n / 2, 0);

  for (std::size_t i = 0; i < n / 2 - 1; i++) {
    uint_fast64_t num = 2 * i + 3;
    if (!lp[i]) {
      lp[i] = num;
      pr.push_back(num);
    }
    for (auto p = pr.begin(); *p <= lp[i] && *p * num <= n && p != pr.end();
         p++) {
      lp[(*p * num) / 2 - 1] = num;
    }
  }

  pr.print();
  return pr;
}

void prime::sieve_segmented(uint_fast64_t n) {
  if (n < 2) {
    return;
  }

  uint_fast64_t lim = std::sqrt(n);
  primes_queque buf;
  std::thread out(output, std::ref(buf));
  primes pr(n);
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
        buf.push(i);
      }
    }

    low += lim;
    high += lim;
    if (high > n) {
      high = n;
    }
    lp.reset(true);
  }
  finished.store(true);
  out.join();
  // pr.print();
}

void prime::sieve_segmented_wheel(uint_fast64_t n) {
  if (n < 2) {
    return;
  }
  uint_fast64_t lim = std::sqrt(n);
  primes pr{n};
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
        if (*it > low && lp[*it - low]) {
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
    while (*(it++) < low) {
    }

    high += lim;
    if (high > n) {
      high = n;
    }

    lp.reset(true);
  }
  pr.print();
}

void prime::sieve_atkhin(uint_fast64_t n) {
  if (n < 2) {
    return;
  }
  primes primes{{2, 3, 5}, n};
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
  primes.print();
}
void prime::sieve_segmented(uint_fast64_t n, primes_queque &buf) {
  if (n < 2) {
    return;
  }

  uint_fast64_t lim = std::sqrt(n);
  primes pr(n);
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
        buf.push(i);
      }
    }

    low += lim;
    high += lim;
    if (high > n) {
      high = n;
    }
    lp.reset(true);
  }
  // pr.print();
}
