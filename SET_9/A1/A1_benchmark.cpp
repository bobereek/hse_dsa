#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

constexpr std::size_t kAlphabetSize = 74;
constexpr std::size_t kMinStringLength = 10;
constexpr std::size_t kMaxStringLength = 200;
constexpr std::size_t kMaxDatasetSize = 3000;
constexpr std::size_t kMinDatasetSize = 100;
constexpr std::size_t kDatasetStep = 100;

const std::string kAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-";

struct ComparisonCounter {
  std::uint64_t char_comparisons{0};
};

struct CompareResult {
  int order{0};
  std::size_t lcp{0};
};

CompareResult CompareStrings(const std::string& lhs, const std::string& rhs, std::size_t start,
                             ComparisonCounter& counter) {
  const std::size_t min_len = std::min(lhs.size(), rhs.size());
  std::size_t i = start;
  while (i < min_len) {
    ++counter.char_comparisons;
    const unsigned char lc = static_cast<unsigned char>(lhs[i]);
    const unsigned char rc = static_cast<unsigned char>(rhs[i]);
    if (lc < rc) {
      return {-1, i};
    }
    if (lc > rc) {
      return {1, i};
    }
    ++i;
  }
  if (lhs.size() < rhs.size()) {
    return {-1, min_len};
  }
  if (lhs.size() > rhs.size()) {
    return {1, min_len};
  }
  return {0, min_len};
}

class StringGenerator {
 public:
  explicit StringGenerator(std::uint64_t seed = 0x9E3779B97F4A7C15ULL)
      : rng_(seed), char_dist_(0, kAlphabet.size() - 1) {
  }

  std::string RandomString(std::size_t min_len = kMinStringLength, std::size_t max_len = kMaxStringLength,
                           const std::string& shared_prefix = "") {
    std::uniform_int_distribution<std::size_t> len_dist(min_len, max_len);
    const std::size_t len = len_dist(rng_);
    std::string result;
    result.reserve(len);

    const std::size_t prefix_len = std::min(shared_prefix.size(), len);
    result.append(shared_prefix.data(), prefix_len);
    while (result.size() < len) {
      result.push_back(kAlphabet[char_dist_(rng_)]);
    }
    return result;
  }

  std::string RandomPrefix(std::size_t len) {
    std::string prefix;
    prefix.reserve(len);
    for (std::size_t i = 0; i < len; ++i) {
      prefix.push_back(kAlphabet[char_dist_(rng_)]);
    }
    return prefix;
  }

  std::vector<std::string> RandomDataset(std::size_t count) {
    std::vector<std::string> data;
    data.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
      data.push_back(RandomString());
    }
    return data;
  }

  std::vector<std::string> CommonPrefixDataset(std::size_t count, std::size_t prefix_len = 8) {
    const std::string prefix = RandomPrefix(prefix_len);
    std::vector<std::string> data;
    data.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
      data.push_back(RandomString(kMinStringLength, kMaxStringLength, prefix));
    }
    return data;
  }

  void MakeAlmostSorted(std::vector<std::string>& data, std::size_t swap_count) {
    if (data.size() < 2) {
      return;
    }
    std::uniform_int_distribution<std::size_t> index_dist(0, data.size() - 1);
    for (std::size_t i = 0; i < swap_count; ++i) {
      const std::size_t a = index_dist(rng_);
      const std::size_t b = index_dist(rng_);
      if (a != b) {
        std::swap(data[a], data[b]);
      }
    }
  }

 private:
  std::mt19937_64 rng_;
  std::uniform_int_distribution<std::size_t> char_dist_;
};

void StandardQuickSort(std::vector<std::string>& data, int left, int right, ComparisonCounter& counter) {
  while (left < right) {
    int i = left;
    int j = right;
    const std::string pivot = data[left + (right - left) / 2];

    while (i <= j) {
      while (CompareStrings(data[i], pivot, 0, counter).order < 0) {
        ++i;
      }
      while (CompareStrings(data[j], pivot, 0, counter).order > 0) {
        --j;
      }
      if (i <= j) {
        std::swap(data[i], data[j]);
        ++i;
        --j;
      }
    }

    if (j - left < right - i) {
      if (left < j) {
        StandardQuickSort(data, left, j, counter);
      }
      left = i;
    } else {
      if (i < right) {
        StandardQuickSort(data, i, right, counter);
      }
      right = j;
    }
  }
}

