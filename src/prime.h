#pragma once

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <ostream>
#include <queue>
#include <thread>
#include <vector>
namespace prime {

class primes_queque {
  std::queue<uint_fast64_t> q;
  std::mutex mtx;
  std::condition_variable v;

public:
  void push(uint_fast64_t val);
  uint_fast64_t pop();
  void end();
};

void output(primes_queque &buffer);

class bitmap {

  uint_fast64_t size;

protected:
  class boolean {
    char *byte;
    int bit;
    unsigned offset;

  public:
    boolean(char &byte, unsigned offset);
    boolean();
    operator bool() const;
    boolean &operator=(bool bit);
    ~boolean();
  };

  boolean bit;
  char *bits;
  void init_bits(uint_fast64_t size, bool fill);

public:
  bitmap(uint_fast64_t size = 0, bool fill = true);
  ~bitmap();
  boolean &operator[](uint_fast64_t index);
  void reset(bool num = true);
};

class primes : private bitmap {
  boolean &operator[](uint_fast64_t index) = delete;
  uint_fast64_t size;
  unsigned true_size;
  uint_fast64_t has_num;
  bool write_cache = true;

  class Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = char &;
    using pointer = char *;
    using reference = uint_fast64_t;
    pointer m_ptr;
    uint_fast64_t size;
    uint_fast64_t byte;
    uint_fast64_t offset;
    uint_fast64_t have_num;

  public:
    Iterator(pointer, uint_fast64_t, uint_fast64_t);
    reference operator*();
    bool operator!=(Iterator &) const;
    friend bool operator!=(Iterator &lhs, Iterator &&rhs) {
      return lhs.byte * 8 + lhs.offset != 0;
    }
    Iterator &operator++();
    Iterator operator++(int);
  };

  void debug_print();
  void find_last_prime();

public:
  primes(uint_fast64_t n, std::string = ".cache.bin");
  primes(const primes &);
  primes(std::initializer_list<uint_fast64_t> list, uint_fast64_t n);
  primes &operator=(primes &);
  primes &operator=(primes &&) noexcept;
  void push_back(uint_fast64_t n);
  void print() const;
  Iterator begin() const;
  Iterator end() const;
  uint_fast64_t getPrime() const;
  void no_cache();
  ~primes();
};

primes sieve_linear(uint_fast64_t n);
void sieve_linear_simple(uint_fast64_t n);
void sieve_segmented(uint_fast64_t n);
void sieve_atkhin(uint_fast64_t n);

} // namespace prime
