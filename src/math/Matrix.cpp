#include "Matrix.h"

SE_BEGIN

template<int m, int n, typename T>
Matrix<m, n, T> Matrix<m, n, T>::identity;
template<int m, int n, typename T>
bool Matrix<m, n, T>::identity_inited = false;

Matrix4 RotateMatrix(float a, float x, float y, float z){
	Vector3 v(x, y, z);
	if (a == 0 || v.IsZero()) return Matrix4::Identity();

	Matrix4 m;
	a *= (float)DEG_TO_RAD;

	float l = v.Length2();
	if (l != 1){
		l = sqrtf(l);
		x /= l;
		y /= l;
		z /= l;
	}

	float c = cos(a);
	float s = sin(a);
	float nc = 1 - c;
	
	/*m[0][0] = c + nc*x*x;
	m[0][1] = nc*x*y - s*z;
	m[0][2] = nc*x*z + s*y;
	m[0][3] = 0;
	
	m[1][0] = nc*y*x + s*z;
	m[1][1] = c + nc*y*y;
	m[1][2] = nc*y*z - s*x;
	m[1][3] = 0;
	
	m[2][0] = nc*z*x - s*y;
	m[2][1] = nc*z*y + s*x;
	m[2][2] = c + nc*z*z;
	m[2][3] = 0;
	
	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
	m[3][3] = 1;*/
	
	m.data[0] = c + nc*x*x;
	m.data[1] = nc*x*y - s*z;
	m.data[2] = nc*x*z + s*y;
	m.data[3] = 0;
	
	m.data[4] = nc*y*x + s*z;
	m.data[5] = c + nc*y*y;
	m.data[6] = nc*y*z - s*x;
	m.data[7] = 0;
	
	m.data[8] = nc*z*x - s*y;
	m.data[9] = nc*z*y + s*x;
	m.data[10] = c + nc*z*z;
	m.data[11] = 0;
	
	m.data[12] = 0;
	m.data[13] = 0;
	m.data[14] = 0;
	m.data[15] = 1;
	
	return m;
}

Matrix4 OrthoMatrix(float left, float right, float bottom, float top, float _near, float _far){
	Matrix4 m(Matrix4::Identity());

	/*m[0][0] = 2.0f / ( right - left  );
	m[3][0] = -( right + left ) / ( right - left );

	m[1][1] = 2.0f / ( top - bottom );
	m[3][1] = -( top + bottom ) / ( top - bottom );

	m[2][2] = -2.0f / ( _far - _near );
	m[3][2] = -( _far + _near ) / ( _far - _near );*/
	
	m.data[0] = 2.0f / ( right - left  );
	m.data[12] = -( right + left ) / ( right - left );

	m.data[5] = 2.0f / ( top - bottom );
	m.data[13] = -( top + bottom ) / ( top - bottom );

	m.data[10] = -2.0f / ( _far - _near );
	m.data[14] = -( _far + _near ) / ( _far - _near );

	return m;
}

Matrix4 FrustumMatrix(float left, float right, float bottom, float top, float _near, float _far){
	Matrix4 m(Matrix4::Identity());

	/*m[0][0] = 2.0f * _near / (right - left);
	m[1][1] = 2.0f * _near / (top - bottom);
	m[2][0] = (right + left) / (right - left);
	m[2][1] = (top + bottom) / (top - bottom);
	m[2][2] = -(_far + _near) / (_far - _near);
	m[2][3] = -1.0f;
	m[3][2] = -2.0f * _far * _near / (_far - _near);
	m[3][3] = 0.0f;*/
	
	m.data[0] = 2.0f * _near / (right - left);
	m.data[5] = 2.0f * _near / (top - bottom);
	m.data[8] = (right + left) / (right - left);
	m.data[9] = (top + bottom) / (top - bottom);
	m.data[10] = -(_far + _near) / (_far - _near);
	m.data[11] = -1.0f;
	m.data[14] = -2.0f * _far * _near / (_far - _near);
	m.data[15] = 0.0f;

	return m;
}

Matrix4 PerspectiveMatrix(float fovy, float aspect, float _near, float _far){//, float screen_orientation){
	Matrix4 m(Matrix4::Identity());
	
	float d = _far - _near,
		  r = (fovy * 0.5f) * (float)DEG_TO_RAD,
		  s = sinf( r ),
		  c = cosf( r ) / s;

    /*m[0][0] = c / aspect;
    m[1][1] = c;
    m[2][2] = -( _far + _near ) / d;
    m[2][3] = -1.0f;
    m[3][2] = -2.0f * _near * _far / d;
    m[3][3] =  0.0f;*/
	
	m.data[0] = c / aspect;
    m.data[5] = c;
    m.data[10] = -( _far + _near ) / d;
    m.data[11] = -1.0f;
    m.data[14] = -2.0f * _near * _far / d;
    m.data[15] =  0.0f;
	
	return m;
	//if( screen_orientation ) GFX_rotate( screen_orientation, 0.0f, 0.0f, 1.0f );
}

SE_END
