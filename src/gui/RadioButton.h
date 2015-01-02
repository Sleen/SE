#pragma once

#include "CheckBox.h"

SE_BEGIN

class RadioButton : public CheckBox{
protected:
	bool clicked;

public:
	RadioButton(const Rect& bounds) : CheckBox(bounds) {
		
	}

	void DrawOption(DrawEventArgs e){
		Graphics g = e.GetGraphics();

		int h = GetClientHeight();
		Rect r(h * 0.1f, h * 0.1f, h * 0.8f, h * 0.8f);
		g.FillOval((mouseIn && mouseDown) ? Brush(0x99ffffff & backBrush.GetColor()) : (mouseIn || mouseDown) ? Brush(0xccffffff & backBrush.GetColor()) : backBrush, r);
		g.DrawOval(Pen(foreColor), r);
		if (check)
		{
			g.FillOval(Brush(foreColor), Rect(r.Left() + r.GetWidth() * 0.1f, r.Top() + r.GetHeight() * 0.1f, r.GetWidth() * 0.8f, r.GetHeight() * 0.8f));
		}
	}

	void SetCheck(bool value)
	{
		// TODO
		CheckBox::SetChecked(value);
	}
};

SE_END
