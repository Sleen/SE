#pragma once

#include "Macro.h"
#include "Tuple.h"
#include "Vector.h"

SE_BEGIN

template<int m, int n, typename T>
class Matrix : public Tuple<m*n, T>
{
protected:
	enum{N = m*n};

	static Matrix identity;
	static bool identity_inited;
public:
	using Tuple<N, T>::data;
	
	static const Matrix& Identity(){
		static_assert(m==n, "Identity matrix must be square !");
		if (!identity_inited){
			identity_inited = true;
			for(int i=0;i<m;++i)
				identity[i][i] = 1;
		}
		return identity;
	}
	
	Matrix& LoadIdentity(){
		static_assert(m==n, "Identity matrix must be square !");
		memset(this, 0, sizeof(Matrix));
		for(int i=0;i<m;++i)
			(*this)[i][i] = 1;
		return *this;
	}

	Matrix() {}

    Matrix(const T* v) : Tuple<N, T>(v) {}
	
    Matrix(const Matrix& v) : Tuple<m*n, T>(v) {}
	
    Matrix(const Tuple<m*n, T>& t) : Tuple<m*n, T>(t) {}
	
	T* operator[](int i){
		return &data[i*n];
	}
	
	const T* operator[](int i) const{
		return &data[i*n];
	}
	
	void SetRow (int row, const Vector<n, T>& t){
		T *d = data + row*n;
		for(int i=0;i<n;++i)
			d[i] = t[i];
	}
	
    Vector<n,T> GetRow (int row) const{
		return Vector<n, T>(data + row*n);
	}
	
    void SetColumn (int col, const Vector<m, T>& t){
		for (int r=0, i=col;r<m;++r, i+=n)
			data[i] = t[r];
	}
	
    Vector<m, T> GetColumn (int col) const{
		Vector<m, T> ret;
		for (int r=0, i=col;r<m;++r, i+=n)
			ret[r] = data[i];
		return ret;
	}
	
    Matrix operator+ (const Matrix& v) const{
		Matrix r(*this);
		for(int i=0;i<N;++i)
			r.data[i] += v.data[i];
		return r;
	}
	
    Matrix operator- (const Matrix& v) const{
		Matrix r(*this);
		for(int i=0;i<N;++i)
			r.data[i] -= v.data[i];
		return r;
	}
	
    Matrix operator* (T s) const{
		Matrix r(*this);
		for(int i=0;i<N;++i)
			r.data[i] *= s;
		return r;
	}
	
	template<int p>
    Matrix<m, p, T> operator* (const Matrix<n, p, T>& a) const{
		Matrix<m, p, T> r;
		for(int i=0;i<m;++i)
			for(int j=0;j<p;++j)
			{
				T t = 0;
				for(int k=0;k<n;++k)
					//t += (*this)[i][k]*a[k][j];
					t += data[i*n+k]*a.data[k*p+j];
				r[i][j] = t;
			}
		return r;
	}
	
    /*Vector<m, T> operator* (const Vector<n, T>& a) const{
		Vector<m, T> r;
		for(int i=0;i<m;++i)
			for(int k=0;k<n;++k)
				r[i] += (*this)[i][k]*a[k];
		return r;
	}*/
	
	// vec * mat
	Vector<n, T> operator* (const Vector<m, T>& a) const{
		Vector<n, T> r;
		for(int i=0;i<n;++i)
			for(int j=0;j<m;++j)
				//r[i] += a[j] * (*this)[j][i];
				r[i] += a[j] * data[j*n+i];
		return r;
	}
	
	friend Matrix operator* (T s, const Matrix& v){
		return v * s;
	}

    Matrix operator/ (T s) const{
		Matrix r(*this);
		for(int i=0;i<N;++i)
			r.data[i] /= s;
		return r;
	}
	
    Matrix operator- () const{
		Matrix r(*this);
		for(int i=0;i<N;++i)
			r.data[i] = -r.data[i];
		return r;
	}
	
    Matrix& operator+= (const Matrix& v){
		for(int i=0;i<N;++i)
			data[i] += v.data[i];
		return *this;
	}
	
    Matrix& operator-= (const Matrix& v){
		for(int i=0;i<N;++i)
			data[i] -= v.data[i];
		return *this;
	}
	
    Matrix& operator*= (T s){
		for(int i=0;i<N;++i)
			data[i] *= s;
		return *this;
	}
	
    Matrix& operator/= (T s){
		for(int i=0;i<N;++i)
			data[i] /= s;
		return *this;
	}
	
	
	Matrix<n, m, T> Transpose (){
		Matrix<n, m, T> r;
		for(int i=0;i<m;++i)
			for(int j=0;j<n;++j)
				//r[j][i] = (*this)[i][j];
				r.data[j*n+i] = data[i*n+j];
		return r;
	}
	
	/*Matrix Inverse (){
		static_assert(m==n, "Inverse operation can only apply on square matrix ! ");
		Matrix r;
		for(int i=0;i<m;++i)
			for(int j=0;j<n;++j)
				r[j][i] = (*this)[i][j];
		return r;
	}*/
	

	friend ostream& operator<<(ostream& o, const Matrix& t){
		o.flags(ios::left);
		for(int i=0;i<m;++i){
			o<<" "<<(i==0?"©³":i==m-1?"©»":"©§")<<" ";
			for(int j=0;j<n-1;++j)
				o<<setw(10)<<t[i][j];
			o<<setw(10)<<t[i][n-1]<<" "<<(i==0?"©·":i==m-1?"©¿":"©§")<<" "<<endl;
		}
		return o;
	}
	
};


using Matrix2 = Matrix<2, 2, float>;
using Matrix3 = Matrix<3, 3, float>;
using Matrix4 = Matrix<4, 4, float>;


inline Matrix4 TranslateMatrix(float x, float y, float z){
	Matrix4 m(Matrix4::Identity());
	//m[3][0] = x;
	//m[3][1] = y;
	//m[3][2] = z;
	m.data[12] = x;
	m.data[13] = y;
	m.data[14] = z;
	return m;
}

inline Matrix4 ScaleMatrix(float x, float y, float z){
	Matrix4 m(Matrix4::Identity());
	//m[0][0] = x;
	//m[1][1] = y;
	//m[2][2] = z;
	m.data[0] = x;
	m.data[5] = y;
	m.data[10] = z;
	return m;
}

Matrix4 RotateMatrix(float a, float x, float y, float z);

Matrix4 OrthoMatrix(float left, float right, float bottom, float top, float _near, float _far);

Matrix4 FrustumMatrix(float left, float right, float bottom, float top, float _near, float _far);

Matrix4 PerspectiveMatrix(float fovy, float aspect, float near, float far);

SE_END
