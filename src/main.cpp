#include "prime.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <ratio>
#include <sys/resource.h>
#define COUNT 1
#define TEST_NUM 1000000
using namespace prime;
using namespace std;

long getMemoryUsage() {
  struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  // Возвращаем количество страниц памяти, умноженное на размер страницы
  return usage.ru_maxrss * 1024L; // ru_maxrss в килобайтах, переводим в байты
}

int main() {
  chrono::duration<double, milli> elapsed_ms;
  double time;
  long mem;
  long long allmem;
  for (int i = 0; i < COUNT; i++) {
    long begmem = getMemoryUsage();
    auto start = chrono::high_resolution_clock::now();
    sieve_linear_print(TEST_NUM);

    auto end = chrono::high_resolution_clock::now();
    long endmem = getMemoryUsage();
    elapsed_ms = end - start;
    mem = endmem - begmem;
    time += elapsed_ms.count();
    allmem += mem;
  }
  time /= COUNT;
  allmem /= COUNT;
  cout << fixed << setprecision(3) << "Avg: " << time << " ms" << endl;
  cout << "Mem usage: " << allmem << " byte" << endl;

  return 0;
}
