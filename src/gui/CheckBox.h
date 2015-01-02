#pragma once

#include "Control.h"

SE_BEGIN

class CheckBox : public Control{
protected:
	bool check;

	virtual void OnDraw(DrawEventArgs e){
		GetDefaultFont()->DrawString(e.GetGraphics(), foreColor, GetText(), -1, GetClientHeight(), 0, GetClientWidth() - GetClientHeight(), GetClientHeight(), FontFlag_Left | FontFlag_Middle);

		DrawOption(e);
	}

	virtual void DrawOption(DrawEventArgs e){
		Graphics g = e.GetGraphics();

		int h = GetClientHeight();
		Rect r(h * 0.1f, h * 0.1f, h * 0.8f, h * 0.8f);
		g.FillRect((mouseIn && mouseDown) ? Brush(0x99ffffff & backBrush.GetColor()) : (mouseIn || mouseDown) ? Brush(0xccffffff & backBrush.GetColor()) : backBrush, r);
		g.DrawRect(Pen(foreColor), r);
		if (check)
		{
			float points[] = { r.GetWidth()*0.4f, r.GetHeight()*0.8f, 0, 0, r.GetHeight()*0.6f, 0, r.GetWidth()*0.4f, r.GetHeight(), 0,
				r.GetWidth(), 0, 0};
			float s = 1.1f;
			g.Translate(r.GetLocation());
			g.FillPoly(Brush(0xfff9f6f2), points, 4);
			g.DrawPoly(Pen((0x00ffffff & foreColor) | 0x9f000000), points, 4);
			//g.DrawLines(Pen(foreColor, r.GetWidth() / 9), points, 3);
			//g.Translate(-2, 0);
			//g.DrawLines(Pen(0xfff9f6f2, r.GetWidth() / 9), points, 3);
		}
	}

	virtual void OnClick() override{
		Control::OnClick();
		SetChecked(!IsChecked());
	}

public:
	Event<Control, bool> CheckChanged;

	CheckBox(const Rect& bounds) : Control(bounds) {
		foreColor = 0xff67584a;
		backBrush.SetColor(0xa08e836d);
	}

	bool IsChecked() const{
		return check;
	}

	virtual void SetChecked(bool checked){
		if (checked != check){
			check = checked;
			CheckChanged(*this, checked);
		}
	}
};

SE_END
