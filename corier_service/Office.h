#pragma once
#include <cmath>
#include <utility>
#include "Letter.h"

class Office
{
public:
	Office(int a, int b);
	~Office() {}

	double get_distance(Office other); // считает расстояние, между двумя филлиалами

	std::pair<int, int> get_pos(); // возвращает 2 координаты текущего филлиала {x, y}

private:
	int cord_x, cord_y;
};

