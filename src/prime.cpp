#include "prime.h"
#include <atomic>
#include <bitset>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <ostream>
#include <thread>
#include <vector>
#define CHANGE                                                                 \
  {                                                                            \
    mtx.lock();                                                                \
    nums[eq] = !nums[eq];                                                      \
    mtx.unlock();                                                              \
  }

std::atomic<bool> finished = false;
std::mutex mtx;
size_t thr_count = 0;

void prime::primes_queque::push(uint_fast64_t val) {
  std::lock_guard<std::mutex> lock(mtx);
  q.push(val);
  v.notify_one();
}

uint_fast64_t prime::primes_queque::pop() {
  std::unique_lock<std::mutex> lock(mtx);
  v.wait(lock, [&] { return !q.empty() || finished.load(); });
  if (q.empty() && finished) {
    return 0;
  }
  uint_fast64_t val = q.front();
  q.pop();
  return val;
}

void prime::primes_queque::end() {
  finished.store(true);
  v.notify_one();
}

void prime::output(primes_queque &buffer) {
  while (true) {
    uint_fast64_t val = buffer.pop();
    if (val == 0 && finished.load()) {
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

prime::primes::primes(uint_fast64_t n, std::string c)
    : size(n), has_num(false) {
  true_size = size / 8 + 1;
  init_bits(true_size, has_num);
  if (std::filesystem::exists(c)) {
    std::ifstream cache(c, std::ios::in);
    if (cache.is_open()) {
      char ch;
      uint_fast64_t i = 0;
      while (cache.get(ch)) {
        if (i >= true_size) {
          break;
        }
        bits[i] = ch;
        i++;
      }
      if (i >= true_size) {
        write_cache = false;
        find_last_prime();
      }
    }
  }
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
  if (!has_num) {
    for (uint_fast64_t i : list) {
      this->push_back(i);
    }
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
}

prime::primes::Iterator prime::primes::begin() const {
  return Iterator(bits, true_size, has_num);
}

prime::primes::Iterator prime::primes::end() const {
  return Iterator(nullptr, 0, false);
}

prime::primes &prime::primes::operator=(primes &pr) {
  this->has_num = pr.has_num;
  pr.write_cache = false;
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
  pr.write_cache = false;
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

uint_fast64_t prime::primes::getPrime() const { return this->has_num; }

void prime::primes::no_cache() { write_cache = false; }

void prime::primes::find_last_prime() {
  for (int_fast64_t i = true_size - 1; i != 0; i--) {
    for (int offset = 7; offset >= 0; offset--) {
      if (bits[i] & (1 << offset)) {
        has_num = to_num(i, offset);
        if (has_num > size) {
          has_num = 0;
          continue;
        }
        return;
      }
    }
  }
}

prime::primes::~primes() {
  if (write_cache) {
    std::ofstream cache =
        std::ofstream(".cache.bin", std::ios::out | std::ios::binary);

    if (cache.is_open()) {
      cache.write(bits, true_size);
      cache.close();
    } else {
      std::cerr << "Cannot create cache file. Check permissions." << std::endl;
    }
  }
}

void prime::sieve(bitmap &nums, uint_fast64_t &x, uint_fast64_t &n) {
  uint_fast64_t x2 = x * x;
  uint_fast64_t lim = std::sqrt(n);
  for (uint_fast64_t y = 1; y <= lim; y++) {
    uint_fast64_t y2 = y * y;
    uint_fast64_t eq = 4 * x2 + y2;
    if (eq <= n && (eq % 12 == 1 || eq % 12 == 5)) {
      CHANGE
    }

    eq = 3 * x2 + y2;
    if (eq <= n && (eq % 12 == 7)) {
      CHANGE
    }
    if (x > y) {
      eq = 3 * x2 - y2;
      if (eq <= n && (eq % 12 == 11)) {
        CHANGE
      }
    }
  }
}

void prime::final_sieve(bitmap &nums, uint_fast64_t &i, uint_fast64_t &n) {
  uint_fast64_t i2 = i * i;
  for (uint_fast64_t j = i2; j < n; j += i2) {
    mtx.lock();
    nums[j] = false;
    mtx.unlock();
  }
}

static void wait_threads(std::vector<std::thread> &threads) {
  for (size_t i = 0; i < threads.size(); i++) {
    if (threads[i].joinable()) {
      threads[i].join();
    }
  }
  threads.clear();
}

static void sieve_atkhin_tail() {
  std::vector<uint_fast64_t> v{2, 3, 5};
  for (auto i : v) {
    std::cout << i << std::endl;
  }
}

void prime::sieve_atkhin_threads(uint_fast64_t n) {
  if (n < 2) {
    return;
  }
  primes primes{{2, 3, 5}, n};
  if (primes.getPrime() && primes.getPrime() != 5) {
    primes.print();
    return;
  }
  primes_queque buf;
  std::thread out(output, std::ref(buf));
  std::vector<std::thread> threads;
  bitmap nums(n, false);
  uint_fast64_t lim = std::sqrt(n);
  if (lim >= 3)
    nums[2] = nums[3] = true;

  for (uint_fast64_t x = 1; x <= lim; x++) {
    if (threads.size() >= thr_count) {
      wait_threads(threads);
    } else {
      threads.emplace_back(sieve, std::ref(nums), std::ref(x), std::ref(n));
    }
  }

  wait_threads(threads);

  for (uint_fast64_t i = 5; i <= lim; i++) {
    if (nums[i]) {
      if (threads.size() >= thr_count) {
        wait_threads(threads);
      } else {
        threads.emplace_back(final_sieve, std::ref(nums), std::ref(i),
                             std::ref(n));
      }
    }
  }

  wait_threads(threads);

  sieve_atkhin_tail();
  for (std::size_t i = 6; i <= n; i++) {
    if (nums[i]) {
      primes.push_back(i);
      buf.push(i);
    }
  }
  buf.end();
  out.join();
}

void prime::sieve_atkhin(uint_fast64_t n) {
  if (n < 2) {
    return;
  }
  primes primes{{2, 3, 5}, n};
  if (primes.getPrime() && primes.getPrime() != 5) {
    primes.print();
    return;
  }
  primes_queque buf;
  std::thread out(output, std::ref(buf));
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

  sieve_atkhin_tail();
  for (std::size_t i = 6; i <= n; i++) {
    if (nums[i]) {
      primes.push_back(i);
      buf.push(i);
    }
  }
  buf.end();
  out.join();
}

void prime::generator(uint_fast64_t n, bool stat) {
  switch (n) {
  case 0:
  case 1:
    std::cerr << "Too low number. Minimal number: 2" << std::endl;
    return;
  case 2:
    std::cout << 2 << std::endl;
    return;
  case 3:
  case 4:
    std::cout << 2 << "\n" << 3 << std::endl;
    return;
  }
  if (n > 1000000000) {
    std::cerr << "Too much or negative number." << std::endl;
    return;
  }

  thr_count = std::thread::hardware_concurrency();
  std::chrono::duration<double, std::milli> elapsed_t;
  auto start = std::chrono::high_resolution_clock::now();
  if (thr_count > 2) {
    sieve_atkhin_threads(n);
  } else {
    sieve_atkhin(n);
  }
  auto end = std::chrono::high_resolution_clock::now();
  elapsed_t = end - start;
  if (stat) {
    std::cout << std::fixed << std::setprecision(1)
              << "Time: " << elapsed_t.count() << " ms\n";
    double size = std::filesystem::file_size(".cache.bin");
    std::string b("B");
    if (size > 1024) {
      size /= 1024;
      b = "KB";
      if (size > 1024) {
        size /= 1024;
        b = "MB";
      }
    }
    std::cout << "Cache size: " << std::setprecision(1) << size << " " << b
              << std::endl;
  }
}
