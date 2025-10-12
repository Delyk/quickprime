#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <initializer_list>
#include <iostream>
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

public:
  primes(uint_fast64_t n);
  ~primes();
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
