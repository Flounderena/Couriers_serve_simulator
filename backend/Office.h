#pragma once
#include <cmath>
#include <utility>

class Office
{
public:
    Office(int a, int b);
    ~Office() {}

    double get_distance(Office other);
    std::pair<int, int> get_pos();

private:
    int cord_x, cord_y;
};
