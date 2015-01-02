#include "Graphics.h"
#include "Transform.h"
#include "Shader.h"
#include "Color.h"
#include "Window.h"

SE_BEGIN

Program* Graphics::shader = NULL;

GLuint Graphics::uCenter = 0;
GLuint Graphics::uRadiusX = 0;
GLuint Graphics::uRadiusY = 0;
GLuint Graphics::uRadiusCorner = 0;
GLuint Graphics::uStartAngle = 0;
GLuint Graphics::uSweepAngle = 0;
GLuint Graphics::uMode = 0;
GLuint Graphics::uCorner = 0;
GLuint Graphics::uWidth = 0;

void Graphics::Init(){
	shader = new Program(R"(
void main(){
	prepare();
}
)", R"(
uniform vec2 center;	// center point
uniform float radius, ry, cr;	// radius, radiusY, corner radius
uniform float start;	// start angle
uniform float sweep;	// sweep angle
uniform int mode;		// 0 - rect;  1 - oval;  2 - round rect
uniform float corner;		// which corners would to be radius
uniform float width;	// stroke thickness ( < 0 to fill )

void ca(vec2 c, float rx, float ry){
	float s=rx/ry;
	float d = (vp.x-c.x)*(vp.x-c.x)+(vp.y-c.y)*(vp.y-c.y)*s*s;
	if(d>rx*rx)
		discard;
	else if(d>(rx-1.0)*(rx-1.0))
		gl_FragColor.a = rx-sqrt(d);
}

float cc(vec2 c, float rx, float ry){
	float s=rx/ry;
	float d = (vp.x-c.x)*(vp.x-c.x)+(vp.y-c.y)*(vp.y-c.y)*s*s;
	if(d>rx*rx)
		return 0.0;
	else if(d>(rx-1.0)*(rx-1.0))
		return rx-sqrt(d);
	return 1.0;
}

float ff(float radius, float ry, float cr){
	if(radius<0.0 || ry<0.0)
		return 0.0;
	
	if(mode==1){
		if(sweep <= 0.0)
			return 0.0;
		else{
			float s = radius/ry;
			float d = (vp.x-center.x)*(vp.x-center.x)+(vp.y-center.y)*s*(vp.y-center.y)*s;
			if(d>radius*radius)
				return 0.0;
			else{
				if(sweep<360.0){
					float rad = atan(vp.y-center.y, vp.x-center.x);
					if(rad<0.0) rad+=6.283185307179586477;
					rad = degrees(rad);
					float s = mod(start, 360.0);
					float end = mod(start+sweep, 360.0);
					if((s>end && rad<s&&rad>end)||(s<end && (rad<s||rad>end)))
						return 0.0;
				}
				if(d>(radius-1.0)*(radius-1.0))
					return radius-sqrt(d);
			}
		}
	}else if(mode==2){
		float r = min(cr, min(radius, ry));
		if(vp.x<center.x-radius || vp.x>center.x+radius || vp.y<center.y-ry || vp.y>center.y+ry)
			return 0.0;
		if(!(vp.x>=center.x-radius+r && vp.x<=center.x+radius-r ||
			vp.y>=center.y-ry+r && vp.y<=center.y+ry-r)){
			if(vp.x<center.x-radius+r){
				if(mod(corner, 2.0)==1.0 && vp.y<center.y-ry+r)
					return cc(vec2(center.x-radius+r, center.y-ry+r), r, r);
				else if(mod(corner, 16.0)>=8.0 && vp.y>center.y+ry-r)
					return cc(vec2(center.x-radius+r, center.y+ry-r), r, r);
			}else{
				if(mod(corner, 4.0)>=2.0 && vp.y<center.y-ry+r)
					return cc(vec2(center.x+radius-r, center.y-ry+r), r, r);
				else if(mod(corner, 8.0)>=4.0 && vp.y>center.y+ry-r)
					return cc(vec2(center.x+radius-r, center.y+ry-r), r, r);
			}
		}
	}
	return 1.0;
}

void main(){
	if(prepare()){
		if(mode == 0)
			return;
		float a = ff(radius, ry, cr);
		if(width>=0.0)
			a -= ff(radius-width, ry-width, cr-width);
		gl_FragColor.a *= a;
	}
}
)");
	
	uCenter = shader->GetUniLoc("center");
	uRadiusX = shader->GetUniLoc("radius");
	uRadiusY = shader->GetUniLoc("ry");
	uRadiusCorner = shader->GetUniLoc("cr");
	uStartAngle = shader->GetUniLoc("start");
	uSweepAngle = shader->GetUniLoc("sweep");
	uMode = shader->GetUniLoc("mode");
	uCorner = shader->GetUniLoc("corner");
	uWidth = shader->GetUniLoc("width");

	glUniform1i(uMode, 0);
}

