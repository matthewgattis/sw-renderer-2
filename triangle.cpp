#include "triangle.hpp"

#include <vector>
#include <limits>

void line(
	int x1,
	int y1,
	double z1,
	int x2,
	int y2,
	double z2,
	std::vector<std::pair<int, double>>& mins,
	std::vector<std::pair<int, double>>& maxs)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	bool p = (dx > 0) != (dy < 0);
	int adx = dx < 0 ? -dx : dx;
	int ady = dy < 0 ? -dy : dy;
	bool adxgady = adx > ady;

    int x;
    int y;
    double z;
    int i1;
    int g;
    int i2;
	double dz;
	int f;

	if (adxgady)
	{
		if (dx > 0)
		{
			x = x1;
			y = y1;
			z = z1;
			f = x2;
			dz = (z2 - z1) / adx;
		}
		else
		{
			x = x2;
			y = y2;
			z = z2;
			f = x1;
			x2 = x1;
			y2 = y1;
			dz = (z1 - z2) / adx;
		}
		i1 = 2 * ady;
		g = i1 - adx;
		i2 = 2 * (ady - adx);
		while (x <= f)
		{
			if (x < mins[y].first)
				mins[y] = std::make_pair(x, z);
			if (x > maxs[y].first)
				maxs[y] = std::make_pair(x, z);
			x++;
			if (g >= (p ? 0 : 1))
			{
				y += p ? 1 : -1;
				g += i2;
			}
			else
			{
				g += i1;
			}
			z += dz;
		}
	}
	else
	{
		if (dy > 0)
		{
			x = x1;
			y = y1;
			z = z1;
			f = y2;
			dz = (z2 - z1) / ady;
		}
		else
		{
			x = x2;
			y = y2;
			z = z2;
			f = y1;
			x2 = x1;
			y2 = y1;
			dz = (z1 - z2) / ady;
		}
		i1 = 2 * adx;
		g = i1 - ady;
		i2 = 2 * (adx - ady);
		while (y <= f)
		{
			if (x < mins[y].first)
				mins[y] = std::make_pair(x, z);
			if (x > maxs[y].first)
				maxs[y] = std::make_pair(x, z);
			y++;
			if (g >= (p ? 0 : 1))
			{
				x += p ? 1 : -1;
				g += i2;
			}
			else
			{
				g += i1;
			}
			z += dz;
		}
	}
}

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
	std::function<void(int x, int y, double z)> dotproc)
{
	int min_y = std::min(y1, std::min(y2, y3));
	int max_y = std::max(y1, std::max(y2, y3));
	int dy = max_y - min_y;
	y1 -= min_y;
	y2 -= min_y;
	y3 -= min_y;

	std::vector<std::pair<int, double>> mins;
	std::vector<std::pair<int, double>> maxs;
	mins.reserve(dy + 1);
	maxs.reserve(dy + 1);
	for (int i = 0; i < dy + 1 ; i++)
	{
		mins.push_back(std::make_pair(std::numeric_limits<int>::max(), 0.0));
		maxs.push_back(std::make_pair(std::numeric_limits<int>::min(), 0.0));
	}

	line(x1, y1, z1, x2, y2, z2, mins, maxs);
	line(x2, y2, z2, x3, y3, z3, mins, maxs);
	line(x3, y3, z3, x1, y1, z1, mins, maxs);

	for (int j = 0; j < dy; j++)
	{
		double z = mins[j].second;
		int dx = std::max(1, maxs[j].first - mins[j].first);
		double dz = (maxs[j].second - mins[j].second) / dx;
		const int k = maxs[j].first;
		for (int i = mins[j].first; i < k; i++)
		{
			dotproc(i, j + min_y, z);
			z += dz;
		}
	}
}
