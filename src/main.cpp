#include "prime.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <ratio>
#define COUNT 100
#define TEST_NUM 30
using namespace prime;
using namespace std;

int main() {
  chrono::duration<double, milli> elapsed_ms;
  double time;
  for (int i = 0; i < COUNT; i++) {
    auto start = chrono::high_resolution_clock::now();
    sieve_linear_print(TEST_NUM);
    auto end = chrono::high_resolution_clock::now();
    elapsed_ms = end - start;
    time += elapsed_ms.count();
  }
  time /= COUNT;
  cout << fixed << setprecision(3) << "Avg: " << time << " ms" << endl;

  return 0;
}
