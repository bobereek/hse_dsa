#include <algorithm>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <map>
#include <cstdint>

class ArrayGenerator {

  const int min_val_ = 0;
  const int max_val_ = 6000;
  const int max_size_ = 100000;

  std::random_device rd_;
  std::mt19937 generator_;

  std::vector<int> master_fully_random_;
  std::vector<int> master_reverse_sorted_;
  std::vector<int> master_almost_sorted_;

 public:
  ArrayGenerator() : generator_(rd_()) {
    master_fully_random_.resize(max_size_);
    std::uniform_int_distribution<> dist(min_val_, max_val_);
    for (int i = 0; i < max_size_; i++) {
      master_fully_random_[i] = dist(generator_);
    }

    master_reverse_sorted_ = master_fully_random_;
    std::sort(master_reverse_sorted_.begin(), master_reverse_sorted_.end(), std::greater<int>());

    master_almost_sorted_ = master_fully_random_;
    std::sort(master_almost_sorted_.begin(), master_almost_sorted_.end());

    std::uniform_int_distribution<> count_dist(2, std::max(2, max_size_ / 10));
    int swaps = count_dist(generator_);
    std::uniform_int_distribution<> idx_dist(0, max_size_ - 1);

    for (int i = 0; i < swaps; i++) {
      int idx1 = idx_dist(generator_);
      int idx2 = idx_dist(generator_);
      std::swap(master_almost_sorted_[idx1], master_almost_sorted_[idx2]);
    }
  }

  std::vector<int> FullyRandomGenerate(int count) {
    return {master_fully_random_.begin(), master_fully_random_.begin() + count};
  }

  std::vector<int> ReverseRandomGenerate(int count) {
    return {master_reverse_sorted_.begin(), master_reverse_sorted_.begin() + count};
  }

  std::vector<int> AlmostRandomGeneration(int count) {
    return {master_almost_sorted_.begin(), master_almost_sorted_.begin() + count};
  }
};

void Merge(std::vector<int>& arr, int left, int mid, int right) {
  std::vector<int> result;
  result.reserve(right - left);

  int l = left;
  int r = mid;

  while (l < mid && r < right) {
    if (arr[l] <= arr[r]) {
      result.push_back(arr[l++]);
    } else {
      result.push_back(arr[r++]);
    }
  }
  while (l < mid) {
    result.push_back(arr[l++]);
  }
  while (r < right) {
    result.push_back(arr[r++]);
  }

  for (int i = 0; i < result.size(); ++i) {
    arr[left + i] = result[i];
  }
}

void InsertionSort(std::vector<int>& arr, int left, int right) {

  for (int i = left + 1; i < right; i++) {
    int key = arr[i];
    int k = i - 1;
    while (k >= left && arr[k] > key) {
      arr[k + 1] = arr[k];
      k--;
    }
    arr[k + 1] = key;
  }
}

void MergeInsertionSort(std::vector<int>& arr, int left, int right, int insertion_threshold) {
  if (right - left <= insertion_threshold) {
    InsertionSort(arr, left, right);
    return;
  }
  int mid = left + (right - left) / 2;
  MergeInsertionSort(arr, left, mid, insertion_threshold);
  MergeInsertionSort(arr, mid, right, insertion_threshold);
  Merge(arr, left, mid, right);
}

void MergeSort(std::vector<int>& arr, int left, int right) {
  if (right - left <= 1) {
    return;
  }
  int mid = left + (right - left) / 2;
  MergeSort(arr, left, mid);
  MergeSort(arr, mid, right);
  Merge(arr, left, mid, right);
}

class SortTester {
  ArrayGenerator array_gen_;

 public:
  SortTester() = default;

  std::tuple<std::vector<int64_t>, std::vector<int64_t>, std::vector<int64_t>> MergeSortBenchmark() {
    std::vector<int64_t> fr_time;
    std::vector<int64_t> rr_time;
    std::vector<int64_t> ar_time;
    fr_time.reserve(995);
    rr_time.reserve(995);
    ar_time.reserve(995);

    for (int count = 500; count < 100000; count += 100) {
      std::vector<int> fr_arr = array_gen_.FullyRandomGenerate(count);
      std::vector<int> rr_arr = array_gen_.ReverseRandomGenerate(count);
      std::vector<int> ar_arr = array_gen_.AlmostRandomGeneration(count);

      {
        auto start_fr = std::chrono::high_resolution_clock::now();
        MergeSort(fr_arr, 0, count);
        auto elapsed_fr = std::chrono::high_resolution_clock::now() - start_fr;
        int64_t msec_fr = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_fr).count();
        fr_time.push_back(msec_fr);
      }

      {
        auto start_rr = std::chrono::high_resolution_clock::now();
        MergeSort(rr_arr, 0, count);
        auto elapsed_rr = std::chrono::high_resolution_clock::now() - start_rr;
        int64_t msec_rr = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_rr).count();
        rr_time.push_back(msec_rr);
      }

