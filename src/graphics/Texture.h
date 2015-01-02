#pragma once

#include "Macro.h"
#include "Transform.h"

SE_BEGIN

class Bitmap;

class Texture{
protected:
	GLuint id = 0;
	int width = 0, height = 0;
	static int current_id;
	
public:
	Texture(){}
	Texture(const Bitmap& bmp);
	Texture(const string& file);

	Texture(const Texture& tex){
		Delete();
		id = tex.id;
		width = tex.width;
		height = tex.height;
		const_cast<Texture&>(tex).id = 0;
	}

	Texture& operator=(const Texture& tex){
		Delete();
		id = tex.id;
		width = tex.width;
		height = tex.height;
		const_cast<Texture&>(tex).id = 0;
		return *this;
	}

	virtual ~Texture(){
		Delete();
	}

    // never invoke glBindTexture directly!
    static void Bind(GLuint id){
        if (current_id != id){
            glBindTexture(GL_TEXTURE_2D, id);
            current_id = id;
        }
    }

	void Bind() const{
		if (current_id != id){
			glBindTexture(GL_TEXTURE_2D, id);
			current_id = id;
		}
	}

	void Delete();
	
	void SetParam(GLenum param, GLfloat value){
		Bind();
		glTexParameterf(GL_TEXTURE_2D, param, value);
	}
	
	int GetWidth() const{
		return width;
	}
	
	int GetHeight() const{
		return height;
	}
	
	GLuint GetID() const{
		return id;
	}

    shared_ptr<Bitmap> SaveToBitmap() const;
};

class RenderableTexture{
protected:
	Texture* tex = NULL;
	bool del = false;
    GLuint depth_stencil_buffer = 0;
	GLuint fbo = 0;
	shared_ptr<Transform> transform, savedState;

    void Setup(bool useStencilBuffer);

public:
	RenderableTexture(int width, int height, bool useStencilBuffer = false);
    RenderableTexture(Texture* tex, bool useStencilBuffer = false);
	void Begin();
	void End();
    shared_ptr<Bitmap> SaveToBitmap() const;

	~RenderableTexture(){
        glDeleteFramebuffers(1, &fbo);

		if (del && tex)
			delete tex;

        if (depth_stencil_buffer)
            glDeleteRenderbuffers(1, &depth_stencil_buffer);
	}

	Texture* GetTexture(){
		return tex;
	}

};

SE_END
