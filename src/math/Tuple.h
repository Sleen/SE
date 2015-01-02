#pragma once

#include "Macro.h"

SE_BEGIN

template<int n, typename T>
class Tuple{
static_assert(n>=2, "the Tuple can not has a dimension less than 2 !");
protected:
	T data[n];
	
public:
	Tuple(){
		memset(data, 0, n*sizeof(T));
	}
	
	Tuple(const T* data){
		for(int i=0;i<n;++i)
			this->data[i] = data[i];
	}
	
	/*Tuple(T t, ...){
		data[0] = t;
		va_list args;
		va_start(args, t);
		for(int i=1;i<n;++i)
			this->data[i] = va_arg(args, T);
		va_end(args);
	}*/
	
	Tuple(const Tuple& t){
		for(int i=0;i<n;++i)
			data[i] = t.data[i];
	}
	
	~Tuple(){
	}
	
	operator T*(){
		return data;
	}
	
	operator const T*() const{
		return data;
	}
	
	T& operator[](int i){
		return data[i];
	}
	
	const T& operator[](int i) const{
		return data[i];
	}
	
	Tuple& operator=(const Tuple& t){
		for(int i=0;i<n;++i)
			data[i] = t.data[i];
		return *this;
	}
	
	bool operator==(const Tuple& t) const{
		for(int i=0;i<n;++i)
			if( data[i] != t.data[i] )
				return false;
		return true;
	}
	
    bool operator!=(const Tuple& t) const{
		for(int i=0;i<n;++i)
			if( data[i] != t.data[i] )
				return true;
		return false;
	}
	
    bool operator<(const Tuple& t) const
	{
		for(int i=0;i<n;++i)
			if( data[i] < t.data[i] )
				return true;
			else if( data[i] > t.data[i] )
				return false;
		return false;
	}
	
    bool operator<=(const Tuple& t) const{
		for(int i=0;i<n;++i)
			if( data[i] < t.data[i] )
				return true;
			else if( data[i] > t.data[i] )
				return false;
		return true;
	}
	
    bool operator>(const Tuple& t) const
	{
		for(int i=0;i<n;++i)
			if( data[i] > t.data[i] )
				return true;
			else if( data[i] < t.data[i] )
				return false;
		return false;
	}
	
    bool operator>=(const Tuple& t) const{
		for(int i=0;i<n;++i)
			if( data[i] > t.data[i] )
				return true;
			else if( data[i] < t.data[i] )
				return false;
		return true;
	}
	
	friend ostream& operator<<(ostream& o, const Tuple& t){
		for(int i=0;i<n-1;++i)
			o<<t[i]<<", ";
		return o<<t[n-1]<<endl;
	}
	
};

SE_END
