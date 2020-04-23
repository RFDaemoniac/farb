#ifndef FARB_MATH_H
#define FARB_MATH_H

#include <vector>
#include <cmath>
#include <random>
#include <type_traits>

namespace Farb
{

constexpr double Pi          = 3.141592653589793238462643383279502884197169399;
constexpr double Phi         = 1.618033988749894848204586834365638117720309179;
constexpr double GoldenAngle = 2.399963229728653322231555506633613853124999011; // radians

template<typename T>
struct Point2D
{
	T x;
	T y;

	Point2D(T x, T y)
		: x(x)
		, y(y)
	{ }

	Point2D operator - (const Point2D & other) const
	{
		return Point2D{x - other.x, y - other.y};
	}

	Point2D operator * (T multiplier) const
	{
		return Point2D{x * multiplier, y * multiplier};
	}

	Point2D operator + (const Point2D & other) const
	{
		return Point2D{x + other.x, y + other.y};
	}

	Point2D operator / (const int divisor) const
	{
		return Point2D{x / divisor, y / divisor};
	}

	T Dot(const Point2D & other) const
	{
		return x * other.x + y * other.y;
	}

	T Magnitude() const
	{
		return static_cast<T>(std::sqrt(MagnitudeSquared()));
	}

	T MagnitudeSquared() const
	{
		return x * x + y * y;
	}

	template<typename Other>
	Point2D<Other> Cast()
	{
		if constexpr(std::is_integral<Other>::value && !std::is_integral<T>::value)
		{
			return Point2D<Other>{
				static_cast<Other>(std::nearbyint(x)),
				static_cast<Other>(std::nearbyint(y))
			};
		}
		else
		{
			return Point2D<Other>{
				static_cast<Other>(x),
				static_cast<Other>(y)
			};
		}
	}
};

double RadiusOfCircleWithArea(double area);


std::pair<bool, double> DistanceBetweenPointAndSegment(Point2D<double> p, Point2D<double> a, Point2D<double> b);

std::pair<bool, double> DistanceBetweenPointAndSegmentSquared(Point2D<double> p, Point2D<double> a, Point2D<double> b);

struct PointDistributionGenerator
{
	const double x_center;
	const double y_center;

	PointDistributionGenerator(double x_center, double y_center);

	virtual Point2D<double> GetNext();
};

struct FibonacciSpiralPointGenerator : PointDistributionGenerator
{
	// this fudge factor is so that we are less likely to fall outside of the area
	// particularly with irregular shapes
	static constexpr double radius_fudge_factor = 0.95;
	const double radius_expansion;
	int index = 0;

	FibonacciSpiralPointGenerator(
		int num_points,
		double area,
		double x_center,
		double y_center);

	Point2D<double> GetNext() override;
};

struct GridPointGenerator : PointDistributionGenerator
{
	// this fudge factor is so that we are less likely to fall outside of the area
	// particularly with irregular shapes
	static constexpr double separation_fudge_factor = 0.95;
	const double separation;
	int layer;
	int leg;
	int x_grid;
	int y_grid;

	GridPointGenerator(int num_points, double area, double x_center, double y_center);

	Point2D<double> GetNext() override;
};

struct RandomPointGenerator : PointDistributionGenerator
{
	std::random_device rd; // seed
	std::mt19937 gen; // mersene twister from seed
	std::uniform_int_distribution<> x_range;
	std::uniform_int_distribution<> y_range;

	RandomPointGenerator(double max_dimension, double x_center, double y_center);

	Point2D<double> GetNext() override;
};

} // namespace Farb


#endif // FARB_MATH_H