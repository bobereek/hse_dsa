#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <unordered_set>
#include <iomanip>

class RandomStreamGen {
  std::mt19937 seed_;
  const std::string charset_;

 public:
  explicit RandomStreamGen(uint32_t seed)
      : seed_(seed), charset_("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-") {
  }

  std::string NextString() {
    std::uniform_int_distribution<size_t> len_dist(5, 30);
    std::uniform_int_distribution<size_t> char_dist(0, charset_.size() - 1);
    size_t length = len_dist(seed_);
    std::string s;
    s.reserve(length);
    for (size_t i = 0; i < length; ++i) {
      s += charset_[char_dist(seed_)];
    }
    return s;
  }
  std::vector<std::string> GenerateBatch(size_t batch_size) {
    std::vector<std::string> batch;
    batch.reserve(batch_size);
    for (size_t i = 0; i < batch_size; ++i) {
      batch.push_back(NextString());
    }
    return batch;
  }
};

class HashFuncGen {
  uint32_t seed_;

  static uint32_t Mix32(uint32_t x) {
    x ^= x >> 16;
    x *= 0x85ebca6b;
    x ^= x >> 13;
    x *= 0xc2b2ae35;
    x ^= x >> 16;
    return x;
  }

 public:
  explicit HashFuncGen(uint32_t seed_val = 0) : seed_(seed_val) {
  }

  uint32_t operator()(const std::string& key) const {
    uint32_t hash = 2166136261u ^ seed_;
    for (unsigned char c : key) {
      hash ^= static_cast<uint32_t>(c);
      hash *= 16777619u;
    }
    return Mix32(hash);
  }
};

class HyperLogLog {
  int b_;
  int m_;
  double alpha_mm_;
  std::vector<uint8_t> registers_;
  HashFuncGen hasher_;

  uint8_t GetRank(uint32_t w) {
    if (w == 0) {
      return static_cast<uint8_t>(32 - b_ + 1);
    }
    int r = 1;
    uint32_t mask = 1u << (32 - b_ - 1);
    while ((w & mask) == 0) {
      ++r;
      mask >>= 1;
    }
    return static_cast<uint8_t>(r);
  }

 public:
  HyperLogLog(int bits, uint32_t hash_seed) : b_(bits), hasher_(hash_seed) {
    if (b_ < 4) {
      b_ = 4;
    }
    if (b_ > 30) {
      b_ = 30;
    }
    m_ = 1 << b_;
    registers_.assign(m_, 0);

    double alpha = NAN;
    switch (m_) {
      case 16:
        alpha = 0.673;
        break;
      case 32:
        alpha = 0.697;
        break;
      case 64:
        alpha = 0.709;
        break;
      default:
        alpha = 0.7213 / (1.0 + 1.079 / m_);
        break;
    }
    alpha_mm_ = alpha * m_ * m_;
  }

  void Add(const std::string& s) {
    uint32_t x = hasher_(s);
    uint32_t j = x >> (32 - b_);
    uint32_t w = x & ((1u << (32 - b_)) - 1u);
    uint8_t rank = GetRank(w);

    if (rank > registers_[j]) {
      registers_[j] = rank;
    }
  }

  double Estimate() const {
    double sum_inv = 0.0;
    int zeros = 0;
    for (int val : registers_) {
      sum_inv += std::pow(2.0, -val);
      if (val == 0) {
        zeros++;
      }
    }

    double E = alpha_mm_ / sum_inv;

    if (E <= 2.5 * m_) {
      if (zeros > 0) {
        E = m_ * std::log(static_cast<double>(m_) / zeros);
      }
    } else if (E > (1.0 / 30.0) * 4294967296.0) {
      E = -4294967296.0 * std::log(1.0 - E / 4294967296.0);
    }

    return E;
  }
};

struct ExperimentResult {
  int step_num;
  size_t processed_items;
  size_t exact_cardinality;
  double estimated_cardinality;
};

int main() {
  const int b_bits = 12;
  const int num_streams = 20;
  const int total_items = 100000;
  const int steps = 50;
  const int batch_size = total_items / steps;

  std::cout << "Running HyperLogLog...\n";

  std::vector<std::vector<ExperimentResult>> all_results(steps);

  for (int run = 0; run < num_streams; ++run) {
    RandomStreamGen stream_gen(1000 + run);
    HyperLogLog hll(b_bits, 999);
    std::unordered_set<std::string> exact_set;

    size_t processed = 0;

    for (int t = 0; t < steps; ++t) {
      auto batch = stream_gen.GenerateBatch(batch_size);

      for (const auto& s : batch) {
        hll.Add(s);
        exact_set.insert(s);
      }
      processed += batch.size();

      ExperimentResult res;
      res.step_num = t + 1;
      res.processed_items = processed;
      res.exact_cardinality = exact_set.size();
      res.estimated_cardinality = hll.Estimate();

      all_results[t].push_back(res);
    }
  }

  // Формат CSV: ProcessedItems, AvgExact, AvgEst, StdDevEst, LowerBound, UpperBound

  std::cout << "CSV_START\n";
  std::cout << "Step,Processed,AvgExact,AvgEst,StdDev,LowerBound,UpperBound\n";

  for (int t = 0; t < steps; ++t) {
    double sum_est = 0;
    double sum_exact = 0;
    double sum_sq_diff = 0;
    size_t n = all_results[t].size();
    size_t processed = all_results[t][0].processed_items;

    for (const auto& r : all_results[t]) {
      sum_est += r.estimated_cardinality;
      sum_exact += static_cast<double>(r.exact_cardinality);
    }
    double avg_est = sum_est / n;
    double avg_exact = sum_exact / n;

    for (const auto& r : all_results[t]) {
      double diff = r.estimated_cardinality - avg_est;
      sum_sq_diff += diff * diff;
    }
    double std_dev = std::sqrt(sum_sq_diff / n);

    std::cout << (t + 1) << "," << processed << "," << std::fixed << std::setprecision(2) << avg_exact << "," << avg_est
              << "," << std_dev << "," << (avg_est - std_dev) << "," << (avg_est + std_dev) << "\n";
  }
  std::cout << "CSV_END\n";

  return 0;
}