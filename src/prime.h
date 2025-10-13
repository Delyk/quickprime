#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <thread>
#include <vector>
namespace prime {

class bitmap {

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
  uint_fast64_t size;
  void init_bits(uint_fast64_t size, bool fill);

public:
  bitmap(uint_fast64_t size, bool fill = true);
  ~bitmap();
  boolean &operator[](uint_fast64_t index);
  void reset(bool num = true);
};

class primes : private bitmap {
  std::fstream cache;
  bitmap::boolean &operator[](uint_fast64_t index) = delete;
  void reset(bool num = true);

  class Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = char &;
    using pointer = char *;
    using reference = char &;
    pointer m_ptr;
    uint_fast64_t size;

  public:
    Iterator(pointer, uint_fast64_t);
    reference operator*();
    pointer operator->();
    Iterator &operator++();
    Iterator operator++(int);
  };

public:
  primes(uint_fast64_t n);
  ~primes();
  Iterator begin() const;
  Iterator end() const;
};

class sieve {

  std::ostream &out;
  void output(std::vector<uint_fast64_t>::iterator begin,
              std::vector<uint_fast64_t>::iterator end);

  std::vector<uint_fast64_t> sieve_linear(uint_fast64_t n);
  std::vector<uint_fast64_t> sieve_linear_skip(uint_fast64_t n);
  void sieve_segmented(uint_fast64_t n);
  void sieve_segmented_wheel(uint_fast64_t n);
  void sieve_atkhin(uint_fast64_t n);

  sieve &operator<<(const uint_fast64_t n);

public:
  sieve(uint_fast64_t n, std::ostream &out = std::cout);
};
} // namespace prime
