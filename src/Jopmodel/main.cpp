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


namespace jopmodel{

	int reflectionCount = 16;
	int startPos;
	int sizeofjsonbin;

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

		Mesh(){}
		~Mesh(){};

		std::vector<unsigned char> vertexBuffer;
		std::vector<unsigned char> indexBuffer;
		uint32_t vertexComponents;


		int type; //triangles, lines etc.
		uint32_t components = 0; //bitfield
		unsigned int startIndex = 0;
		unsigned int start = 0;

		unsigned int lengthIndex = 0;
		unsigned int length = 0; //bytes

	};

	class Textures
	{
	public:
		enum Repeat
		{
			Basic,      ///< Repeat
			Mirrored,   ///< Mirrored repeat
			ClampEdge,  ///< Clamp to edge
			ClampBorder ///< Clamp to border
		};

		Textures(){}
		~Textures(){}

		std::string texturePath;// = "pathpath";
		int type = 0; //diffuse, specular etc.
		int wrapmode = 0; //repeat, clamp etc.
	};

	class Material
	{
	public:

		Material()
		{
			for (size_t i = 0; i < jopmodel::reflectionCount; ++i)
			{
				switch (i)
				{
				case 3:
				case 7:
				case 11:
				case 15:
				{
					reflections[i] = 1.0f;
					break;
				}
				default:
				{
					reflections[i] = 0.0f;
				}
				}
			}
		}
		~Material(){};

		typedef uint32_t AttribType;

		enum Map
		{
			Diffuse = 1,
			Specular,
			Emission,
			Environment,
			Reflection,
			//Normal
			//Parallax
			Opacity,
			Gloss,

			/// For internal use. Never use this
			Last
		};

		char* materialName;
		float reflections[16]; //= { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 };
		float shininess = 1.0f;
		float reflectivity = 0.0f;
		std::vector<Textures> textures;
	};

	class Model
	{
	public:
		Model(){};
		~Model(){};

		std::string modelName;
		std::vector<Material> materials;
		std::vector<Mesh> meshes;
	};
}

bool binaryWriter(jopmodel::Model& model, std::string fileOut)
{
	std::ofstream writeFile(fileOut, std::ios::out | std::ios::app | std::ios::ate);
	if (writeFile)
	{
		for (auto& j : model.meshes)
		{
			writeFile.write(reinterpret_cast<const char*>(j.vertexBuffer.data()), j.length);
			writeFile.write(reinterpret_cast<const char*>(j.indexBuffer.data()), j.lengthIndex);
		}
		writeFile.close();
		return true;
	}
	return false;
}

bool jsonWriter(jopmodel::Model& model, std::string fileOut)
{
	namespace rj = rapidjson;

	rapidjson::Document modeldoc;
	modeldoc.SetObject();

	rapidjson::Value materials(rapidjson::kArrayType);

	rapidjson::Value textures(rapidjson::kArrayType);

	modeldoc.AddMember(rj::StringRef("name"), rj::StringRef(model.modelName.c_str()), modeldoc.GetAllocator());

	auto& materialArray = modeldoc.AddMember(rj::StringRef("materials"), rj::kArrayType, modeldoc.GetAllocator())["materials"];
	for (auto& j : model.materials) //MATERIALS
	{
		materialArray.PushBack(rj::kObjectType, modeldoc.GetAllocator());

		auto& materialObject = materialArray[materialArray.Size() - 1];

		auto& reflectionsArray = materialObject.AddMember(rj::StringRef("reflection"), rj::kArrayType, modeldoc.GetAllocator())["reflection"];
		for (int k = 0; k * sizeof(float) < sizeof(j.reflections); ++k) //MATERIAL REFLECTIONS
		{
			reflectionsArray.PushBack(j.reflections[k], modeldoc.GetAllocator());
		}
		materialObject.AddMember(rj::StringRef("shininess"), j.shininess, modeldoc.GetAllocator())["shininess"];
		materialObject.AddMember(rj::StringRef("reflectivity"), j.reflectivity, modeldoc.GetAllocator())["reflectivity"];


		auto& texturesArray = materialObject.AddMember(rj::StringRef("textures"), rj::kObjectType, modeldoc.GetAllocator())["textures"];
		for (auto& l : j.textures) //TEXTURES
		{
			auto& textureObject = texturesArray.AddMember(rj::StringRef(l.texturePath.c_str()), rj::kObjectType, modeldoc.GetAllocator())[l.texturePath.c_str()];
			textureObject.AddMember(rj::StringRef("type"), l.type, modeldoc.GetAllocator())["type"];
			textureObject.AddMember(rj::StringRef("wrapmode"), l.wrapmode, modeldoc.GetAllocator())["wrapmode"];
		}
	}

	auto& meshArray = modeldoc.AddMember(rj::StringRef("meshes"), rj::kArrayType, modeldoc.GetAllocator())["meshes"];
	for (auto& i : model.meshes) //MESHES
	{
		meshArray.PushBack(rj::kObjectType, modeldoc.GetAllocator());

		auto& modelObject = meshArray[meshArray.Size() - 1];

		modelObject.AddMember(rj::StringRef("type"), i.type, modeldoc.GetAllocator())["type"];
		modelObject.AddMember(rj::StringRef("components"), i.components, modeldoc.GetAllocator())["components"];
		modelObject.AddMember(rj::StringRef("start"), i.start, modeldoc.GetAllocator())["start"];
		modelObject.AddMember(rj::StringRef("length"), i.length, modeldoc.GetAllocator())["length"];
		modelObject.AddMember(rj::StringRef("startIndex"), i.startIndex, modeldoc.GetAllocator())["startIndex"];
		modelObject.AddMember(rj::StringRef("lengthIndex"), i.lengthIndex, modeldoc.GetAllocator())["lengthIndex"];
	}

	//write json to file
	std::ofstream file(fileOut);
	if (file.is_open())
	{
		rapidjson::StringBuffer mdbuff;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> mdwriter(mdbuff);
		modeldoc.Accept(mdwriter);

		file << mdbuff.GetString();
		file.close();
	}
	return true;
}

