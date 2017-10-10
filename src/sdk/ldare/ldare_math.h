/**
 * win32_math.h
 * ldare math functions and structures
 */
#ifndef __LDARE_MATH__
#define __LDARE_MATH__

#include <math.h>
#define PI 3.14159265359L
#define RADIAN(n) (((n) * PI)/180.0f)

namespace ldare
{
	//---------------------------------------------------------------------------
	// Vector2 
	//---------------------------------------------------------------------------
	struct Vec2
	{
		float x, y;
		float operator[](int32 n)
		{ ASSERT(n >= 0 && n < 2,"vector index should be between positive and within vector size");	
			return (&x)[n]; }

		//Vec2& operator=(const Vec2& other)
		//{
		//	this->x = other.x;
		//	this->y = other.y;
		//	return *this;
		//}

		Vec2 operator+(const Vec2& other)
		{
			Vec2 result;
			result.x = this->x + other.x;
			result.y = this->y + other.y;
			return result;
		}

		Vec2 operator-(const Vec2& other)
		{
			Vec2 result;
			result.x = this->x - other.x;
			result.y = this->y - other.y;
			return result;
		}

		Vec2 operator*(const Vec2& other)
		{

			Vec2 result;
			result.x = this->x * other.x;
			result.y = this->y * other.y;
			return result;
		}

		Vec2 operator/(const Vec2& other)
		{
			Vec2 result;
			result.x = this->x / other.x;
			result.y = this->y / other.y;
			return result;
		}

		Vec2& operator+=(const Vec2& other)
		{
			this->x += other.x;
			this->y += other.y;
			return *this;
		}

		Vec2& operator-=(const Vec2& other)
		{
			this->x -= other.x;
			this->y -= other.y;
			return *this;
		}

		Vec2& operator*=(const Vec2& other)
		{
			this->x *= other.x;
			this->y *= other.y;
			return *this;
		}

		Vec2& operator/=(const Vec2& other)
		{
			this->x /= other.x;
			this->y /= other.y;
			return *this;
		}

	};

	//---------------------------------------------------------------------------
	// Vector3
	//---------------------------------------------------------------------------
	struct Vec3
	{
		float x, y, z;

		float operator[](int32 n)
		{ 
			ASSERT(n >= 0 && n < 3, "vector index should be between positive and within vector size");
			return (&x)[n]; 
		}

		Vec3 operator+(const Vec3& other)
		{
			Vec3 result;
			result.x = this->x + other.x;
			result.y = this->y + other.y;
			result.z = this->z + other.z;
			return result;
		}

		Vec3 operator-(const Vec3& other)
		{
			Vec3 result;
			result.x = this->x - other.x;
			result.y = this->y - other.y;
			result.z = this->z - other.z;
			return result;
		}

		Vec3 operator*(float scalar)
		{

			Vec3 result;
			result.x = this->x * scalar;
			result.y = this->y * scalar;
			result.z = this->z * scalar;
			return result;
		}

		Vec3 operator*(const Vec3& other)
		{

			Vec3 result;
			result.x = this->x * other.x;
			result.y = this->y * other.y;
			result.z = this->z * other.z;
			return result;
		}

		Vec3 operator/(const Vec3& other)
		{
			Vec3 result;
			result.x = this->x / other.x;
			result.y = this->y / other.y;
			result.z = this->z / other.z;
			return result;
		}

		Vec3& operator+=(const Vec3& other)
		{
			this->x += other.x;
			this->y += other.y;
			this->z += other.z;
			return *this;
		}

		Vec3& operator-=(const Vec3& other)
		{
			this->x -= other.x;
			this->y -= other.y;
			this->z -= other.z;
			return *this;
		}

		Vec3& operator*=(const Vec3& other)
		{
			this->x *= other.x;
			this->y *= other.y;
			this->z *= other.z;
			return *this;
		}

		Vec3& operator*=(float scalar)
		{
			this->x *= scalar;
			this->y *= scalar;
			this->z *= scalar;
			return *this;
		}

		Vec3& operator/=(const Vec3& other)
		{
			this->x /= other.x;
			this->y /= other.y;
			this->z /= other.z;
			return *this;
		}

	};

	//---------------------------------------------------------------------------
	// Vector4
	//---------------------------------------------------------------------------
	struct Vec4
	{
		float x, y, z, w;

		float operator[](int32 n)
		{ 
			ASSERT(n >= 0 && n < 4, "vector index should be between positive and within vector size");
			return (&x)[n]; 
		}

		Vec4 operator+(const Vec4& other)
		{
			Vec4 result;
			result.x = this->x + other.x;
			result.y = this->y + other.y;
			result.z = this->z + other.z;
			result.w = this->w + other.w;
			return result;
		}

		Vec4 operator-(const Vec4& other)
		{
			Vec4 result;
			result.x = this->x - other.x;
			result.y = this->y - other.y;
			result.z = this->z - other.z;
			result.w = this->w - other.w;
			return result;
		}

		Vec4 operator*(const Vec4& other)
		{

			Vec4 result;
			result.x = this->x * other.x;
			result.y = this->y * other.y;
			result.z = this->z * other.z;
			result.w = this->w * other.w;
			return result;
		}