void StandardMerge(std::vector<std::string>& data, int left, int mid, int right, std::vector<std::string>& buffer,
                   ComparisonCounter& counter) {
  int i = left;
  int j = mid + 1;
  int k = left;

  while (i <= mid && j <= right) {
    if (CompareStrings(data[i], data[j], 0, counter).order <= 0) {
      buffer[k++] = std::move(data[i++]);
    } else {
      buffer[k++] = std::move(data[j++]);
    }
  }
  while (i <= mid) {
    buffer[k++] = std::move(data[i++]);
  }
  while (j <= right) {
    buffer[k++] = std::move(data[j++]);
  }
  for (int idx = left; idx <= right; ++idx) {
    data[idx] = std::move(buffer[idx]);
  }
}

void StandardMergeSort(std::vector<std::string>& data, int left, int right, std::vector<std::string>& buffer,
                       ComparisonCounter& counter) {
  if (left >= right) {
    return;
  }
  const int mid = left + (right - left) / 2;
  StandardMergeSort(data, left, mid, buffer, counter);
  StandardMergeSort(data, mid + 1, right, buffer, counter);
  StandardMerge(data, left, mid, right, buffer, counter);
}

int GetChar(const std::string& s, int depth) {
  if (depth >= static_cast<int>(s.length())) {
    return -1;
  }
  return static_cast<unsigned char>(s[depth]);
}

void StringQuickSort(std::vector<std::string>& arr, int low, int high, int depth, ComparisonCounter& counter) {
  if (low >= high) {
    return;
  }

  int lt = low;
  int gt = high;
  int pivot = GetChar(arr[low], depth);
  int i = low + 1;

  while (i <= gt) {
    int t = GetChar(arr[i], depth);
    ++counter.char_comparisons;
    if (t < pivot) {
      std::swap(arr[lt++], arr[i++]);
    } else if (t > pivot) {
      std::swap(arr[i], arr[gt--]);
    } else {
      ++i;
    }
  }

  StringQuickSort(arr, low, lt - 1, depth, counter);
  if (pivot >= 0) {
    StringQuickSort(arr, lt, gt, depth + 1, counter);
  }
  StringQuickSort(arr, gt + 1, high, depth, counter);
}

void LcpMerge(std::vector<std::string>& arr, int left, int mid, int right, std::vector<std::string>& buffer,
              ComparisonCounter& counter) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  std::vector<std::string> left_arr;
  left_arr.reserve(n1);
  for (int i = 0; i < n1; ++i) {
    left_arr.push_back(std::move(arr[left + i]));
  }

  std::vector<std::string> right_arr;
  right_arr.reserve(n2);
  for (int j = 0; j < n2; ++j) {
    right_arr.push_back(std::move(arr[mid + 1 + j]));
  }

  int i = 0;
  int j = 0;
  int k = left;

  while (i < n1 && j < n2) {
    const CompareResult cmp = CompareStrings(left_arr[i], right_arr[j], 0, counter);
    if (cmp.order <= 0) {
      arr[k++] = std::move(left_arr[i++]);
    } else {
      arr[k++] = std::move(right_arr[j++]);
    }
  }

  while (i < n1) {
    arr[k++] = std::move(left_arr[i++]);
  }
  while (j < n2) {
    arr[k++] = std::move(right_arr[j++]);
  }
}

void LcpMergeSort(std::vector<std::string>& arr, int left, int right, std::vector<std::string>& buffer,
                  ComparisonCounter& counter) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    LcpMergeSort(arr, left, mid, buffer, counter);
    LcpMergeSort(arr, mid + 1, right, buffer, counter);
    LcpMerge(arr, left, mid, right, buffer, counter);
  }
}

