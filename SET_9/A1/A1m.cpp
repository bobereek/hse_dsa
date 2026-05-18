#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int LcpCompare(const std::string& s1, const std::string& s2) {
  size_t len1 = s1.length();
  size_t len2 = s2.length();
  size_t min_len = std::min(len1, len2);
  size_t lcp = 0;
  while (lcp < min_len && s1[lcp] == s2[lcp]) {
    lcp++;
  }
  if (lcp == len1 && lcp == len2) {
    return 0;
  }
  if (lcp == len1) {
    return -1;
  }
  if (lcp == len2) {
    return 1;
  }
  return (static_cast<unsigned char>(s1[lcp]) < static_cast<unsigned char>(s2[lcp])) ? -1 : 1;
}

void Merge(std::vector<std::string>& arr, int left, int mid, int right) {
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
    if (LcpCompare(left_arr[i], right_arr[j]) <= 0) {
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

void MergeSort(std::vector<std::string>& arr, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    MergeSort(arr, left, mid);
    MergeSort(arr, mid + 1, right);
    Merge(arr, left, mid, right);
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
    MergeSort(arr, 0, n - 1);
  }

  for (int i = 0; i < n; ++i) {
    std::string ender = i == n - 1 ? "" : "\n";
    std::cout << arr[i] << ender;
  }

  return 0;
}