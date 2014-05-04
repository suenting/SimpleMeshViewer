#include "MathUtil.h"

namespace Math
{

	float Vector4f::operator % (Vector4f& rhs)
	{
		return (v0*rhs.v0) + (v1*rhs.v1) + (v2*rhs.v2) + (v3*rhs.v3);
	}

	bool Vector3f::operator == (Vector3f& rhs)
	{
		if( (x == rhs.x) &&
			(y == rhs.y) &&
			(z == rhs.z))
		{
			return true;
		}
		return false;
	}

	bool Vector3f::operator != (Vector3f& rhs)
	{
		if( (x == rhs.x) &&
			(y == rhs.y) &&
			(z == rhs.z))
		{
			return false;
		}
		return true;
	}

	Vector3f Vector3f::operator * (Vector3f& rhs)
	{
		Vector3f out;
		out.x = ((y*rhs.z) - (z*rhs.y));
		out.y = ((z*rhs.x) - (x*rhs.z));
		out.z = ((x*rhs.y) - (y*rhs.x));
		return out;
	}

	Vector3f Vector3f::operator + (Vector3f& rhs)
	{
		Vector3f out;
		out.x = x+rhs.x;
		out.y = y+rhs.y;
		out.z = z+rhs.z;
		return out;
	}

	Vector3f Vector3f::operator / (float scaler)
	{
		Vector3f out;
		out.x = x/scaler;
		out.y = y/scaler;
		out.z = z/scaler;
		return out;
	}

	Vector3f Vector3f::operator * (float scaler)
	{
		Vector3f out;
		out.x = x*scaler;
		out.y = y*scaler;
		out.z = z*scaler;
		return out;
	}

	Vector3f Vector3f::operator - (Vector3f& rhs)
	{
		Vector3f out;
		out.x = x-rhs.x;
		out.y = y-rhs.y;
		out.z = z-rhs.z;
		return out;
	}

	float Vector3f::Length()
	{
		return sqrt((x*x)+(y*y)+(z*z));
	}

	void Vector3f::NormalizeSelf()
	{
		(*this)/Length();
	}

	// constructor
	Matrix4f::Matrix4f()
	{
	}

	// copy constructor
	Matrix4f::Matrix4f(const Matrix4f &rhs)
	{
		m00 = rhs.m00; m01 = rhs.m01; m02 = rhs.m02; m03 = rhs.m03;
		m10 = rhs.m10; m11 = rhs.m11; m12 = rhs.m12; m13 = rhs.m13;
		m20 = rhs.m20; m21 = rhs.m21; m22 = rhs.m22; m23 = rhs.m23;
		m30 = rhs.m30; m31 = rhs.m31; m32 = rhs.m32; m33 = rhs.m33;
	}

	Vector4f Matrix4f::operator * (Vector4f& rhs)
	{
		//Vector4f out;
		float v0, v1, v2, v3;
		v0 = m00*rhs.v0 + m01*rhs.v1 + m02*rhs.v2 + m03*rhs.v3;
		v1 = m10*rhs.v0 + m11*rhs.v1 + m12*rhs.v2 + m13*rhs.v3;
		v2 = m20*rhs.v0 + m21*rhs.v1 + m22*rhs.v2 + m23*rhs.v3;
		v3 = m30*rhs.v0 + m31*rhs.v1 + m32*rhs.v2 + m33*rhs.v3;
		Vector4f out(v0,v1,v2,v3);
		return out;

	}

	Matrix4f Matrix4f::operator * (Matrix4f& rhs)
	{
		Matrix4f ret;
		ret.m00 = (m00 * rhs.m00)+(m01 * rhs.m10)+(m02 * rhs.m20)+(m03 * rhs.m30);
		ret.m01 = (m00 * rhs.m01)+(m01 * rhs.m11)+(m02 * rhs.m21)+(m03 * rhs.m31);
		ret.m02 = (m00 * rhs.m02)+(m01 * rhs.m12)+(m02 * rhs.m22)+(m03 * rhs.m32);
		ret.m03 = (m00 * rhs.m03)+(m01 * rhs.m13)+(m02 * rhs.m23)+(m03 * rhs.m33);
		ret.m10 = (m10 * rhs.m00)+(m11 * rhs.m10)+(m12 * rhs.m20)+(m13 * rhs.m30);
		ret.m11 = (m10 * rhs.m01)+(m11 * rhs.m11)+(m12 * rhs.m21)+(m13 * rhs.m31);
		ret.m12 = (m10 * rhs.m02)+(m11 * rhs.m12)+(m12 * rhs.m22)+(m13 * rhs.m32);
		ret.m13 = (m10 * rhs.m03)+(m11 * rhs.m13)+(m12 * rhs.m23)+(m13 * rhs.m33);
		ret.m20 = (m20 * rhs.m00)+(m21 * rhs.m10)+(m22 * rhs.m20)+(m23 * rhs.m30);
		ret.m21 = (m20 * rhs.m01)+(m21 * rhs.m11)+(m22 * rhs.m21)+(m23 * rhs.m31);
		ret.m22 = (m20 * rhs.m02)+(m21 * rhs.m12)+(m22 * rhs.m22)+(m23 * rhs.m32);
		ret.m23 = (m20 * rhs.m03)+(m21 * rhs.m13)+(m22 * rhs.m23)+(m23 * rhs.m33);
		ret.m30 = (m30 * rhs.m00)+(m31 * rhs.m10)+(m32 * rhs.m20)+(m33 * rhs.m30);
		ret.m31 = (m30 * rhs.m01)+(m31 * rhs.m11)+(m32 * rhs.m21)+(m33 * rhs.m31);
		ret.m32 = (m30 * rhs.m02)+(m31 * rhs.m12)+(m32 * rhs.m22)+(m33 * rhs.m32);
		ret.m33 = (m30 * rhs.m03)+(m31 * rhs.m13)+(m32 * rhs.m23)+(m33 * rhs.m33);
		return ret;
	}