void MsdRadixSortImpl(std::vector<std::string>& arr, int low, int high, int depth, std::vector<std::string>& aux,
                      bool allow_quicksort_fallback, ComparisonCounter& counter) {
  if (low >= high) {
    return;
  }

  if (allow_quicksort_fallback && high - low + 1 < static_cast<int>(kAlphabetSize)) {
    StringQuickSort(arr, low, high, depth, counter);
    return;
  }

  int count[259] = {0};
  for (int i = low; i <= high; ++i) {
    int c = (depth < static_cast<int>(arr[i].length())) ? static_cast<unsigned char>(arr[i][depth]) : -1;
    count[c + 2]++;
  }

  for (int r = 0; r < 258; ++r) {
    count[r + 1] += count[r];
  }

  int cp[259];
  for (int r = 0; r < 259; ++r) {
    cp[r] = count[r];
  }

  for (int i = low; i <= high; ++i) {
    int c = (depth < static_cast<int>(arr[i].length())) ? static_cast<unsigned char>(arr[i][depth]) : -1;
    aux[low + cp[c + 1]++] = std::move(arr[i]);
  }

  for (int i = low; i <= high; ++i) {
    arr[i] = std::move(aux[i]);
  }

  for (int r = 0; r < 256; ++r) {
    int b_low = low + count[r + 2];
    int b_high = low + count[r + 3] - 1;
    if (b_low < b_high) {
      MsdRadixSortImpl(arr, b_low, b_high, depth + 1, aux, allow_quicksort_fallback, counter);
    }
  }
}

void MsdRadixSort(std::vector<std::string>& arr, bool allow_quicksort_fallback, ComparisonCounter& counter) {
  if (arr.size() <= 1) {
    return;
  }
  std::vector<std::string> aux(arr.size());
  MsdRadixSortImpl(arr, 0, static_cast<int>(arr.size() - 1), 0, aux, allow_quicksort_fallback, counter);
}

struct DatasetCollection {
  std::vector<std::string> random;
  std::vector<std::string> sorted;
  std::vector<std::string> reverse_sorted;
  std::vector<std::string> almost_sorted;
  std::vector<std::string> prefix_heavy;
};

DatasetCollection BuildDatasets(StringGenerator& generator) {
  DatasetCollection datasets;
  datasets.random = generator.RandomDataset(kMaxDatasetSize);
  datasets.sorted = datasets.random;
  std::sort(datasets.sorted.begin(), datasets.sorted.end());
  datasets.reverse_sorted = datasets.sorted;
  std::reverse(datasets.reverse_sorted.begin(), datasets.reverse_sorted.end());
  datasets.almost_sorted = datasets.sorted;
  generator.MakeAlmostSorted(datasets.almost_sorted, std::max<std::size_t>(1, kMaxDatasetSize / 100));
  datasets.prefix_heavy = generator.CommonPrefixDataset(kMaxDatasetSize, 8);
  return datasets;
}

struct ExperimentResult {
  std::string dataset;
  std::size_t size{};
  std::string algorithm;
  double average_time_us{};
  std::uint64_t char_comparisons{};
};

bool IsSortedLexicographically(const std::vector<std::string>& data) {
  for (std::size_t i = 1; i < data.size(); ++i) {
    if (data[i - 1] > data[i]) {
      return false;
    }
  }
  return true;
}

class StringSortTester {
 public:
  explicit StringSortTester(std::vector<std::size_t> sizes) : sizes_(std::move(sizes)) {
  }

  std::vector<ExperimentResult> Run(const DatasetCollection& datasets) {
    std::vector<ExperimentResult> results;
    const auto algorithms = BuildAlgorithms();

    auto RunDataset = [this, &results, &algorithms](const std::string& name, const std::vector<std::string>& data) {
      for (std::size_t size : sizes_) {
        std::vector<std::string> base(data.begin(), data.begin() + static_cast<std::ptrdiff_t>(size));
        for (const auto& algorithm : algorithms) {
          results.push_back(BenchmarkOne(name, size, base, algorithm));
        }
      }
    };

    RunDataset("random", datasets.random);
    RunDataset("sorted", datasets.sorted);
    RunDataset("reverse_sorted", datasets.reverse_sorted);
    RunDataset("almost_sorted", datasets.almost_sorted);
    RunDataset("common_prefix", datasets.prefix_heavy);

    return results;
  }

