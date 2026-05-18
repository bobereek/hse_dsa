#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

void StringQuickSort(std::vector<std::string>& arr, int low, int high, int depth) {
  if (low >= high) {
    return;
  }

  int lt = low;
  int gt = high;
  int pivot = (depth < static_cast<int>(arr[low].length())) ? static_cast<unsigned char>(arr[low][depth]) : -1;
  int i = low + 1;

  while (i <= gt) {
    int t = (depth < static_cast<int>(arr[i].length())) ? static_cast<unsigned char>(arr[i][depth]) : -1;
    if (t < pivot) {
      std::swap(arr[lt++], arr[i++]);
    } else if (t > pivot) {
      std::swap(arr[i], arr[gt--]);
    } else {
      ++i;
    }
  }

  StringQuickSort(arr, low, lt - 1, depth);
  if (pivot != -1) {
    StringQuickSort(arr, lt, gt, depth + 1);
  }
  StringQuickSort(arr, gt + 1, high, depth);
}

void MsdRadixSort(std::vector<std::string>& arr, int low, int high, int depth, std::vector<std::string>& aux) {
  if (low >= high) {
    return;
  }

  if (high - low + 1 < 74) {
    StringQuickSort(arr, low, high, depth);
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
      MsdRadixSort(arr, b_low, b_high, depth + 1, aux);
    }
  }
}

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n;
  std::cin >> n;

  std::vector<std::string> arr(n);
  for (int i = 0; i < n; ++i) {
    std::cin >> arr[i];
  }

  if (n > 1) {
    std::vector<std::string> aux(n);
    MsdRadixSort(arr, 0, n - 1, 0, aux);
  }

  for (int i = 0; i < n; ++i) {
    std::cout << arr[i] << "\n";
  }

  return 0;
}