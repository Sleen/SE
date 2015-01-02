#pragma once

#include "Macro.h"
#include "Vector.h"
#include "Rect.h"
#include "Color.h"
#include "Pen.h"
#include "Brush.h"
#include "Graphics.h"
#include "Event.h"
#include "Shader.h"
#include "Transform.h"
#include "Font.h"

SE_BEGIN

enum class MouseButton{ None, Left, Right, Middle };
enum class MouseAction{ None, Down, Up, Move };
enum class KeyAction{ Down, Up, Press };


class MouseEventArgs{
public:
	float X, Y;
	int Delta;
	MouseButton Button;
	MouseAction Action;

	Vector2 Location(){
		return Vector2(X, Y);
	}

	MouseEventArgs(float x, float y, MouseButton button, MouseAction action, int delta = 0){
		X = x;
		Y = y;
		Delta = delta;
		Button = button;
		Action = action;
	}
};

class KeyEventArgs{
public:
	int KeyCode;
	KeyAction Action;

	KeyEventArgs(int keyCode, KeyAction action){
		KeyCode = keyCode;
		Action = action;
	}
};

class DrawEventArgs{
private:
	Graphics g;
public:
	DrawEventArgs(Graphics g) : g(g) { }

	Graphics& GetGraphics(){
		return g;
	}
};

class Unit{
protected:
	static int widthPixel;
	static int heightPixel;
	static float widthInch;
	static float heightInch;
	static float screenDpi;

	static float rates[6];

public:
	const static int UNIT_PX = 0;		// pixels
	const static int UNIT_DP = 1;		// device independent pixels, 1dp = 1/160in
	const static int UNIT_MM = 2;
	const static int UNIT_CM = 3;
	const static int UNIT_IN = 4;		// inches, 1in = 25.4mm
	const static int UNIT_PT = 5;		// points, 1pt = 1/72in

	static float ScreenDpi(){
		return screenDpi;
	}

	static void InitDeviceInfo();

	static void Init(){
		InitDeviceInfo();
		rates[0] = 1;
		rates[1] = screenDpi / 160;
		rates[2] = screenDpi / 25.4f;
		rates[3] = screenDpi / 2.54f;
		rates[4] = screenDpi;
		rates[5] = screenDpi / 72;
	}

	static float ToPixel(float value, int unit){
		return value * rates[unit];
	}
};

class Control
{
protected:
	Rect bounds;
	Padding padding, margin, border;
	int foreColor = Color::Black;
	Pen borderPen = Pen(Color::Black, 0);
	Brush backBrush = Brush(Color::Transparent);
	bool mouseIn = false, mouseDown = false;
	bool enabled = true, visibled = true;
	list< shared_ptr<Control> > controls;
	string text;
	ClipMode clipMode = ClipMode::None;

	static Font* defaultFont;
	static RenderableTexture *tex;

	friend class Window;

public:
	// Events
	Event<Control> Click, MouseEnter, MouseLeave;
	Event<Control, MouseEventArgs> MouseDown, MouseUp, MouseMove;
	Event<Control, KeyEventArgs> KeyDown, KeyUp, KeyPress;
	Event<Control, Vector2> SizeChanged, LocationChanged;
	Event<Control, DrawEventArgs> Draw;
	Event<Control, float> Update;

	Control(){
		bounds = Rect(0, 0, 100, 100);
	}

	Control(Rect bounds){
		//this->bounds = bounds;
		this->bounds.Set(Unit::ToPixel(bounds.Left(), Unit::UNIT_DP), 
			Unit::ToPixel(bounds.Top(), Unit::UNIT_DP), 
			Unit::ToPixel(bounds.GetWidth(), Unit::UNIT_DP),
			Unit::ToPixel(bounds.GetHeight(), Unit::UNIT_DP));
	}

	virtual ~Control(){}

	bool IsEnabled() const{
		return enabled;
	}

	bool IsVisibled() const{
		return visibled;
	}

	void SetEnabled(bool enabled){
		this->enabled = enabled;
	}

	void SetVisibled(bool visibled){
		this->visibled = visibled;
	}

	void SetText(const string& text){
		this->text = text;
	}

	const string& GetText() const{
		return text;
	}

	static Font* GetDefaultFont(){
		if (defaultFont == NULL){
#ifdef WINDOWS
			string ff = "C:\\Windows\\Fonts\\msyh.ttc";
#elif defined(ANDROID)
			string ff = "/system/fonts/DroidSansFallback.ttf";
#endif
			defaultFont = new Font(ff, 12);
		}

		return defaultFont;
	}

	static void SetDefaultFont(Font* font){
		defaultFont = font;
	}

	void SetClipMode(ClipMode mode){
		this->clipMode = mode;
	}

