#include "Font.h"
#include "Shader.h"
#include "Brush.h"
#include "Graphics.h"

SE_BEGIN


int UTF8ToUnicode(uint16_t *unicode, uint8_t *p, int bytes)
{
	int e = 0, n = 0;
	if (p && unicode)
	{
		if (*p >= 0xf0)
		{
			if (bytes < 4)
				return -1;
			/* 4:<11110000> */
			e = (p[0] & 0x07) << 18;
			e |= (p[1] & 0x3f) << 12;
			e |= (p[2] & 0x3f) << 6;
			e |= (p[3] & 0x3f);
			n = 4;
		}
		else if (*p >= 0xe0)
		{
			if (bytes < 3)
				return -1;
			/* 3:<11100000> */
			e = (p[0] & 0x0f) << 12;
			e |= (p[1] & 0x3f) << 6;
			e |= (p[2] & 0x3f);
			n = 3;
		}
		else if (*p >= 0xc0)
		{
			if (bytes < 2)
				return -1;
			/* 2:<11000000> */
			e = (p[0] & 0x1f) << 6;
			e |= (p[1] & 0x3f);
			n = 2;
		}
		else
		{
			if (bytes < 1)
				return -1;
			e = p[0];
			n = 1;
		}
		*unicode = e;
	}
	/* Return bytes count of this utf-8 character */
	return n;
}

int UTF8StrToUnicodeStr(uint16_t * unicode_str, uint8_t * utf8_str, int unicode_str_size){
	uint16_t unicode = 0;
	int n = 0;
	int count = 0;
	unsigned char *s = NULL;
	unsigned short *e = NULL;

	s = utf8_str;
	e = unicode_str;

	if ((utf8_str) && (unicode_str))
	{
		while (*s)
		{
			if ((n = UTF8ToUnicode(&unicode, s, 4)) > 0)
			{
				if (++count >= unicode_str_size)
				{
					return count;
				}
				else
				{
					*e = (unsigned short)unicode;
					e++;
					*e = 0;

					s += n;
				}
			}
			else
			{
				/* Converting error occurs */
				return count;
			}
		}
	}

	return count;
}


FT_Library Font::lib = NULL;

Font::Font(const string& file, float size, int TEXTURE_SIZE){
	if (!lib){
		if (FT_Init_FreeType(&lib))
		{
			lib = NULL;
			DEBUG_LOG("failed to init freetype!\n");
			return;
		}
	}

	if (FT_New_Face(lib, file.c_str(), 0, &face)){
		face = NULL;
		DEBUG_LOG("failed to load font \"%s\"!\n", file.c_str());
		return;
	}

	FT_Set_Char_Size(face, size * 0x40, size * 0x40, 96, 96);
	this->size = size;
	this->TEXTURE_SIZE = TEXTURE_SIZE;
	memset(map, 0, sizeof(map));
	map[0x000A] = new Char;
	map[0x000D] = new Char;
	map[0x0085] = new Char;
	map[0x2028] = new Char;
	map[0x2029] = new Char;
}

Font::~Font(){
	DEBUG_LOG("delete font\n");
	if (!textures.empty())
		glDeleteTextures(textures.size(), textures.data());

	for (int i = 0; i < USHRT_MAX; ++i)
	if (map[i])
		delete map[i];
}

