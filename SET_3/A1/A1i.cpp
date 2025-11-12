#include <iostream>
#include <random>
#include <fstream>

bool BelongsCircle(double r, double cx, double cy, double x, double y) {
  double dx = x - cx;
  double dy = y - cy;
  return dx * dx + dy * dy <= r * r;
}
bool BelongsIntersection(double x, double y, double r1, double cx1, double cy1, double r2, double cx2, double cy2,
                         double r3, double cx3, double cy3) {
  return BelongsCircle(r1, cx1, cy1, x, y) && BelongsCircle(r2, cx2, cy2, x, y) && BelongsCircle(r3, cx3, cy3, x, y);
}

int main() {
  double r1, cx1, cy1, r2, cx2, cy2, r3, cx3, cy3;
  std::cin >> cx1 >> cy1 >> r1 >> cx2 >> cy2 >> r2 >> cx3 >> cy3 >> r3;

  std::random_device rd;
  std::mt19937 generator(rd());
  double x1 = std::max(cx1 - r1, std::max(cx2 - r2, cx3 - r3));
  double x2 = std::min(std::min(cx1 + r1, cx2 + r2), cx3 + r3);
  double y1 = std::max(std::max(cy1 - r1, cy2 - r2), cy3 - r3);
  double y2 = std::min(std::min(cy1 + r1, cy2 + r2), cy3 + r3);
  double bounding_area = (x2 - x1) * (y2 - y1);

  if (bounding_area <= 0) {
    std::cout << 0.0 << std::endl;
    return 0;
  }

  std::uniform_real_distribution<> x_distrib1(x1, x2);
  std::uniform_real_distribution<> y_distrib1(y1, y2);

  int count = 300000;
  int belong_counter = 0;

  for (int i = 0; i < count; i++) {
    double cur_x = x_distrib1(generator);
    double cur_y = y_distrib1(generator);
    if (BelongsIntersection(cur_x, cur_y, r1, cx1, cy1, r2, cx2, cy2, r3, cx3, cy3)) {
      belong_counter++;
    }
  }
  double ratio = static_cast<double>(belong_counter) / count;
  double approximate_area = ratio * bounding_area;
  std::cout << approximate_area;
  return 0;
}