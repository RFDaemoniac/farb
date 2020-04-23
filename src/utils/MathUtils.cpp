#include "MathUtils.h"

namespace Farb
{

double RadiusOfCircleWithArea(double area)
{
	return std::sqrt(area / Pi);
}

std::pair<bool, double> DistanceBetweenPointAndSegment(Point2D<double> p, Point2D<double> a, Point2D<double> b)
{
	auto result = DistanceBetweenPointAndSegmentSquared(p, a, b);
	return {result.first, std::sqrt(result.second)};
}

std::pair<bool, double> DistanceBetweenPointAndSegmentSquared(Point2D<double> p, Point2D<double> a, Point2D<double> b)
{
	Point2D<double> line = a - b;
	double line_mag_2 = line.MagnitudeSquared();
	// avoid divide by zero, a and b are close to each other
	if (line_mag_2 < 0.01)
	{
		return {true, (p - a).MagnitudeSquared()};
	}
	double u = (
		(p.x - a.x) * (b.x - a.x)
		+ (p.y - a.y) * (b.y - a.y)
	) / line_mag_2;

	// point does not intersect at segment, closer to a
	if (u < 0.0)
	{
		return {false, (p - a).MagnitudeSquared()};
	}
	// point closer to b
	else if (u > 1.0)
	{
		return {false, (p - b).MagnitudeSquared()};
	}

	Point2D<double> intersection = a + (b - a) * u;

	return {true, (p - intersection).MagnitudeSquared()};
}

PointDistributionGenerator::PointDistributionGenerator(double x_center, double y_center)
	: x_center(x_center)
	, y_center(y_center)
{ }

Point2D<double> PointDistributionGenerator::GetNext()
{
	return {x_center, y_center};
}

FibonacciSpiralPointGenerator::FibonacciSpiralPointGenerator(
	int num_points,
	double area,
	double x_center,
	double y_center)
	: PointDistributionGenerator(x_center, y_center)
	, radius_expansion(RadiusOfCircleWithArea(area)
		/ std::sqrt((double)num_points)
		* radius_fudge_factor)
	, index(0)
{ }

Point2D<double> FibonacciSpiralPointGenerator::GetNext()
{
	const double radius = std::sqrt((double)index) * radius_expansion;
	const double angle = GoldenAngle * (double) index;

	double x = x_center + std::cos(angle) * radius;
	double y = y_center + std::sin(angle) * radius;

	index++;

	return {x, y};
}

GridPointGenerator::GridPointGenerator(int num_points, double area, double x_center, double y_center)
	: PointDistributionGenerator(x_center, y_center)
	, separation(std::sqrt(area / (double)num_points) * separation_fudge_factor)
	, layer(0)
	, leg(0)
	, x_grid(0)
	, y_grid(0)
{ }

Point2D<double> GridPointGenerator::GetNext()
{
	double x = x_grid * separation;
	double y = y_grid * separation;

	switch(leg)
	{
		case 0:
			// east
			x_grid++;
			if (x_grid == layer)
			{
				leg++;
			}
			break;
		case 1:
			// north
			y_grid++;
			if (y_grid == layer)
			{
				leg++;
			}
			break;
		case 2:
			// west
			x_grid--;
			if (-x_grid == layer)
			{
				leg++;
			}
			break;
		case 3:
			// south
			y_grid--;
			if (-y == layer)
			{
				leg = 0;
				layer++;
			}
			break;
	}
	return {x, y};
}

RandomPointGenerator::RandomPointGenerator(double max_dimension, double x_center, double y_center)
	: PointDistributionGenerator(x_center, y_center)
	, rd()
	, gen(rd())
	, x_range(x_center - max_dimension, x_center + max_dimension)
	, y_range(y_center - max_dimension, y_center + max_dimension)
{

}

Point2D<double> RandomPointGenerator::GetNext()
{
	return {static_cast<double>(x_range(gen)), static_cast<double>(y_range(gen))};
}

} // namespace Farb
