#include <iostream>
#include <string>
#include <utility>
#include <vector>

void MsdRadixSort(std::vector<std::string>& arr, int low, int high, int depth, std::vector<std::string>& aux) {
  if (low >= high) {
    return;
  }

  const int kAlphabetSize = 256;
  int count[kAlphabetSize + 2] = {0};

  for (int i = low; i <= high; ++i) {
    int c = (depth < arr[i].length()) ? (static_cast<unsigned char>(arr[i][depth]) + 1) : 0;
    count[c + 1]++;
  }

  for (int r = 0; r < kAlphabetSize + 1; ++r) {
    count[r + 1] += count[r];
  }

  int bucket_offset[kAlphabetSize + 2];
  for (int r = 0; r < kAlphabetSize + 2; ++r) {
    bucket_offset[r] = count[r];
  }

  for (int i = low; i <= high; ++i) {
    int c = (depth < arr[i].length()) ? (static_cast<unsigned char>(arr[i][depth]) + 1) : 0;
    aux[low + bucket_offset[c]++] = std::move(arr[i]);
  }

  for (int i = low; i <= high; ++i) {
    arr[i] = std::move(aux[i]);
  }

  for (int r = 0; r < kAlphabetSize; ++r) {
    int bucket_low = low + count[r + 1];
    int bucket_high = low + count[r + 2] - 1;
    if (bucket_low < bucket_high) {
      MsdRadixSort(arr, bucket_low, bucket_high, depth + 1, aux);
    }
  }
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
    std::vector<std::string> aux(n);
    MsdRadixSort(arr, 0, n - 1, 0, aux);
  }

  for (int i = 0; i < n; ++i) {
    std::cout << arr[i] << "\n";
  }

  return 0;
}