#pragma once

#include "Macro.h"
#include "Matrix.h"
#include "Shader.h"

SE_BEGIN

#define MODELVIEW_MATRIX 	0
#define PROJECTION_MATRIX 	1
#define TEXTURE_MATRIX 		2

class Transform{
private:

	enum{ MAX_MATRIX = 32 };

	Matrix4 ms[3][MAX_MATRIX];
	int index[3];
	int mode = 0;
	int left = 0, top = 0, width = 0, height = 0;

	static shared_ptr<Transform> instance;

	int& I(){
		return index[mode];
	}
	
	Matrix4* M(){
		return ms[mode];
	}
	
	Matrix4& Now(){
		return M()[I()];
	}
	
public:
	Transform(){
		memset(index, 0, sizeof(int)*3);
		for (int i = 0; i < 3; ++i)
			ms[i][0].LoadIdentity();
	}

    static shared_ptr<Transform> Instance(){
        if (instance == nullptr) instance = make_shared<Transform>();
		return instance;
	}

    static void SetInstace(shared_ptr<Transform> t){
		instance = t;
	}

	void SetViewport(int left, int top, int width, int height){
		this->left = left;
		this->top = top;
		this->width = width;
		this->height = height;
		glViewport(left, top, width, height);
	}

	void UpdateViewport(){
		glViewport(left, top, width, height);
	}

	void LoadIdentity(){
		Now().LoadIdentity();
	}
	
	void PushMatrix(){
		if (I() == MAX_MATRIX - 1)
		{
			DEBUG_LOG("%s matrix stack over flow\n", mode==0?"model view":mode==1?"projection":mode==2?"texture":"unknown");
			return;
		}
		//cout << "push >> "<<(void*)this << ", " << mode << ", " << index[mode] << width<< " - "<<height << endl;
		M()[I()+1] = Now();
		I()++;
	}
	
	void PopMatrix(){
		if (I() == MAX_MATRIX - 1)
		{
			DEBUG_LOG("%s matrix stack empty\n", mode == 0 ? "model view" : mode == 1 ? "projection" : mode == 2 ? "texture" : "unknown");
			return;
		}
		//cout << "pop >> " << (void*)this << endl;
		I()--;
	}
	
	void LoadMatrix(const Matrix4& m){
		Now() = m;
	}
	
	void MultiplyMatrix(const Matrix4& m){
        Now() = m * Now();
	}
	
	int GetMode() const{
		return mode;
	}

	void MatrixMode(int mode){
		this->mode = mode;
	}
	
	const Matrix4& GetMatrix(int mode) const{
		return ms[mode][index[mode]];
	}

	const Matrix4& GetMatrix() const{
		return ms[mode][index[mode]];
	}
};

inline const Matrix4& ModelViewMatrix(){
	return Transform::Instance()->GetMatrix(MODELVIEW_MATRIX);
}

inline const Matrix4& ProjectionMatrix(){
	return Transform::Instance()->GetMatrix(PROJECTION_MATRIX);
}

inline Matrix4 MVPMatrix(){
	return ModelViewMatrix() * ProjectionMatrix();
}

inline const Matrix4& TextureMatrix(){
	return Transform::Instance()->GetMatrix(TEXTURE_MATRIX);
}

inline void ApplyMatrix(){
	if (Program::CurrentShader() == NULL)
		return;

	switch (Transform::Instance()->GetMode()){
	case MODELVIEW_MATRIX:
		glUniformMatrix4fv(Program::CurrentShader()->uMMatrix, 1, false, ModelViewMatrix());
		break;
	case PROJECTION_MATRIX:
		glUniformMatrix4fv(Program::CurrentShader()->uPMatrix, 1, false, ProjectionMatrix());
		break;
	case TEXTURE_MATRIX:
		glUniformMatrix4fv(Program::CurrentShader()->uTMatrix, 1, false, TextureMatrix());
		break;
	}
}

inline void LoadIdentity(){
	Transform::Instance()->LoadIdentity();
	ApplyMatrix();
}

inline void Ortho(float left, float right, float bottom, float top, float _near, float _far){
	Transform::Instance()->MultiplyMatrix(OrthoMatrix(left, right, bottom, top, _near, _far));
	ApplyMatrix();
}

inline void Frustum(float left, float right, float bottom, float top, float _near, float _far){
	Transform::Instance()->MultiplyMatrix(FrustumMatrix(left, right, bottom, top, _near, _far));
	ApplyMatrix();
}

inline void Translate(float x, float y, float z){
	Transform::Instance()->MultiplyMatrix(TranslateMatrix(x, y, z));
	ApplyMatrix();
}

inline void Rotate(float a, float x, float y, float z){
	Transform::Instance()->MultiplyMatrix(RotateMatrix(a, x, y, z));
	ApplyMatrix();
}

inline void RotateAt(float a, float x, float y, float z, float cx, float cy, float cz){
	Transform::Instance()->MultiplyMatrix(TranslateMatrix(cx, cy, cz));
	Transform::Instance()->MultiplyMatrix(RotateMatrix(a, x, y, z));
	Transform::Instance()->MultiplyMatrix(TranslateMatrix(-cx, -cy, -cz));
	ApplyMatrix();
}

inline void Scale(float x, float y, float z){
	Transform::Instance()->MultiplyMatrix(ScaleMatrix(x, y, z));
	ApplyMatrix();
}

inline void ScaleAt(float x, float y, float z, float cx, float cy, float cz){
	Transform::Instance()->MultiplyMatrix(TranslateMatrix(cx, cy, cz));
	Transform::Instance()->MultiplyMatrix(ScaleMatrix(x, y, z));
	Transform::Instance()->MultiplyMatrix(TranslateMatrix(-cx, -cy, -cz));
	ApplyMatrix();
}

inline void PushMatrix(){
	Transform::Instance()->PushMatrix();
}

inline void PopMatrix(){
	Transform::Instance()->PopMatrix();
	ApplyMatrix();
}

inline void MatrixMode(int mode){
	Transform::Instance()->MatrixMode(mode);
}

inline void LoadMatrix(const Matrix4& m){
	Transform::Instance()->LoadMatrix(m);
	ApplyMatrix();
}

inline shared_ptr<Transform> SaveState(){
	return Transform::Instance();
}

inline void LoadState(shared_ptr<Transform> state){
	Transform::SetInstace(state);
	Transform::Instance()->UpdateViewport();
	if (Program::CurrentShader() != NULL){
		glUniformMatrix4fv(Program::CurrentShader()->uMMatrix, 1, false, ModelViewMatrix());
		glUniformMatrix4fv(Program::CurrentShader()->uPMatrix, 1, false, ProjectionMatrix());
		glUniformMatrix4fv(Program::CurrentShader()->uTMatrix, 1, false, TextureMatrix());
	}
}

inline void Viewport(int left, int top, int width, int height){
	Transform::Instance()->SetViewport(left, top, width, height);
}

SE_END