		Vec4 operator/(const Vec4& other)
		{
			Vec4 result;
			result.x = this->x / other.x;
			result.y = this->y / other.y;
			result.z = this->z / other.z;
			result.w = this->w / other.w;
			return result;
		}

		Vec4& operator+=(const Vec4& other)
		{
			this->x += other.x;
			this->y += other.y;
			this->z += other.z;
			this->w += other.w;
			return *this;
		}

		Vec4& operator-=(const Vec4& other)
		{
			this->x -= other.x;
			this->y -= other.y;
			this->z -= other.z;
			this->w -= other.w;
			return *this;
		}

		Vec4& operator*=(const Vec4& other)
		{
			this->x *= other.x;
			this->y *= other.y;
			this->z *= other.z;
			this->w *= other.w;
			return *this;
		}

		Vec4& operator/=(const Vec4& other)
		{
			this->x /= other.x;
			this->y /= other.y;
			this->z /= other.z;
			this->w /= other.w;
			return *this;
		}

	};

	//---------------------------------------------------------------------------
	// Mat4
	//NOTE: Matrix layout in memory is column major
	// 0	4	8	12
	// 1	5	9	13
	// 2	6	10	14
	// 3	7	11	15
	// Considering Y is up and prositive Z is towards the screen:
	// Column 0 is right
	// Column 1 is up
	// Column 2 is back
	// Column 3 is position
	//---------------------------------------------------------------------------
	struct Mat4
	{
		union{
			float element[16];
			Vec4 column[4];
		};

		Mat4()
		{
			column[0] ={};
			column[1] ={};
			column[2] ={};
			column[3] ={};
			identity();
		}

		static Mat4 multiply(const Mat4& a, const Mat4& b)
		{
			Mat4 result;

			for (int32 column = 0; column < 4; column++)
			{
				for (int32 line = 0; line < 4; line++)
				{
					float sum = 0.0f;
					sum += a.element[line] * b.element[column * 4];
					sum += a.element[line + 1 * 4] * b.element[column * 4 + 1];
					sum += a.element[line + 2 * 4] * b.element[column * 4 + 2];
					sum += a.element[line + 3 * 4] * b.element[column * 4 + 3];
					result.element[line + column * 4] = sum;
				}
			}
			return result;
		}

		float operator[](int32 n)
		{ 
			ASSERT(n >= 0 && n < 16, "vector index should be between positive and within vector size");
			return element[n];
		}

		//Note: This does NOT modify the original value. Instead, operates and returns a copy of the matrix.
		inline Mat4 operator*(const Mat4& other)
		{
			return Mat4::multiply(*this, other);
		}

		inline Mat4& operator*=(const Mat4& other)
		{
			*this = Mat4::multiply(*this, other);
			return *this;
		}

		Mat4& translate(Vec3& translation)
		{
			return translate(translation.x, translation.y, translation.z);
		}

		Mat4& translate(float x, float y, float z)
		{
			// Create a translation matrix
			Mat4 translationMatrix;
			translationMatrix.element[12] = x;
			translationMatrix.element[13] = y;
			translationMatrix.element[14] = z;
			*this *= translationMatrix;
			return *this;
		}

		Mat4& scale(Vec3& factor)
		{
			return scale(factor.x, factor.y, factor.z);
		}

		Mat4& scale(float x, float y, float z)
		{
			// Create a translation matrix
			Mat4 scaleMatrix;
			scaleMatrix.element[0] = x;
			scaleMatrix.element[5] = y;
			scaleMatrix.element[10] = z;
			*this *= scaleMatrix;
			return *this;
		}

		inline Mat4& rotate(float rad)
		{
			Mat4 zRotationMatrix;
			float costTheta = cosf(rad);
			float sinTheta = sin(rad);
			zRotationMatrix.element[0] = costTheta;
			zRotationMatrix.element[1] = sinTheta;

			zRotationMatrix.element[4] = -sinTheta;
			zRotationMatrix.element[5] = costTheta;
			*this *= zRotationMatrix;
			return *this;
		}

		inline void diagonal(float value)
		{
			element[0] = 1.0f;
			element[5] = 1.0f;
			element[10] = 1.0f;
			element[15] = 1.0f;
		}

		inline void identity()
		{
			diagonal(1.0f);
		}

		void orthographic(float left, float right, float bottom, float top, float near, float far)
		{
			float width = right - left;
			float height = top - bottom;
			float depth = far - near;
			
			diagonal(1.0f);
			//ldare::Mat4 ortho;
			ASSERT(width != 0, "Orthographic width can not be zero");
			ASSERT(height != 0, "Orthographic height can not be zero");
			ASSERT(depth != 0, "Orthographic depth can not be zero");

			// Diagonal
			element[0] = 2.0f / width;
			element[5] = 2.0f / height;
			element[10] = -(2.0f / depth);

			// Last column
			element[12] = -((right+left) / width);
			element[13] = -((top - bottom) / height);
			element[14] = -((near - far) / depth);
		}

	};
} // ldare

#endif // __LDARE_MATH__

