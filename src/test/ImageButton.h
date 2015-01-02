#include "ImageBox.h"

USING_SE

class ImageButton : public SE::ImageBox{
private:
	float cornerRadius = 0;
	Texture* tex;

public:
	ImageButton(const Rect& bounds, Texture* tex) : ImageBox(bounds){
		this->tex = tex;
	}

	virtual void OnDraw(DrawEventArgs e){
		Graphics g = e.GetGraphics();

		if (tex != NULL)
			g.DrawImage(*tex, GetClientRect(), sizeMode, (mouseIn && mouseDown) ? 0x80ffffff : (mouseIn || mouseDown) ? 0xbbffffff  : 0xffffffff, cornerRadius);
		g.DrawRoundRect(borderPen, GetClientRect(), 5);
		GetDefaultFont()->DrawString(e.GetGraphics(), foreColor, GetText(), -1, 0, 0, GetClientWidth(), GetClientHeight(), FontFlag_Center | FontFlag_Middle);
		Control::OnDraw(e);
	}
};