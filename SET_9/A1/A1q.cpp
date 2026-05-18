#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int GetChar(const std::string& s, int depth) {
  if (depth >= static_cast<int>(s.length())) {
    return -1;
  }
  return static_cast<unsigned char>(s[depth]);
}

void StringQuickSort(std::vector<std::string>& arr, int low, int high, int depth) {
  if (low >= high) {
    return;
  }

  int lt = low;
  int gt = high;
  int pivot = GetChar(arr[low], depth);
  int i = low + 1;

  while (i <= gt) {
    int t = GetChar(arr[i], depth);
    if (t < pivot) {
      std::swap(arr[lt++], arr[i++]);
    } else if (t > pivot) {
      std::swap(arr[i], arr[gt--]);
    } else {
      ++i;
    }
  }

  StringQuickSort(arr, low, lt - 1, depth);
  if (pivot >= 0) {
    StringQuickSort(arr, lt, gt, depth + 1);
  }
  StringQuickSort(arr, gt + 1, high, depth);
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n{};
  std::cin >> n;

  std::vector<std::string> arr(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> arr[i];
  }

  if (n > 1) {
    StringQuickSort(arr, 0, n - 1, 0);
  }

  for (int i = 0; i < n; ++i) {
    std::cout << arr[i] << "\n";
  }

  return 0;
}