#pragma once

#include "Macro.h"
#include "Tuple.h"
#include "Vector.h"

#include "Type.h"

SE_BEGIN

class Padding : public Object{
    DECL_CLASS(Padding);
private:
	float left, top, right, bottom;

public:

	Padding() : Padding(0, 0, 0, 0){}

	Padding(float left, float top, float right, float bottom){
		Set(left, top, right, bottom);
	}

	Padding(float all){
		Set(all, all, all, all);
	}

	Padding& Set(float left, float top, float right, float bottom){
		this->left = left;
		this->top = top;
		this->right = right;
		this->bottom = bottom;
		return *this;
	}

	Padding& Set(float all){
		return Set(all, all, all, all);
	}

	Padding& Set(const Padding& padding){
		return Set(padding.left, padding.top, padding.right, padding.bottom);
	}

    Padding operator+(const Padding& p){
        return Padding(left + p.left, top + p.top, right + p.right, bottom + p.bottom);
    }

    Padding operator*(float scale){
        return Padding(left * scale, top * scale, right * scale, bottom * scale);
    }

	float GetLeft() const{
		return left;
	}

	float GetTop() const{
		return top;
	}

	float GetRight() const{
		return right;
	}

	float GetBottom() const{
		return bottom;
	}

	float GetWidth() const{
		return left + right;
	}

	float GetHeight() const{
		return top + bottom;
	}

	Vector2 GetSize() const{
		return Vector2(GetWidth(), GetHeight());
	}

	bool IsZero() const{
		return left == 0 && top == 0 && right == 0 && bottom == 0;
	}
};

// left, top, right, bottom
class Rect : public Tuple<4, float>{
public:
	Rect() : Rect(0, 0, 0, 0) { }

	Rect(float left, float top, float width, float height){
		Set(left, top, width, height);
	}

	Rect(const Vector2& loc, const Vector2& size){
		Set(loc.X(), loc.Y(), size.X(), size.Y());
	}

	Rect(const Rect& r){
		Set(r);
	}

	void Set(const Rect& r){
		Set(r.Left(), r.Top(), r.GetWidth(), r.GetHeight());
	}

	void Set(float left, float top, float width, float height){
		data[0] = left;
		data[1] = top;
		data[2] = left+width;
		data[3] = top+height;
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

	float& Left(){
		return data[0];
	}

	float Left() const{
		return data[0];
	}

	float& Top(){
		return data[1];
	}

	float Top() const{
		return data[1];
	}

	float& Right(){
		return data[2];
	}

	float Right() const{
		return data[2];
	}

	float& Bottom(){
		return data[3];
	}

	float Bottom() const{
		return data[3];
	}

	float GetWidth() const{
		return Right() - Left();
	}

	float GetHeight() const{
		return Bottom() - Top();
	}

	void SetWidth(float w){
		Right() = Left() + w;
	}

	void SetHeight(float h){
		Bottom() = Top() + h;
	}

	Vector2 GetLocation() const{
		return Vector2(X(), Y());
	}

	Vector2 GetSize() const{
		return Vector2(GetWidth(), GetHeight());
	}

	void SetLocation(float x, float y){
		Vector2 s = GetSize();
		X() = x;
		Y() = y;
		SetSize(s);
	}

	void SetLocation(const Vector2& v){
		Vector2 s = GetSize();
		X() = v.X();
		Y() = v.Y();
		SetSize(s);
	}

	void SetSize(float w, float h){
		SetWidth(w);
		SetHeight(h);
	}

	void SetSize(const Vector2& v){
		SetWidth(v.X());
		SetHeight(v.Y());
	}

	void Offset(float x, float y){
		SetLocation(X() + x, Y() + y);
	}

	void Offset(const Vector2& v){
		SetLocation(X() + v.X(), Y() + v.Y());
	}

	bool IsIntersects(float l, float t, float r, float b) const{
		return !(l > Right() || r<Left() || t>Bottom() || b < Top());
	}

	bool IsIntersects(const Rect& r) const{
		return IsIntersects(r[0], r[1], r[2], r[3]);
	}

	Rect IntersectRect(float l, float t, float r, float b) const{
		if (l < Left()) l = Left();
		if (t < Top())t = Top();
		if (r > Right())r = Right();
		if (b > Bottom())b = Bottom();
		return Rect(l, t, r - l, b - t);
	}

	Rect IntersectRect(const Rect& r) const{
		return IntersectRect(r[0], r[1], r[2], r[3]);
	}

	Rect GetPaddedRect(float paddingAll) const{
		return GetPaddedRect(paddingAll, paddingAll, paddingAll, paddingAll);
	}

	Rect GetPaddedRect(float paddingLeft, float paddingTop, float paddingRight, float paddingBottom) const{
		return Rect(Left() + paddingLeft, Top() + paddingTop, GetWidth() - paddingLeft - paddingRight, GetHeight() - paddingTop - paddingBottom);
	}

	Rect GetPaddedRect(const Padding& padding) const{
		return GetPaddedRect(padding.GetLeft(), padding.GetTop(), padding.GetRight(), padding.GetBottom());
	}

	bool Contains(const Vector2& point) const{
		return Contains(point.X(), point.Y());
	}

	bool Contains(float x, float y) const{
		return x >= Left() && x <= Right() && y >= Top() && y <= Bottom();
	}
};

SE_END
