#include "line.hpp"

#include <ios>
#include <memory>

Line::Iterator::Iterator() :
	x_(),
	y_(),
	z_(),
	p_(),
	adxgady_(),
	i1_(),
	g_(),
	i2_(),
	dz_(),
	coord_()
{
}

Line::Iterator::Iterator(
	int x,
	int y,
	double z,
	bool p,
	bool adxgady,
	int i1,
	int g,
	int i2,
	double dz) :
	x_(x),
	y_(y),
	z_(z),
	p_(p),
	adxgady_(adxgady),
	i1_(i1),
	g_(g),
	i2_(i2),
	dz_(dz),
	coord_(x, y, z)
{
}

Line::Iterator Line::Iterator::operator++()
{
	coord_ = Coord(x_, y_, z_);
	if (adxgady_)
	{
		x_++;
		if (g_ >= (p_ ? 0 : 1))
		{
			y_ += p_ ? 1 : -1;
			g_ += i2_;
		}
		else
			g_ += i1_;
	}
	else
	{
		y_++;
		if (g_ >= (p_ ? 0 : 1))
		{
			x_ += p_ ? 1 : -1;
			g_ += i2_;
		}
		else
			g_ += i1_;
	}
	z_ += dz_;
	return *this;
}

Line::Line(int x1, int y1, double z1, int x2, int y2, double z2)
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

	if (adxgady)
	{
		if (dx > 0)
		{
			x = x1;
			y = y1;
			z = z1;
			dz = (z2 - z1) / adx;
		}
		else
		{
			x = x2;
			y = y2;
			z = z2;
			x2 = x1;
			y2 = y1;
			dz = (z1 - z2) / adx;
		}
		i1 = 2 * ady;
		g = i1 - adx;
		i2 = 2 * (ady - adx);
	}
	else
	{
		if (dy > 0)
		{
			x = x1;
			y = y1;
			z = z1;
			dz = (z2 - z1) / ady;
		}
		else
		{
			x = x2;
			y = y2;
			z = z2;
			x2 = x1;
			y2 = y1;
			dz = (z1 - z2) / ady;
		}
		i1 = 2 * adx;
		g = i1 - ady;
		i2 = 2 * (adx - ady);
	}

	begin_ = Iterator(x, y, z, p, adxgady, i1, g, i2, dz);
	end_ = Iterator(x2, y2, z2, p, adxgady, i1, g, i2, dz);
	end_++;
}

