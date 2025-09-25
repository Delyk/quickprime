#pragma once

#include <cstddef>
#include <cstdint>
#include <thread>
#include <vector>
namespace prime {
void sieve_linear_print(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_linear(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_linear_skip(uint_fast64_t n);

std::vector<uint_fast64_t> sieve_segmented(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_segmented_parallel(uint_fast64_t n);
std::vector<uint_fast64_t> sieve_linear_parallel(uint_fast64_t n);
void sieve_atkhin(uint_fast64_t n);
} // namespace prime