void Font::PutChar(char_type c){
	//if (map.find(c) != map.end())
	if (map[c] != NULL)
		return;

	FT_Glyph    glyph;
	FT_Load_Glyph(face, FT_Get_Char_Index(face, c), FT_LOAD_FORCE_AUTOHINT | FT_LOAD_NO_BITMAP);
	FT_Error error = FT_Get_Glyph(face->glyph, &glyph);
	if (!error)
	{
		FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
		FT_BitmapGlyph    bitmap_glyph = (FT_BitmapGlyph)glyph;
		FT_Bitmap &     bitmap = bitmap_glyph->bitmap;

		int w = bitmap.width;
		int h = bitmap.rows;
		FT_BBox b;
		FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_UNSCALED, &b);
		
		if (w > TEXTURE_SIZE || h > TEXTURE_SIZE){
			DEBUG_LOG("error: have no enough space to place the character \"\\%x\", size: %d x %d\n", c, w, h);
			return;
		}

		if (left + w > TEXTURE_SIZE){
			top += maxHeight + 1;
			left = 0;
		}

		glActiveTexture(GL_TEXTURE1);
		if (textures.empty() || top + h > TEXTURE_SIZE){
			GLuint id;
			glGenTextures(1, &id);
			DEBUG_LOG("texture id: %d\n", id);
			glBindTexture(GL_TEXTURE_2D, id);
			//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			textures.push_back(id);
			left = top = maxHeight = 0;
		}
		else
			glBindTexture(GL_TEXTURE_2D, textures.back());

		int l = w*h;
		int *d = new int[l];
		/*for (int i = 0; i < l; i++)
		{
			int alpha = 255 - bitmap.buffer[i];
			d[i] = alpha | alpha << 8 | alpha << 16 | 0xff000000;
		}*/
		for (int i = 0; i < l; ++i)
			d[i] = bitmap.buffer[i] << 24 | 0xffffff;
		glTexSubImage2D(GL_TEXTURE_2D, 0, left, top, w, h, GL_RGBA, GL_UNSIGNED_BYTE, d);
		delete[] d;

		glActiveTexture(GL_TEXTURE0);

		//glTexSubImage2D(GL_TEXTURE_2D, 0, left, top, w, h, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap.buffer);
		w = glyph->advance.x / 0x40 / 1024;
		//Char cc;
		//cc.bounds = Rect(left, top + (textures.size()-1)*TEXTURE_SIZE, w, h);
		//cc.left = bitmap_glyph->left;
		//cc.top = bitmap_glyph->top - face->size->metrics.y_ppem;//face->height/72;
		//map[c] = cc;

		Char *cc = new Char;
		cc->bounds = Rect(left, top + (textures.size() - 1)*TEXTURE_SIZE, w, h);
		cc->left = bitmap_glyph->left;
		cc->top = bitmap_glyph->top - face->size->metrics.y_ppem - face->size->metrics.descender/96;//face->height/72;
		cc->width = cc->bounds.GetWidth();
		cc->height = cc->bounds.GetHeight();
		map[c] = cc;

		left += w + 1;

		if (maxHeight < h)
			maxHeight = h;

		FT_Done_Glyph(glyph);

		count++;
	}
}

#ifdef ANDROID
#	define READ_CHAR(d, s, z)	UTF8ToUnicode((uint16_t*)d, (uint8_t*)s, z)
#else
#	define READ_CHAR(d, s, z)	mbtowc((wchar_t*)d, (char*)s, z)
#endif

