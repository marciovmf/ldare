#ifndef _LDK_MATH_H_
#define _LDK_MATH_H_

#define PI 3.14159265359L
#define DEG2RAD (PI/180f)
#define RADIAN(n) (((n) * PI)/180.0f)

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

namespace ldk
{

  /// @defgroup Math Math
  /// @details Math Functions
  /// @{
	struct Rect
	{
		float x, y, w, h;
	};

	//---------------------------------------------------------------------------
	// Vector2 
	//---------------------------------------------------------------------------
	struct Vec2
	{
		float x, y;
		float operator[](int32 n);

		bool operator==(const Vec2& other);
		
		bool operator!=(const Vec2& other);

		Vec2 operator+(const Vec2& other);

		Vec2 operator-(const Vec2& other);

		Vec2 operator*(const Vec2& other);

		Vec2 operator/(const Vec2& other);

		Vec2& operator+=(const Vec2& other);

		Vec2& operator-=(const Vec2& other);

		Vec2& operator*=(const Vec2& other);

		Vec2& operator/=(const Vec2& other);

		float magnitude();
		
		Vec2 normalize();

		static const Vec2& one();

		static const Vec2& zero();
	};

	//---------------------------------------------------------------------------
	// Vector3
	//---------------------------------------------------------------------------
	struct LDK_API Vec3
	{
		float x, y, z;

		float operator[](int32 n);

		bool operator==(const Vec3& other);
		
		bool operator!=(const Vec3& other);

		Vec3 operator+(const Vec3& other);

		Vec3 operator-(const Vec3& other);

		Vec3 operator*(float scalar);

		Vec3 operator*(const Vec3& other);

		Vec3 operator/(const Vec3& other);

		Vec3& operator+=(const Vec3& other);

		Vec3& operator-=(const Vec3& other);

		Vec3& operator*=(const Vec3& other);

		Vec3& operator*=(float scalar);

		Vec3& operator/=(const Vec3& other);

		float magnitude();
		
		Vec3 normalize();

		static const Vec3& one();

		static const Vec3& zero();

	};

	//---------------------------------------------------------------------------
	// Vector4
	//---------------------------------------------------------------------------
	struct LDK_API Vec4
	{
		float x, y, z, w;

		float operator[](int32 n);

		bool operator==(const Vec4& other);
		
		bool operator!=(const Vec4& other);

		Vec4 operator+(const Vec4& other);

		Vec4 operator-(const Vec4& other);

		Vec4 operator*(const Vec4& other);

		Vec4 operator/(const Vec4& other);

		Vec4& operator+=(const Vec4& other);

		Vec4& operator-=(const Vec4& other);

		Vec4& operator*=(const Vec4& other);

		Vec4& operator/=(const Vec4& other);

		float magnitude();
		
		Vec4 normalize();

		static const Vec4& one();

		static const Vec4& zero();
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
	struct LDK_API Mat4
	{
		union{
			float element[16];
			Vec4 column[4];
		};

		Mat4();

		static Mat4 multiply(const Mat4& a, const Mat4& b);

		float operator[](int32 n);

		//Note: This does NOT modify the original value. Instead, operates and returns a copy of the matrix.
		inline Mat4 operator*(const Mat4& other);

		inline Mat4& operator*=(const Mat4& other);

		Mat4& translate(Vec3& translation);

		Mat4& translate(float x, float y, float z);

		Mat4& scale(Vec3& factor);

		Mat4& scale(float x, float y, float z);

    void rotate(float x, float y, float z, float angle_rad);

		inline void diagonal(float value);

		inline void identity();

		void orthographic(float left, float right, float bottom, float top, float near, float far);

		void perspective(float fov, float ratio, float near, float far);

	};

	LDK_API float lerp(float start, float end, float t); 

	LDK_API Vec2 lerpVec2(const Vec2& start, const Vec2& end, float t);

	LDK_API Vec3 lerpVec3(const Vec3& start, const Vec3& end, float t);
	
	LDK_API Vec4 lerpVec4(const Vec4& start, const Vec4& end, float t);

    /// @}
} // ldk

#endif // _LDK_MATH_H_

