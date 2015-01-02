#include "Control.h"
#include "Window.h"

SE_BEGIN

int Unit::widthPixel;
int Unit::heightPixel;
float Unit::widthInch;
float Unit::heightInch;
float Unit::screenDpi;

float Unit::rates[6];

Font* Control::defaultFont = NULL;
RenderableTexture* Control::tex = NULL;

void Container::OnDraw(DrawEventArgs e){
	Graphics g = e.GetGraphics();
	Control::OnDraw(e);
	Rect r = g.GetClipRect();
	for (Control* c : controls){
		g.SetClipMode(c->GetClipMode() == ClipMode::ClipSelf ? ClipMode::ClipSelf : ClipMode::None);//c->GetClipMode());
		if (c->GetClipMode() == ClipMode::ClipByFBO){
			if (tex == NULL)
				tex = new RenderableTexture(Window::GetScreenWidth(), Window::GetScreenHeight());
			tex->Begin();
			c->OnDraw(e);
			tex->End();
			//g.Translate(c->GetLocation());
			//g.DrawImage(*tex->GetTexture(), c->GetClientRect(), c->GetClientRect());
			g.DrawImage(*tex->GetTexture(), c->GetClientRect(), c->GetBounds());
		}else{
			g.Push();
			g.Translate(c->GetLocation());
			Rect cr = c->GetBounds();
			cr.Offset(-c->GetLocation());
			g.SetClipRect(cr);
			c->OnDraw(e);
			g.Pop();
		}
	}
	g.SetClipRect(r);
}

SE_END
