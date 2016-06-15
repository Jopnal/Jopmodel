// Jopnal Engine C++ Library
// Copyright(c) 2016 Team Jopnal
// 
// This software is provided 'as-is', without any express or implied
// warranty.In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
// 
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgement in the product documentation would be
// appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.




#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/pointer.h>

#include <iostream>
#include <conio.h>
#include <fstream>
#include <vector>


//std::vector<uint8_t> vertexBuffer();
namespace jopmodel{

	class Mesh
	{
	public:

		enum VertexComponent : uint32_t
		{
			Position = 1,
			TexCoords = 1 << 1,
			Normal = 1 << 2,
			Tangents = 1 << 3,
			Color = 1 << 4
		};

		Mesh(char* meshName, int type, int start, int length)
		{
			this->meshName = meshName;
			this->type = type;
			this->startLocation = start;
			this->length = length;
		}
		Mesh(aiString meshName2, int type)
		{
			this->meshName2 = meshName2;
			this->type = type;
		}
		~Mesh();


	private:
		char* meshName;
		aiString meshName2;
		int type; //triangles, lines etc.

		//bitfield here

		int startLocation = 0;
		int length = 0; //bytes

	};

	class Material
	{
	public:
		Material(float position[3], float reflectionAmbient[4], float reflectionDiffuse[4], float reflectionSpecular[4], float reflectionEmissive[4], float shininess, float reflectivity)
		{
			this->position[3] = position[3];
			this->reflectionAmbient[4] = reflectionAmbient[4];
			this->reflectionDiffuse[4] = reflectionDiffuse[4];
			this->reflectionSpecular[4] = reflectionSpecular[4];
			this->reflectionEmissive[4] = reflectionEmissive[4];
			this->shininess = shininess;
			this->reflectivity = reflectivity;
		}
		~Material();

	private:
		float position[3];
		float reflectionAmbient[4];
		float reflectionDiffuse[4];
		float reflectionSpecular[4];
		float reflectionEmissive[4];
		float shininess = 0;
		float reflectivity = 0;
	};

	class Model
	{
	public:
		Model();
		Model(int wat[7]);
		Model(std::vector<Mesh*> meshes)
		{
			this->meshes = meshes;
		}
		Model(std::vector<Material*> materials)
		{
			this->materials = materials;
		}
		Model(std::vector<Mesh*> meshes, std::vector<Material*> materials)
		{
			this->meshes = meshes;
			this->materials = materials;
		}
		~Model();

	private:
		std::vector<Mesh*> meshes;
		std::vector<Material*> materials;
	};

	size_t startPos = 0;
	size_t sizeofjson = 0;
	size_t sizeofjsonbin = 0;
	size_t sizeofmeshes = 0;

	const char* mainData = "{ \"name\":\"\", \"materials\":{ \"materialName\":{ \"reflections\":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], \"shininess\":\"\", \"reflectivity\":\"\" }}}";

	char* modelName;
	int dataStartPosition;
	std::vector<int> mainDataVector;
	std::vector<unsigned char*> dataPacket;
	std::vector<size_t> dataPacketSize;
	std::vector<float> reflections;
	std::vector<Mesh*> meshes;
	std::vector<aiTexture*> textures;
	float shininess = 0.0f;
	float reflectivity = 0.0f;
}




bool jsonWriter(const char* data, rapidjson::Document &doc, const char* fileName)
{
	rapidjson::StringBuffer buff;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> vraitteri(buff);
	doc.Accept(vraitteri);

	data = buff.GetString();


	//write json to file
	std::ofstream file(fileName);

	int siz = buff.GetSize();
	int commas = 0;

	if (!file.is_open())
	{
		printf("Writing json failed!");
		return false;
	}

	for (size_t i = 0; i < siz; i++)
	{

		file << data[i];

	}
	file.close();
	return true;
}

