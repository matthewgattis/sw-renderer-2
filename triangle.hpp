#pragma once
#include <functional>

void triangle(
	int x1,
	int y1,
	double z1,
	int x2,
	int y2,
	double z2,
	int x3,
	int y3,
	double z3,
	std::function<void(int x, int y, double z)> dotproc);