Graphics::Graphics(Window* window){
	this->window = window;
	this->clipRect = window->GetClientRect();
}

void Graphics::Begin(){
	if (shader){
		shader->Use();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		Viewport(0, 0, window->GetClientWidth(), window->GetClientHeight());

		MatrixMode(PROJECTION_MATRIX);
		LoadIdentity();
		Ortho(0, window->GetClientWidth(), window->GetClientHeight(), 0, -1, 1);
		MatrixMode(MODELVIEW_MATRIX);
		LoadIdentity();
	}
	else{
		DEBUG_LOG("graphics shader is not ready\n");
	}
}

void Graphics::DrawBase(const Brush& brush, float *points, int count, int type, int dimension) const{
	const Texture* tex = brush.GetTexture();
	if (tex != NULL){
		glUniform1i(Program::CurrentShader()->useTex, 1);
		glUniform1i(Program::CurrentShader()->uTexWidth, tex->GetWidth());
		glUniform1i(Program::CurrentShader()->uTexHeight, tex->GetHeight());
		tex->Bind();
	}
	glUniform1i(Program::CurrentShader()->useUColor, 1);
	int color = brush.GetColor();
	glUniform4f(Program::CurrentShader()->uColor, RGBAF(color));
	glEnableVertexAttribArray(POSITION_ATTRIB_LOC);
	glVertexAttribPointer(POSITION_ATTRIB_LOC, dimension, GL_FLOAT, GL_FALSE, 0, points);
	glDrawArrays(type, 0, count);
	glDisableVertexAttribArray(POSITION_ATTRIB_LOC);
	glUniform1i(Program::CurrentShader()->useUColor, 0);
	if (tex != NULL)
		glUniform1i(Program::CurrentShader()->useTex, 0);
}

void Graphics::DrawLinesBase(const Pen& pen, float *points, int count, int type, int dimension) const{
	if (pen.GetWidth() <= 0)
		return;
	glLineWidth(pen.GetWidth());
#ifndef GLES
	glPointSize(pen.GetWidth());
#endif
	DrawBase(pen.GetBrush(), points, count, type, dimension);
}

void Graphics::DrawOvalBase(const Pen& pen, float left, float top, float width, float height, float startAngle, float sweepAngle, int type, bool center) const{
	if (sweepAngle>360)
		sweepAngle = sweepAngle - (int)sweepAngle + (int)sweepAngle % 360;
	int count = (int)(sweepAngle / 1.8f);
	if (count == 0)count = 1;
	float step = sweepAngle / count;
	int d = center ? 1 : 0;
	count += d + 1;
	float* pts = new float[count * 3];
	float rx = width / 2;
	float ry = height / 2;
	float cx = left + rx;
	float cy = top + ry;
	for (int i = d; i < count; i++){
		pts[i * 3] = (float)(cx + rx*cos((i*step + startAngle)*M_PI / 180));
		pts[i * 3 + 1] = (float)(cy + ry*sin((i*step + startAngle)*M_PI / 180));
		pts[i * 3 + 2] = 0;
	}
	if (center){
		pts[0] = cx;
		pts[1] = cy;
		pts[2] = 0;
	}
	DrawLinesBase(pen, pts, count, type);
	delete[] pts;
}

