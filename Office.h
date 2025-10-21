#pragma once
#include <cmath>
#include <queue>
#include <utility>
#include "Letter.h"

class Office
{
public:
	Office(int a, int b): cord_x(a), cord_y(b){}
	~Office() {}

	double get_distance(Office other) {
		double dist_x = (cord_x - other.cord_x) * (cord_x - other.cord_x);
		double dist_y = (cord_y - other.cord_y) * (cord_y - other.cord_y);
		double dist = dist_x + dist_y;
		double ans = sqrt(dist);
		return ans;
	}

	std::pair<int, int> get_pos() {
		return std::make_pair(cord_x, cord_y);
	}


private:
	int cord_x, cord_y;
};