	Matrix4f& Matrix4f::operator *= (Matrix4f& rhs)
	{
		Matrix4f temp = *this;
		
		m00 = (temp.m00 * rhs.m00)+(temp.m01 * rhs.m10)+(temp.m02 * rhs.m20)+(temp.m03 * rhs.m30);
		m01 = (temp.m00 * rhs.m01)+(temp.m01 * rhs.m11)+(temp.m02 * rhs.m21)+(temp.m03 * rhs.m31);
		m02 = (temp.m00 * rhs.m02)+(temp.m01 * rhs.m12)+(temp.m02 * rhs.m22)+(temp.m03 * rhs.m32);
		m03 = (temp.m00 * rhs.m03)+(temp.m01 * rhs.m13)+(temp.m02 * rhs.m23)+(temp.m03 * rhs.m33);
		m10 = (temp.m10 * rhs.m00)+(temp.m11 * rhs.m10)+(temp.m12 * rhs.m20)+(temp.m13 * rhs.m30);
		m11 = (temp.m10 * rhs.m01)+(temp.m11 * rhs.m11)+(temp.m12 * rhs.m21)+(temp.m13 * rhs.m31);
		m12 = (temp.m10 * rhs.m02)+(temp.m11 * rhs.m12)+(temp.m12 * rhs.m22)+(temp.m13 * rhs.m32);
		m13 = (temp.m10 * rhs.m03)+(temp.m11 * rhs.m13)+(temp.m12 * rhs.m23)+(temp.m13 * rhs.m33);
		m20 = (temp.m20 * rhs.m00)+(temp.m21 * rhs.m10)+(temp.m22 * rhs.m20)+(temp.m23 * rhs.m30);
		m21 = (temp.m20 * rhs.m01)+(temp.m21 * rhs.m11)+(temp.m22 * rhs.m21)+(temp.m23 * rhs.m31);
		m22 = (temp.m20 * rhs.m02)+(temp.m21 * rhs.m12)+(temp.m22 * rhs.m22)+(temp.m23 * rhs.m32);
		m23 = (temp.m20 * rhs.m03)+(temp.m21 * rhs.m13)+(temp.m22 * rhs.m23)+(temp.m23 * rhs.m33);
		m30 = (temp.m30 * rhs.m00)+(temp.m31 * rhs.m10)+(temp.m32 * rhs.m20)+(temp.m33 * rhs.m30);
		m31 = (temp.m30 * rhs.m01)+(temp.m31 * rhs.m11)+(temp.m32 * rhs.m21)+(temp.m33 * rhs.m31);
		m32 = (temp.m30 * rhs.m02)+(temp.m31 * rhs.m12)+(temp.m32 * rhs.m22)+(temp.m33 * rhs.m32);
		m33 = (temp.m30 * rhs.m03)+(temp.m31 * rhs.m13)+(temp.m32 * rhs.m23)+(temp.m33 * rhs.m33);
		
		return *this;
	}

	Matrix4f Matrix4f::operator + (Matrix4f& rhs)
	{
		Matrix4f self = *this;
		Matrix4f ret;
		ret.m00 = self.m00+rhs.m00; ret.m01 = self.m01+rhs.m01; ret.m02 = self.m02+rhs.m02; ret.m03 = self.m03+rhs.m03;
		ret.m10 = self.m10+rhs.m10; ret.m11 = self.m11+rhs.m11; ret.m12 = self.m12+rhs.m12; ret.m13 = self.m13+rhs.m13;
		ret.m20 = self.m20+rhs.m20; ret.m21 = self.m21+rhs.m21; ret.m22 = self.m22+rhs.m22; ret.m23 = self.m23+rhs.m23;
		ret.m30 = self.m30+rhs.m30; ret.m31 = self.m31+rhs.m31; ret.m32 = self.m32+rhs.m32; ret.m33 = self.m33+rhs.m33;
		return ret;
	}

