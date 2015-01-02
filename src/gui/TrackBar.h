#pragma once

#include "Control.h"

SE_BEGIN

class TrackBar : public Control{
protected:
	float min, max, value = 0;
	bool mouseOver = false, mouseDown = false;
	Vector2 downPoint;

	Rect GetTrackRect() const
	{
		int w = GetClientWidth();
		int h = GetClientHeight();
		float p = GetPercent() * (w - h) + h / 2;
		return Rect(p - h / 2, 0, h, h);
	}

	virtual void OnDraw(DrawEventArgs e){
		Graphics g = e.GetGraphics();
		float w = GetClientWidth();
		float h = GetClientHeight();
		g.DrawLine(Pen(Color::Gray, h / 10), h / 2, h / 2, w - h / 2, h / 2);
		int c = mouseDown ? 0xff56504a : mouseOver ? 0xb456504a : 0x6456504a;
		g.FillOval(Brush(c), GetTrackRect());
		g.DrawOval(Pen(0xffb4b4b4), GetTrackRect());
	}

	virtual void OnMouseMove(MouseEventArgs e)
	{
		Control::OnMouseMove(e);
		if (mouseDown)
		{
			SetPercent(GetPercent() + (e.X - downPoint.X()) / (GetClientWidth() - GetClientHeight()));
			downPoint = e.Location();
		}
		else
			mouseOver = GetTrackRect().Contains(e.Location());
	}

	virtual void OnMouseDown(MouseEventArgs e)
	{
		Control::OnMouseDown(e);
		if (mouseDown = mouseOver)
			downPoint = e.Location();
	}

	virtual void OnMouseUp(MouseEventArgs e)
	{
		Control::OnMouseUp(e);
		mouseDown = false;
	}

	virtual void OnMouseLeave()
	{
		Control::OnMouseLeave();
		mouseOver = false;
	}

public:
	Event<TrackBar> ValueChanged;

	TrackBar(const Rect& bounds, float min = 0, float max = 10) : Control(bounds) {
		this->min = min;
		this->max = max;
	}

	float GetPercent() const
	{
		return (value - min) / (max - min);
	}

	void SetPercent(float value)
	{
		if (value < 0)
		{
			value = 0;
		}
		else if (value > 1)
		{
			value = 1;
		}
		SetValue(min + (max - min) * value);
	}

	float GetMin() const
	{
		return min;
	}

	void SetMin(float value)
	{
		min = value;
	}

	float GetMax() const
	{
		return max;
	}

	void SetMax(float value)
	{
		max = value;
	}

	float GetValue() const
	{
		return value;
	}

	void SetValue(float value)
	{
		if (value < min)
		{
			value = min;
		}
		else if (value > max)
		{
			value = max;
		}
		if (this->value != value)
		{
			this->value = value;
			ValueChanged(*this);
		}
	}
};

SE_END