 private:
  std::vector<std::size_t> sizes_;

  struct AlgorithmSpec {
    std::string name;
    std::function<void(std::vector<std::string>&, ComparisonCounter&)> sorter;
  };

  std::vector<AlgorithmSpec> BuildAlgorithms() const {
    return {
        {"standard_quicksort",
         [](std::vector<std::string>& data, ComparisonCounter& counter) {
           if (!data.empty()) {
             StandardQuickSort(data, 0, static_cast<int>(data.size() - 1), counter);
           }
         }},
        {"standard_mergesort",
         [](std::vector<std::string>& data, ComparisonCounter& counter) {
           if (!data.empty()) {
             std::vector<std::string> buffer(data.size());
             StandardMergeSort(data, 0, static_cast<int>(data.size() - 1), buffer, counter);
           }
         }},
        {"ternary_string_quicksort",
         [](std::vector<std::string>& data, ComparisonCounter& counter) {
           if (!data.empty()) {
             StringQuickSort(data, 0, static_cast<int>(data.size() - 1), 0, counter);
           }
         }},
        {"string_mergesort_lcp",
         [](std::vector<std::string>& data, ComparisonCounter& counter) {
           if (!data.empty()) {
             std::vector<std::string> buffer(data.size());
             LcpMergeSort(data, 0, static_cast<int>(data.size() - 1), buffer, counter);
           }
         }},
        {"msd_radix_sort",
         [](std::vector<std::string>& data, ComparisonCounter& counter) { MsdRadixSort(data, false, counter); }},
        {"msd_radix_sort_hybrid",
         [](std::vector<std::string>& data, ComparisonCounter& counter) { MsdRadixSort(data, true, counter); }},
    };
  }

  ExperimentResult BenchmarkOne(const std::string& dataset_name, std::size_t size, const std::vector<std::string>& base,
                                const AlgorithmSpec& algorithm) const {
    const int repeats = 5;
    std::uint64_t comparisons = 0;
    double total_us = 0.0;

    for (int rep = 0; rep < repeats; ++rep) {
      std::vector<std::string> data = base;
      ComparisonCounter counter;
      const auto start = std::chrono::steady_clock::now();
      algorithm.sorter(data, counter);
      const auto finish = std::chrono::steady_clock::now();
      total_us += std::chrono::duration<double, std::micro>(finish - start).count();
      comparisons = counter.char_comparisons;

      if (!IsSortedLexicographically(data)) {
        throw std::runtime_error("Sorting failed: " + algorithm.name + " on " + dataset_name);
      }
    }

    return {dataset_name, size, algorithm.name, total_us / static_cast<double>(repeats), comparisons};
  }
};

void WriteCsv(const std::string& path, const std::vector<ExperimentResult>& results) {
  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("Cannot open output file: " + path);
  }
  out << "dataset,size,algorithm,avg_time_us,char_comparisons\n";
  out << std::fixed << std::setprecision(3);
  for (const auto& row : results) {
    out << row.dataset << ',' << row.size << ',' << row.algorithm << ',' << row.average_time_us << ','
        << row.char_comparisons << '\n';
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  std::string output_path{"string_sort_results.csv"};
  StringGenerator generator;
  const DatasetCollection datasets = BuildDatasets(generator);
  const std::vector<std::size_t> sizes = {100, 500, 1000, 2000, 3000};

  StringSortTester tester(sizes);
  const std::vector<ExperimentResult> results = tester.Run(datasets);

  WriteCsv(output_path, results);
  std::cout << std::fixed << std::setprecision(3);
  std::cout << "dataset,size,algorithm,avg_time_us,char_comparisons\n";
  for (const auto& row : results) {
    std::cout << row.dataset << ',' << row.size << ',' << row.algorithm << ',' << row.average_time_us << ','
              << row.char_comparisons << '\n';
  }
  std::cerr << "Saved results to " << output_path << '\n';
  return 0;
}
