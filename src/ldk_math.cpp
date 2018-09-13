
#define PI 3.14159265359L
#define RADIAN(n) (((n) * PI)/180.0f)

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

#include <math.h>
namespace ldk
{
	static const float _zero[] = {0,0,0,0};
	static const float _one[] = {1,1,1,1};

	//---------------------------------------------------------------------------
	// Vector2 
	//---------------------------------------------------------------------------
	float x, y;
	float Vec2::operator[](int32 n)
	{ LDK_ASSERT(n >= 0 && n < 2,"vector index should be between positive and within vector size");	
		return (&x)[n]; }

	bool Vec2::operator==(const Vec2& other)
	{
		return (int)this->x == (int)	other.x && (int)this->y == (int)other.y ;
	}

	bool Vec2::operator!=(const Vec2& other)
	{
		return (int)this->x != (int)other.x && 
			(int)this->y != (int)other.y ;
	}

	Vec2 Vec2::operator+(const Vec2& other)
	{
		Vec2 result;
		result.x = this->x + other.x;
		result.y = this->y + other.y;
		return result;
	}

	Vec2 Vec2::operator-(const Vec2& other)
	{
		Vec2 result;
		result.x = this->x - other.x;
		result.y = this->y - other.y;
		return result;
	}

	Vec2 Vec2::operator*(const Vec2& other)
	{

		Vec2 result;
		result.x = this->x * other.x;
		result.y = this->y * other.y;
		return result;
	}

	Vec2 Vec2::operator/(const Vec2& other)
	{
		Vec2 result;
		result.x = this->x / other.x;
		result.y = this->y / other.y;
		return result;
	}