bool jsonWriter(const char* fileName)
{


	rapidjson::Document doc;
	doc.Parse(jopmodel::mainData);

	//nimi
	rapidjson::Value& v = doc["name"];
	v = rapidjson::StringRef(jopmodel::modelName);


	//materialName here


	rapidjson::Value& ref = doc["materials"]["materialName"]["reflections"];
	for (size_t i = 0; i < 16; i++)
	{
		ref[i] = jopmodel::reflections[i];
	}


	rapidjson::StringBuffer buff;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> vraitteri(buff);
	doc.Accept(vraitteri);

	jopmodel::mainData = buff.GetString();
	jopmodel::sizeofjson = sizeof(jopmodel::mainData);
	jopmodel::mainDataVector.push_back((int)jopmodel::mainData);

	//write json to file
	std::ofstream file(fileName);
	if (file.is_open())
	{
		file << jopmodel::mainData;
		file.close();
		return true;
	}
	printf("Writing json failed!");
	return false;
}

bool binaryWriter(const char* fileName)
{
	std::ofstream writeFile(fileName, std::ios::out | std::ios::app | std::ios::ate);
	if (writeFile)
	{
		jopmodel::startPos = writeFile.tellp(); //size of the json data == where to start writing binary data

		for (size_t i = 0; i < jopmodel::mainDataVector.size(); i++)
		{
			writeFile.write((char*)&jopmodel::mainDataVector[i], sizeof(int)); //write all the data to file, needs changing to know the byte sizes of the data
		}
		jopmodel::sizeofjsonbin = (int)writeFile.tellp() - jopmodel::startPos;
		writeFile.close();
		return true;
	}
	return false;
}

bool binaryReader(const char* fileName)
{
	std::ifstream readFile(fileName, std::ios::binary | std::ios::in);
	if (readFile)
	{
		readFile.seekg(jopmodel::startPos); //lukuosoitin binäärien alkuun
		std::cout << "startpos: " << jopmodel::startPos << ", size of jsonbin: " << jopmodel::sizeofjsonbin << std::endl;
		int n;
		int count = 0;
		for (size_t i = 0; i < jopmodel::sizeofjsonbin; i++)
		{
			readFile.read((char*)&n, sizeof(int));	//read
			std::cout << n;							//print
			count++;
			if (count % 25 == 0)
			{
				std::cout << std::endl;
			}
		}
		readFile.close();
	}
	return true;
}

void pushReflections(aiColor3D col)
{
	jopmodel::reflections.push_back(col.r);
	jopmodel::reflections.push_back(col.g);
	jopmodel::reflections.push_back(col.b);
	jopmodel::reflections.push_back(1.0);
}

bool processModel(const aiScene* scene)
{
	if (!scene->HasMaterials())
	{
		printf("Scene has no materials!\n");
	}
	if (!scene->HasMeshes())
	{
		printf("Scene has no meshes!\n");
	}

	aiMesh* mesh = scene->mMeshes[0];
	mesh->mName;
	mesh->mMaterialIndex;


	aiColor3D col;
	aiMaterial* mat = scene->mMaterials[0];

	mat->Get(AI_MATKEY_SHININESS, jopmodel::shininess);
	mat->Get(AI_MATKEY_REFLECTIVITY, jopmodel::reflectivity);

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		mat = scene->mMaterials[i];

		mat->Get(AI_MATKEY_COLOR_AMBIENT, col);
		pushReflections(col);
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, col);
		pushReflections(col);
		mat->Get(AI_MATKEY_COLOR_SPECULAR, col);
		pushReflections(col);
		mat->Get(AI_MATKEY_COLOR_EMISSIVE, col);
		pushReflections(col);

		//iterate from diffuse to unknown
		//if there is a texture of that type do something with it
		for (int i = aiTextureType::aiTextureType_DIFFUSE; i < aiTextureType::aiTextureType_UNKNOWN; i++)
		{
			if (mat->GetTextureCount((aiTextureType)i))
			{
				aiTexture tex;
				aiString path;

				mat->GetTexture((aiTextureType)i, 0, &path);

				if (path.length)
				{
					printf("found texture\n");
				}
			}
		}
	}




	int type = 0;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		const uint32_t comps = jopmodel::Mesh::Position

			| (mesh->HasTextureCoords(0)         * jopmodel::Mesh::TexCoords)
			| (mesh->HasNormals()                * jopmodel::Mesh::Normal)
			| (mesh->HasTangentsAndBitangents()  * jopmodel::Mesh::Tangents)
			| (mesh->HasVertexColors(0)          * jopmodel::Mesh::Color)
			;



		//jopmodel::meshes.push_back(std::make_pair(&ResourceManager::getNamedResource<Mesh>("jop_mesh_" + getHex(), vertBuf.data(), vertBuf.size(), comps, indBuf.data(), elemSize, mesh->mNumFaces * 3), mesh->mMaterialIndex));



		jopmodel::meshes.push_back(new jopmodel::Mesh(scene->mMeshes[i]->mName, type));
	}
	return true;
}

