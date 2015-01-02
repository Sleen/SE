#pragma once

#include "Control.h"

SE_BEGIN

class Button : public Control{
protected:

	virtual void OnDraw(DrawEventArgs e){
		Graphics g = e.GetGraphics();
		
		g.FillRoundRect((mouseIn && mouseDown) ? Brush(0x80ffffff & backBrush.GetColor()) : (mouseIn || mouseDown) ? Brush(0xbbffffff & backBrush.GetColor()) : backBrush, GetClientRect(), 5);
		g.DrawRoundRect(borderPen, GetClientRect(), 5);
		GetDefaultFont()->DrawString(e.GetGraphics(), foreColor, GetText(), -1, 0, 0, GetClientWidth(), GetClientHeight(), FontFlag_Center|FontFlag_Middle);
	}

public:
	Button(const Rect& bounds) : Control(bounds) {
		borderPen.SetWidth(1);
		backBrush.SetColor(Color::LightBlue);
	}

};

SE_END
