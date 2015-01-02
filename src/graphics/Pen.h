#pragma once

#include "Macro.h"
#include "Brush.h"

SE_BEGIN

class Pen
{
private:
	Brush brush;
	float width = 1;
	short dashPattern = 0xFFFF;
	int dashFactor = 1;
public:
	const static short Solid = 0xFFFF,	// 1111
		Dot = 0x5555,	// 0101
		Dash = 0xEEEE,	// 1110
		DashDot = 0xF6F6,	// 11110010
		DashDotDot = 0xEAEA;	// 11101010

	Pen(){ }
	Pen(int color){ brush = Brush(color); }
	Pen(int color, float width){ brush = Brush(color); this->width = width; }
	Pen(const Brush& brush){ this->brush = brush; }
	Pen(const Brush& brush, float width){ this->brush = brush; this->width = width; }
	Pen& SetColor(int color){ brush.SetColor(color); return *this; }
	Pen& SetWidth(float width){ this->width = width; return *this; }
	Pen& SetDashStyle(short pattern){ dashPattern = pattern; return *this; }
	Pen& SetDashFactor(int factor){ dashFactor = MAX(0, factor); return *this; }
	int GetColor() const{ return brush.GetColor(); }
	const Brush& GetBrush() const{ return brush; }
	float GetWidth() const{ return width; }
	short GetDashStyle() const{ return dashPattern; }
	int GetDashFactor() const{ return dashFactor; }
};

class Pens{
public:

	const static Pen Transparent;
	const static Pen AliceBlue;
	const static Pen AntiqueWhite;
	const static Pen Aqua;
	const static Pen Aquamarine;
	const static Pen Azure;
	const static Pen Beige;
	const static Pen Bisque;
	const static Pen Black;
	const static Pen BlanchedAlmond;
	const static Pen Blue;
	const static Pen BlueViolet;
	const static Pen Brown;
	const static Pen BurlyWood;
	const static Pen CadetBlue;
	const static Pen Chartreuse;
	const static Pen Chocolate;
	const static Pen Coral;
	const static Pen CornflowerBlue;
	const static Pen Cornsilk;
	const static Pen Crimson;
	const static Pen Cyan;
	const static Pen DarkBlue;
	const static Pen DarkCyan;
	const static Pen DarkGoldenrod;
	const static Pen DarkGray;
	const static Pen DarkGreen;
	const static Pen DarkKhaki;
	const static Pen DarkMagenta;
	const static Pen DarkOliveGreen;
	const static Pen DarkOrange;
	const static Pen DarkOrchid;
	const static Pen DarkRed;
	const static Pen DarkSalmon;
	const static Pen DarkSeaGreen;
	const static Pen DarkSlateBlue;
	const static Pen DarkSlateGray;
	const static Pen DarkTurquoise;
	const static Pen DarkViolet;
	const static Pen DeepPink;
	const static Pen DeepSkyBlue;
	const static Pen DimGray;
	const static Pen DodgerBlue;
	const static Pen Firebrick;
	const static Pen FloralWhite;
	const static Pen ForestGreen;
	const static Pen Fuchsia;
	const static Pen Gainsboro;
	const static Pen GhostWhite;
	const static Pen Gold;
	const static Pen Goldenrod;
	const static Pen Gray;
	const static Pen Green;
	const static Pen GreenYellow;
	const static Pen Honeydew;
	const static Pen HotPink;
	const static Pen IndianRed;
	const static Pen Indigo;
	const static Pen Ivory;
	const static Pen Khaki;
	const static Pen Lavender;
	const static Pen LavenderBlush;
	const static Pen LawnGreen;
	const static Pen LemonChiffon;
	const static Pen LightBlue;
	const static Pen LightCoral;
	const static Pen LightCyan;
	const static Pen LightGoldenrodYellow;
	const static Pen LightGreen;
	const static Pen LightGray;
	const static Pen LightPink;
	const static Pen LightSalmon;
	const static Pen LightSeaGreen;
	const static Pen LightSkyBlue;
	const static Pen LightSlateGray;
	const static Pen LightSteelBlue;
	const static Pen LightYellow;
	const static Pen Lime;
	const static Pen LimeGreen;
	const static Pen Linen;
	const static Pen Magenta;
	const static Pen Maroon;
	const static Pen MediumAquamarine;
	const static Pen MediumBlue;
	const static Pen MediumOrchid;
	const static Pen MediumPurple;
	const static Pen MediumSeaGreen;
	const static Pen MediumSlateBlue;
	const static Pen MediumSpringGreen;
	const static Pen MediumTurquoise;
	const static Pen MediumVioletRed;
	const static Pen MidnightBlue;
	const static Pen MintCream;
	const static Pen MistyRose;
	const static Pen Moccasin;
	const static Pen NavajoWhite;
	const static Pen Navy;
	const static Pen OldLace;
	const static Pen Olive;
	const static Pen OliveDrab;
	const static Pen Orange;
	const static Pen OrangeRed;
	const static Pen Orchid;
	const static Pen PaleGoldenrod;
	const static Pen PaleGreen;
	const static Pen PaleTurquoise;
	const static Pen PaleVioletRed;
	const static Pen PapayaWhip;
	const static Pen PeachPuff;
	const static Pen Peru;
	const static Pen Pink;
	const static Pen Plum;
	const static Pen PowderBlue;
	const static Pen Purple;
	const static Pen Red;
	const static Pen RosyBrown;
	const static Pen RoyalBlue;
	const static Pen SaddleBrown;
	const static Pen Salmon;
	const static Pen SandyBrown;
	const static Pen SeaGreen;
	const static Pen SeaShell;
	const static Pen Sienna;
	const static Pen Silver;
	const static Pen SkyBlue;
	const static Pen SlateBlue;
	const static Pen SlateGray;
	const static Pen Snow;
	const static Pen SpringGreen;
	const static Pen SteelBlue;
	const static Pen Tan;
	const static Pen Teal;
	const static Pen Thistle;
	const static Pen Tomato;
	const static Pen Turquoise;
	const static Pen Violet;
	const static Pen Wheat;
	const static Pen White;
	const static Pen WhiteSmoke;
	const static Pen Yellow;
	const static Pen YellowGreen;

};

SE_END