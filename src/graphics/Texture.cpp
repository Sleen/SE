#include "Texture.h"
#include "Bitmap.h"
#include "MyTime.h"
SE_BEGIN

int Texture::current_id = 0;

Texture::Texture(const Bitmap& bmp){
    glGenTextures(1, &id);
    //glBindTexture(GL_TEXTURE_2D, id);
    Bind();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.GetWidth(), bmp.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp.GetData());
	this->width = bmp.GetWidth();
	this->height = bmp.GetHeight();
    cout << "Texture " << id << endl;
}

Texture::Texture(const string& file){
	glGenTextures(1, &id);
	//glBindTexture(GL_TEXTURE_2D, id);
    Bind();
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	Bitmap bmp(file);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.GetWidth(), bmp.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp.GetData());
	this->width = bmp.GetWidth();
    this->height = bmp.GetHeight();
    cout << "Texture " << id << endl;
}

shared_ptr<Bitmap> Texture::SaveToBitmap() const{
    return make_shared<Bitmap>(id, 0, 0, width, height);
}

void Texture::Delete(){
	if (id){
		DEBUG_LOG("delete: %d\n", id);
		glDeleteTextures(1, &id);
		id = 0;
	}
}

void RenderableTexture::Setup(bool useStencilBuffer){
	glGenFramebuffers(1, &fbo);
	if (fbo == 0){
		DEBUG_LOG("failed to glGenFramebuffers\n");
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->GetID(), 0);

    if (useStencilBuffer){
        GLuint depth_stencil_rb;
        glGenRenderbuffers(1, &depth_stencil_rb);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_stencil_rb);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, tex->GetWidth(), tex->GetHeight());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_rb);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE){
        DEBUG_LOG("FBO error!\n");
    }

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    transform = make_shared<Transform>();
    Begin();
	Viewport(0, 0, tex->GetWidth(), tex->GetHeight());
	MatrixMode(PROJECTION_MATRIX);
	LoadIdentity();
	Ortho(0, tex->GetWidth(), 0, tex->GetHeight(), -1, 1);
	MatrixMode(MODELVIEW_MATRIX);
	LoadIdentity();
	End();
}

RenderableTexture::RenderableTexture(int width, int height, bool useStencilBuffer){
	tex = new Texture(Bitmap(width, height));
	del = true;
    Setup(useStencilBuffer);
    cout << "RenderableTexture " << tex->GetID() << endl;
}

RenderableTexture::RenderableTexture(Texture* tex, bool useStencilBuffer){
	this->tex = tex;
    Setup(useStencilBuffer);
    cout << "RenderableTexture " << tex->GetID() << endl;
}

void RenderableTexture::Begin(){
	//cout << "begin: " << fbo << endl;
	//int64_t t1 = GetMicroSecond();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	//int64_t t2 = GetMicroSecond();
	savedState = SaveState();
	LoadState(transform);
	//int64_t t3 = GetMicroSecond();
	//cout << t2 - t1 <<"/"<< t3 - t1 << endl;
}

void RenderableTexture::End(){
	//cout << "end: " << fbo << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	LoadState(savedState);
}

shared_ptr<Bitmap> RenderableTexture::SaveToBitmap() const{
    auto bmp = make_shared<Bitmap>(tex->GetWidth(), tex->GetHeight());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadPixels(0, 0, tex->GetWidth(), tex->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, bmp->GetData());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return bmp;
}

SE_END
