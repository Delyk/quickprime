#include "prime.h"
#include <chrono>
#include <iomanip>
#include <ratio>
#define COUNT 1
#define TEST_NUM 1000000
using namespace prime;
using namespace std;

int main() {
  chrono::duration<double, std::milli> elapsed_t{0};
  primes_queque buf;
  for (int i = 0; i < COUNT; i++) {
    auto start = chrono::high_resolution_clock::now();
    // sieve_linear_simple(TEST_NUM);
    sieve_segmented(TEST_NUM);
    // sieve_atkhin(TEST_NUM);
    auto end = chrono::high_resolution_clock::now();
    elapsed_t += end - start;
  }
  elapsed_t /= COUNT;
  std::cout << fixed << setprecision(3) << "Avg: " << elapsed_t.count()
            << " ms\n";

  return 0;
}
