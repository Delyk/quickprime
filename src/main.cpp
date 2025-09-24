#include "prime.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <ratio>
using namespace prime;
using namespace std;

int main() {
  auto start = chrono::high_resolution_clock::now();
  sieve_linear_print(30);
  auto end = chrono::high_resolution_clock::now();
  chrono::duration<double, milli> elapsed_ms = end - start;
  cout << fixed << setprecision(3) << elapsed_ms.count() << " ms" << endl;

  return 0;
}
