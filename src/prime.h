#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <ostream>
#include <thread>
#include <vector>
namespace prime {

class bitmap {
  class boolean {
    char *byte;
    int bit;
    unsigned offset;

  public:
    boolean(char &byte, unsigned offset);
    boolean();
    operator bool() const;
    boolean &operator=(bool bit);
  };

  char *bits;
  uint_fast64_t size;
  boolean bit;

public:
  bitmap(uint_fast64_t size, bool fill = true);
  ~bitmap();
  boolean &operator[](uint_fast64_t index);
  void reset(bool num = true);
};

class sieve {

  std::string cache;
  std::ostream &out;
  void output(std::vector<uint_fast64_t>::iterator begin,
              std::vector<uint_fast64_t>::iterator end);

  void sieve_linear_print(uint_fast64_t n);
  std::vector<uint_fast64_t> sieve_linear(uint_fast64_t n);
  std::vector<uint_fast64_t> sieve_linear_skip(uint_fast64_t n);

  std::vector<uint_fast64_t> sieve_segmented(uint_fast64_t n);
  std::vector<uint_fast64_t> sieve_segmented_wheel(uint_fast64_t n);
  std::vector<uint_fast64_t> sieve_atkhin(uint_fast64_t n);

  sieve &operator<<(const uint_fast64_t n);

public:
  sieve(uint_fast64_t n);
  sieve(uint_fast64_t n, std::string cache);
};
} // namespace prime