int main(int argc, char* argv[])
{
	std::string str(argv[0]); //polku .exeen


	std::vector<jopmodel::Mesh*> globmeshes;
	std::vector<jopmodel::Material*> globmaterials;

	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************



	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************


	//const char* jiisoni2 = "{   \"hello\":\"world\",   \"t\":true,   \"f\":false,   \"n\":null,   \"i\":123,   \"pi\":3.1416,   \"a\":[1, 2, 3, 4]   }";


	//muuttujat
	//char* pt1 = "DOOMGUY";

	char* fileName = "data.jop";

	//int wat[] = { 3, 5, 7 };
	//float reflections[] = { 1.0, 0.5, 0.2, 1.0, 0.0, 0.0, 0.0, 1.0 };

	////initialize
	//const char* data = "{ \"name\":\"\", \"Model\":{\"position\":[1,2,3], \"reflections\":[1,2,3,4,5,6,7,8] }}";

	//rapidjson::Document doc;
	//doc.Parse(data);

	//nimi
	//rapidjson::Value& v = doc["name"];
	//v = rapidjson::StringRef(pt1);
	//v.SetString(rapidjson::StringRef(pt1));


	////position
	//rapidjson::Value& xyz = doc["Model"]["position"];
	//for (size_t i = 0; i < 3; i++)
	//{
	//	xyz[i] = wat[i];
	//}

	//rapidjson::Value& ref = doc["Model"]["reflections"];
	//for (size_t i = 0; i < 8; i++)
	//{
	//	ref[i] = reflections[i];
	//}


	//read old model file with assimp
	Assimp::Importer imp;
	char* modelFile = ""; //path to model file

	printf("Loading model...\n");
	const aiScene *scene = imp.ReadFile(modelFile, NULL);// aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
	if (!scene) {
		printf("Unable to load mesh: %s\n", imp.GetErrorString());
	}
	else
	{
		printf("Successfully loaded file!\n");
	}

	////get modelname
	//int count = 0;
	//int countStartPos = 0;
	//int countEndPos = 0;
	//unsigned int nameSize = 255;
	//for (size_t i = 0; i < nameSize; i++)
	//{
	//	if (modelFile[i] == (char)47)
	//	{
	//		count++;
	//		countStartPos = i;
	//		if (count == 2)
	//		{
	//			countEndPos = i;
	//			i += nameSize;
	//		}
	//		break;
	//	}
	//	i++;
	//}
	//for (size_t i = countStartPos; i < countEndPos; i++)
	//{
	//	jopmodel::modelName += modelFile[i];
	//}
	////end get modelname

	jopmodel::modelName = "wqertyuiop"; //temp while getting that to work ^

	if (!processModel(scene))
	{
		printf("Handling model failed! (inside processModel())\n");
	}

	//write data to file
	jsonWriter(fileName);
	binaryWriter(fileName);
	binaryReader(fileName);

	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************












	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************


	argv[argc] = "wat";
	argc++;

	std::cout << "\nAmount: argc = " << argc << "\n" << std::endl;
	for (size_t i = 0; i < argc; i++)
	{
		std::cout << "argv at " << i << " == " << argv[i] << "\n" << std::endl;
	}

	_getch();
	return 1;

}