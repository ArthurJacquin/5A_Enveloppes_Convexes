#pragma once
#include <math.h>
#include <iostream>


class Vec3 {

public:

	union {
		double data[3];
		struct {
			double x;
			double y;
			double z;
		};
	};

	// Constructors

	// Vectors default to 0, 0, 0.
	Vec3() {
		x = 0;
		y = 0;
		z = 0;
	}

	// Construct with values, 3D
	Vec3(double ax, double ay, double az) {
		x = ax;
		y = ay;
		z = az;
	}

	// Construct with values, 2D
	Vec3(double ax, double ay) {
		x = ax;
		y = ay;
		z = 0;
	}

	// Copy constructor
	Vec3(const Vec3& o) {
		x = o.x;
		y = o.y;
		z = o.z;
	}

	//Equality
	bool operator==(const Vec3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	//Inequality
	bool operator!=(const Vec3& other) const {
		return !(*this == other);
	}

	// Addition
	Vec3 operator+(const Vec3& o) {
		return Vec3(x + o.x, y + o.y, z + o.z);
	}

	Vec3& operator+=(const Vec3& o) {
		x += o.x;
		y += o.y;
		z += o.z;
		return *this;
	}

	// Subtraction

	Vec3 operator-() {
		return Vec3(-x, -y, -z);
	}

	Vec3 operator-(const Vec3 o) {
		return Vec3(x - o.x, y - o.y, z - o.z);
	}

	Vec3& operator-=(const Vec3 o) {
		x -= o.x;
		y -= o.y;
		z -= o.z;
		return *this;
	}

	// Multiplication by scalars

	Vec3 operator*(const double s) {
		return Vec3(x * s, y * s, z * s);
	}

	Vec3& operator*=(const double s) {
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	// Division by scalars

	Vec3 operator/(const double s) {
		return Vec3(x / s, y / s, z / s);
	}

	Vec3& operator/=(const double s) {
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}

	// Dot product

	double operator*(const Vec3 o) {
		return (x * o.x) + (y * o.y) + (z * o.z);
	}

	// An in-place dot product does not exist because
	// the result is not a vector.

	// Cross product

	Vec3 operator^(const Vec3 o) {
		double nx = y * o.z - o.y * z;
		double ny = z * o.x - o.z * x;
		double nz = x * o.y - o.x * y;
		return Vec3(nx, ny, nz);
	}

	Vec3& operator^=(const Vec3 o) {
		double nx = y * o.z - o.y * z;
		double ny = z * o.x - o.z * x;
		double nz = x * o.y - o.x * y;
		x = nx;
		y = ny;
		z = nz;
		return *this;
	}

	double getDeterminant(const Vec3 o) {
		return x * o.y - y * o.x;
	}

	// Other functions

	// Length of vector
	double magnitude() {
		return sqrt(magnitude_sqr());
	}

	// Length of vector squared
	double magnitude_sqr() {
		return (x * x) + (y * y) + (z * z);
	}

	// Modified the vector so it becomes normalised
	Vec3& normalise() {
		double invLength = 1 / sqrtf(x * x + y * y + z * z);
		x *= invLength;
		y *= invLength;
		z *= invLength;
		return *this;
	}

	double dot(Vec3 b) {
		return (*this).x * b.x + (*this).y * b.y + (*this).z * b.z;
	}

	//return the angle between 2 vectors in radians
	double Angle(Vec3 b)
	{
		double alpha = acos(this->dot(b) / (this->magnitude() * b.magnitude()));
		return alpha;
	}

	double AngleClockwise(Vec3 b)
	{
		double det = (*this).x * b.y - (*this).y * b.x;
		double alpha = atan2(det, this->dot(b));
		return alpha;
	}

	friend std::ostream& operator <<(std::ostream& os, const Vec3& obj){
		os << "(" << obj.x << "," << obj.y << "," << obj.z << ")" << std::endl;
		return os;
	}
};