	Matrix4f& Matrix4f::operator += (Matrix4f& rhs)
	{
		// temp var
		Matrix4f self = *this;
		m00 = self.m00+rhs.m00; m01 = self.m01+rhs.m01; m02 = self.m02+rhs.m02; m03 = self.m03+rhs.m03;
		m10 = self.m10+rhs.m10; m11 = self.m11+rhs.m11; m12 = self.m12+rhs.m12; m13 = self.m13+rhs.m13;
		m20 = self.m20+rhs.m20; m21 = self.m21+rhs.m21; m22 = self.m22+rhs.m22; m23 = self.m23+rhs.m23;
		m30 = self.m30+rhs.m30; m31 = self.m31+rhs.m31; m32 = self.m32+rhs.m32; m33 = self.m33+rhs.m33;

		return *this;
	}

	// note: ret must be size 16
	void  Matrix4f::GetGLMultMatrix(float ret[])const
	{
		ret[0] = m00;
		ret[1] = m10;
		ret[2] = m20;
		ret[3] = m30;
		ret[4] = m01;
		ret[5] = m11;
		ret[6] = m21;
		ret[7] = m31;
		ret[8] = m02;
		ret[9] = m12;
		ret[10] = m22;
		ret[11] = m32;
		ret[12] = m03;
		ret[13] = m13;
		ret[14] = m23;
		ret[15] = m33;
	}

	Matrix4f Matrix4f::GetIdentity()
	{
		Matrix4f ret;
		ret.m00 = 1; ret.m01 = 0; ret.m02 = 0; ret.m03 = 0;
		ret.m10 = 0; ret.m11 = 1; ret.m12 = 0; ret.m13 = 0;
		ret.m20 = 0; ret.m21 = 0; ret.m22 = 1; ret.m23 = 0;
		ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 1;
		return ret;
	}

	Matrix4f Matrix4f::GetZeroMatrix()
	{
		Matrix4f ret;
		ret.m00 = 0; ret.m01 = 0; ret.m02 = 0; ret.m03 = 0;
		ret.m10 = 0; ret.m11 = 0; ret.m12 = 0; ret.m13 = 0;
		ret.m20 = 0; ret.m21 = 0; ret.m22 = 0; ret.m23 = 0;
		ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 0;
		return ret;
	}

	Matrix4f GetRotationMatrixX(float rad)
	{
		Matrix4f ret;
		float s = sin(rad);
		float c = cos(rad);
		ret.m00 = 1; ret.m01 = 0; ret.m02 = 0; ret.m03 = 0;
		ret.m10 = 0; ret.m11 = c; ret.m12 = -1*s; ret.m13 = 0;
		ret.m20 = 0; ret.m21 = s; ret.m22 = c; ret.m23 = 0;
		ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 1;

		return ret;
	}
	Matrix4f GetRotationMatrixY(float rad)
	{
		Matrix4f ret;
		float s = sin(rad);
		float c = cos(rad);
		ret.m00 = c; ret.m01 = 0; ret.m02 = s; ret.m03 = 0;
		ret.m10 = 0; ret.m11 = 1; ret.m12 = 0; ret.m13 = 0;
		ret.m20 = -1*s; ret.m21 = 0; ret.m22 = c; ret.m23 = 0;
		ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 1;

		return ret;
	}
	Matrix4f GetRotationMatrixZ(float rad)
	{
		Matrix4f ret;
		float s = sin(rad);
		float c = cos(rad);
		ret.m00 = c; ret.m01 = -1*s; ret.m02 = 0; ret.m03 = 0;
		ret.m10 = s; ret.m11 = c; ret.m12 = 0; ret.m13 = 0;
		ret.m20 = 0; ret.m21 = 0; ret.m22 = 1; ret.m23 = 0;
		ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 1;

		return ret;
	}
	Matrix4f GetTranslateMatrix(float x, float y, float z)
	{
		Matrix4f ret;
		ret.m00 = 1; ret.m01 = 0; ret.m02 = 0; ret.m03 = x;
		ret.m10 = 0; ret.m11 = 1; ret.m12 = 0; ret.m13 = y;
		ret.m20 = 0; ret.m21 = 0; ret.m22 = 1; ret.m23 = z;
		ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 1;
		return ret;
	}
	Matrix4f GetScaleMatrix(float x, float y, float z)
	{
		Matrix4f ret;
		ret.m00 = x; ret.m01 = 0; ret.m02 = 0; ret.m03 = 0;
		ret.m10 = 0; ret.m11 = y; ret.m12 = 0; ret.m13 = 0;
		ret.m20 = 0; ret.m21 = 0; ret.m22 = z; ret.m23 = 0;
		ret.m30 = 0; ret.m31 = 0; ret.m32 = 0; ret.m33 = 1;
		return ret;
	}
	float ConvertDegreesToRadians(float deg)
	{
		return (deg*(Math::PI/180));
	}
}
