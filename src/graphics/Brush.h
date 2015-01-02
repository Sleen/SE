#pragma once

#include "Macro.h"
#include "Texture.h"

SE_BEGIN

class Brush{
private:
	int color = 0xff000000;
	const GLubyte* hatch = NULL;
	Texture* texture = NULL;

public:
	Brush(){}
	Brush(Texture* texture){ this->texture = texture; color = 0xffffffff; }
	Brush(Texture* texture, int color){ this->texture = texture; this->color = color; }
	Brush(int color){ this->color = color; }
	Brush& SetColor(int color){ this->color = color;  return *this; }
	Brush& SetHatch(const GLubyte *hatch){ this->hatch = hatch; return *this; }
	int GetColor() const{ return color; }
	const GLubyte* GetHatch() const{ return hatch; }
	const Texture* GetTexture() const{ return texture; }
};

#ifdef HAS_HATCH_BRUSH

class HatchStyle{
public:
	const static GLubyte Horizontal[];
	const static GLubyte Vertical[];
	const static GLubyte ForwardDiagonal[];
	const static GLubyte BackwardDiagonal[];
	const static GLubyte LargeGrid[];
	const static GLubyte DiagonalCross[];
	const static GLubyte Percent05[];
	const static GLubyte Percent10[];
	const static GLubyte Percent20[];
	const static GLubyte Percent25[];
	const static GLubyte Percent30[];
	const static GLubyte Percent40[];
	const static GLubyte Percent50[];
	const static GLubyte Percent60[];
	const static GLubyte Percent70[];
	const static GLubyte Percent75[];
	const static GLubyte Percent80[];
	const static GLubyte Percent90[];
	const static GLubyte LightDownwardDiagonal[];
	const static GLubyte LightUpwardDiagonal[];
	const static GLubyte DarkDownwardDiagonal[];
	const static GLubyte DarkUpwardDiagonal[];
	const static GLubyte WideDownwardDiagonal[];
	const static GLubyte WideUpwardDiagonal[];
	const static GLubyte LightVertical[];
	const static GLubyte LightHorizontal[];
	const static GLubyte NarrowVertical[];
	const static GLubyte NarrowHorizontal[];
	const static GLubyte DarkVertical[];
	const static GLubyte DarkHorizontal[];
	const static GLubyte DashedDownwardDiagonal[];
	const static GLubyte DashedUpwardDiagonal[];
	const static GLubyte DashedHorizontal[];
	const static GLubyte DashedVertical[];
	const static GLubyte SmallConfetti[];
	const static GLubyte LargeConfetti[];
	const static GLubyte ZigZag[];
	const static GLubyte Wave[];
	const static GLubyte DiagonalBrick[];
	const static GLubyte HorizontalBrick[];
	const static GLubyte Weave[];
	const static GLubyte Plaid[];
	const static GLubyte Divot[];
	const static GLubyte DottedGrid[];
	const static GLubyte DottedDiamond[];
	const static GLubyte Shingle[];
	const static GLubyte Trellis[];
	const static GLubyte Sphere[];
	const static GLubyte SmallGrid[];
	const static GLubyte SmallCheckerBoard[];
	const static GLubyte LargeCheckerBoard[];
	const static GLubyte OutlinedDiamond[];
	const static GLubyte SolidDiamond[];

};

#endif

class Brushes{
public:

	const static Brush Transparent;
	const static Brush AliceBlue;
	const static Brush AntiqueWhite;
	const static Brush Aqua;
	const static Brush Aquamarine;
	const static Brush Azure;
	const static Brush Beige;
	const static Brush Bisque;
	const static Brush Black;
	const static Brush BlanchedAlmond;
	const static Brush Blue;
	const static Brush BlueViolet;
	const static Brush Brown;
	const static Brush BurlyWood;
	const static Brush CadetBlue;
	const static Brush Chartreuse;
	const static Brush Chocolate;
	const static Brush Coral;
	const static Brush CornflowerBlue;
	const static Brush Cornsilk;
	const static Brush Crimson;
	const static Brush Cyan;
	const static Brush DarkBlue;
	const static Brush DarkCyan;
	const static Brush DarkGoldenrod;
	const static Brush DarkGray;
	const static Brush DarkGreen;
	const static Brush DarkKhaki;
	const static Brush DarkMagenta;
	const static Brush DarkOliveGreen;
	const static Brush DarkOrange;
	const static Brush DarkOrchid;
	const static Brush DarkRed;
	const static Brush DarkSalmon;
	const static Brush DarkSeaGreen;
	const static Brush DarkSlateBlue;
	const static Brush DarkSlateGray;
	const static Brush DarkTurquoise;
	const static Brush DarkViolet;
	const static Brush DeepPink;
	const static Brush DeepSkyBlue;
	const static Brush DimGray;
	const static Brush DodgerBlue;
	const static Brush Firebrick;
	const static Brush FloralWhite;
	const static Brush ForestGreen;
	const static Brush Fuchsia;
	const static Brush Gainsboro;
	const static Brush GhostWhite;
	const static Brush Gold;
	const static Brush Goldenrod;
	const static Brush Gray;
	const static Brush Green;
	const static Brush GreenYellow;
	const static Brush Honeydew;
	const static Brush HotPink;
	const static Brush IndianRed;
	const static Brush Indigo;
	const static Brush Ivory;
	const static Brush Khaki;
	const static Brush Lavender;
	const static Brush LavenderBlush;
	const static Brush LawnGreen;
	const static Brush LemonChiffon;
	const static Brush LightBlue;
	const static Brush LightCoral;
	const static Brush LightCyan;
	const static Brush LightGoldenrodYellow;
	const static Brush LightGreen;
	const static Brush LightGray;
	const static Brush LightPink;
	const static Brush LightSalmon;
	const static Brush LightSeaGreen;
	const static Brush LightSkyBlue;
	const static Brush LightSlateGray;
	const static Brush LightSteelBlue;
	const static Brush LightYellow;
	const static Brush Lime;
	const static Brush LimeGreen;
	const static Brush Linen;
	const static Brush Magenta;
	const static Brush Maroon;
	const static Brush MediumAquamarine;
	const static Brush MediumBlue;
	const static Brush MediumOrchid;
	const static Brush MediumPurple;
	const static Brush MediumSeaGreen;
	const static Brush MediumSlateBlue;
	const static Brush MediumSpringGreen;
	const static Brush MediumTurquoise;
	const static Brush MediumVioletRed;
	const static Brush MidnightBlue;
	const static Brush MintCream;
	const static Brush MistyRose;
	const static Brush Moccasin;
	const static Brush NavajoWhite;
	const static Brush Navy;
	const static Brush OldLace;
	const static Brush Olive;
	const static Brush OliveDrab;
	const static Brush Orange;
	const static Brush OrangeRed;
	const static Brush Orchid;
	const static Brush PaleGoldenrod;
	const static Brush PaleGreen;
	const static Brush PaleTurquoise;
	const static Brush PaleVioletRed;
	const static Brush PapayaWhip;
	const static Brush PeachPuff;
	const static Brush Peru;
	const static Brush Pink;
	const static Brush Plum;
	const static Brush PowderBlue;
	const static Brush Purple;
	const static Brush Red;
	const static Brush RosyBrown;
	const static Brush RoyalBlue;
	const static Brush SaddleBrown;
	const static Brush Salmon;
	const static Brush SandyBrown;
	const static Brush SeaGreen;
	const static Brush SeaShell;
	const static Brush Sienna;
	const static Brush Silver;
	const static Brush SkyBlue;
	const static Brush SlateBlue;
	const static Brush SlateGray;
	const static Brush Snow;
	const static Brush SpringGreen;
	const static Brush SteelBlue;
	const static Brush Tan;
	const static Brush Teal;
	const static Brush Thistle;
	const static Brush Tomato;
	const static Brush Turquoise;
	const static Brush Violet;
	const static Brush Wheat;
	const static Brush White;
	const static Brush WhiteSmoke;
	const static Brush Yellow;
	const static Brush YellowGreen;

};

SE_END