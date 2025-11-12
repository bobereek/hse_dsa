#include <iostream>
#include <random>
#include <cmath>
#include <fstream>
#include <vector>

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
  std::random_device rd;
  std::mt19937 generator(rd());
  const double real_area = 0.25 * M_PI + 1.25 * std::asin(0.8) - 1.0;
  {
    // part 1. Gen inside Rectangle
    std::vector<double> areas;
    std::vector<double> abs_errors;
    std::vector<double> rel_errors;
    double x1 = 0;
    double x2 = 2 + std::sqrt(5) / 2;
    double y1 = 0;
    double y2 = 2 + std::sqrt(5) / 2;
    double bounding_area = (x2 - x1) * (y2 - y1);

    std::uniform_real_distribution<> x_distrib1(x1, x2);

    std::uniform_real_distribution<> y_distrib1(y1, y2);

    for (int count = 100; count < 100000; count += 500) {
      double belong_counter = 0;
      for (int i = 0; i < count; i++) {
        double cur_x = x_distrib1(generator);
        double cur_y = y_distrib1(generator);
        if (BelongsIntersection(cur_x, cur_y, 1, 1, 1, std::sqrt(5) / 2, 1.5, 2, std::sqrt(5) / 2, 2, 1.5)) {
          belong_counter++;
        }
      }
      double ratio = static_cast<double>(belong_counter) / count;
      double approximate_area = ratio * bounding_area;
      areas.push_back(approximate_area);
      abs_errors.push_back(approximate_area - real_area);
      rel_errors.push_back(std::abs(approximate_area - real_area) / real_area);
    }

    std::ofstream output_file1("areas1.txt");
    if (output_file1.is_open()) {
      for (const auto& area : areas) {
        output_file1 << area << "\n";
      }
      output_file1.close();
    }

    std::ofstream output_file2("abs_errors1.txt");
    if (output_file2.is_open()) {
      for (const auto& error : abs_errors) {
        output_file2 << error << "\n";
      }
      output_file2.close();
    }

    std::ofstream output_file3("rel_errors1.txt");
    if (output_file3.is_open()) {
      for (const auto& error : rel_errors) {
        output_file3 << error << "\n";
      }
      output_file3.close();
    }

  }

  {
    // part 2. Gen inside Square
    std::vector<double> areas;
    std::vector<double> abs_errors;
    std::vector<double> rel_errors;
    double x1 = 2 - sqrt(5) / 2;
    double x2 = 2;
    double y1 = 2 - sqrt(5) / 2;
    double y2 = 2;
    double bounding_area = (x2 - x1) * (y2 - y1);


    std::uniform_real_distribution<> x_distrib2(x1, x2);

    std::uniform_real_distribution<> y_distrib2(y1, y2);

    for (int count = 100; count < 100000; count += 500) {
      double belong_counter = 0;
      for (int i = 0; i < count; i++) {
        double cur_x = x_distrib2(generator);
        double cur_y = y_distrib2(generator);
        if (BelongsIntersection(cur_x, cur_y, 1, 1, 1, std::sqrt(5) / 2, 1.5, 2, std::sqrt(5) / 2, 2, 1.5)) {
          belong_counter++;
        }
      }
      double ratio = static_cast<double>(belong_counter) / count;
      double approximate_area = ratio * bounding_area;
      areas.push_back(approximate_area);
      abs_errors.push_back(approximate_area - real_area);
      rel_errors.push_back(std::abs(approximate_area - real_area) / real_area);
    }

    std::ofstream output_file1("areas2.txt");
    if (output_file1.is_open()) {
      for (const auto& area : areas) {
        output_file1 << area << "\n";
      }
      output_file1.close();
    }

    std::ofstream output_file2("abs_errors2.txt");
    if (output_file2.is_open()) {
      for (const auto& error : abs_errors) {
        output_file2 << error << "\n";
      }
      output_file2.close();
    }

    std::ofstream output_file3("rel_errors2.txt");
    if (output_file3.is_open()) {
      for (const auto& error : rel_errors) {
        output_file3 << error << "\n";
      }
      output_file3.close();
    }
  }
  return 0;
}