      {
        auto start_ar = std::chrono::high_resolution_clock::now();
        MergeSort(ar_arr, 0, count);
        auto elapsed_ar = std::chrono::high_resolution_clock::now() - start_ar;
        int64_t msec_ar = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_ar).count();
        ar_time.push_back(msec_ar);
      }
    }
    return std::make_tuple(fr_time, rr_time, ar_time);
  }

  std::tuple<std::vector<int64_t>, std::vector<int64_t>, std::vector<int64_t>> MergeInsertionSortBenchmark(
      int insertion_threshold) {
    std::vector<int64_t> fr_time;
    std::vector<int64_t> rr_time;
    std::vector<int64_t> ar_time;
    fr_time.reserve(995);
    rr_time.reserve(995);
    ar_time.reserve(995);

    for (int count = 500; count < 100000; count += 100) {
      std::vector<int> fr_arr = array_gen_.FullyRandomGenerate(count);
      std::vector<int> rr_arr = array_gen_.ReverseRandomGenerate(count);
      std::vector<int> ar_arr = array_gen_.AlmostRandomGeneration(count);

      {
        auto start_fr = std::chrono::high_resolution_clock::now();
        MergeInsertionSort(fr_arr, 0, count, insertion_threshold);
        auto elapsed_fr = std::chrono::high_resolution_clock::now() - start_fr;
        int64_t msec_fr = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_fr).count();
        fr_time.push_back(msec_fr);
      }

      {
        auto start_rr = std::chrono::high_resolution_clock::now();
        MergeInsertionSort(rr_arr, 0, count, insertion_threshold);
        auto elapsed_rr = std::chrono::high_resolution_clock::now() - start_rr;
        int64_t msec_rr = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_rr).count();
        rr_time.push_back(msec_rr);
      }

      {
        auto start_ar = std::chrono::high_resolution_clock::now();
        MergeInsertionSort(ar_arr, 0, count, insertion_threshold);
        auto elapsed_ar = std::chrono::high_resolution_clock::now() - start_ar;
        int64_t msec_ar = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_ar).count();
        ar_time.push_back(msec_ar);
      }
    }
    return std::make_tuple(fr_time, rr_time, ar_time);
  }
};

void SaveData(const std::string& filename, const std::vector<int64_t>& data) {
  std::ofstream output_file(filename);
  if (output_file.is_open()) {
    for (const auto& value : data) {
      output_file << value << "\n";
    }
    output_file.close();
  } else {
    std::cerr << "FileError: " << filename << '\n';
  }
}

int main() {

  const int num_runs = 5;

  for (int i = 1; i <= num_runs; ++i) {
    SortTester tester;

    std::cout << "Run #" << i << "..." << '\n';

    std::cout << "Testing MergeSort..." << '\n';
    std::vector<int64_t> fr_time_merge_sort;
    std::vector<int64_t> rr_time_merge_sort;
    std::vector<int64_t> ar_time_merge_sort;
    std::tie(fr_time_merge_sort, rr_time_merge_sort, ar_time_merge_sort) = tester.MergeSortBenchmark();

    SaveData("merge_sort_fully_random_run_" + std::to_string(i) + ".txt", fr_time_merge_sort);
    SaveData("merge_sort_reverse_random_run_" + std::to_string(i) + ".txt", rr_time_merge_sort);
    SaveData("merge_sort_almost_random_run_" + std::to_string(i) + ".txt", ar_time_merge_sort);
    std::cout << "MergeInsertionSort results saved." << '\n';

    std::map<int, std::tuple<std::vector<int64_t>, std::vector<int64_t>, std::vector<int64_t>>> merge_insertion_results;
    std::vector<int> insertion_thresholds = {5, 10, 20, 30, 50};

    for (const auto& threshold : insertion_thresholds) {
      std::cout << "Testing MergeInsertionSort with threshold " << threshold << "..." << '\n';
      merge_insertion_results[threshold] = tester.MergeInsertionSortBenchmark(threshold);
    }

    for (const auto& pair : merge_insertion_results) {
      int threshold = pair.first;
      const auto& results = pair.second;

      SaveData("merge_insertion_sort_fully_random_threshold_" + std::to_string(threshold) + "_run_" +
                   std::to_string(i) + ".txt",
               std::get<0>(results));
      SaveData("merge_insertion_sort_reverse_random_threshold_" + std::to_string(threshold) + "_run_" +
                   std::to_string(i) + ".txt",
               std::get<1>(results));
      SaveData("merge_insertion_sort_almost_random_threshold_" + std::to_string(threshold) + "_run_" +
                   std::to_string(i) + ".txt",
               std::get<2>(results));
    }
    std::cout << "MergeInsertionSort results saved." << '\n';
    std::cout << "Run #" << i << " completed.\n" << '\n';
  }

  return 0;
}