void Graphics::FillPolyBase(const Brush& brush, float *points, int count, int dimension) const{
	DrawBase(brush, points, count, GL_TRIANGLE_FAN, dimension);
}

void Graphics::FillOvalBase(const Brush& brush, float left, float top, float width, float height, float startAngle, float sweepAngle, bool center) const{
	if (sweepAngle>360)
		sweepAngle = sweepAngle - (int)sweepAngle + (int)sweepAngle % 360;
	int count = (int)(sweepAngle / 1.8f);
	if (count == 0)count = 1;
	float step = sweepAngle / count;
	int d = center ? 1 : 0;
	float *pts = new float[(count + d + 1) * 3];
	float rx = width / 2;
	float ry = height / 2;
	float cx = left + rx;
	float cy = top + ry;
	for (int i = 0; i <= count; i++){
		pts[(i + d) * 3] = (float)(cx + rx*cos((i*step + startAngle)*M_PI / 180));
		pts[(i + d) * 3 + 1] = (float)(cy + ry*sin((i*step + startAngle)*M_PI / 180));
		pts[(i + d) * 3 + 2] = 0;
	}
	if (center){
		pts[0] = cx;
		pts[1] = cy;
		pts[2] = 0;
	}
	FillPolyBase(brush, pts, count + d + 1);
	delete[] pts;
}

void Graphics::DrawImage(Texture& tex, const Rect& src, const Rect& dest, int color, float cornerRadius) const{
	MatrixMode(TEXTURE_MATRIX);
	PushMatrix();
	LoadIdentity();
	Translate(src.Left(), src.Top());
	Scale(src.GetWidth() / dest.GetWidth(), src.GetHeight() / dest.GetHeight());
	Translate(-dest.Left(), -dest.Top());
	if (cornerRadius <= 0)
		FillRect(Brush(&tex, color), dest);
	else
		FillRoundRect(Brush(&tex, color), dest, cornerRadius);
	PopMatrix();
	MatrixMode(MODELVIEW_MATRIX);
}

void Graphics::DrawImage(Texture& tex, const Rect& d, SizeMode sizeMode, int color, float cornerRadius) const{
	int width = tex.GetWidth();
	int height = tex.GetHeight();
	float w = d.GetWidth();
	float h = d.GetHeight();
	float r1 = (float)width / height;
	float r2 = w / h;

	Rect src = Rect(0, 0, width, height), dest = d;
	switch (sizeMode)
	{
	case SizeMode::Strech:
		break;
	case SizeMode::Center:
		dest = Rect((w - width) / 2, (h - height) / 2, width, height);
		break;
	case SizeMode::Zoom:
		dest = r1 > r2 ? Rect(0, (h - w / r1) / 2, w, w / r1) :
			Rect((w - h * r1) / 2, 0, h * r1, h);
		break;
	case SizeMode::Fill:
		dest = r1 > r2 ? Rect((w - h * r1) / 2, 0, h * r1, h) :
			Rect(0, (h - w / r1) / 2, w, w / r1);
		break;
	default:
		if (src.GetWidth() > w) src.SetWidth(w);
		if (src.GetHeight() > h)src.SetHeight(h);
	}

	float l;
	if ((l = dest.Left()) < 0){
		float t = sizeMode == SizeMode::Center ? l : (l*height / h);
		src.Left() = -t;
		src.Right() += t;
		dest.Left() = 0;
		dest.Right() += l;
	}
	if ((l = dest.Top()) < 0){
		float t = sizeMode == SizeMode::Center ? l : (l*width / w);
		src.Top() = -t;
		src.Bottom() += t;
		dest.Top() = 0;
		dest.Bottom() += l;
	}

	DrawImage(tex, src, dest, color, cornerRadius);
}

