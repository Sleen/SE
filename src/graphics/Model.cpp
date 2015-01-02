#include "Model.h"
#include "MyTime.h"
#include "Shader.h"
#include "Texture.h"
#include "Bitmap.h"
#include "FileSystem.h"
#include "Transform.h"
#include "Graphics.h"

#include <random>

SE_BEGIN

Program* Model::shader = NULL;

const char* const Model::vs = R"(
attribute vec3 aNormal;
varying vec3 vNormal;

void main()
{
	prepare();
	vNormal = normalize((uMMatrix * vec4(aNormal, 1.0)).xyz);
}
	)";

const char* const Model::fs = R"(
const int POSITION		= 0;
const int DIRECTION		= 1;
const int DIFFUSE		= 2;
const int SPECULAR		= 3;
const int ATTENUATION	= 4;		// constant, linear, quadratic, unused
const int SPOT_PARAM	= 5;		// cutoff, exponent, unused, unused

const int MAX_NUMBER_OF_LIGHTS = 8;
uniform int numberOfLights;
uniform vec4 lights[6*MAX_NUMBER_OF_LIGHTS];

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform Material material;

//uniform sampler normalMap;

varying vec3 vNormal;

void main( void ) {
	if(prepare()){
	vec3 normalDirection = vNormal;
	vec3 viewDirection = normalize(vec3(0.0, 0.0, 0.0) - vPosition);
	vec3 lightDirection;
	float attenuation;
 
	// initialize total lighting with ambient lighting
	vec3 totalLighting = vec3(material.ambient);

	for (int i = 0; i < numberOfLights*6; i+=6)
    {
		if (lights[i+POSITION].w == 0.0) // directional light
		{
			attenuation = 1.0; // no attenuation
			lightDirection = normalize(vec3(lights[i+DIRECTION]));//normalize(vec3(lights[i+POSITION]));
		} 
		else // point light or spotlight (or other kind of light) 
		{
			vec3 positionToLightSource = vec3(lights[i+POSITION]) - vPosition;
			float distance = length(positionToLightSource);
			lightDirection = normalize(positionToLightSource);
			attenuation = 1.0 / (lights[i+ATTENUATION][0]				// constant
			       + lights[i+ATTENUATION][1] * distance				// linear
			       + lights[i+ATTENUATION][2] * distance * distance);	// quadratic
 
			if (lights[i+SPOT_PARAM][0] <= 90.0) // spotlight?
			{
				float clampedCosine = max(0.0, dot(-lightDirection, normalize(vec3(lights[i+DIRECTION]))));
				if (clampedCosine < cos(radians(lights[i+SPOT_PARAM][0]))) // outside of spotlight cone?
				{
					attenuation = 0.0;
				}
				else
				{
					attenuation = attenuation * pow(clampedCosine, lights[i+SPOT_PARAM][1]);   
				}
			}
		}
 
		vec3 diffuseReflection = attenuation 
			* vec3(lights[i+DIFFUSE]) * vec3(material.diffuse)
			* max(0.0, dot(normalDirection, lightDirection));
		
		vec3 specularReflection;
		//specularReflection = attenuation * vec3(lights[i+SPECULAR]) * vec3(material.specular)
		//	* max(0.0, pow(dot(normalDirection, normalize(lightDirection+viewDirection)), material.shininess));
		if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
		{
			//totalLighting = vec3(0.0);
			specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
		}
		else // light source on the right side
		{
			specularReflection = attenuation * vec3(lights[i+SPECULAR]) * vec3(material.specular) 
				* pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
		}
 
		totalLighting = totalLighting + diffuseReflection + specularReflection;
	}

	gl_FragColor *= vec4(totalLighting, 1.0);//vec4(totalLighting, 1.0);//texture2D(tex, vTexCoord) * vec4(totalLighting, 1.0);
	//gl_FragColor = vec4(1, 0, 0, 1);
	//gl_FragColor = vColor;
	//gl_FragColor = texture2D(tex, vTexCoord)*uAmbient;
	//vec4 t = vec4(vPosition, 1.0);//m * vec4(vPosition, 1.0);
	//if(mod(t.x+100.0, 0.4)>0.3)
	//	gl_FragColor += vec4(0.4, 0.4, 0.4, 1.0);
	//gl_FragColor = vec4(vec3(rand(vPosition)), 1.0);//vec4(1.0 - vColor.rgb, 1.0);
	
	}
}
	)";

