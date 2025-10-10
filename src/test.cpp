
#include "prime.h"
#include "gtest/gtest.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <gtest/gtest.h>
#include <iomanip>
#include <ios>
#include <ratio>
#include <unordered_map>
#include <vector>
#define COUNT 1
using namespace std;
using namespace prime;

class Sieves : public ::testing::TestWithParam<uint_fast64_t> {
protected:
  static unordered_map<string, double> speed_check;
  static vector<uint_fast64_t> primes;

public:
  static void load_primes(string filename) {
    ifstream in;
    in.open(filename);
    if (!in) {
      cerr << "Error open file: " << filename;
      exit(1);
    }
    vector<uint_fast64_t> primes;
    uint_fast64_t num;
    while (in >> num) {
      Sieves::primes.push_back(num);
    }
    in.close();
  }
  static void results() {
    cout << "Algorithms speed for 1000000:\n";
    for (auto i : speed_check) {
      cout << fixed << setprecision(3) << i.first << ": " << i.second / 1000
           << "s\n";
    }
  }
  bool check_primes(vector<uint_fast64_t> &generated) {
    for (size_t i = 0; i < generated.size(); i++) {
      if (generated[i] != primes[i]) {
        cout << "Pos: " << i << " Generated: " << generated[i]
             << " Control: " << primes[i] << endl;
        return false;
      }
    }
    return true;
  }
};
unordered_map<string, double> Sieves::speed_check;
vector<uint_fast64_t> Sieves::primes;

TEST_P(Sieves, SieveLinear) {
  uint_fast64_t val = GetParam();
  chrono::duration<double, milli> elapsed_ms{0};

  for (int i = 0; i < COUNT; i++) {
    auto start = chrono::high_resolution_clock::now();
    vector<uint_fast64_t> primes = sieve_linear(val);
    auto end = chrono::high_resolution_clock::now();
    elapsed_ms += end - start;
  }
  elapsed_ms /= COUNT;

  ASSERT_TRUE(check_primes(primes));

  cout << fixed << "Sieve " << val << " for " << elapsed_ms.count() << " ms\n";
  speed_check["Linear sieve"] = elapsed_ms.count();
}

TEST_P(Sieves, SieveLinearSkip) {
  uint_fast64_t val = GetParam();
  chrono::duration<double, milli> elapsed_ms{0};

  for (int i = 0; i < COUNT; i++) {
    auto start = chrono::high_resolution_clock::now();
    vector<uint_fast64_t> primes = sieve_linear_skip(val);
    auto end = chrono::high_resolution_clock::now();
    elapsed_ms += end - start;
  }
  elapsed_ms /= COUNT;

  ASSERT_TRUE(check_primes(primes));

  cout << fixed << "Sieve " << val << " for " << elapsed_ms.count() << " ms\n";
  speed_check["Linear sieve with skip 2"] = elapsed_ms.count();
}

TEST_P(Sieves, SieveSegmented) {
  uint_fast64_t val = GetParam();
  chrono::duration<double, milli> elapsed_ms{0};

  for (int i = 0; i < COUNT; i++) {
    auto start = chrono::high_resolution_clock::now();
    vector<uint_fast64_t> primes = sieve_segmented(val);
    auto end = chrono::high_resolution_clock::now();
    elapsed_ms += end - start;
  }
  elapsed_ms /= COUNT;

  ASSERT_TRUE(check_primes(primes));

  cout << fixed << "Sieve " << val << " for " << elapsed_ms.count() << " ms\n";
  speed_check["Segmented sieve"] = elapsed_ms.count();
}

TEST_P(Sieves, SieveSegmentedWheel) {
  uint_fast64_t val = GetParam();
  chrono::duration<double, milli> elapsed_ms{0};

  for (int i = 0; i < COUNT; i++) {
    auto start = chrono::high_resolution_clock::now();
    vector<uint_fast64_t> primes = sieve_segmented_wheel(val);
    auto end = chrono::high_resolution_clock::now();
    elapsed_ms += end - start;
  }
  elapsed_ms /= COUNT;

  ASSERT_TRUE(check_primes(primes));

  cout << fixed << "Sieve " << val << " for " << elapsed_ms.count() << " ms\n";
  speed_check["Segmented sieve with wheel factorization"] = elapsed_ms.count();
}

TEST_P(Sieves, SieveOfAtkhin) {
  uint_fast64_t val = GetParam();
  chrono::duration<double, milli> elapsed_ms{0};

  for (int i = 0; i < COUNT; i++) {
    auto start = chrono::high_resolution_clock::now();
    vector<uint_fast64_t> primes = sieve_atkhin(val);
    auto end = chrono::high_resolution_clock::now();
    elapsed_ms += end - start;
  }
  elapsed_ms /= COUNT;

  ASSERT_TRUE(check_primes(primes));

  cout << fixed << "Sieve " << val << " for " << elapsed_ms.count() << " ms\n";
  speed_check["Atkhin sieve"] = elapsed_ms.count();
}

INSTANTIATE_TEST_SUITE_P(SievesTest, Sieves,
                         ::testing::Values(0, 1, 2, 3, 4, 5, 30, 1000, 10000,
                                           1000000));

class TimingListener : public ::testing::EmptyTestEventListener {
public:
  void OnTestProgramStart(const testing::UnitTest &unit_test) override {
    Sieves::load_primes("primes_chk");
  }
  void OnTestProgramEnd(const ::testing::UnitTest &unit_test) override {
    Sieves::results();
  }
};

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::TestEventListeners &listeners =
      ::testing::UnitTest::GetInstance()->listeners();

  listeners.Append(new TimingListener);
  return RUN_ALL_TESTS();
}
