#include "stdafx.h"
#include "Vector2D.h"

//
// Constructor
//
Vector2D::Vector2D() {
	x = 0;
	y = 0;
}

//
// Destructor
//
Vector2D::~Vector2D() {
}

void Vector2D::Set(double x, double y) {
	this->x = x;
	this->y = y;
}

void Vector2D::Set(Vector2D vector) {
	Set(vector.x, vector.y);
}

void Vector2D::Add(double x, double y) {
	this->x += x;
	this->y += y;
}

void Vector2D::Add(Vector2D vector) {
	Add(vector.x, vector.y);
}

void Vector2D::Multiply(double value) {
	this->x *= value;
	this->y *= value;
}

double Vector2D::Distance(Vector2D target) {
	double dx = target.x - this->x;
	double dy = target.y - this->y;
	return sqrt(dx * dx + dy * dy);
}


Vector2D Vector2D::operator+ (const Vector2D &rhs) const
{
    Vector2D v;
	v.x = x + rhs.x;
	v.y = y + rhs.y;
	return v;
}
Vector2D Vector2D::operator- (const Vector2D &rhs) const
{
    Vector2D v;
	v.x = x - rhs.x;
	v.y = y - rhs.y;
	return v;
}
Vector2D Vector2D::operator* (const double rhs) const 
{
    Vector2D v;
	v.x = x * rhs;
	v.y = y * rhs;
	return v;
}
Vector2D Vector2D::operator/ (const double rhs) const
{
    Vector2D v;
	v.x = x / rhs;
	v.y = y / rhs;
	return v;
}
Vector2D operator*(const double lhs, const Vector2D &rhs)
{
	return rhs.operator*(lhs);
}
bool Vector2D::operator== (const Vector2D &rhs) const
{
	return x == rhs.x && y == rhs.y;
}