shared_ptr<Model> Model::Load(const string& file, float scale){
	string ex = file.substr(file.find_last_of('.'));//.Substring(file.Length() - 4).ToLower();
	transform(ex.begin(), ex.end(), ex.begin(), (int(*)(int))tolower);
	if (ex == ".obj")
		return shared_ptr<Model>(new ObjModel(*FileSystem::GetInstance().Open(file), scale));
	else
		return shared_ptr<Model>(NULL);
}

void Model::Draw(){
    if (shader == NULL)
        shader = new Program(vs, fs);

    if (!valid || vertices.empty())
        return;

    shader->Use();
    glUniformMatrix4fv(shader->uMMatrix, 1, false, ModelViewMatrix());
    glUniformMatrix4fv(shader->uPMatrix, 1, false, ProjectionMatrix());

    LightSource ls;
    static float t = 0;
    t += 0.016f;
    ls.position = Vector4(cosf(t), 0, sinf(t), 0);
    ls.direction = Vector4(cosf(t), 0.0, sinf(t), 0.0);
    ls.diffuse = Vector4(1.0, 1.0, 1.0, 1.0);
    ls.specular = Vector4(1.0, 1.0, 1.0, 1.0);
    ls.attenuation = Vector4(0.0, 1, 0.0, 0.0);
    ls.spotParams = Vector4(40, 0.0, 0.0, 0.0);

    Material m;
    m.ambient = Vector4(0.2, 0.2, 0.2, 1.0);
    m.diffuse = Vector4(1.0, 0.8, 0.8, 1.0);
    m.specular = Vector4(1.0, 1.0, 1.0, 1.0);
    m.shininess = 5.0;
    glUniform1i(shader->GetUniLoc("numberOfLights"), lights.size());
    if (!lights.empty())
        glUniform4fv(shader->GetUniLoc("lights"), 6 * lights.size(), (float*)lights.data());
    glEnableVertexAttribArray(POSITION_ATTRIB_LOC);
    if (textureEnabled && !coords.empty()){
        glEnableVertexAttribArray(TEXCOORD_ATTRIB_LOC);
    }
    if (normalEnabled && !normals.empty()){
        glEnableVertexAttribArray(NORMAL_ATTRIB_LOC);
    }
    for (unsigned int i = 0; i < groups.size(); ++i){
        Material* mtl = groups[i]->material;

        //cout << i << ": " << start << " - " << length  << " | " << vertices.size() << endl;

        glUniform4fv(shader->GetUniLoc("material.ambient"), 1, mtl->ambient);
        glUniform4fv(shader->GetUniLoc("material.diffuse"), 1, mtl->diffuse);
        glUniform4fv(shader->GetUniLoc("material.specular"), 1, mtl->specular);
        glUniform1f(shader->GetUniLoc("material.shininess"), mtl->shininess);
        //glUniform4fv(shader->GetUniLoc("uAmbient"), 1, Vector4(0.2,0.2,0.2,1.0));

        if (mtl->texture){
            glUniform1i(shader->useTex2, 1);
            glActiveTexture(GL_TEXTURE1);
            mtl->texture->Bind();
            glActiveTexture(GL_TEXTURE0);
        }
        else{
            glUniform1i(shader->useTex2, 0);
        }

        auto& indices = groups[i]->indices;
        for (int i = 0; i < indices.size(); i += 2){
            int start = indices[i];
            int length = indices[i + 1];
            glVertexAttribPointer(POSITION_ATTRIB_LOC, 3, GL_FLOAT, GL_FALSE, 0, ((Vector3 *)vertices.data() + start));
            if (textureEnabled && !coords.empty())
                glVertexAttribPointer(TEXCOORD_ATTRIB_LOC, 2, GL_FLOAT, GL_FALSE, 0, ((Vector2 *)coords.data() + start));
            if (normalEnabled && !normals.empty())
                glVertexAttribPointer(NORMAL_ATTRIB_LOC, 3, GL_FLOAT, GL_FALSE, 0, ((Vector3 *)normals.data() + start));
            glDrawArrays(GL_TRIANGLES, 0, length);
        }


        /*Translate(-2 * ls.direction.X(), -2 * ls.direction.Y(), -2 * ls.direction.Z());
        Scale(0.1, 0.1, 0.1);
        glUniformMatrix4fv(shader->GetUniLoc("uMVPMatrix"), 1, false, MVPMatrix());
        glUniformMatrix4fv(shader->GetUniLoc("uMMatrix"), 1, false, ModelViewMatrix());
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());*/
    }
    glDisableVertexAttribArray(POSITION_ATTRIB_LOC);
    glDisableVertexAttribArray(TEXCOORD_ATTRIB_LOC);
    glDisableVertexAttribArray(NORMAL_ATTRIB_LOC);

    glUniform1i(shader->useTex2, 0);
}