void Graphics::DrawDots(const Pen& pen, Vector2 *points, int count) const{
	DrawLinesBase(pen, (float*)points, count, GL_POINTS, 2);
}

void Graphics::DrawDots(const Pen& pen, float *points, int count) const{
	DrawLinesBase(pen, points, count, GL_POINTS, 2);
}

void Graphics::DrawDot(const Pen& pen, const Vector2& p) const{
	DrawDot(pen, p.X(), p.Y());
}

void Graphics::DrawDot(const Pen& pen, float x, float y) const{
    float ps[2] = { x, y };
    DrawLinesBase(pen, ps, 2, GL_POINTS);
}

void Graphics::DrawLines(const Pen& pen, Vector2 *points, int count) const{
	DrawLinesBase(pen, (float*)points, count, GL_LINE_STRIP, 2);
}

void Graphics::DrawLines(const Pen& pen, float *points, int count) const{
	DrawLinesBase(pen, points, count, GL_LINE_STRIP, 3);
}

void Graphics::DrawPoly(const Pen& pen, Vector2 *points, int count) const{
	DrawLinesBase(pen, (float*)points, count, GL_LINE_LOOP, 2);
}

void Graphics::DrawPoly(const Pen& pen, float *points, int count) const{
	DrawLinesBase(pen, points, count, GL_LINE_LOOP);
}

void Graphics::DrawLine(const Pen& pen, const Vector2& v1, const Vector2& v2) const{
	DrawLine(pen, v1.X(), v1.Y(), v2.X(), v2.Y());
}

void Graphics::DrawLine(const Pen& pen, const Vector3& v1, const Vector3& v2) const{
    float ps[6] = { v1.X(), v1.Y(), v1.Z(), v2.X(), v2.Y(), v2.Z() };
    DrawLinesBase(pen, ps, 2, GL_LINES);
}

void Graphics::DrawLine(const Pen& pen, float x1, float y1, float x2, float y2) const{
    float ps[4] = { x1, y1, x2, y2 };
	DrawLinesBase(pen, ps, 2, GL_LINES, 2);
}

void Graphics::DrawRect(const Pen& pen, const Rect& rect) const{
	DrawRect(pen, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight());
}

void Graphics::DrawRect(const Pen& pen, const Vector2& location, const Vector2& size) const{
	DrawRect(pen, location.X(), location.Y(), size.X(), size.Y());
}

void Graphics::DrawRect(const Pen& pen, float left, float top, float width, float height) const{
    float ps[8] = { left, top, left + width, top, left + width, top + height, left, top + height };
	DrawLinesBase(pen, ps, 4, GL_LINE_LOOP, 2);
}

void Graphics::DrawRoundRect(const Pen& pen, const Rect& rect, float radius, int corner) const{
	DrawRoundRect(pen, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight(), radius, corner);
}

void Graphics::DrawRoundRect(const Pen& pen, const Vector2& location, const Vector2& size, float radius, int corner) const{
	DrawRoundRect(pen, location.X(), location.Y(), size.X(), size.Y(), radius, corner);
}

