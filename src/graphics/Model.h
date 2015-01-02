#pragma once

#include "Macro.h"
#include "Vector.h"
#include "Bitmap.h"

SE_BEGIN

class Program;
class Texture;
class Stream;

class Material
{
public:
	Vector4 ambient;				// Ka
	Vector4 diffuse;				// Kd
	Vector4 specular;				// Ks
	float shininess = 0;            // Ns
	float opticalDensity = 0;       // Ni
	float opaque = 0;               // d
	Vector4 transmissionFilter;		// Tf

	int illumination;				// illum

	Texture* texture = NULL;

	Material(const Vector4& ambient, const Vector4& diffuse, const Vector4& specular, float shininess){
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
	}

	Material() {
        ambient = Vector4(0.5882, 0.5882, 0.5882, 0);
        diffuse = Vector4(0.5882, 0.5882, 0.5882, 0);
	}

	~Material();
};

class LightSource
{
public:
	Vector4 position;
	Vector4 direction;
	Vector4 diffuse;
	Vector4 specular;
	Vector4 attenuation;
	Vector4 spotParams;

	bool operator==(const LightSource& ls){
		return position == ls.position
			&& direction == ls.direction
			&& diffuse == ls.diffuse
			&& specular == ls.specular
			&& attenuation == ls.attenuation
			&& spotParams == ls.spotParams;
	}

	static LightSource DirectionalLight(Vector3 direction, Vector4 diffuse, Vector4 specular){
		LightSource ls;
		ls.position = Vector4(0, 0, 0, 0);
		ls.direction = Vector4(direction.X(), direction.Y(), direction.Z());
		ls.diffuse = diffuse;
		ls.specular = specular;
		return ls;
	}

	static LightSource PointLight(Vector3 position, Vector4 diffuse, Vector4 specular,
		float constantAttenuation, float linearAttenuation, float quadraticAttenuation){
		LightSource ls;
		ls.position = Vector4(position.X(), position.Y(), position.Z(), 1);
		ls.diffuse = diffuse;
		ls.specular = specular;
		ls.attenuation = Vector4(constantAttenuation, linearAttenuation, quadraticAttenuation, 0);
		return ls;
	}

	static LightSource SpotLight(Vector3 position, Vector3 direction, Vector4 diffuse, Vector4 specular,
		float constantAttenuation, float linearAttenuation, float quadraticAttenuation,
		float spotCutoff, float spotExponent){
		LightSource ls;
		ls.position = Vector4(position.X(), position.Y(), position.Z(), 1);
		ls.direction = Vector4(direction.X(), direction.Y(), direction.Z(), 0);
		ls.diffuse = diffuse;
		ls.specular = specular;
		ls.attenuation = Vector4(constantAttenuation, linearAttenuation, quadraticAttenuation, 0);
		ls.spotParams = Vector4(spotCutoff, spotExponent, 0, 0);
		return ls;
	}
};

class Model{

	friend class Window;

private:
	bool textureEnabled = true;
	bool normalEnabled = true;

protected:
	vector<Vector3> vertices;
	vector<Vector2> coords;
	vector<Vector3> normals;

	typedef struct{
        string name;
        vector<int> indices;
		Material* material = nullptr;
	} Group;

	vector<shared_ptr<Group>> groups;

	bool valid = false;

	vector<LightSource> lights;
	Material material{ Vector4(0.2f, 0.2f, 0.2f, 1.0f), Vector4(1.0f, 0.8f, 0.8f, 1.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 5.0f };

	static Program* shader;

	static const char* const vs;
	static const char* const fs;

	Model() { }

public:
	virtual ~Model(){ }

	virtual void Draw();

    shared_ptr<Bitmap> MakeUVMap(Window* w, int size = 1024);

	static shared_ptr<Model> Load(const string& file, float scale = 1.0f);

	bool IsTextureEnabled(){
		return textureEnabled;
	}

	bool IsNormalEnabled(){
		return normalEnabled;
	}

	bool IsValid(){
		return valid;
	}

	void SetTextureEnabled(bool enabled){
		textureEnabled = enabled;
	}

	void SetNormalEnabled(bool enabled){
		normalEnabled = enabled;
	}

	void AddLight(const LightSource& light){
		lights.push_back(light);
	}

	void ClearLights(){
		lights.clear();
	}

	void RemoveLight(const LightSource& light){
		lights.erase(find(lights.begin(), lights.end(), light));
	}

	vector<LightSource>& Lights(){
		return lights;
	}

	void SetMaterial(const Material& materail){
		this->material = materail;
	}

	vector<shared_ptr<Group>>& Groups(){
		return groups;
	}

    vector<Vector3>& Vertices(){
        return vertices;
    }

    vector<Vector2>& TextureCoords(){
        return coords;
    }

    vector<Vector3>& Normals(){
        return normals;
    }
};

class ObjModel : public Model{
private:

	enum class STATE{
		COMMENT,
		TOKEN,
		SPACE,
		NONE
	};

	enum class COMMAND{
		UNKNOWN,
		V,
		VT,
		VN,
		VP,
		F,
		P,
		L,
		S,
		G,
		MTLLIB,
		USEMTL
	};

	enum class MTLCMD{
		UNKNOWN,
		NS,
		NI,
		D,
		TR,
		TF,
		KA,
		KD,
		KS,
		ILLUM,
		NEWMTL,
		MAP_KA,
        MAP_KD
	};

	unordered_map<string, Material*> materials;
	unordered_map<string, Texture*> textures;

	bool LoadMTL(Stream& stream);

public:
	~ObjModel() override;

	ObjModel(Stream& stream, float scale = 1.0f);
};

SE_END
