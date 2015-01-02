#pragma once

#include "Control.h"

SE_BEGIN

class Label : public Control{
public:
	enum class Alignment{
		TopLeft = 0,
		TopCenter = 1,
		TopRight = 2,
		MiddleLeft = 4,
		MiddleCenter = 5,
		MiddleRight = 6,
		BottomLeft = 8,
		BottomCenter = 9,
		BottomRight = 10
	};

protected:
	Alignment alignment;

	virtual void OnDraw(DrawEventArgs e){
		GetDefaultFont()->DrawString(e.GetGraphics(), foreColor, GetText(), -1, 0, 0, GetClientWidth(), GetClientHeight(), (int)alignment);
	}

public:
	Label(const Rect& bounds) : Control(bounds) {
		backBrush.SetColor(Color::Transparent);
	}

	Alignment GetAlignment() const{
		return alignment;
	}

	void SetAlignment(Alignment alignment){
		this->alignment = alignment;
	}
};

SE_END