	ClipMode GetClipMode() const{
		return clipMode;
	}

	virtual void OnUpdate(float dt){
		Update(*this, dt);
	}

	virtual void OnMeasure(){

	}

	virtual void OnLayout(){

	}

	virtual void OnDraw(DrawEventArgs e){
		Graphics g = e.GetGraphics();

		g.Push();
		g.Translate(border.GetLeft(), border.GetTop());
		OnDrawBack(g);
		OnDrawFore(g);
		g.Pop();
		OnDrawBorder(g);
		//g.FillRect(0x40ff0000, g.GetClipRect());
		Draw(*this, e);
	}

	virtual void OnDrawFore(const Graphics& g){

	}

	virtual void OnDrawBack(const Graphics& g){
		if (backBrush.GetColor() != Color::Transparent)
			g.FillRect(backBrush, GetClientRect());
	}

	virtual void OnDrawBorder(const Graphics& g){
		if (!border.IsZero())
			g.DrawRoundRect(borderPen, GetClientRect(), border.GetWidth() / 2);
	}

	virtual void SetBounds(float left, float top, float width, float height){
		bounds.Set(left, top, width, height);
	}

	virtual void SetBounds(const Rect& bounds){
		this->bounds.Set(bounds);
	}

	const Rect& GetBounds() const{
		return bounds;
	}

	const Vector2 GetSize() const{
		return bounds.GetSize();
	}

	const Vector2 GetLocation() const{
		return bounds.GetLocation();
	}

	virtual void SetLocation(const Vector2& location){
		if (bounds.GetLocation() == location)
			return;
		bounds.SetLocation(location);
		LocationChanged(*this, location);
	}

	virtual void SetLocation(float left, float top){
		SetLocation(Vector2(left, top));
	}

	virtual void SetSize(const Vector2& size){
		if (bounds.GetSize() == size)
			return;
		bounds.SetSize(size);
		OnResize(GetClientSize());
	}

	virtual void SetSize(float width, float height){
		SetSize(Vector2(width, height));
	}

	virtual void SetClientSize(float clientWidth, float clientHeight){
		SetSize(clientWidth + border.GetWidth(), clientHeight + border.GetHeight());
	}

	virtual void SetClientSize(const Vector2& clientSize){
		SetClientSize(clientSize.X(), clientSize.Y());
	}

	float GetLeft() const{
		return bounds.Left();
	}

	float GetTop() const{
		return bounds.Top();
	}

	float GetWidth() const{
		return bounds.GetWidth();
	}

	float GetHeight() const{
		return bounds.GetHeight();
	}

	float GetClientWidth() const{
		return bounds.GetWidth() - border.GetWidth();
	}

	float GetClientHeight() const{
		return bounds.GetHeight() - border.GetHeight();
	}

	const Vector2 GetClientSize() const{		// size of client rect
		return Vector2(GetClientWidth(), GetClientHeight());
	}

	const Rect GetClientRect() const{			// the region without border
		return Rect(Vector2(), GetClientSize());
	}

	const Rect GetContentRect() const{
		return GetClientRect().GetPaddedRect(padding);
	}

	const Padding& GetPadding()const{
		return padding;
	}

	const Padding& GetMargin(){
		return margin;
	}

	const Padding& GetBorder(){
		return border;
	}

	void SetPadding(const Padding& padding){
		this->padding = padding;
	}

	void SetMargin(const Padding& margin){
		this->margin = margin;
	}

	void SetBorder(const Padding& border){
		borderPen.SetWidth(border.GetLeft());
		this->border = border;
	}

	void SetForeColor(int color){
		foreColor = color;
	}

	int GetForeColor() const{
		return foreColor;
	}

	void SetBackColor(int color){
		backBrush.SetColor(color);
	}

	int GetBackColor() const{
		return backBrush.GetColor();
	}

	void SetBorderColor(int color){
		borderPen.SetColor(color);
	}

	int GetBorderColor() const{
		return borderPen.GetColor();
	}



	virtual void OnResize(const Vector2& size){
		SizeChanged(*this, size);
	}

	virtual void OnKeyDown(KeyEventArgs e){
		KeyDown(*this, e);
	}

	virtual void OnKeyUp(KeyEventArgs e){
		KeyUp(*this, e);
	}

	virtual void OnKeyPress(KeyEventArgs e){
		KeyPress(*this, e);
	}

	virtual void OnMouseDown(MouseEventArgs e)
	{
		mouseDown = true;
		MouseDown(*this, e);
	}

	virtual void OnMouseMove(MouseEventArgs e){
		MouseMove(*this, e);
		if (mouseIn)
		{
			//if (!ClientRect.Contains(e.Location))
			//    OnMouseLeave(new EventArgs());
		}
		else
		{
			if (GetClientRect().Contains(e.X, e.Y))
				OnMouseEnter();
		}
	}

