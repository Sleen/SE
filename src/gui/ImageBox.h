#pragma once

#include "Control.h"

SE_BEGIN

class ImageBox : public Control{
protected:
	Texture* tex = NULL;
	SizeMode sizeMode = SizeMode::Strech;

	virtual void OnDraw(DrawEventArgs e){
		Graphics g = e.GetGraphics();
		
		if (tex != NULL)
			g.DrawImage(*tex, GetClientRect(), sizeMode);

		Control::OnDraw(e);
	}

public:
	ImageBox(const Rect& bounds) : Control(bounds) {
		backBrush.SetColor(Color::Transparent);
	}

	void SetTexture(Texture *tex){
		this->tex = tex;
	}

	Texture* GetTexture() const{
		return tex;
	}

	void SetSizeMode(SizeMode mode){
		sizeMode = mode;
	}

	SizeMode GetSizeMode() const{
		return sizeMode;
	}
};

SE_END