bool binaryReader(std::string fileOut)
{
	std::ifstream readFile(fileOut, std::ios::binary | std::ios::in);
	if (readFile)
	{
		while (!EOF)
		{
			//	if (char )
		}
		readFile.seekg(jopmodel::startPos); //find the last } in the file


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

void pushReflections(jopmodel::Material &jopmat, aiColor3D col, int refTypeIndex)
{
	jopmat.reflections[refTypeIndex * 4 + 0] = col.r;
	jopmat.reflections[refTypeIndex * 4 + 1] = col.g;
	jopmat.reflections[refTypeIndex * 4 + 2] = col.b;
	jopmat.reflections[refTypeIndex * 4 + 3] = 1.0;
}

bool getMaterials(const aiScene* scene, jopmodel::Model& model)
{
	if (!scene->HasMaterials())
	{
		printf("Scene has no materials!\n");
	}
	if (!scene->HasMeshes())
	{
		printf("Scene has no meshes!\n");
	}

	aiColor3D col;

	for (size_t j = 0; j < scene->mNumMaterials; j++)
	{
		jopmodel::Material jopmaterial;

		jopmaterial.materialName = "mnhere_pcmodel_";// +i;

		aiMaterial aiMat = *scene->mMaterials[j];

		//reflections
		aiMat.Get(AI_MATKEY_COLOR_AMBIENT, col);
		pushReflections(jopmaterial, col, 0);
		aiMat.Get(AI_MATKEY_COLOR_DIFFUSE, col);
		pushReflections(jopmaterial, col, 1);
		aiMat.Get(AI_MATKEY_COLOR_SPECULAR, col);
		pushReflections(jopmaterial, col, 2);
		aiMat.Get(AI_MATKEY_COLOR_EMISSIVE, col);
		pushReflections(jopmaterial, col, 3);

		float sh;
		if (aiMat.Get(AI_MATKEY_SHININESS, sh) == aiReturn_SUCCESS)
		{
			jopmaterial.shininess = sh;
		}
		float rf;
		if (aiMat.Get(AI_MATKEY_REFLECTIVITY, rf) == aiReturn_SUCCESS)
		{
			jopmaterial.reflectivity = rf;
		}

		jopmodel::Textures joptexture;

		auto kek = [&aiMat, &joptexture](const aiTextureType aiType, const int ind, aiTextureFlags flags)
		{
			aiMat.Get(AI_MATKEY_TEXFLAGS(aiType, ind), flags);
			switch (flags)
			{
			case aiTextureFlags_UseAlpha:

				break;
			case aiTextureFlags_IgnoreAlpha:

				break;
			}

			//Wrapmode
			aiTextureMapMode mapMode;
			aiMat.Get(AI_MATKEY_MAPPING(aiType, ind), mapMode);
			switch (mapMode)
			{
			case aiTextureMapMode_Wrap:
				joptexture.wrapmode = jopmodel::Textures::Repeat::Basic;
				break;
			case aiTextureMapMode_Clamp:
				joptexture.wrapmode = jopmodel::Textures::Repeat::ClampEdge;
				break;
			case aiTextureMapMode_Decal:
				joptexture.wrapmode = jopmodel::Textures::Repeat::ClampBorder;
				break;
			case aiTextureMapMode_Mirror:
				joptexture.wrapmode = jopmodel::Textures::Repeat::Mirrored;
			}
		};


		//Types
		{
			// Diffuse
			if (aiMat.GetTextureCount(aiTextureType_DIFFUSE))
			{
				aiString path;
				aiMat.GetTexture(aiTextureType_DIFFUSE, 0, &path);

				if (path.length)
				{
					joptexture.texturePath = path.C_Str();
					joptexture.type = jopmodel::Material::Map::Diffuse;
					jopmaterial.textures.push_back(joptexture);
				}
			}

			// Specular
			if (aiMat.GetTextureCount(aiTextureType_SPECULAR))
			{
				aiString path;
				aiMat.GetTexture(aiTextureType_SPECULAR, 0, &path);

				if (path.length)
				{
					joptexture.texturePath = path.C_Str();
					joptexture.type = jopmodel::Material::Map::Specular;
					jopmaterial.textures.push_back(joptexture);
				}
			}

			// Gloss
			if (aiMat.GetTextureCount(aiTextureType_SHININESS))
			{
				aiString path;
				aiMat.GetTexture(aiTextureType_SHININESS, 0, &path);

				if (path.length)
				{
					joptexture.texturePath = path.C_Str();
					joptexture.type = jopmodel::Material::Map::Gloss;
					jopmaterial.textures.push_back(joptexture);
				}
			}

			// Emission
			if (aiMat.GetTextureCount(aiTextureType_EMISSIVE))
			{
				aiString path;
				aiMat.GetTexture(aiTextureType_EMISSIVE, 0, &path);

				if (path.length)
				{
					joptexture.texturePath = path.C_Str();
					joptexture.type = jopmodel::Material::Map::Emission;
					jopmaterial.textures.push_back(joptexture);
				}
			}

			// Reflection
			if (aiMat.GetTextureCount(aiTextureType_REFLECTION))
			{
				aiString path;
				aiMat.GetTexture(aiTextureType_REFLECTION, 0, &path);

				if (path.length)
				{
					joptexture.texturePath = path.C_Str();
					joptexture.type = jopmodel::Material::Map::Reflection;
					jopmaterial.textures.push_back(joptexture);
				}
			}

			// Opacity
			if (aiMat.GetTextureCount(aiTextureType_OPACITY))
			{
				aiString path;
				aiMat.GetTexture(aiTextureType_OPACITY, 0, &path);

				if (path.length)
				{
					joptexture.texturePath = path.C_Str();
					joptexture.type = jopmodel::Material::Map::Opacity;
					jopmaterial.textures.push_back(joptexture);
				}
			}
		}
		model.materials.push_back(jopmaterial);
	}
	return true;
}

bool getMeshes(const aiScene* scene, jopmodel::Model& model)
{
	unsigned int totalSize = 0;

	for (size_t j = 0; j < scene->mNumMeshes; ++j)
	{
		jopmodel::Mesh jopmesh;
		jopmesh.start = totalSize;
		aiMesh* mesh = scene->mMeshes[j];
		if (!mesh->mVertices)
		{
			continue;
		}

		//VERTICES
		//std::vector<unsigned char> vertBuf
		jopmesh.vertexBuffer.resize
			(
			(sizeof(aiVector3D) +
			sizeof(aiVector2D) * mesh->HasTextureCoords(0) +
			sizeof(aiVector3D) * mesh->HasNormals() +
			sizeof(aiVector3D) * mesh->HasTangentsAndBitangents() * 2 +
			sizeof(aiColor4D)     * mesh->HasVertexColors(0)
			)
			* mesh->mNumVertices
			);

		unsigned int meshSize = 0;

		for (size_t k = 0, vertIndex = 0; k < mesh->mNumVertices; ++k)
		{
			// Position
			{
				auto& pos = mesh->mVertices[k];
				reinterpret_cast<aiVector3D&>(jopmesh.vertexBuffer[vertIndex]) = aiVector3D(pos.x, pos.y, pos.z);
				vertIndex += sizeof(aiVector3D);
				meshSize += sizeof(aiVector3D);
			}

			// Tex coordinates
			if (mesh->HasTextureCoords(0))
			{
				auto& tc = mesh->mTextureCoords[0][k];
				reinterpret_cast<aiVector2D&>(jopmesh.vertexBuffer[vertIndex]) = aiVector2D(tc.x, tc.y);
				vertIndex += sizeof(aiVector2D);
				meshSize += sizeof(aiVector2D);
			}

			// Normal
			if (mesh->HasNormals())
			{
				auto& norm = mesh->mNormals[k];
				reinterpret_cast<aiVector3D&>(jopmesh.vertexBuffer[vertIndex]) = aiVector3D(norm.x, norm.y, norm.z);
				vertIndex += sizeof(aiVector3D);
				meshSize += sizeof(aiVector3D);
			}

			// Tangents
			if (mesh->HasTangentsAndBitangents())
			{
				auto& tang = mesh->mTangents[k], bitang = mesh->mBitangents[k];
				reinterpret_cast<aiVector3D&>(jopmesh.vertexBuffer[vertIndex]) = aiVector3D(tang.x, tang.y, tang.z);
				vertIndex += sizeof(aiVector3D);
				meshSize += sizeof(aiVector3D);

				reinterpret_cast<aiVector3D&>(jopmesh.vertexBuffer[vertIndex]) = aiVector3D(bitang.x, bitang.y, bitang.z);
				vertIndex += sizeof(aiVector3D);
				meshSize += sizeof(aiVector3D);
			}

			// Color
			if (mesh->HasVertexColors(0))
			{
				auto& col = mesh->mColors[0][k];
				reinterpret_cast<aiColor4D&>(jopmesh.vertexBuffer[vertIndex]) = aiColor4D(col.r, col.g, col.b, col.a);
				vertIndex += sizeof(aiColor4D);
				meshSize += sizeof(aiColor4D);
			}
		}
		jopmesh.length = meshSize;
		totalSize += meshSize;


		// Indices
		const auto elemSize = 4;
		if (mesh->HasFaces())
		{
			jopmesh.startIndex = totalSize;

			unsigned int indexSize = 0;

			jopmesh.indexBuffer.resize(elemSize * mesh->mNumFaces * 3);

			for (std::size_t j = 0, bufIndex = 0; j < mesh->mNumFaces; ++j, bufIndex += elemSize * 3)
			{
				auto& face = mesh->mFaces[j];

				switch (elemSize)
				{
				case sizeof(unsigned char) :
					jopmesh.indexBuffer[bufIndex] = static_cast<unsigned char>(face.mIndices[0]);
					jopmesh.indexBuffer[bufIndex + sizeof(unsigned char)] = static_cast<unsigned char>(face.mIndices[1]);
					jopmesh.indexBuffer[bufIndex + sizeof(unsigned char) * 2] = static_cast<unsigned char>(face.mIndices[2]);
					break;

				case sizeof(unsigned short int) :
					reinterpret_cast<unsigned short int&>(jopmesh.indexBuffer[bufIndex]) = static_cast<unsigned short int>(face.mIndices[0]);
					reinterpret_cast<unsigned short int&>(jopmesh.indexBuffer[bufIndex + sizeof(unsigned short int)]) = static_cast<unsigned short int>(face.mIndices[1]);
					reinterpret_cast<unsigned short int&>(jopmesh.indexBuffer[bufIndex + sizeof(unsigned short int) * 2]) = static_cast<unsigned short int>(face.mIndices[2]);
					break;

				case sizeof(unsigned int) :
					reinterpret_cast<unsigned int&>(jopmesh.indexBuffer[bufIndex]) = face.mIndices[0];
					reinterpret_cast<unsigned int&>(jopmesh.indexBuffer[bufIndex + sizeof(unsigned int)]) = face.mIndices[1];
					reinterpret_cast<unsigned int&>(jopmesh.indexBuffer[bufIndex + sizeof(unsigned int) * 2]) = face.mIndices[2];
				}
				indexSize += elemSize;
			}
			totalSize += indexSize;
			jopmesh.lengthIndex = indexSize;
		}

		//COMPONENTS
		jopmesh.components = jopmesh.Position
			| (mesh->HasTextureCoords(0)         * jopmesh.TexCoords)
			| (mesh->HasNormals()                * jopmesh.Normal)
			| (mesh->HasTangentsAndBitangents()  * jopmesh.Tangents)
			| (mesh->HasVertexColors(0)          * jopmesh.Color)
			;

		model.meshes.push_back(jopmesh);
	}
	return true;
}

int main(int argc, char* argv[])
{
	std::string str(argv[0]); //path to .exe
	if (argc > 1)
	{
		std::string pathIn = argv[1];

		//read old model file with assimp
		Assimp::Importer imp;

		printf("Loading model...\n");
		const aiScene *scene = imp.ReadFile(pathIn, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_OptimizeGraph | aiProcess_RemoveRedundantMaterials);
		if (!scene) {
			printf("Unable to load mesh: %s\n", imp.GetErrorString());
		}
		else
		{
			printf("Successfully loaded file!\n");
		}

		jopmodel::Model model;
		model.modelName = "placehold";
		std::string fileOut = "model.jop";

		if (!getMaterials(scene, model))
		{
			printf("Handling model failed! (inside processModel())\n");
		}

		if (!getMeshes(scene, model))
		{
			printf("Handling model failed! (inside processModel())\n");
		}
		jsonWriter(model, fileOut);
		binaryWriter(model, fileOut);
	}
}