void Graphics::DrawRoundRect(const Pen& pen, float left, float top, float width, float height, float radius, int corner) const{
	glUniform1i(uMode, 2);
	glUniform2f(uCenter, left + width/2, top + height/2);
	glUniform1f(uRadiusX, width/2);
	glUniform1f(uRadiusY, height/2);
	glUniform1f(uRadiusCorner, radius);
	glUniform1f(uCorner, corner);
	glUniform1f(uWidth, pen.GetWidth());
	FillRect(pen.GetBrush(), left, top, width, height);
	glUniform1i(uMode, 0);
	glUniform1f(uWidth, -1);
	/*radiusX = MAX(MIN(width / 2, radiusX), 0);
	radiusY = MAX(MIN(height / 2, radiusY), 0);
	DrawArc(pen, left, top, radiusX * 2, radiusY * 2, 180, 90);
	DrawArc(pen, left + width - radiusX * 2, top, radiusX * 2, radiusY * 2, 270, 90);
	DrawArc(pen, left + width - radiusX * 2, top + height - radiusY * 2, radiusX * 2, radiusY * 2, 0, 90);
	DrawArc(pen, left, top + height - radiusY * 2, radiusX * 2, radiusY * 2, 90, 90);

	DrawLine(pen, left + radiusX, top, left + width - radiusX, top);
	DrawLine(pen, left + width, top + radiusY, left + width, top + height - radiusY);
	DrawLine(pen, left + width - radiusX, top + height, left + radiusX, top + height);
	DrawLine(pen, left, top + height - radiusY, left, top + radiusY);*/
}

void Graphics::DrawOval(const Pen& pen, const Rect& rect) const{
	DrawOval(pen, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight());
}

void Graphics::DrawOval(const Pen& pen, const Vector2& location, const Vector2& size) const{
	DrawOval(pen, location.X(), location.Y(), size.X(), size.Y());
}

void Graphics::DrawOval(const Pen& pen, float left, float top, float width, float height) const{
	glUniform1i(uMode, 1);
	glUniform1f(uStartAngle, 0);
	glUniform1f(uSweepAngle, 360);
	glUniform2f(uCenter, left + width / 2, top + height / 2);
	glUniform1f(uRadiusX, width / 2);
	glUniform1f(uRadiusY, height / 2);
	glUniform1f(uWidth, pen.GetWidth());
	FillRect(pen.GetBrush(), left, top, width, height);
	glUniform1i(uMode, 0);
	glUniform1f(uWidth, -1);
	//DrawOvalBase(pen, left, top, width, height, 0, 360, GL_LINE_LOOP);
}

void Graphics::DrawArc(const Pen& pen, const Rect& rect, float startAngle, float sweepAngle) const{
	DrawArc(pen, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight(), startAngle, sweepAngle);
}

