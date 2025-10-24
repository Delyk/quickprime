#include "prime.h"
#include <cstdint>
#include <cstdlib>
#define COUNT 3
#define TEST_NUM 300
using namespace prime;
using namespace std;

int main(int argc, char *argv[]) {
  uint_fast64_t n;
  bool stat = true;
  if (argc < 2) {
    cerr << "Usage: quickprime [number] [--nostat]" << endl;
    return 1;
  }
  n = atoi(argv[1]);

  if (!n) {
    cerr << "Usage: quickprime [number] [--nostat]" << endl;
    return 1;
  }
  if (argc == 3) {
    if (std::string(argv[2]) == "--nostat") {
      stat = false;
    } else {
      cerr << "Usage: quickprime [number] [--nostat]" << endl;
      return 1;
    }
  }

  generator(n, stat);

  return 0;
}
