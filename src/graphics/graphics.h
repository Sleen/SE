#pragma once

#include "Macro.h"
#include "Vector.h"
#include "Rect.h"
#include "Pen.h"
#include "Brush.h"
#include "Shader.h"
#include "Color.h"

SE_BEGIN

class Pen;
class Brush;
class Window;

enum class ClipMode {
	None = 0,
	ClipSelf = 1,
	ClipByFBO = 2
};

enum class SizeMode { 
	None, 
	Strech, 
	Center, 
	Zoom, 
	Fill
};

class Graphics
{
private:
	static Program* shader;

	static GLuint uCenter;
	static GLuint uRadiusX;
	static GLuint uRadiusY;
	static GLuint uRadiusCorner;
	static GLuint uStartAngle;
	static GLuint uSweepAngle;
	static GLuint uMode;
	static GLuint uCorner;
	static GLuint uWidth;

	Window* window;

	Rect clipRect;
	ClipMode clipMode;
	void DrawLinesBase(const Pen& pen, float *points, int count, int type, int dimension = 3) const;
	void DrawOvalBase(const Pen& pen, float left, float top, float width, float height, float startAngle, float sweepAngle, int type, bool center = false) const;
	void FillPolyBase(const Brush& brush, float *points, int count, int dimension = 3) const;
	void FillOvalBase(const Brush& brush, float left, float top, float width, float height, float startAngle, float sweepAngle, bool center = false) const;
	void DrawBase(const Brush& brush, float *points, int count, int type, int dimension = 3) const;

public:
	const static int CLIP_MODE_REPLACE = 0;
	const static int CLIP_MODE_INTERSECT = 1;

	static void Init();
	static void Free(){
		if (shader){
			delete shader;
			shader = NULL;
		}
	}

	Graphics(Window* window);

	const Rect& GetClipRect(){
		return clipRect;
	}

	void Begin();
    static void UseShader(){
        shader->Use();
    }
    
	void SetClipRect(const Rect& rect, int mode = CLIP_MODE_REPLACE){
		if (mode == CLIP_MODE_REPLACE)
			clipRect = rect;
		else if (mode == CLIP_MODE_INTERSECT)
			clipRect = clipRect.IntersectRect(rect);
		glUniform4i(Program::CurrentShader()->uClipRect, clipRect.Left(), clipRect.Top(), clipRect.Right(), clipRect.Bottom());
	}

	void SetClipMode(ClipMode mode){
        this->clipMode = mode;
        glUniform1i(Program::CurrentShader()->uClipMode, (int)clipMode);
    }

	ClipMode GetClipMode() const{
		return clipMode;
	}
	
	void DrawImage(Texture& tex, const Rect& dest, SizeMode sizeMode = SizeMode::Strech, int color = Color::White, float cornerRadius = 0) const;
	void DrawImage(Texture& tex, const Rect& src, const Rect& dest, int color = Color::White, float cornerRadius = 0) const;

	void DrawDots(const Pen& pen, Vector2 *points, int count) const;
	void DrawDots(const Pen& pen, float *points, int count) const;
	void DrawDot(const Pen& pen, const Vector2& p) const;
	void DrawDot(const Pen& pen, float x, float y) const;
	void DrawLines(const Pen& pen, Vector2 *points, int count) const;
	void DrawLines(const Pen& pen, float *points, int count) const;
	void DrawPoly(const Pen& pen, Vector2 *points, int count) const;
	void DrawPoly(const Pen& pen, float *points, int count) const;
    void DrawLine(const Pen& pen, const Vector2& v1, const Vector2& v2) const;
    void DrawLine(const Pen& pen, const Vector3& v1, const Vector3& v2) const;
	void DrawLine(const Pen& pen, float x1, float y1, float x2, float y2) const;
	void DrawRect(const Pen& pen, const Rect& rect) const;
	void DrawRect(const Pen& pen, const Vector2& location, const Vector2& size) const;
	void DrawRect(const Pen& pen, float left, float top, float width, float height) const;
	void DrawRoundRect(const Pen& pen, const Rect& rect, float radius, int corner = 15) const;
	void DrawRoundRect(const Pen& pen, const Vector2& location, const Vector2& size, float radius, int corner = 15) const;
	void DrawRoundRect(const Pen& pen, float left, float top, float width, float height, float radius, int corner = 15) const;
	void DrawOval(const Pen& pen, const Rect& rect) const;
	void DrawOval(const Pen& pen, const Vector2& location, const Vector2& size) const;
	void DrawOval(const Pen& pen, float left, float top, float width, float height) const;
	void DrawArc(const Pen& pen, const Rect& rect, float startAngle, float sweepAngle) const;
	void DrawArc(const Pen& pen, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const;
	void DrawArc(const Pen& pen, float left, float top, float width, float height, float startAngle, float sweepAngle) const;
	void DrawArcClosed(const Pen& pen, const Rect& rect, float startAngle, float sweepAngle) const;
	void DrawArcClosed(const Pen& pen, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const;
	void DrawArcClosed(const Pen& pen, float left, float top, float width, float height, float startAngle, float sweepAngle) const;
	void DrawPie(const Pen& pen, const Rect& rect, float startAngle, float sweepAngle) const;
	void DrawPie(const Pen& pen, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const;
	void DrawPie(const Pen& pen, float left, float top, float width, float height, float startAngle, float sweepAngle) const;

	void FillPoly(const Brush& brush, Vector2 *points, int count) const;
	void FillPoly(const Brush& brush, float *points, int count) const;
	void FillRect(const Brush& brush, const Rect& rect) const;
	void FillRect(const Brush& brush, const Vector2& location, const Vector2& size) const;
	void FillRect(const Brush& brush, float left, float top, float width, float height) const;
	void FillRoundRect(const Brush& brush, const Rect& rect, float radius, int corner = 15) const;
	void FillRoundRect(const Brush& brush, const Vector2& location, const Vector2& size, float radius, int corner = 15) const;
	void FillRoundRect(const Brush& brush, float left, float top, float width, float height, float radius, int corner = 15) const;
	void FillOval(const Brush& brush, const Rect& rect) const;
	void FillOval(const Brush& brush, const Vector2& location, const Vector2& size) const;
	void FillOval(const Brush& brush, float left, float top, float width, float height) const;
	void FillArcClosed(const Brush& brush, const Rect& rect, float startAngle, float sweepAngle) const;
	void FillArcClosed(const Brush& brush, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const;
	void FillArcClosed(const Brush& brush, float left, float top, float width, float height, float startAngle, float sweepAngle) const;
	void FillPie(const Brush& brush, const Rect& rect, float startAngle, float sweepAngle) const;
	void FillPie(const Brush& brush, const Vector2& location, const Vector2& size, float startAngle, float sweepAngle) const;
	void FillPie(const Brush& brush, float left, float top, float width, float height, float startAngle, float sweepAngle) const;
	
	void Translate(float x, float y) const;
	void Translate(const Vector2& offset) const;
    void Rotate(float angle) const;
    void RotateAt(float angle, const Vector2& center) const;
	void Scale(float scaleX, float scaleY) const;
	void Scale(const Vector2& scale) const;
    void Scale(float scale) const;
    void ScaleAt(float scaleX, float scaleY, const Vector2& center) const;
    void ScaleAt(const Vector2& scale, const Vector2& center) const;
    void ScaleAt(float scale, const Vector2& center) const;
	void Push() const;
	void Pop() const;

};

SE_END
