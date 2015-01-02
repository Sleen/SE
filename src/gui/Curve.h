#pragma once
#include "Pen.h"
#include "Graphics.h"
#include <cmath>

SE_BEGIN

class InCurve;
class InOutCurve;
class RepeatCurve;
class ReverseCurve;
class CombineCurve;
class MergeCurve;

class Curve{
public:
	virtual float Value(float percent) = 0;
	virtual void Draw(const Graphics& g, const Pen& pen, const Rect& bounds, int segment = 100){
		float t = 1.0f / segment;
		g.Push();
		g.Translate(bounds.GetLeft(), bounds.GetBottom());
		g.Scale(bounds.GetWidth(), -bounds.GetHeight());
		for (float i = 0; i < 1; i += t)
		{
			float j = min(1, i + t);
			g.DrawLine(pen, i, Value(i), j, Value(j));
		}
		g.Pop();
	}
	InCurve *In();
	Curve *Out();
	InOutCurve *InOut();
	ReverseCurve *Reverse();
	CombineCurve *operator+(Curve* c);
	MergeCurve *Merge(Curve* another, float position = 0.5f);
	RepeatCurve *Repeat(float count = 2);
	virtual ~Curve(){
		//cout << "Curve->release\n";
	}
};

class LinearCurve : public Curve{
public:
	virtual float Value(float percent){
		return percent;
	}
};

class BezierCurve : public Curve{
public:
	Vector2 p1, p2;

	BezierCurve(){
		p1(0, 0);
		p2(1, 1);
	}

	BezierCurve(float x1, float y1, float x2, float y2){
		p1(x1, y1);
		p2(x2, y2);
	}

	BezierCurve(const Vector2& c1, const Vector2& c2){
		p1(c1);
		p2(c2);
	}

	float GetY(float t) const{
		float t1 = 1 - t;
		return (t*t1*t1*p1.y + t*t*t1*p2.y) * 3 + t*t*t;
	}

	float GetX(float t) const{
		float t1 = 1 - t;
		return (t*t1*t1*p1.x + t*t*t1*p2.x) * 3 + t*t*t;
	}

	virtual float Value(float percent){
		int d = 20;		// 二分比较次数
		float min = 0;
		float max = 1;
		float mid;
		while (d-- > 0){
			mid = (min + max) / 2;
			float x = GetX(mid);
			if (x>percent)
				max = mid;
			else
				min = mid;
		}

		return GetY((max + min) / 2);
	}

};

class PowerCurve : public Curve
{
protected:
	float power;
public:
	void SetPower(float power){
		this->power = power;
	}
	float GetPower() const{
		return power;
	}
	PowerCurve(float p = 2) { power = p; }
	virtual float Value(float percent)
	{
		return 1 - (float)pow(1 - percent, power);
	}
};

class SineCurve : public Curve
{
	virtual float Value(float percent)
	{
		return (float)sin(percent * M_PI / 2);
	}
};

class CircleCurve : public Curve
{
	virtual float Value(float percent)
	{
		return (float)sqrt(1 - (percent -= 1) * percent);
	}
};

class ExponentCurve : public Curve
{
	virtual float Value(float percent)
	{
		return percent == 1 ? 1 : (-(float)pow(2, -10 * percent) + 1);
	}
};

class BounceCurve : public Curve
{
protected:
	float times, decay;
public:
	void SetTimes(float times){
		if (times < 1)times = 1;
		this->times = times;
	}
	void SetDecay(float decay){
		if (decay < 1.1f)decay = 1.1f;
		this->decay = decay;
	}
	float GetTimes() const{
		return times;
	}
	float GetDecay() const{
		return decay;
	}
	BounceCurve(float times = 4, float decay = 4)
	{
		this->times = times;
		this->decay = decay;
	}
	virtual float Value(float percent)
	{
		float d = 1 / (float)sqrt(decay);
		float x = 1 / (1 + d * 2 * (1 - (float)pow(d, times - 1)) / (1 - d));
		float n = x;
		float s = 1;
		while (n < percent) { n += x * 2 * d; x *= d; s /= decay; }
		return s / x / x * (percent -= n - x) * percent + 1 - s;
	}
};

class FlexCurve : public Curve
{
protected:
	float times;
public:
	void SetTimes(float times){
		if (times < 0)times = 0;
		this->times = times;
	}
	float GetTimes() const{
		return times;
	}
	FlexCurve(float times = 4)
	{
		this->times = times;
	}
/*
	virtual ~FlexCurve(){
		cout << "FlexCurve->release\n";
	}*/
	virtual float Value(float percent)
	{
		return (float)(pow(2, -10 * percent) * (times == 0 ? -1 : sin((percent - 1 / times / 4) * (2 * M_PI) * times)) + 1);
	}
};

