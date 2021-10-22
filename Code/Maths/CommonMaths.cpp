#include "CommonMaths.h"

#include <math.h>

// ===============================================================================================//
//------------------------------------------- Vector2D -------------------------------------------//
// ===============================================================================================//

Vector2D Vector2D::worldUp = Vector2D(0.0f, 1.0f);
Vector2D Vector2D::zero    = Vector2D(0.0f, 0.0f);

Vector2D::Vector2D()
	: x(0.0f)
	, y(0.0f)
{

}

// ------------------------------------------------------------------------------------------------

Vector2D::Vector2D(float x, float y)
	: x(x)
	, y(y)
{

}

// ------------------------------------------------------------------------------------------------

Vector2D::Vector2D(const Vector2D& other)
	: x(other.x)
	, y(other.y)
{

}

// ------------------------------------------------------------------------------------------------

Vector2D Vector2D::operator+(const Vector2D& other)
{
	// Return back a new vector with the change applied
	return Vector2D(x + other.x, y + other.y);
}

// ------------------------------------------------------------------------------------------------

Vector2D& Vector2D::operator+=(const Vector2D& other)
{
	// Internally add on the value and then return the vector back
	x += other.x;
	y += other.y;
	
	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector2D Vector2D::operator-(const Vector2D& other)
{
	return Vector2D(x - other.x, y - other.y);
}

// ------------------------------------------------------------------------------------------------

Vector2D& Vector2D::operator-=(const Vector2D& other)
{
	x -= other.x;
	y -= other.y;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector2D Vector2D::operator*(const float& factor)
{
	return Vector2D(x * factor, y * factor);
}

// ------------------------------------------------------------------------------------------------

Vector2D& Vector2D::operator*=(const float& factor)
{
	x *= factor;
	y *= factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector2D Vector2D::operator*(const int& factor)
{
	return Vector2D(x * (float)factor, y * (float)factor);
}

// ------------------------------------------------------------------------------------------------

Vector2D& Vector2D::operator*=(const int& factor)
{
	x *= (float)factor;
	y *= (float)factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector2D Vector2D::operator/(const float& factor)
{
	return Vector2D(x / factor, y / factor);
}

// ------------------------------------------------------------------------------------------------

Vector2D& Vector2D::operator/=(const float& factor)
{
	x /= factor;
	y /= factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector2D Vector2D::operator/(const int& factor)
{
	return Vector2D(x / (float)factor, y / (float)factor);
}

// ------------------------------------------------------------------------------------------------

Vector2D& Vector2D::operator/=(const int& factor)
{
	x /= (float)factor;
	y /= (float)factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

void     Vector2D::operator=(const Vector2D& other)
{
	x = other.x;
	y = other.y;
}

// ------------------------------------------------------------------------------------------------

bool     Vector2D::operator==(const Vector2D other)
{
	if (x == other.x && y == other.y)
		return true;

	return false;
}

// ------------------------------------------------------------------------------------------------

float    Vector2D::Dot(const Vector2D& other)
{
	return (x * other.x) + (y * other.y);
}

// ------------------------------------------------------------------------------------------------

Vector2D Vector2D::Normalised() // Retuns back the normalised vector - doesn't change the data stored internally
{
	float magnitude = Length();

	return Vector2D(x / magnitude, y / magnitude);
}

// ------------------------------------------------------------------------------------------------

Vector2D& Vector2D::Normalise() // Changes the internal data and then returns itself back
{
	float magnitude = Length();

	x /= magnitude;
	y /= magnitude;

	return *this;
}

// ------------------------------------------------------------------------------------------------

float    Vector2D::Length() // Calculates the length of the vector
{
	return (float)sqrt(LengthSquared());
}

// ------------------------------------------------------------------------------------------------

float    Vector2D::LengthSquared() // Calculates the length squared of the vector
{
	return ((x * x) + (y * y));
}

// ------------------------------------------------------------------------------------------------

DirectX::XMFLOAT2 Vector2D::ConvertToDirectXFloat2()
{
	return DirectX::XMFLOAT2(x, y);
}

// ------------------------------------------------------------------------------------------------

// ===============================================================================================//
//------------------------------------------- Vector3D -------------------------------------------//
// ===============================================================================================//

Vector3D Vector3D::worldUp = Vector3D(0.0f, 1.0f, 0.0f);
Vector3D Vector3D::zero    = Vector3D(0.0f, 0.0f, 0.0f);

Vector3D::Vector3D()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
{

}

// ------------------------------------------------------------------------------------------------

Vector3D::Vector3D(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
{

}

// ------------------------------------------------------------------------------------------------

Vector3D::Vector3D(const Vector3D& other)
	: x(other.x)
	, y(other.y)
	, z(other.z)
{

}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::operator+(const Vector3D& other)
{
	// Return back a new vector with the change applied
	return Vector3D(x + other.x, y + other.y, z + other.z);
}

// ------------------------------------------------------------------------------------------------

Vector3D& Vector3D::operator+=(const Vector3D& other)
{
	// Internally add on the value and then return the vector back
	x += other.x;
	y += other.y;
	z += other.z;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::operator-(const Vector3D& other)
{
	return Vector3D(x - other.x, y - other.y, z - other.z);
}

// ------------------------------------------------------------------------------------------------

Vector3D& Vector3D::operator-=(const Vector3D& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::operator*(const float& factor)
{
	return Vector3D(x * factor, y * factor, z * factor);
}

// ------------------------------------------------------------------------------------------------

Vector3D& Vector3D::operator*=(const float& factor)
{
	x *= factor;
	y *= factor;
	z *= factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::operator*(const int& factor)
{
	return Vector3D(x * (float)factor, y * (float)factor, z * (float)factor);
}

// ------------------------------------------------------------------------------------------------

Vector3D& Vector3D::operator*=(const int& factor)
{
	x *= (float)factor;
	y *= (float)factor;
	z *= (float)factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::operator*(const DirectX::XMFLOAT3X3& matrix)
{
	Vector3D returnVector = Vector3D::zero;

	returnVector.x = (x * matrix._11) + (y * matrix._21) + (z * matrix._31);
	returnVector.y = (x * matrix._12) + (y * matrix._22) + (z * matrix._32);
	returnVector.z = (x * matrix._13) + (y * matrix._23) + (z * matrix._33);

	return returnVector;
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::operator*=(const DirectX::XMFLOAT3X3& matrix)
{
	Vector3D currentState = *this;

	x = (currentState.x * matrix._11) + (currentState.y * matrix._21) + (currentState.z * matrix._31);
	y = (currentState.x * matrix._12) + (currentState.y * matrix._22) + (currentState.z * matrix._32);
	z = (currentState.x * matrix._13) + (currentState.y * matrix._23) + (currentState.z * matrix._33);

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::operator/(const float& factor)
{
	return Vector3D(x / factor, y / factor, z / factor);
}

// ------------------------------------------------------------------------------------------------

Vector3D& Vector3D::operator/=(const float& factor)
{
	x /= factor;
	y /= factor;
	z /= factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::operator/(const int& factor)
{
	return Vector3D(x / (float)factor, y / (float)factor, z / (float)factor);
}

// ------------------------------------------------------------------------------------------------

Vector3D& Vector3D::operator/=(const int& factor)
{
	x /= (float)factor;
	y /= (float)factor;
	z /= (float)factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

void     Vector3D::operator=(const Vector3D& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
}

// ------------------------------------------------------------------------------------------------

bool     Vector3D::operator==(const Vector3D other)
{
	if (x == other.x && y == other.y && z == other.z)
		return true;

	return false;
}

// ------------------------------------------------------------------------------------------------

float    Vector3D::Dot(const Vector3D& other)
{
	return (x * other.x) + (y * other.y) + (z * other.z);
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::Cross(const Vector3D& other)
{
	return Vector3D((y * other.z) - (z * other.y),
		            (z * other.x) - (x * other.z),
		            (x * other.y) - (y * other.x));
}

// ------------------------------------------------------------------------------------------------

Vector3D Vector3D::Normalised() // Retuns back the normalised vector - doesn't change the data stored internally
{
	float magnitude = Length();

	return Vector3D(x / magnitude, y / magnitude, z / magnitude);
}

// ------------------------------------------------------------------------------------------------

Vector3D& Vector3D::Normalise() // Changes the internal data and then returns itself back
{
	float magnitude = Length();

	x /= magnitude;
	y /= magnitude;
	z /= magnitude;

	return *this;
}

// ------------------------------------------------------------------------------------------------

float    Vector3D::Length() // Calculates the length of the vector
{
	return (float)sqrt(LengthSquared());
}

// ------------------------------------------------------------------------------------------------

float    Vector3D::LengthSquared() // Calculates the length squared of the vector
{
	return ((x * x) + (y * y) + (z * z));
}

// ------------------------------------------------------------------------------------------------

DirectX::XMFLOAT3 Vector3D::ConvertToDirectXFloat3()
{
	return DirectX::XMFLOAT3(x, y, z);
}

// ------------------------------------------------------------------------------------------------

// ===============================================================================================//
//------------------------------------------- Vector4D -------------------------------------------//
// ===============================================================================================//

Vector4D Vector4D::worldUp = Vector4D(0.0f, 1.0f, 0.0f, 0.0f);
Vector4D Vector4D::zero    = Vector4D(0.0f, 0.0f, 0.0f, 0.0f);

Vector4D::Vector4D()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, w(0.0f)
{

}

// ------------------------------------------------------------------------------------------------

Vector4D::Vector4D(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{

}

// ------------------------------------------------------------------------------------------------

Vector4D::Vector4D(const Vector4D& other)
	: x(other.x)
	, y(other.y)
	, z(other.z)
	, w(other.w)
{

}

// ------------------------------------------------------------------------------------------------

Vector4D Vector4D::operator+(const Vector4D& other)
{
	// Return back a new vector with the change applied
	return Vector4D(x + other.x, y + other.y, z + other.z, w + other.w);
}

// ------------------------------------------------------------------------------------------------

Vector4D& Vector4D::operator+=(const Vector4D& other)
{
	// Internally add on the value and then return the vector back
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector4D Vector4D::operator-(const Vector4D& other)
{
	return Vector4D(x - other.x, y - other.y, z - other.z, w - other.w);
}

// ------------------------------------------------------------------------------------------------

Vector4D& Vector4D::operator-=(const Vector4D& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector4D Vector4D::operator*(const float& factor)
{
	return Vector4D(x * factor, y * factor, z * factor, w * factor);
}

// ------------------------------------------------------------------------------------------------

Vector4D& Vector4D::operator*=(const float& factor)
{
	x *= factor;
	y *= factor;
	z *= factor;
	w *= factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector4D Vector4D::operator*(const int& factor)
{
	return Vector4D(x * (float)factor, y * (float)factor, z * (float)factor, w * (float)factor);
}

// ------------------------------------------------------------------------------------------------

Vector4D& Vector4D::operator*=(const int& factor)
{
	x *= (float)factor;
	y *= (float)factor;
	z *= (float)factor;
	w *= (float)factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector4D Vector4D::operator/(const float& factor)
{
	return Vector4D(x / factor, y / factor, z / factor, w / factor);
}

// ------------------------------------------------------------------------------------------------

Vector4D& Vector4D::operator/=(const float& factor)
{
	x /= factor;
	y /= factor;
	z /= factor;
	w /= factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

Vector4D Vector4D::operator/(const int& factor)
{
	return Vector4D(x / (float)factor, y / (float)factor, z / (float)factor, w / (float)factor);
}

// ------------------------------------------------------------------------------------------------

Vector4D& Vector4D::operator/=(const int& factor)
{
	x /= (float)factor;
	y /= (float)factor;
	z /= (float)factor;
	w /= (float)factor;

	return *this;
}

// ------------------------------------------------------------------------------------------------

void     Vector4D::operator=(const Vector4D& other)
{
	x = other.x;
	y = other.y;
	z = other.z;
	w = other.w;
}

// ------------------------------------------------------------------------------------------------

bool     Vector4D::operator==(const Vector4D other)
{
	if (x == other.x && y == other.y && z == other.z && w == other.w)
		return true;

	return false;
}

// ------------------------------------------------------------------------------------------------

float    Vector4D::Dot(const Vector4D& other)
{
	return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
}

// ------------------------------------------------------------------------------------------------

Vector4D Vector4D::Normalised() // Retuns back the normalised vector - doesn't change the data stored internally
{
	float magnitude = Length();

	return Vector4D(x / magnitude, y / magnitude, z / magnitude, w / magnitude);
}

// ------------------------------------------------------------------------------------------------

Vector4D& Vector4D::Normalise() // Changes the internal data and then returns itself back
{
	float magnitude = Length();

	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
	w /= magnitude;

	return *this;
}

// ------------------------------------------------------------------------------------------------

float    Vector4D::Length() // Calculates the length of the vector
{
	return (float)sqrt(LengthSquared());
}

// ------------------------------------------------------------------------------------------------

float    Vector4D::LengthSquared() // Calculates the length squared of the vector
{
	return ((x * x) + (y * y) + (z * z) + (w * w));
}

// ------------------------------------------------------------------------------------------------

DirectX::XMFLOAT4 Vector4D::ConvertToDirectXFloat4()
{
	return DirectX::XMFLOAT4(x, y, z, w);
}

// ------------------------------------------------------------------------------------------------

DirectX::XMFLOAT3X3 MatrixMaths::Identity3X3 = {1.0f, 0.0f, 0.0f, 
                                                0.0f, 1.0f, 0.0f,
                                                0.0f, 0.0f, 1.0f};

// ------------------------------------------------------------------------------------------------

DirectX::XMFLOAT4X4 MatrixMaths::Identity4X4 = { 1.0f, 0.0f, 0.0f, 0.0f,
												 0.0f, 1.0f, 0.0f, 0.0f, 
												 0.0f, 0.0f, 1.0f, 0.0f,
                                                 0.0f, 0.0f, 0.0f, 1.0f};

// ------------------------------------------------------------------------------------------------