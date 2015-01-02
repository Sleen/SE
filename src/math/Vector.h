#pragma once

#include "Tuple.h"
#include "Type.h"

SE_BEGIN

template<int n, typename T>
class Vector : public Tuple<n, T>{
protected:
	using Tuple<n, T>::data;
	
public:
	Vector(){}
	
    Vector(const T* v) : Tuple<n, T>(v) {}
	
    Vector(const Vector& v) : Tuple<n, T>(v) {}
	
    Vector(const Tuple<n, T>& t) : Tuple<n, T>(t) {}
	
    Vector operator+ (const Vector& v) const{
		Vector r(*this);
		for(int i=0;i<n;++i)
			r[i] += v[i];
		return r;
	}
	
    Vector operator- (const Vector& v) const{
		Vector r(*this);
		for(int i=0;i<n;++i)
			r[i] -= v[i];
		return r;
	}
	
    Vector operator* (T s) const{
		Vector r(*this);
		for(int i=0;i<n;++i)
			r[i] *= s;
		return r;
	}
	
	friend Vector operator* (T s, const Vector& v){
		return v * s;
	}

	friend float operator*(const Vector& v1, const Vector& v2){
		float r = 0;
		for (int i = 0; i < n; ++i)
			r += v1[i] * v2[i];
		return r;
	}

    Vector operator/ (T s) const{
		Vector r(*this);
		for(int i=0;i<n;++i)
			r[i] /= s;
		return r;
	}
	
    Vector operator- () const{
		Vector r(*this);
		for(int i=0;i<n;++i)
			r[i] = -r[i];
		return r;
	}
	
    Vector& operator+= (const Vector& v){
		for(int i=0;i<n;++i)
			data[i] += v[i];
		return *this;
	}
	
    Vector& operator-= (const Vector& v){
		for(int i=0;i<n;++i)
			data[i] -= v[i];
		return *this;
	}
	
    Vector& operator*= (T s){
		for(int i=0;i<n;++i)
			data[i] *= s;
		return *this;
	}
	
    Vector& operator/= (T s){
		for(int i=0;i<n;++i)
			data[i] /= s;
		return *this;
	}

	T Length() const{
		T t = 0;
		for(int i=0;i<n;++i)
			t += data[i] * data[i];
		return (T) sqrt(t);
	}
	
    T Length2() const{
		T t = 0;
		for(int i=0;i<n;++i)
			t += data[i] * data[i];
		return t;
	}
	
    T Dot (const Vector& v) const{
		T t = 0;
		for(int i=0;i<n;++i)
			t += data[i] * v.data[i];
		return t;
	}
	
    T Normalize (){
		T l = Length();
		if(l!=0 && l!=1){
			T r = (T)1 / l;
			for(int i=0;i<n;++i)
				data[i] *= r;
		}
		return l;
	}
	
	bool IsZero(){
		for (int i = 0; i<n; ++i)
		if (data[i] != 0)
			return false;
		return true;
	}

	// Compute the axis-aligned bounding box of the points.
	void ComputeExtremes (int numVectors, const Vector* vectors, Vector& vmin, Vector& vmax){
		//assertion(numVectors > 0 && vectors, "Invalid inputs to ComputeExtremes\n");
		vmin = vectors[0];
		vmax = vmin;
		for (int j = 1; j < numVectors; ++j)
		{
			const Vector& vec = vectors[j];
			for (int i = 0; i < n; ++i)
			{
				if (vec[i] < vmin[i])
				{
					vmin[i] = vec[i];
				}
				else if (vec[i] > vmax[i])
				{
					vmax[i] = vec[i];
				}
			}
		}
	}
};

class Vector2 : public Vector<2, float>{ //, public Object{
    //DECL_CLASS(Vector2);
public:
    static Class clazz;

	//using Vector<2, float>::Vector(const Vector<2, float>&); 
	//using Vector<2, float>::Vector;
	//using Vector<2, float>::Vector(const float*);
	Vector2(const float* data) : Vector<2, float>::Vector(data){}
	Vector2(const Vector<2, float>& v) : Vector<2, float>::Vector(v) {}
	Vector2() : Vector<2, float>::Vector(){}

	Vector2& operator=(const Vector2& v){
		Vector<2, float>::operator=(v);
		return *this;
	}

    void operator()(float x, float y){
        data[0] = x;
        data[1] = y;
    }

    void operator()(const Vector2& v){
        data[0] = v.data[0];
        data[1] = v.data[1];
    }

	float& X(){
		return data[0];
	}
	
	float X() const{
		return data[0];
	}
	
	float& Y(){
		return data[1];
	}
	
	float Y() const{
		return data[1];
	}
	
	Vector2(float x, float y){
		data[0] = x;
		data[1] = y;
	}

    static void* CastFromString(void* str){
        string& s = *(string*)str;
        float x, y;
        sscanf(s.c_str(), "%f,%f", &x, &y);
        return new Vector2(x, y);
    }
};

class Vector3 : public Vector<3, float>{
public:
	Vector3(const Vector<3, float>& v) : Vector<3, float>::Vector(v) {}
	Vector3() : Vector<3, float>::Vector() {}

	Vector3& operator=(const Vector3& v){
		Vector<3, float>::operator=(v);
		return *this;
	}

	float& X(){
		return data[0];
	}
	
	float X() const{
		return data[0];
	}
	
	float& Y(){
		return data[1];
	}
	
	float Y() const{
		return data[1];
	}
	
	float& Z(){
		return data[2];
	}
	
	float Z() const{
		return data[2];
	}
	
	Vector3(float x, float y, float z){
		data[0] = x;
		data[1] = y;
		data[2] = z;
	}
};

class Vector4 : public Vector<4, float>{
public:
	Vector4(const Vector<4, float>& v) : Vector<4, float>::Vector(v) {}
	Vector4() : Vector<4, float>::Vector() {}
	
	Vector4& operator=(const Vector4& v){
		Vector<4, float>::operator=(v);
		return *this;
	}

	float& X(){
		return data[0];
	}
	
	float X() const{
		return data[0];
	}
	
	float& Y(){
		return data[1];
	}
	
	float Y() const{
		return data[1];
	}
	
	float& Z(){
		return data[2];
	}
	
	float Z() const{
		return data[2];
	}
	
	float& W(){
		return data[3];
	}
	
	float W() const{
		return data[3];
	}
	
	Vector4(float x, float y, float z = 0, float w = 0){
		data[0] = x;
		data[1] = y;
		data[2] = z;
		data[3] = w;
	}
};

SE_END
