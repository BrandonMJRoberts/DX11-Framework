#ifndef _COMMON_MATHS_H_
#define _COMMON_MATHS_H_

#include <DirectXMath.h>

// ---------------------------------------------------------------------------------------------------------------------------- //

struct Vector2D final
{
public:
	Vector2D();
	Vector2D(float x, float y);
	Vector2D(const Vector2D& copy);


	//------------------------ Overloaded operator functions ------------------------//
	Vector2D  operator+(const Vector2D& other);  // Plus
	Vector2D& operator+=(const Vector2D& other); // Plus equals

	Vector2D  operator-(const Vector2D& other);  // Minus
	Vector2D& operator-=(const Vector2D& other); // Minus equals

	Vector2D  operator*(const float& factor);    // Multiply (float)
	Vector2D& operator*=(const float& factor);   // Multiply equals (float)

	Vector2D  operator*(const int& factor);      // Multiply (int) (casts the factor to a float)
	Vector2D& operator*=(const int& factor);     // Multiply equals (int) (casts the factor to a float)

	Vector2D  operator/(const float& factor);    // Divide (float)
	Vector2D& operator/=(const float& factor);   // Divide equals (float)

	Vector2D  operator/(const int& factor);      // Divide (int) (casts the factor to a float)
	Vector2D& operator/=(const int& factor);     // Divide equals (int) (casts the factor to a float)

	void     operator=(const Vector2D& other);  // Setter

	bool     operator==(const Vector2D other);

	//------------------------ Functionality functions ------------------------//
	float    Dot(const Vector2D& other);

	Vector2D Normalised(); // Retuns back the normalised vector - doesn't change the data stored internally
	Vector2D& Normalise();  // Changes the internal data and then returns itself back

	float    Length();        // Calculates the length of the vector
	float    LengthSquared(); // Calculates the length squared of the vector

	DirectX::XMFLOAT2 ConvertToDirectXFloat2();

	float x, y;
};

// ---------------------------------------------------------------------------------------------------------------------------- //

struct Vector3D final
{
public:
	Vector3D();
	Vector3D(float x, float y, float z);
	Vector3D(const Vector3D& copy);


	//------------------------ Overloaded operator functions ------------------------//
	Vector3D operator+(const Vector3D& other);  // Plus
	Vector3D& operator+=(const Vector3D& other); // Plus equals

	Vector3D operator-(const Vector3D& other);  // Minus
	Vector3D& operator-=(const Vector3D& other); // Minus equals

	Vector3D operator*(const float& factor);    // Multiply (float)
	Vector3D& operator*=(const float& factor);   // Multiply equals (float)

	Vector3D operator*(const int& factor);      // Multiply (int)
	Vector3D& operator*=(const int& factor);     // Multiply equals (int)

	Vector3D operator/(const float& factor);    // Divide (float)
	Vector3D& operator/=(const float& factor);   // Divide equals (float)

	Vector3D operator/(const int& factor);      // Divide (int)
	Vector3D& operator/=(const int& factor);     // Divide equals (int)

	void     operator=(const Vector3D& other);  // Setter

	bool     operator==(const Vector3D other);

	//------------------------ Functionality functions ------------------------//
	float    Dot(const Vector3D& other);
	Vector3D Cross(const Vector3D& other);

	Vector3D Normalised(); // Retuns back the normalised vector - doesn't change the data stored internally
	Vector3D& Normalise();  // Changes the internal data and then returns itself back

	float    Length();        // Calculates the length of the vector
	float    LengthSquared(); // Calculates the length squared of the vector

	DirectX::XMFLOAT3 ConvertToDirectXFloat3();


	float x, y, z;
};

// ---------------------------------------------------------------------------------------------------------------------------- //

struct Vector4D final
{
public:
	Vector4D();
	Vector4D(float x, float y, float z, float w);
	Vector4D(const Vector4D& copy);


	//------------------------ Overloaded operator functions ------------------------//
	Vector4D operator+(const Vector4D& other);  // Plus
	Vector4D& operator+=(const Vector4D& other); // Plus equals

	Vector4D operator-(const Vector4D& other);  // Minus
	Vector4D& operator-=(const Vector4D& other); // Minus equals

	Vector4D operator*(const float& factor);    // Multiply (float)
	Vector4D& operator*=(const float& factor);   // Multiply equals (float)

	Vector4D operator*(const int& factor);      // Multiply (int)
	Vector4D& operator*=(const int& factor);     // Multiply equals (int)

	Vector4D operator/(const float& factor);    // Divide (float)
	Vector4D& operator/=(const float& factor);   // Divide equals (float)

	Vector4D operator/(const int& factor);      // Divide (int)
	Vector4D& operator/=(const int& factor);     // Divide equals (int)

	void     operator=(const Vector4D& other);  // Setter

	bool     operator==(const Vector4D other);

	//------------------------ Functionality functions ------------------------//
	float    Dot(const Vector4D& other);

	Vector4D Normalised();  // Retuns back the normalised vector - doesn't change the data stored internally
	Vector4D& Normalise();  // Changes the internal data and then returns itself back

	float    Length();        // Calculates the length of the vector
	float    LengthSquared(); // Calculates the length squared of the vector

	DirectX::XMFLOAT4 ConvertToDirectXFloat4();


	float x, y, z, w;
};

// ---------------------------------------------------------------------------------------------------------------------------- //

#endif