	Vec2& Vec2::operator+=(const Vec2& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	Vec2& Vec2::operator-=(const Vec2& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	Vec2& Vec2::operator*=(const Vec2& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		return *this;
	}

	Vec2& Vec2::operator/=(const Vec2& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		return *this;
	}

	float Vec2::magnitude()
	{
		return sqrt(x * x + y * y);
	}

	Vec2 Vec2::normalize()
	{
		Vec2 result;
		float magnitude = sqrt(x * x + y * y);
		result.x = this->x / magnitude;
		result.y = this->y / magnitude;
		return result;
	}

	const Vec2& Vec2::one()
	{
		return (const Vec2&)_one;
	}

	const Vec2& Vec2::zero()
	{
		return (const Vec2& )_zero;
	}

	//---------------------------------------------------------------------------
	// Vector3
	//---------------------------------------------------------------------------
	float Vec3::operator[](int32 n)
	{ 
		LDK_ASSERT(n >= 0 && n < 3, "vector index should be between positive and within vector size");
		return (&x)[n]; 
	}

	bool Vec3::operator==(const Vec3& other)
	{
		return  (int)	this->x == (int)	other.x && 
			(int)	this->y == (int)	other.y && 
			(int)	this->z == (int)	other.z;
	}

	bool Vec3::operator!=(const Vec3& other)
	{
		return  (int)	this->x == (int)	other.x && 
			(int)	this->y != (int)	other.y && 
			(int)	this->z != (int)	other.z;
	}

	Vec3 Vec3::operator+(const Vec3& other)
	{
		Vec3 result;
		result.x = this->x + other.x;
		result.y = this->y + other.y;
		result.z = this->z + other.z;
		return result;
	}

	Vec3 Vec3::operator-(const Vec3& other)
	{
		Vec3 result;
		result.x = this->x - other.x;
		result.y = this->y - other.y;
		result.z = this->z - other.z;
		return result;
	}

	Vec3 Vec3::operator*(float scalar)
	{

		Vec3 result;
		result.x = this->x * scalar;
		result.y = this->y * scalar;
		result.z = this->z * scalar;
		return result;
	}

	Vec3 Vec3::operator*(const Vec3& other)
	{

		Vec3 result;
		result.x = this->x * other.x;
		result.y = this->y * other.y;
		result.z = this->z * other.z;
		return result;
	}

	Vec3 Vec3::operator/(const Vec3& other)
	{
		Vec3 result;
		result.x = this->x / other.x;
		result.y = this->y / other.y;
		result.z = this->z / other.z;
		return result;
	}

	Vec3& Vec3::operator+=(const Vec3& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	Vec3& Vec3::operator-=(const Vec3& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		return *this;
	}

	Vec3& Vec3::operator*=(const Vec3& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
		return *this;
	}

	Vec3& Vec3::operator*=(float scalar)
	{
		this->x *= scalar;
		this->y *= scalar;
		this->z *= scalar;
		return *this;
	}

	Vec3& Vec3::operator/=(const Vec3& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		this->z /= other.z;
		return *this;
	}

	float Vec3::magnitude()
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vec3 Vec3::normalize()
	{
		Vec3 result;
		float magnitude = sqrt(x * x + y * y + z * z);
		result.x = this->x / magnitude;
		result.y = this->y / magnitude;
		result.z = this->z / magnitude;
		return result;
	}

	const Vec3& Vec3::one()
	{
		return (const Vec3&)_one;
	}

	const Vec3& Vec3::zero()
	{
		return (const Vec3& )_zero;
	}


	//---------------------------------------------------------------------------
	// Vector4
	//---------------------------------------------------------------------------
	float Vec4::operator[](int32 n)
	{ 
		LDK_ASSERT(n >= 0 && n < 4, "vector index should be between positive and within vector size");
		return (&x)[n]; 
	}

	bool Vec4::operator==(const Vec4& other)
	{
		return (int)	this->x == (int)	other.x && (int)	this->y == (int)	other.y &&
			(int)	this->z == (int)	other.z && (int)	this->w == (int)	other.w;
	}

	bool Vec4::operator!=(const Vec4& other)
	{
		return (int)	this->x != (int)	other.x && 
			(int)	this->y != (int)	other.y &&
			(int)	this->z != (int)	other.z && 
			(int)	this->w != (int)	other.w;
	}

	Vec4 Vec4::operator+(const Vec4& other)
	{
		Vec4 result;
		result.x = this->x + other.x;
		result.y = this->y + other.y;
		result.z = this->z + other.z;
		result.w = this->w + other.w;
		return result;
	}

	Vec4 Vec4::operator-(const Vec4& other)
	{
		Vec4 result;
		result.x = this->x - other.x;
		result.y = this->y - other.y;
		result.z = this->z - other.z;
		result.w = this->w - other.w;
		return result;
	}

	Vec4 Vec4::operator*(const Vec4& other)
	{

		Vec4 result;
		result.x = this->x * other.x;
		result.y = this->y * other.y;
		result.z = this->z * other.z;
		result.w = this->w * other.w;
		return result;
	}

	Vec4 Vec4::operator/(const Vec4& other)
	{
		Vec4 result;
		result.x = this->x / other.x;
		result.y = this->y / other.y;
		result.z = this->z / other.z;
		result.w = this->w / other.w;
		return result;
	}

	Vec4& Vec4::operator+=(const Vec4& other)
	{
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		this->w += other.w;
		return *this;
	}

	Vec4& Vec4::operator-=(const Vec4& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;
		this->w -= other.w;
		return *this;
	}

	Vec4& Vec4::operator*=(const Vec4& other)
	{
		this->x *= other.x;
		this->y *= other.y;
		this->z *= other.z;
		this->w *= other.w;
		return *this;
	}

	Vec4& Vec4::operator/=(const Vec4& other)
	{
		this->x /= other.x;
		this->y /= other.y;
		this->z /= other.z;
		this->w /= other.w;
		return *this;
	}

	float Vec4::magnitude()
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	Vec4 Vec4::normalize()
	{
		Vec4 result;
		float magnitude = sqrt(x * x + y * y + z * z + w * w);
		result.x = this->x / magnitude;
		result.y = this->y / magnitude;
		result.z = this->z / magnitude;
		result.w = this->w / magnitude;
		return result;
	}

	const Vec4& Vec4::one()
	{
		return (const Vec4&)_one;
	}

	const Vec4& Vec4::zero()
	{
		return (const Vec4& )_zero;
	}

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
	Mat4::Mat4()
		{
			column[0] ={};
			column[1] ={};
			column[2] ={};
			column[3] ={};
			identity();
		}

		Mat4 Mat4::multiply(const Mat4& a, const Mat4& b)
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

		float Mat4::operator[](int32 n)
		{ 
			LDK_ASSERT(n >= 0 && n < 16, "vector index should be between positive and within vector size");
			return element[n];
		}

		//Note: This does NOT modify the original value. Instead, operates and returns a copy of the matrix.
		inline Mat4 Mat4::operator*(const Mat4& other)
		{
			return Mat4::multiply(*this, other);
		}

		inline Mat4& Mat4::operator*=(const Mat4& other)
		{
			*this = Mat4::multiply(*this, other);
			return *this;
		}

		Mat4& Mat4::translate(Vec3& translation)
		{
			return translate(translation.x, translation.y, translation.z);
		}

		Mat4& Mat4::translate(float x, float y, float z)
		{
			// Create a translation matrix
			Mat4 translationMatrix;
			translationMatrix.element[12] = x;
			translationMatrix.element[13] = y;
			translationMatrix.element[14] = z;
			*this *= translationMatrix;
			return *this;
		}

		Mat4& Mat4::scale(Vec3& factor)
		{
			return scale(factor.x, factor.y, factor.z);
		}

		Mat4& Mat4::scale(float x, float y, float z)
		{
			// Create a translation matrix
			Mat4 scaleMatrix;
			scaleMatrix.element[0] = x;
			scaleMatrix.element[5] = y;
			scaleMatrix.element[10] = z;
			*this *= scaleMatrix;
			return *this;
		}

		inline Mat4& Mat4::rotate(float rad)
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

		inline void Mat4::diagonal(float value)
		{
			element[0] = 1.0f;
			element[5] = 1.0f;
			element[10] = 1.0f;
			element[15] = 1.0f;
		}

		inline void Mat4::identity()
		{
			diagonal(1.0f);
		}

		void Mat4::orthographic(float left, float right, float bottom, float top, float near, float far)
		{
			float width = right - left;
			float height = top - bottom;
			float depth = far - near;

			diagonal(1.0f);
			//ldk::Mat4 ortho;
			LDK_ASSERT(width != 0, "Orthographic width can not be zero");
			LDK_ASSERT(height != 0, "Orthographic height can not be zero");
			LDK_ASSERT(depth != 0, "Orthographic depth can not be zero");

			// Diagonal
			element[0] = 2.0f / width;
			element[5] = 2.0f / height;
			element[10] = -(2.0f / depth);

			// Last column
			element[12] = -((right + left) / width);
			element[13] = -((top + bottom) / height);
			element[14] = -((far + near) / depth);
		}


	float lerp(float start, float end, float t) 
	{
		return (1 - t) * start + t * end;
	}

	Vec2 lerpVec2(const Vec2& start, const Vec2& end, float t)
	{
		Vec2 result;
		result.x = lerp(start.x, end.x, t);
		result.y = lerp(start.y, end.y, t);
		return result;
	}

	Vec3 lerpVec3(const Vec3& start, const Vec3& end, float t)
	{
		Vec3 result;
		result.x = lerp(start.x, end.x, t);
		result.y = lerp(start.y, end.y, t);
		result.z = lerp(start.z, end.z, t);
		return result;
	}

	Vec4 lerpVec4(const Vec4& start, const Vec4& end, float t)
	{
		Vec4 result;
		result.x = lerp(start.x, end.x, t);
		result.y = lerp(start.y, end.y, t);
		result.z = lerp(start.z, end.z, t);
		result.w = lerp(start.w, end.w, t);
		return result;
	}

} // ldk

