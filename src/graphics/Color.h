#pragma once

#include "Macro.h"

SE_BEGIN

#ifdef RGB
#	undef RGB
#endif

#define A(c) ((c>>24)&0xff)
#define R(c) ((c>>16)&0xff)
#define G(c) ((c>>8)&0xff)
#define B(c) (c&0xff)

#define AF(c) (A(c)/255.0f)
#define RF(c) (R(c)/255.0f)
#define GF(c) (G(c)/255.0f)
#define BF(c) (B(c)/255.0f)

#define RGB(c) R(c), G(c), B(c)
#define ARGB(c) A(c), R(c), G(c), B(c)
#define RGBA(c) R(c), G(c), B(c), A(c)
#define RGBF(c) RF(c), GF(c), BF(c)
#define ARGBF(c) AF(c), RF(c), GF(c), BF(c)
#define RGBAF(c) RF(c), GF(c), BF(c), AF(c)

class Color
{
public:
//private:
//    int color;
//
//public:
//    Color(){
//        color = 0;
//    }
//
//    operator int(){
//        return color;
//    }
//
//    Color(int color){
//        this->color = color;
//    }
//
//    Color operator+(const Color& c){
//        return (min(255, A(color) + A(c.color))) << 24
//            | (min(255, R(color) + R(c.color))) << 16
//            | (min(255, G(color) + G(c.color))) << 8
//            | (min(255, B(color) + B(c.color)));
//    }
//
//    Color operator-(const Color& c){
//        return (max(0, A(color) - A(c.color))) << 24
//            | (max(0, R(color) - R(c.color))) << 16
//            | (max(0, G(color) - G(c.color))) << 8
//            | (max(0, B(color) - B(c.color)));
//    }
//
//    Color operator*(float s){
//        return min(255, max(0, (int)(A(color) * s))) << 24
//            | min(255, max(0, (int)(R(color) * s))) << 16
//            | min(255, max(0, (int)(G(color) * s))) << 8
//            | min(255, max(0, (int)(B(color) * s)));
//    }
//
	const static int Transparent = 0x00000000; 	// 0x00000000
	const static int AliceBlue = 0xFFF0F8FF; 	// 0xFFF0F8FF
	const static int AntiqueWhite = 0xFFFAEBD7; 	// 0xFFFAEBD7
	const static int Aqua = 0xFF00FFFF; 	// 0xFF00FFFF
	const static int Aquamarine = 0xFF7FFFD4; 	// 0xFF7FFFD4
	const static int Azure = 0xFFF0FFFF; 	// 0xFFF0FFFF
	const static int Beige = 0xFFF5F5DC; 	// 0xFFF5F5DC
	const static int Bisque = 0xFFFFE4C4; 	// 0xFFFFE4C4
	const static int Black = 0xFF000000; 	// 0xFF000000
	const static int BlanchedAlmond = 0xFFFFEBCD; 	// 0xFFFFEBCD
	const static int Blue = 0xFF0000FF; 	// 0xFF0000FF
	const static int BlueViolet = 0xFF8A2BE2; 	// 0xFF8A2BE2
	const static int Brown = 0xFFA52A2A; 	// 0xFFA52A2A
	const static int BurlyWood = 0xFFDEB887; 	// 0xFFDEB887
	const static int CadetBlue = 0xFF5F9EA0; 	// 0xFF5F9EA0
	const static int Chartreuse = 0xFF7FFF00; 	// 0xFF7FFF00
	const static int Chocolate = 0xFFD2691E; 	// 0xFFD2691E
	const static int Coral = 0xFFFF7F50; 	// 0xFFFF7F50
	const static int CornflowerBlue = 0xFF6495ED; 	// 0xFF6495ED
	const static int Cornsilk = 0xFFFFF8DC; 	// 0xFFFFF8DC
	const static int Crimson = 0xFFDC143C; 	// 0xFFDC143C
	const static int Cyan = 0xFF00FFFF; 	// 0xFF00FFFF
	const static int DarkBlue = 0xFF00008B; 	// 0xFF00008B
	const static int DarkCyan = 0xFF008B8B; 	// 0xFF008B8B
	const static int DarkGoldenrod = 0xFFB8860B; 	// 0xFFB8860B
	const static int DarkGray = 0xFFA9A9A9; 	// 0xFFA9A9A9
	const static int DarkGreen = 0xFF006400; 	// 0xFF006400
	const static int DarkKhaki = 0xFFBDB76B; 	// 0xFFBDB76B
	const static int DarkMagenta = 0xFF8B008B; 	// 0xFF8B008B
	const static int DarkOliveGreen = 0xFF556B2F; 	// 0xFF556B2F
	const static int DarkOrange = 0xFFFF8C00; 	// 0xFFFF8C00
	const static int DarkOrchid = 0xFF9932CC; 	// 0xFF9932CC
	const static int DarkRed = 0xFF8B0000; 	// 0xFF8B0000
	const static int DarkSalmon = 0xFFE9967A; 	// 0xFFE9967A
	const static int DarkSeaGreen = 0xFF8FBC8B; 	// 0xFF8FBC8B
	const static int DarkSlateBlue = 0xFF483D8B; 	// 0xFF483D8B
	const static int DarkSlateGray = 0xFF2F4F4F; 	// 0xFF2F4F4F
	const static int DarkTurquoise = 0xFF00CED1; 	// 0xFF00CED1
	const static int DarkViolet = 0xFF9400D3; 	// 0xFF9400D3
	const static int DeepPink = 0xFFFF1493; 	// 0xFFFF1493
	const static int DeepSkyBlue = 0xFF00BFFF; 	// 0xFF00BFFF
	const static int DimGray = 0xFF696969; 	// 0xFF696969
	const static int DodgerBlue = 0xFF1E90FF; 	// 0xFF1E90FF
	const static int Firebrick = 0xFFB22222; 	// 0xFFB22222
	const static int FloralWhite = 0xFFFFFAF0; 	// 0xFFFFFAF0
	const static int ForestGreen = 0xFF228B22; 	// 0xFF228B22
	const static int Fuchsia = 0xFFFF00FF; 	// 0xFFFF00FF
	const static int Gainsboro = 0xFFDCDCDC; 	// 0xFFDCDCDC
	const static int GhostWhite = 0xFFF8F8FF; 	// 0xFFF8F8FF
	const static int Gold = 0xFFFFD700; 	// 0xFFFFD700
	const static int Goldenrod = 0xFFDAA520; 	// 0xFFDAA520
	const static int Gray = 0xFF808080; 	// 0xFF808080
	const static int Green = 0xFF008000; 	// 0xFF008000
	const static int GreenYellow = 0xFFADFF2F; 	// 0xFFADFF2F
	const static int Honeydew = 0xFFF0FFF0; 	// 0xFFF0FFF0
	const static int HotPink = 0xFFFF69B4; 	// 0xFFFF69B4
	const static int IndianRed = 0xFFCD5C5C; 	// 0xFFCD5C5C
	const static int Indigo = 0xFF4B0082; 	// 0xFF4B0082
	const static int Ivory = 0xFFFFFFF0; 	// 0xFFFFFFF0
	const static int Khaki = 0xFFF0E68C; 	// 0xFFF0E68C
	const static int Lavender = 0xFFE6E6FA; 	// 0xFFE6E6FA
	const static int LavenderBlush = 0xFFFFF0F5; 	// 0xFFFFF0F5
	const static int LawnGreen = 0xFF7CFC00; 	// 0xFF7CFC00
	const static int LemonChiffon = 0xFFFFFACD; 	// 0xFFFFFACD
	const static int LightBlue = 0xFFADD8E6; 	// 0xFFADD8E6
	const static int LightCoral = 0xFFF08080; 	// 0xFFF08080
	const static int LightCyan = 0xFFE0FFFF; 	// 0xFFE0FFFF
	const static int LightGoldenrodYellow = 0xFFFAFAD2; 	// 0xFFFAFAD2
	const static int LightGreen = 0xFF90EE90; 	// 0xFF90EE90
	const static int LightGray = 0xFFD3D3D3; 	// 0xFFD3D3D3
	const static int LightPink = 0xFFFFB6C1; 	// 0xFFFFB6C1
	const static int LightSalmon = 0xFFFFA07A; 	// 0xFFFFA07A
	const static int LightSeaGreen = 0xFF20B2AA; 	// 0xFF20B2AA
	const static int LightSkyBlue = 0xFF87CEFA; 	// 0xFF87CEFA
	const static int LightSlateGray = 0xFF778899; 	// 0xFF778899
	const static int LightSteelBlue = 0xFFB0C4DE; 	// 0xFFB0C4DE
	const static int LightYellow = 0xFFFFFFE0; 	// 0xFFFFFFE0
	const static int Lime = 0xFF00FF00; 	// 0xFF00FF00
	const static int LimeGreen = 0xFF32CD32; 	// 0xFF32CD32
	const static int Linen = 0xFFFAF0E6; 	// 0xFFFAF0E6
	const static int Magenta = 0xFFFF00FF; 	// 0xFFFF00FF
	const static int Maroon = 0xFF800000; 	// 0xFF800000
	const static int MediumAquamarine = 0xFF66CDAA; 	// 0xFF66CDAA
	const static int MediumBlue = 0xFF0000CD; 	// 0xFF0000CD
	const static int MediumOrchid = 0xFFBA55D3; 	// 0xFFBA55D3
	const static int MediumPurple = 0xFF9370DB; 	// 0xFF9370DB
	const static int MediumSeaGreen = 0xFF3CB371; 	// 0xFF3CB371
	const static int MediumSlateBlue = 0xFF7B68EE; 	// 0xFF7B68EE
	const static int MediumSpringGreen = 0xFF00FA9A; 	// 0xFF00FA9A
	const static int MediumTurquoise = 0xFF48D1CC; 	// 0xFF48D1CC
	const static int MediumVioletRed = 0xFFC71585; 	// 0xFFC71585
	const static int MidnightBlue = 0xFF191970; 	// 0xFF191970
	const static int MintCream = 0xFFF5FFFA; 	// 0xFFF5FFFA
	const static int MistyRose = 0xFFFFE4E1; 	// 0xFFFFE4E1
	const static int Moccasin = 0xFFFFE4B5; 	// 0xFFFFE4B5
	const static int NavajoWhite = 0xFFFFDEAD; 	// 0xFFFFDEAD
	const static int Navy = 0xFF000080; 	// 0xFF000080
	const static int OldLace = 0xFFFDF5E6; 	// 0xFFFDF5E6
	const static int Olive = 0xFF808000; 	// 0xFF808000
	const static int OliveDrab = 0xFF6B8E23; 	// 0xFF6B8E23
	const static int Orange = 0xFFFFA500; 	// 0xFFFFA500
	const static int OrangeRed = 0xFFFF4500; 	// 0xFFFF4500
	const static int Orchid = 0xFFDA70D6; 	// 0xFFDA70D6
	const static int PaleGoldenrod = 0xFFEEE8AA; 	// 0xFFEEE8AA
	const static int PaleGreen = 0xFF98FB98; 	// 0xFF98FB98
	const static int PaleTurquoise = 0xFFAFEEEE; 	// 0xFFAFEEEE
	const static int PaleVioletRed = 0xFFDB7093; 	// 0xFFDB7093
	const static int PapayaWhip = 0xFFFFEFD5; 	// 0xFFFFEFD5
	const static int PeachPuff = 0xFFFFDAB9; 	// 0xFFFFDAB9
	const static int Peru = 0xFFCD853F; 	// 0xFFCD853F
	const static int Pink = 0xFFFFC0CB; 	// 0xFFFFC0CB
	const static int Plum = 0xFFDDA0DD; 	// 0xFFDDA0DD
	const static int PowderBlue = 0xFFB0E0E6; 	// 0xFFB0E0E6
	const static int Purple = 0xFF800080; 	// 0xFF800080
	const static int Red = 0xFFFF0000; 	// 0xFFFF0000
	const static int RosyBrown = 0xFFBC8F8F; 	// 0xFFBC8F8F
	const static int RoyalBlue = 0xFF4169E1; 	// 0xFF4169E1
	const static int SaddleBrown = 0xFF8B4513; 	// 0xFF8B4513
	const static int Salmon = 0xFFFA8072; 	// 0xFFFA8072
	const static int SandyBrown = 0xFFF4A460; 	// 0xFFF4A460
	const static int SeaGreen = 0xFF2E8B57; 	// 0xFF2E8B57
	const static int SeaShell = 0xFFFFF5EE; 	// 0xFFFFF5EE
	const static int Sienna = 0xFFA0522D; 	// 0xFFA0522D
	const static int Silver = 0xFFC0C0C0; 	// 0xFFC0C0C0
	const static int SkyBlue = 0xFF87CEEB; 	// 0xFF87CEEB
	const static int SlateBlue = 0xFF6A5ACD; 	// 0xFF6A5ACD
	const static int SlateGray = 0xFF708090; 	// 0xFF708090
	const static int Snow = 0xFFFFFAFA; 	// 0xFFFFFAFA
	const static int SpringGreen = 0xFF00FF7F; 	// 0xFF00FF7F
	const static int SteelBlue = 0xFF4682B4; 	// 0xFF4682B4
	const static int Tan = 0xFFD2B48C; 	// 0xFFD2B48C
	const static int Teal = 0xFF008080; 	// 0xFF008080
	const static int Thistle = 0xFFD8BFD8; 	// 0xFFD8BFD8
	const static int Tomato = 0xFFFF6347; 	// 0xFFFF6347
	const static int Turquoise = 0xFF40E0D0; 	// 0xFF40E0D0
	const static int Violet = 0xFFEE82EE; 	// 0xFFEE82EE
	const static int Wheat = 0xFFF5DEB3; 	// 0xFFF5DEB3
	const static int White = 0xFFFFFFFF; 	// 0xFFFFFFFF
	const static int WhiteSmoke = 0xFFF5F5F5; 	// 0xFFF5F5F5
	const static int Yellow = 0xFFFFFF00; 	// 0xFFFFFF00
	const static int YellowGreen = 0xFF9ACD32; 	// 0xFF9ACD32