void Graphics::DrawArc(const Pen& pen, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const{
	DrawArc(pen, location.X(), location.Y(), size.X(), size.Y(), startAngle, sweepAngle);
}

void Graphics::DrawArc(const Pen& pen, float left, float top, float width, float height, float startAngle, float sweepAngle) const{
	glUniform1i(uMode, 1);
	glUniform1f(uStartAngle, startAngle);
	glUniform1f(uSweepAngle, sweepAngle);
	glUniform2f(uCenter, left + width / 2, top + height / 2);
	glUniform1f(uRadiusX, width / 2);
	glUniform1f(uRadiusY, height / 2);
	glUniform1f(uWidth, pen.GetWidth());
	FillRect(pen.GetBrush(), left, top, width, height);
	glUniform1i(uMode, 0);
	glUniform1f(uWidth, -1);
	//DrawOvalBase(pen, left, top, width, height, startAngle, sweepAngle, GL_LINE_STRIP);
}

void Graphics::DrawArcClosed(const Pen& pen, const Rect& rect, float startAngle, float sweepAngle) const{
	DrawArcClosed(pen, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight(), startAngle, sweepAngle);
}

void Graphics::DrawArcClosed(const Pen& pen, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const{
	DrawArcClosed(pen, location.X(), location.Y(), size.X(), size.Y(), startAngle, sweepAngle);
}

void Graphics::DrawArcClosed(const Pen& pen, float left, float top, float width, float height, float startAngle, float sweepAngle) const{
	DrawOvalBase(pen, left, top, width, height, startAngle, sweepAngle, GL_LINE_LOOP);
}

void Graphics::DrawPie(const Pen& pen, const Rect& rect, float startAngle, float sweepAngle) const{
	DrawPie(pen, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight(), startAngle, sweepAngle);
}

void Graphics::DrawPie(const Pen& pen, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const{
	DrawPie(pen, location.X(), location.Y(), size.X(), size.Y(), startAngle, sweepAngle);
}

void Graphics::DrawPie(const Pen& pen, float left, float top, float width, float height, float startAngle, float sweepAngle) const{
	DrawOvalBase(pen, left, top, width, height, startAngle, sweepAngle, GL_LINE_LOOP, true);
}



void Graphics::FillPoly(const Brush& brush, Vector2 *points, int count) const{
	//float *fs = Vectors2floats(points, count);
	FillPolyBase(brush, (float*)points, count, 2);
	//DrawBase(brush, (float*)points, count, GL_TRIANGLE_STRIP, 2);
	//free(fs);
}

void Graphics::FillPoly(const Brush& brush, float *points, int count) const{
	FillPolyBase(brush, points, count);
	//DrawBase(brush, points, count, GL_TRIANGLE_STRIP, 3);
}

void Graphics::FillRect(const Brush& brush, const Rect& rect) const{
	FillRect(brush, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight());
}

void Graphics::FillRect(const Brush& brush, const Vector2& location, const Vector2& size) const{
	FillRect(brush, location.X(), location.Y(), size.X(), size.Y());
}

void Graphics::FillRect(const Brush& brush, float left, float top, float width, float height) const{
	float ps[12];
	ps[0] = left;
	ps[1] = top;
	ps[2] = 0;
	ps[3] = left + width;
	ps[4] = top;
	ps[5] = 0;
	ps[6] = left + width;
	ps[7] = top + height;
	ps[8] = 0;
	ps[9] = left;
	ps[10] = top + height;
	ps[11] = 0;
	FillPolyBase(brush, ps, 4);
}

void Graphics::FillRoundRect(const Brush& brush, const Rect& rect, float radius, int corner) const{
	FillRoundRect(brush, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight(), radius, corner);
}

void Graphics::FillRoundRect(const Brush& brush, const Vector2& location, const Vector2& size, float radius, int corner) const{
	FillRoundRect(brush, location.X(), location.Y(), size.X(), size.Y(), radius, corner);
}

void Graphics::FillRoundRect(const Brush& brush, float left, float top, float width, float height, float radius, int corner) const{
	glUniform1i(uMode, 2);
	glUniform2f(uCenter, left + width / 2, top + height / 2);
	glUniform1f(uRadiusX, width/2);
	glUniform1f(uRadiusY, height/2);
	glUniform1f(uRadiusCorner, radius);
	glUniform1f(uCorner, corner);
	glUniform1f(uWidth, -1);
	FillRect(brush, left, top, width, height);
	glUniform1i(uMode, 0);
	/*radiusX = MAX(MIN(width / 2, radiusX), 0);
	radiusY = MAX(MIN(height / 2, radiusY), 0);
	FillPie(brush, left, top, radiusX * 2, radiusY * 2, 180, 90);
	FillPie(brush, left + width - radiusX * 2, top, radiusX * 2, radiusY * 2, 270, 90);
	FillPie(brush, left + width - radiusX * 2, top + height - radiusY * 2, radiusX * 2, radiusY * 2, 0, 90);
	FillPie(brush, left, top + height - radiusY * 2, radiusX * 2, radiusY * 2, 90, 90);

	FillRect(brush, left + radiusX, top, width - radiusX * 2, height);
	FillRect(brush, left, top + radiusY, radiusX, height - radiusY * 2);
	FillRect(brush, left + width - radiusX, top + radiusY, radiusX, height - radiusY * 2);*/
}

void Graphics::FillOval(const Brush& brush, const Rect& rect) const{
	FillOval(brush, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight());
}

void Graphics::FillOval(const Brush& brush, const Vector2& location, const Vector2& size) const{
	FillOval(brush, location.X(), location.Y(), size.X(), size.Y());
}

void Graphics::FillOval(const Brush& brush, float left, float top, float width, float height) const{
	glUniform1i(uMode, 1);
	glUniform1f(uStartAngle, 0);
	glUniform1f(uSweepAngle, 360);
	glUniform2f(uCenter, left + width / 2, top + height / 2);
	glUniform1f(uRadiusX, width/2);
	glUniform1f(uRadiusY, height/2);
	glUniform1f(uWidth, -1);
	FillRect(brush, left, top, width, height);
	glUniform1i(uMode, 0);
	//FillOvalBase(brush, left, top, width, height, 0, 360);
}

void Graphics::FillArcClosed(const Brush& brush, const Rect& rect, float startAngle, float sweepAngle) const{
	FillArcClosed(brush, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight(), startAngle, sweepAngle);
}

void Graphics::FillArcClosed(const Brush& brush, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const{
	FillArcClosed(brush, location.X(), location.Y(), size.X(), size.Y(), startAngle, sweepAngle);
}

void Graphics::FillArcClosed(const Brush& brush, float left, float top, float width, float height, float startAngle, float sweepAngle) const{
	FillOvalBase(brush, left, top, width, height, startAngle, sweepAngle);
}

void Graphics::FillPie(const Brush& brush, const Rect& rect, float startAngle, float sweepAngle) const{
	FillPie(brush, rect.Left(), rect.Top(), rect.GetWidth(), rect.GetHeight(), startAngle, sweepAngle);
}

void Graphics::FillPie(const Brush& brush, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const{
	FillPie(brush, location.X(), location.Y(), size.X(), size.Y(), startAngle, sweepAngle);
}

void Graphics::FillPie(const Brush& brush, float left, float top, float width, float height, float startAngle, float sweepAngle) const{
	glUniform1i(uMode, 1);
	glUniform1f(uStartAngle, startAngle);
	glUniform1f(uSweepAngle, sweepAngle);
	glUniform2f(uCenter, left + width / 2, top + height / 2);
	glUniform1f(uRadiusX, width / 2);
	glUniform1f(uRadiusY, height / 2);
	glUniform1f(uWidth, -1);
	FillRect(brush, left, top, width, height);
	glUniform1i(uMode, 0);
	//FillOvalBase(brush, left, top, width, height, startAngle, sweepAngle, true);
}


void Graphics::Translate(float x, float y) const{
	SE::Translate(x, y, 0);
}

void Graphics::Translate(const Vector2& offset) const{
	SE::Translate(offset.X(), offset.Y(), 0);
}

void Graphics::Rotate(float angle) const{
	SE::Rotate(angle, 0, 0, 1);
}

void Graphics::RotateAt(float angle, const Vector2& center) const{
    SE::RotateAt(angle, 0, 0, 1, center.X(), center.Y(), 0);
}

void Graphics::Scale(float scaleX, float scaleY) const{
	SE::Scale(scaleX, scaleY, 1);
}

void Graphics::Scale(const Vector2& scalar) const{
	SE::Scale(scalar.X(), scalar.Y(), 1);
}

void Graphics::Scale(float scale) const{
	SE::Scale(scale, scale, 1);
}

void Graphics::ScaleAt(float scaleX, float scaleY, const Vector2& center) const{
    SE::ScaleAt(scaleX, scaleY, 1, center.X(), center.Y(), 0);
}

void Graphics::ScaleAt(const Vector2& scalar, const Vector2& center) const{
    SE::ScaleAt(scalar.X(), scalar.Y(), 1, center.X(), center.Y(), 0);
}

void Graphics::ScaleAt(float scale, const Vector2& center) const{
    SE::ScaleAt(scale, scale, 1, center.X(), center.Y(), 0);
}

void Graphics::Push() const{
	SE::PushMatrix();
}

void Graphics::Pop() const{
	SE::PopMatrix();
}

SE_END