#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype.h>
#include <ftglyph.h>

#include "Macro.h"
#include "Rect.h"

SE_BEGIN

class Brush;
class Graphics;

enum FontFlag{
	FontFlag_Left = 0,
	FontFlag_Center = 1,
	FontFlag_Right = 2,
	FontFlag_Top = 0,
	FontFlag_Middle = 4,
	FontFlag_Bottom = 8,
	FontFlag_ClipNotVisual = 0,
	FontFlag_ClipNotWhole = 32,
	FontFlag_NoWrap = 64
};

class Font{
protected:
	static FT_Library lib;
	FT_Face face = NULL;
	float size;
	int left = 0, top = 0, maxHeight;

	typedef struct{
		Rect bounds;
		int left;
		int top;
		int width;
		int height;
	} Char;

	vector<GLuint> textures;
	//unordered_map<char_type, Char> map;
	Char *map[USHRT_MAX];
	int count = 0;
	int TEXTURE_SIZE = 1024;

public:
	typedef uint16_t char_type;
	//typedef wchar_t char_type;

	Font(const string& file, float size = 10, int TEXTURE_SIZE = 512);
	~Font();
	void PutChar(char_type c);
	void DrawString(const Graphics& g, const Brush& brush, const string& s, int len, int x, int y, int w = -1, int h = -1, int flags = FontFlag_Left | FontFlag_Top);
	Vector2 MeasureString(const string& s, int w, int h, int flags);

    float GetSize() const{
        return size;
    }

    string GetName() const{
        return face->family_name;
    }

	float FontHeight() const{
		return face->height;
	}

	float GetTexturesCount() const{
		if (textures.empty())
			return 0;
		return (top + (float)left*maxHeight / TEXTURE_SIZE) / TEXTURE_SIZE + textures.size() - 1;
	}

	int GetCharsCount() const{
		return count;
	}

	const vector<GLuint>& GetTextures() const{
		return textures;
	}

	static void Free(){
		FT_Done_FreeType(lib);
		lib = NULL;
	}
};


SE_END
