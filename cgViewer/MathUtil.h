#ifndef MATHUTIL_H
#define MATHUTIL_H

#include "pch.h"

namespace Math
{

	const float PI = 3.14159265;


	struct Vector4f
	{
		float v0;
		float v1;
		float v2;
		float v3;

		Vector4f(const Vector4f& rhs)
		{
			v0 = rhs.v0;
			v1 = rhs.v1;
			v2 = rhs.v2;
			v3 = rhs.v3;
		}
		Vector4f(float a, float b, float c, float d)
		{
			v0 = a;
			v1 = b;
			v2 = c;
			v3 = d;
		}

		// dot product
		float operator % (Vector4f& rhs);
	};

	struct Vector3f
	{
		float x;
		float y;
		float z;

		Vector3f()
		{
		}

		Vector3f(float a, float b, float c)
		{
			x = a;
			y = b;
			z = c;
		}

		Vector3f operator + (Vector3f& rhs); 
		Vector3f operator - (Vector3f& rhs); 
		Vector3f operator * (Vector3f& rhs); // cross product

		//scaler operations
		Vector3f operator * (float scaler); 
		Vector3f operator / (float scaler);

		// comparison
		bool				operator == (Vector3f& rhs);
		bool				operator != (Vector3f& rhs);

		float				Length();
		void				NormalizeSelf();
	};

	class Matrix4f
	{
	public:
		Matrix4f(void);
		Matrix4f(const Matrix4f& rhs);

		void GetGLMultMatrix(float ret[])const;
		Vector4f operator * (Vector4f& rhs);
		Matrix4f operator * (Matrix4f& rhs);
		Matrix4f& operator *= (Matrix4f& rhs);
		Matrix4f operator + (Matrix4f& rhs);
		Matrix4f& operator += (Matrix4f& rhs);

		static Matrix4f GetIdentity();
		static Matrix4f GetZeroMatrix();

		float m00; float m01; float m02; float m03;
		float m10; float m11; float m12; float m13;
		float m20; float m21; float m22; float m23;
		float m30; float m31; float m32; float m33;
	};

	Matrix4f GetRotationMatrixX(float rad);
	Matrix4f GetRotationMatrixY(float rad);
	Matrix4f GetRotationMatrixZ(float rad);
	Matrix4f GetTranslateMatrix(float x, float y, float z);
	Matrix4f GetScaleMatrix(float x, float y, float z);
	float	 ConvertDegreesToRadians(float deg);

}

#endif