	virtual void OnMouseUp(MouseEventArgs e){
		mouseDown = false;
		MouseUp(*this, e);
		if (mouseIn){
			if (!GetClientRect().Contains(e.X, e.Y))
				OnMouseLeave();
			else
				OnClick();
		}
	}

	virtual void OnMouseEnter(){
		if (!mouseIn){
			mouseIn = true;
			MouseEnter(*this);
		}
	}

	virtual void OnMouseLeave(){
		if (mouseIn){
			mouseIn = false;
			MouseLeave(*this);
		}
	}

	virtual void OnClick(){
		Click(*this);
	}
};

class Container : public Control{
protected:
	list<Control*> controls;
	bool mousePreview = false;
	Control *downControl = NULL, *moveControl = NULL;
	
public:
	Container(){
		
	}

	Container(Rect bounds) : Control(bounds){
		
	}

	virtual ~Container(){
		for (auto c : controls)
			delete c;
		controls.clear();
		downControl = moveControl = NULL;
	}

	list<Control*>& Controls(){
		return controls;
	}

	void Add(Control *control){
		controls.push_back(control);
	}

	void Remove(Control *control){
		controls.remove(control);
	}

	virtual void OnUpdate(float dt){
		Control::OnUpdate(dt);
		for (Control* c : controls){
			c->OnUpdate(dt);
		}
	}

	virtual void OnDraw(DrawEventArgs e);

	virtual void OnMouseDown(MouseEventArgs e)
	{
		if (mousePreview)
		{
			downControl = this;
			Control::OnMouseDown(e);
		}
		else
		{
			for (list<Control*>::reverse_iterator c = controls.rbegin(); c != controls.rend(); c++)
			{
				if ((*c)->IsVisibled() && (*c)->IsEnabled() && (*c)->GetBounds().Contains(e.Location()))
				{
					downControl = *c;
					(*c)->OnMouseDown(MouseEventArgs(e.X - (*c)->GetBounds().Left(), e.Y - (*c)->GetBounds().Top(), e.Button, e.Action, e.Delta));
					return;
				}
			}
			downControl = this;
			Control::OnMouseDown(e);
		}
	}

	virtual void OnMouseMove(MouseEventArgs e)
	{
		Control *con = NULL;
		if (downControl)
		{
			if (downControl == this)
			{
				con = this;
			}
			else
			{
				con = downControl;
			}
		}
		else
		{
			if (mousePreview)
			{
				con = this;
			}
			else
			{
				for (list<Control*>::reverse_iterator c = controls.rbegin(); c != controls.rend(); c++)
				{
					if ((*c)->IsVisibled() && (*c)->IsEnabled() && (*c)->GetBounds().Contains(e.Location()))
					{
						con = *c;
						break;
					}
				}
				if (!con)
				{
					con = this;
				}
			}
		}
		if (moveControl && con != moveControl)
		{
			moveControl->OnMouseLeave();
			//com.OnMouseEnter(new EventArgs());
		}
		moveControl = con;
		if (con == this)
		{
			Control::OnMouseMove(e);
		}
		else if (con)
		{
			con->OnMouseMove(MouseEventArgs(e.X - con->GetBounds().Left(), e.Y - con->GetBounds().Top(), e.Button, e.Action, e.Delta));
		}
	}

	virtual void OnMouseUp(MouseEventArgs e)
	{
		if (downControl != 0)
		{
			if (downControl == this)
			{
				Control::OnMouseUp(e);
			}
			else
			{
				downControl->OnMouseUp(MouseEventArgs(e.X - downControl->GetBounds().Left(), e.Y - downControl->GetBounds().Top(), e.Button, e.Action, e.Delta));
			}
			downControl = 0;
		}
		else
		{
			cout << "something goes wrong\n";
		}
	}

	virtual void OnKeyDown(KeyEventArgs e)
	{
		Control::OnKeyDown(e);
		for (list<Control*>::iterator c = controls.begin(); c != controls.end(); c++)
		{
			(*c)->OnKeyDown(e);
		}
	}

	virtual void OnKeyUp(KeyEventArgs e)
	{
		Control::OnKeyUp(e);
		for (list<Control*>::iterator c = controls.begin(); c != controls.end(); c++)
		{
			(*c)->OnKeyUp(e);
		}
	}

	virtual void OnKeyPress(KeyEventArgs e)
	{
		Control::OnKeyPress(e);
		for (list<Control*>::iterator c = controls.begin(); c != controls.end(); c++)
		{
			(*c)->OnKeyPress(e);
		}
	}
};

SE_END