unsigned int str_hash(char *str)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	while (*str)
	{
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

Material::~Material(){
	
}

ObjModel::~ObjModel(){
	DEBUG_LOG("Releasing model..\n");
	for (auto p : materials)
	if (p.second)
		delete p.second;
	for (auto p : textures)
	if (p.second)
		delete p.second;
}

bool ObjModel::LoadMTL(Stream& stream){
	list<unsigned int> logged;

	char c;			// current character
	char s[64];		// token
	int l = 0;		// token length
	STATE state = STATE::NONE;
	int n = 0;		// token index of this line
	bool complete = false;	// token completed?
	MTLCMD cmd = MTLCMD::UNKNOWN;		// current command
	bool newline = true;
	bool cc = true;		// request a command?
	int line = 1;		// current line number
	int faceIndex = 0;
	
	Material *m;

	while (stream.Read(&c, 1))
	{
		if (complete){
			complete = false;
			s[l] = 0;

			if (cc)		// command
			{
				if (strcmp(s, "newmtl") == 0)
					cmd = MTLCMD::NEWMTL;
				else if (strcmp(s, "Ns") == 0)
					cmd = MTLCMD::NS;
				else if (strcmp(s, "Ni") == 0)
					cmd = MTLCMD::NI;
				else if (strcmp(s, "d") == 0)
					cmd = MTLCMD::D;
				else if (strcmp(s, "Tr") == 0)
					cmd = MTLCMD::TR;
				else if (strcmp(s, "Tf") == 0)
					cmd = MTLCMD::TF;
				else if (strcmp(s, "Ka") == 0)
					cmd = MTLCMD::KA;
				else if (strcmp(s, "Kd") == 0)
					cmd = MTLCMD::KD;
				else if (strcmp(s, "Ks") == 0)
					cmd = MTLCMD::KS;
				else if (strcmp(s, "illum") == 0)
					cmd = MTLCMD::ILLUM;
				else if (strcmp(s, "map_Ka") == 0)
                    cmd = MTLCMD::MAP_KA;
                else if (strcmp(s, "map_Kd") == 0)
                    cmd = MTLCMD::MAP_KD;
				else{
					cmd = MTLCMD::UNKNOWN;
					if (find(logged.begin(), logged.end(), str_hash(s)) == logged.end()){
						logged.push_front(str_hash(s));
						DEBUG_LOG("warning(%d): unknown command \"%s\"\n", line, s);
					}
					state = STATE::COMMENT;
				}
			}
			else{
				//DEBUG_LOG("%d(%d): %s\n", line, n, s);
				if (l > 0){
					if (cmd < MTLCMD::ILLUM){
						float f;
						if (!sscanf(s, "%f", &f)){
							DEBUG_LOG("error(%d): failed to parse float number \"%s\"\n", line, s);
							return false;
						}
						switch (cmd){
						case MTLCMD::NS:
							m->shininess = f;
							break;
						case MTLCMD::NI:
							m->opticalDensity = f;
							break;
						case MTLCMD::D:
							m->opaque = f;
							break;
						case MTLCMD::TR:
							m->opaque = 1 - f;
							break;
						case MTLCMD::TF:
							m->transmissionFilter[n] = f;
							break;
						case MTLCMD::KA:
							m->ambient[n] = f;
							break;
						case MTLCMD::KD:
							m->diffuse[n] = f;
							break;
						case MTLCMD::KS:
							m->specular[n] = f;
							break;
						}
					}
					else if (cmd < MTLCMD::NEWMTL){
						int d;
						if (!sscanf(s, "%d", &d)){
							DEBUG_LOG("error(%d): failed to parse integer number \"%s\"\n", line, s);
							return false;
						}
						if (cmd == MTLCMD::ILLUM){
							m->illumination = d;
						}
					}
					else{
						if (cmd == MTLCMD::NEWMTL){
							materials[s] = m = new Material;
						}
						else if (cmd == MTLCMD::MAP_KA){
                            DEBUG_LOG("warning(%d): 'map_Ka' property is not supported yet.\n", line);
							//auto f = textures.find(s);
							//if (f != textures.end())
							//	m->texture = f->second;
       //                     else{
       //                         auto tex = new Texture(s);
       //                         //tex->SetParam(GL_TEXTURE_WRAP_S, GL_REPEAT);
       //                         //tex->SetParam(GL_TEXTURE_WRAP_T, GL_REPEAT);
       //                         textures[s] = m->texture = tex;
       //                     }
                        }
                        else if (cmd == MTLCMD::MAP_KD){
                            auto f = textures.find(s);
                            if (f != textures.end())
                                m->texture = f->second;
                            else{
                                auto tex = new Texture(s);
                                //tex->SetParam(GL_TEXTURE_WRAP_S, GL_REPEAT);
                                //tex->SetParam(GL_TEXTURE_WRAP_T, GL_REPEAT);
                                textures[s] = m->texture = tex;
                            }
                        }
					}

					n++;
				}
			}

			l = 0;
		}
		if (c == '\r')
			continue;
		else if (c == '\n'){
			if (state == STATE::TOKEN)
				complete = true;
			state = STATE::NONE;
			newline = true;
			line++;
		}
		else if (state == STATE::COMMENT)
			continue;
		else if (c == '#')
			state = STATE::COMMENT;
		else if (c == ' ' || c == '\t'){
			if (state == STATE::SPACE)
				continue;
			else{
				if (state == STATE::TOKEN)
					complete = true;
				state = STATE::SPACE;
			}
		}
		else{
			s[l++] = c;
			if (state != STATE::TOKEN){
				state = STATE::TOKEN;
				cc = newline;
				if (newline){
					newline = false;
					n = 0;
				}
			}
		}
	}

	return true;
}

ObjModel::ObjModel(Stream& stream, float scale){
	int64_t t = GetMicroSecond();

	vector<Vector3> vs;		// vertices
	vector<Vector2> ts;		// texture vertices
	vector<Vector3> ns;		// normal vectors
    
	list<unsigned int> logged;

	char c;			// current character
	char s[32];		// token
	int l = 0;		// token length
	STATE state = STATE::NONE;
	int n = 0;		// token index of this line
	int nn = 0;		// '/' index of this part
	bool complete = false;	// token completed?
	COMMAND cmd = COMMAND::UNKNOWN;		// current command
	bool newline = true;
	bool cc = true;		// request a command?
	int line = 1;		// current line number
	bool slash = false;	// separated by slash?
	int faceIndex = 0;

    Material* curMat = nullptr;
    shared_ptr<Group> curGroup = nullptr;

	while (stream.Read(&c, 1))
	{
		if (complete){
			complete = false;
			s[l] = 0;

			if (cc)		// command
			{
				if (strcmp(s, "v") == 0)
					cmd = COMMAND::V;
				else if (strcmp(s, "vt") == 0)
					cmd = COMMAND::VT;
				else if (strcmp(s, "vn") == 0)
					cmd = COMMAND::VN;
				//else if (strcmp(s, "vp") == 0)
				//	cmd = COMMAND::VP;
				else if (strcmp(s, "f") == 0){
					cmd = COMMAND::F;
					faceIndex++;
				}
				//else if (strcmp(s, "p") == 0)
				//	cmd = COMMAND::P;
				//else if (strcmp(s, "l") == 0)
				//	cmd = COMMAND::L;
				//else if (strcmp(s, "s") == 0)
				//	cmd = COMMAND::S;
				else if (strcmp(s, "g") == 0)
					cmd = COMMAND::G;
				else if (strcmp(s, "mtllib") == 0)
					cmd = COMMAND::MTLLIB;
				else if (strcmp(s, "usemtl") == 0)
					cmd = COMMAND::USEMTL;
				else{
					cmd = COMMAND::UNKNOWN;
					if (find(logged.begin(), logged.end(), str_hash(s)) == logged.end()){
						logged.push_front(str_hash(s));
						DEBUG_LOG("warning(%d): unknown command \"%s\"\n", line, s);
					}
					state = STATE::COMMENT;
				}
			}
			else{
				//cout << line <<  "("<<n <<"-" << nn<<")"<< ": " << s << endl;
				if (l > 0){
					if (cmd < COMMAND::F){
						float f;
						if (!sscanf(s, "%f", &f)){
							DEBUG_LOG("error(%d): failed to parse float number \"%s\"\n", line, s);
							valid = false;
							return;
						}
						switch (cmd){
						case COMMAND::V:
							if (n == 0)
								vs.push_back(Vector3());
							vs.back()[n] = f * scale;
							break;
						case COMMAND::VT:
							if (n == 0)
								ts.push_back(Vector2());
							else if (n == 2)
								break;
							ts.back()[n] = f;
							break;
						case COMMAND::VN:
							if (n == 0)
								ns.push_back(Vector3());
							ns.back()[n] = f;
							break;
						}
					}
					else if (cmd < COMMAND::G){
						int d;
						if (!sscanf(s, "%d", &d)){
							DEBUG_LOG("error(%d): failed to parse integer number \"%s\"\n", line, s);
							valid = false;
							return;
						}
						if (cmd == COMMAND::F){
                            if (n == 3){
                                switch (nn){
                                case 0:
                                    vertices.push_back(vertices[vertices.size() - 3]);
                                    vertices.push_back(vertices[vertices.size() - 2]);
                                    break;
                                case 1:
                                    coords.push_back(coords[coords.size() - 3]);
                                    coords.push_back(coords[coords.size() - 2]);
                                    break;
                                case 2:
                                    normals.push_back(normals[normals.size() - 3]);
                                    normals.push_back(normals[normals.size() - 2]);
                                    break;
                                default:
                                    DEBUG_LOG("warning(%d): unknown data \"%s\", after '/'\n", line, s);
                                    break;
                                }
								/*DEBUG_LOG("error(%d): quads are not supported yet\n", line);
								valid = false;
								return;*/
							}

							switch (nn){
							case 0:		// vertex
								vertices.push_back(vs[d - 1]);
								break;
							case 1:		// texture coord
								coords.push_back(ts[d - 1]);
								break;
							case 2:		// normal vector
								normals.push_back(ns[d - 1]);
								break;
							default:
								DEBUG_LOG("warning(%d): unknown data \"%s\", after '/'\n", line, s);
								break;
							}
						}
					}
					else{
						if (cmd == COMMAND::MTLLIB){
							if (!LoadMTL(*FileSystem::GetInstance().Open(s))){
								DEBUG_LOG("error(%d): failed to load material library \"%s\"\n", line, s);
								valid = false;
								return;
							}
                        }
                        else if (cmd == COMMAND::USEMTL){
                            curMat = materials.find(s)->second;
                            if (!groups.empty()){
                                curGroup->material = curMat;
                            }
                        }
						else if (cmd == COMMAND::G){
                            if (curGroup){
                                curGroup->indices.push_back(vertices.size() - curGroup->indices.back());
                            }
                            for (auto& g : groups){
                                if (g->name == s){
                                    curGroup = g;
                                    break;
                                }
                            }
                            if (curGroup == nullptr || curGroup->name != s){
                                curGroup = make_shared<Group>();
                                curGroup->name = s;
                                curGroup->material = curMat;
                                groups.push_back(curGroup);
                            }
                            curGroup->indices.push_back(vertices.size());
							//Group g;
							//g.offset = faceIndex;
       //                     g.material = curMat;//materials.find(s)->second;
							//groups.push_back(g);
						}
					}
				}

				if (!slash) n++;
			}

			l = 0;
		}
		if (c == '\r')
			continue;
		else if (c == '\n'){
			if (state == STATE::TOKEN)
				complete = true;
			state = STATE::NONE;
			newline = true;
			line++;
			if (slash) nn++;
			slash = false;
		}
		else if (state == STATE::COMMENT)
			continue;
		else if (c == '#')
			state = STATE::COMMENT;
		else if (c == ' ' || c == '\t'){
			if (state == STATE::SPACE)
				continue;
			else{
				if (state == STATE::TOKEN)
					complete = true;
				state = STATE::SPACE;
				if (slash) nn++;
				slash = false;
			}
		}
		else if (c == '/'){
			complete = true;
			if (!slash)
				nn = 0;
			else
				nn++;
			slash = true;
		}
		else{
			s[l++] = c;
			if (state != STATE::TOKEN){
				state = STATE::TOKEN;
				cc = newline;
				if (newline){
					newline = false;
					n = 0;
				}
			}
		}
	}

	vector<Vector3>(vertices).swap(vertices);
	vector<Vector2>(coords).swap(coords);
	vector<Vector3>(normals).swap(normals);

    if (groups.empty()){
        curGroup = make_shared<Group>();
        curGroup->name = "default";
        curGroup->material = curMat;
        curGroup->indices.push_back(0);
        groups.push_back(curGroup);
    }
    if (curGroup->indices.size() % 2 != 0){
        curGroup->indices.push_back(vertices.size() - curGroup->indices.back());
    }

	DEBUG_LOG("model loaded success, faces: %d, groups: %d, time consumed: %.3f ms\n", vertices.size() / 3, groups.size(), (GetMicroSecond() - t) / 1000.0f);
	valid = true;
}

shared_ptr<Bitmap> Model::MakeUVMap(Window* w, int size){
    Graphics g(w);
    g.Begin();
    auto map = make_shared<RenderableTexture>(size, size);
    map->Begin();
    mt19937 rnd(GetMicroSecond());
    g.Push();
    g.Scale(size, size);
    for (int i = 0; i < groups.size(); i++){
        auto& indices = groups[i]->indices;
        int color = rnd() | 0xff000000;
        for (int n = 0; n < indices.size(); n += 2){
            int start = indices[n];
            int end = start + indices[n+1];
            for (int j = start; j < end; j += 3){
                g.FillPoly(color, coords.data() + j, 3);
                g.DrawPoly(Color::Gray, coords.data() + j, 3);
            }
        }
    }
    g.Pop();
    map->End();
    return map->SaveToBitmap();
}

SE_END