    static int FromArgb(unsigned char alpha, unsigned char red, unsigned char green, unsigned char blue){
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }

    static int FromRgb(unsigned char red, unsigned char green, unsigned char blue){
        return 0xff000000 | (red << 16) | (green << 8) | blue;
    }

    static int AlphaColor(float alpha, int color){
        return ((int)(AF(color) * alpha * 255) << 24) | (color & 0x00ffffff);
    }

private:
    static int from_hex(const string& str){
        int r = 0;
        for (int i = 0; i < str.length(); i++){
            char c = str[i];
            int n = isdigit(c) ? c - '0' : isupper(c) ? c - 'A' + 10 : islower(c) ? c - 'a' + 10 : 0;
            r *= 16;
            r += n;
        }
        return r;
    }

public:

    static int FromName(const string& name){
        if (name.empty()) return Color::Black;
        if (name[0] == '#'){
            if (name.length() == 4){
                return FromRgb(
                    from_hex(name.substr(1, 1)) * 255 / 15, 
                    from_hex(name.substr(2, 1)) * 255 / 15,
                    from_hex(name.substr(3, 1)) * 255 / 15);
            }
            else if (name.length() == 5){
                return FromArgb(
                    from_hex(name.substr(1, 1)) * 255 / 15,
                    from_hex(name.substr(2, 1)) * 255 / 15,
                    from_hex(name.substr(3, 1)) * 255 / 15,
                    from_hex(name.substr(4, 1)) * 255 / 15);
            }
            else if (name.length() == 7){
                return FromRgb(
                    from_hex(name.substr(1, 2)),
                    from_hex(name.substr(3, 2)),
                    from_hex(name.substr(5, 2)));
            }
            else if (name.length() == 9){
                return FromArgb(
                    from_hex(name.substr(1, 2)),
                    from_hex(name.substr(3, 2)),
                    from_hex(name.substr(5, 2)),
                    from_hex(name.substr(7, 2)));
            }
            else{
                return Color::Black;
            }
        }
        return Color::Black;
    }
};

SE_END