class BackCurve : public Curve
{
protected:
	float s;
public:
	float GetS() const{ return s; }
	void SetS(float s){ this->s = s; }
	BackCurve(float s = 1.70158f) { this->s = s; }
	virtual float Value(float percent)
	{
		return (percent -= 1) * percent * ((s + 1) * percent + s) + 1;
	}
};

class BlinkCurve : public Curve
{
protected:
	float position;

public:
	void SetPosition(float position){
		if (position < 0) position = 0;
		else if (position > 1) position = 1;
		this->position = position;
	}
	float GetPosition() const{
		return position;
	}

	BlinkCurve(float position = 0.5f)
	{
		this->position = position;
	}

	virtual float Value(float percent)
	{
		return percent >= position ? 1 : 0;
	}
};

class ModifyCurve : public Curve
{
protected:
	Curve *curve;
	ModifyCurve(Curve *curve) { this->curve = curve; }
public:
	void SetCurve(Curve& curve){
		this->curve = &curve;
	}
	~ModifyCurve(){ 
		delete curve;
		//cout << "ModifyCurve->release\n";
	}
	Curve& GetCurve(){
		return *curve;
	}
};

class InCurve : public ModifyCurve
{
public:
	InCurve(Curve* curve) : ModifyCurve(curve) { }

	virtual float Value(float percent)
	{
		return 1 - curve->Value(1 - percent);
	}
};

class MergeCurve : public ModifyCurve
{
protected:
	Curve *curve2;
	float position;
public:
	void SetPosition(float position){
		if (position < 0) position = 0;
		else if (position > 1) position = 1;
		this->position = position;
	}
	float GetPosition() const{
		return position;
	}
	Curve& GetCurve2(){
		return *curve2;
	}
	void SetCurve2(Curve& curve2){
		this->curve2 = &curve2;
	}

	MergeCurve(Curve* c1, Curve* c2, float position = 0.5f)
		: ModifyCurve(c1)
	{
		this->curve2 = c2;
		this->position = position;
	}

	virtual float Value(float percent)
	{
		if (position == 0)
			return curve2->Value(percent);
		else if (position == 1)
			return curve->Value(percent);
		float p = 1 - position;
		return percent > position ? (curve2->Value((percent - position) / p) * p + position) : (curve->Value(percent / position) * position);
	}
};

class InOutCurve : public MergeCurve
{
public:
	InOutCurve(Curve* curve, float position = 0.5f) : MergeCurve(new InCurve(curve), curve, position) { }

};

class CombineCurve : public MergeCurve
{
public:
	CombineCurve(Curve* c1, Curve* c2, float position = 0.5f)
		: MergeCurve(c1, c2, position) { }
	virtual float Value(float percent)
	{
		if (position == 0)
			return curve2->Value(percent);
		else if (position == 1)
			return curve->Value(percent);
		float p = 1 - position;
		return percent > position ? curve2->Value((percent - position) / p) : curve->Value(percent / position);
	}
};

class RepeatCurve : public ModifyCurve
{
protected:
	float repeatCount;

public:
	void SetRepeatCount(float count){
		repeatCount = count;
	}
	float GetRepeatCount()const{
		return repeatCount;
	}
	//~RepeatCurve(){ cout << "RepeatCurve::~\n"; ModifyCurve::~ModifyCurve(); }
	RepeatCurve(Curve *curve, float count) : ModifyCurve(curve) { this->repeatCount = count; }

	virtual float Value(float percent)
	{
		return curve->Value((percent *= repeatCount) - (int)percent);
	}
};

class ReverseCurve : public ModifyCurve
{
public:
	ReverseCurve(Curve* curve) : ModifyCurve(curve) { }

	virtual float Value(float percent)
	{
		return curve->Value(1 - percent);
	}
};

InCurve *Curve::In()
{
	return new InCurve(this);
}
Curve *Curve::Out()
{
	return this;
}
InOutCurve *Curve::InOut()
{
	return new InOutCurve(this);
}
RepeatCurve *Curve::Repeat(float count)
{
	return new RepeatCurve(this, count);
}
ReverseCurve *Curve::Reverse()
{
	return new ReverseCurve(this);
}
CombineCurve *Curve::operator+(Curve* c)
{
	return new CombineCurve(this, c);
}
MergeCurve *Curve::Merge(Curve* another, float position)
{
	return new MergeCurve(this, another, position);
}
SE_END