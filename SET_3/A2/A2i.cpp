#include <iostream>
#include <vector>

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

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n{};
  std::cin >> n;
  std::vector<int> arr(n);
  for (int i = 0; i < n; i++) {
    std::cin >> arr[i];
  }
  MergeInsertionSort(arr, 0, n, 15);
  for (int i = 0; i < n; i++) {
    std::cout << arr[i] << " ";
  }
  return 0;
}