void Font::DrawString(const Graphics& g, const Brush& brush, const string& s, int len, int x, int y, int w, int h, int flags){
	int xx = x;
	int yy = y;
	int rr = x + w;
	int bb = y + h;
    //g.DrawRect(Pens::Blue, x, y, w, h);
	if(len < 0) len = s.length();

	vector<int> ws, cs;
	int wi = 0, c = 0;
	
	for (int i = 0; i<len;){
		char_type cc;
		int rs = READ_CHAR(&cc, s.c_str() + i, len-i);
		if (rs>0)
			i += rs;
		else
			break;

		bool n = cc == L'\n' || cc == 0x0085 || cc == 0x2028 || cc == 0x2029;
		//if (!n && cc != L'\r')
		PutChar(cc);

        int ww = n ? 0 : map[cc]->width;
        if ((flags & FontFlag_NoWrap) == 0 && (n || (w >= 0 && wi + ww>w))){
            if (c == 0){
                ws.push_back(ww);
                cs.push_back(1);
                wi = 0;
                c = 0;
            }
            else{
                ws.push_back(wi);
                cs.push_back(c);
                wi = ww;
                c = 1;
                if (n){
                    wi = 0;
                }
            }
		}
		else {
			wi += ww;
			c++;
		}
    }
    if (c > 0){
        ws.push_back(wi);
        cs.push_back(c);
    }

    int rh = cs.size()*face->size->metrics.y_ppem;	// real height

	// vertical alignment
	switch ((flags >> 2) & 3) {
	case 1:
		yy += (h-rh) / 2;
		break;
	case 2:
		yy += h-rh;
		break;
	}

	//bool clip_not_whole = (flags & FontFlag_ClipNotWhole) != 0;

	glUniform1i(Program::CurrentShader()->useTex2, 1);

	int line = -1;
	int row = 0;
	yy -= face->size->metrics.y_ppem;
	for (int i = 0; i < len; ){

		if (row == 0){
			line++;
			xx = x;
			yy += face->size->metrics.y_ppem;
			switch (flags & 0x03) {
			case 1:
				xx += (w - ws[line]) / 2;
				break;
			case 2:
				xx += w - ws[line];
				break;
			}
		}

		char_type cc;
		int rs = READ_CHAR(&cc, s.c_str() + i, len - i);
		if (rs>0)
			i += rs;
		else
			return;
		if (++row == cs[line])
			row = 0;
		
		if (cc == L'\r')
			continue;
		bool n = cc == L'\n' || cc == 0x0085 || cc == 0x2028 || cc == 0x2029;	// new line
		if (!n){
			Char& c = *map[cc];
			Rect& r = c.bounds;
			float ss = TEXTURE_SIZE;
			float L = r.Left() / ss;
			float T = ((int)r.Top() % TEXTURE_SIZE) / ss;
			float R = r.Right() / ss;
			float B = T + c.height / ss;
			float ps[] = {
				L, T,
				R, T,
				R, B,
				L, B
			};
			glEnableVertexAttribArray(TEXCOORD_ATTRIB_LOC);
			glVertexAttribPointer(TEXCOORD_ATTRIB_LOC, 2, GL_FLOAT, GL_FALSE, 0, ps);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures[r.Top() / TEXTURE_SIZE]);
			glActiveTexture(GL_TEXTURE0);
            g.FillRect(brush, xx + c.left, yy - c.top, c.width, c.height);
            glDisableVertexAttribArray(TEXCOORD_ATTRIB_LOC);
            //glUniform1i(Program::CurrentShader()->useTex2, 0);
            //g.DrawRect(Pens::Green, xx + c.left, yy - c.top, c.width, c.height);
            //glUniform1i(Program::CurrentShader()->useTex2, 1);
			xx += c.width;
		}

	}

	glUniform1i(Program::CurrentShader()->useTex2, 0);
}

Vector2 Font::MeasureString(const string& s, int w, int h, int flags){
	int len = s.length();
	int xx = 0;
	int yy = 0;

	vector<int> ws, cs;
	int wi = 0, c = 0;

    for (int i = 0; i<len;){
        char_type cc;
        int rs = READ_CHAR(&cc, s.c_str() + i, len - i);
        if (rs>0)
            i += rs;
        else
            break;

        bool n = cc == L'\n' || cc == 0x0085 || cc == 0x2028 || cc == 0x2029;

        PutChar(cc);

        int ww = n ? 0 : map[cc]->width;
        if ((flags & FontFlag_NoWrap) == 0 && (n || (w >= 0 && wi + ww>w))){
            if (c == 0){
                ws.push_back(ww);
                cs.push_back(1);
                wi = 0;
                c = 0;
            }
            else{
                ws.push_back(wi);
                cs.push_back(c);
                wi = ww;
                c = 1;
                if (n){
                    wi = 0;
                }
            }
        }
        else {
            wi += ww;
            c++;
        }
    }
    if (c > 0){
        ws.push_back(wi);
        cs.push_back(c);
    }

	int max = 0;
	for (int i : ws){
		if (i > max){
			max = i;
		}
	}

	return Vector2(max, cs.size() * face->size->metrics.y_ppem);
}

SE_END
