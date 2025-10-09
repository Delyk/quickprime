#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
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

void sieve_linear_print(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_linear(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_linear_skip(uint_fast64_t n);

std::vector<uint_fast64_t> sieve_segmented(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_segmented_wheel(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_segmented_parallel(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_linear_parallel(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_atkhin(uint_fast64_t n);
} // namespace prime
