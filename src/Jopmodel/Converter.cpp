#include "Converter.hpp"

namespace jop
{
	Converter::Converter()
	{
	}


	Converter::~Converter()
	{
	}

	bool Converter::binaryWriter(jop::Model& model, std::string fileOut)
	{
		std::ofstream writeFile(fileOut, std::ios::out | std::ios::app | std::ios::ate);
		if (writeFile)
		{
			for (auto& j : model.m_meshes)
			{
				writeFile.write(reinterpret_cast<const char*>(j.m_vertexBuffer.data()), j.m_length);
				writeFile.write(reinterpret_cast<const char*>(j.m_indexBuffer.data()), j.m_lengthIndex);
			}
			writeFile.close();
			return true;
		}
		printf("Writing binary failed\n");
		return false;
	}

	bool Converter::jsonWriter(jop::Model& model, std::string fileOut)
	{
		namespace rj = rapidjson;

		rapidjson::Document modeldoc;
		modeldoc.SetObject();

		auto& materialArray = modeldoc.AddMember(rj::StringRef("materials"), rj::kArrayType, modeldoc.GetAllocator())["materials"];
		for (auto& j : model.m_materials) //MATERIALS
		{
			materialArray.PushBack(rj::kObjectType, modeldoc.GetAllocator());

			auto& materialObject = materialArray[materialArray.Size() - 1];

			auto& reflectionsArray = materialObject.AddMember(rj::StringRef("reflection"), rj::kArrayType, modeldoc.GetAllocator())["reflection"];
			for (int k = 0; k * sizeof(float) < sizeof(j.m_reflections); ++k) //MATERIAL REFLECTIONS
			{
				reflectionsArray.PushBack(j.m_reflections[k], modeldoc.GetAllocator());
			}
			materialObject.AddMember(rj::StringRef("shininess"), j.m_shininess, modeldoc.GetAllocator())["shininess"];
			materialObject.AddMember(rj::StringRef("reflectivity"), j.m_reflectivity, modeldoc.GetAllocator())["reflectivity"];


			auto& texturesArray = materialObject.AddMember(rj::StringRef("textures"), rj::kObjectType, modeldoc.GetAllocator())["textures"];
			for (auto& l : j.m_textures) //TEXTURES
			{
				auto& textureObject = texturesArray.AddMember(rj::StringRef(l.m_texturePath.c_str()), rj::kObjectType, modeldoc.GetAllocator())[l.m_texturePath.c_str()];
				textureObject.AddMember(rj::StringRef("type"), l.m_type, modeldoc.GetAllocator())["type"];
				textureObject.AddMember(rj::StringRef("wrapmode"), l.m_wrapmode, modeldoc.GetAllocator())["wrapmode"];
			}
		}

		auto& meshArray = modeldoc.AddMember(rj::StringRef("meshes"), rj::kArrayType, modeldoc.GetAllocator())["meshes"];
		for (auto& i : model.m_meshes) //MESHES
		{
			meshArray.PushBack(rj::kObjectType, modeldoc.GetAllocator());

			auto& modelObject = meshArray[meshArray.Size() - 1];

			modelObject.AddMember(rj::StringRef("type"), i.m_type, modeldoc.GetAllocator())["type"];
			modelObject.AddMember(rj::StringRef("components"), i.m_components, modeldoc.GetAllocator())["components"];
			modelObject.AddMember(rj::StringRef("start"), i.m_start, modeldoc.GetAllocator())["start"];
			modelObject.AddMember(rj::StringRef("length"), i.m_length, modeldoc.GetAllocator())["length"];
			modelObject.AddMember(rj::StringRef("startIndex"), i.m_startIndex, modeldoc.GetAllocator())["startIndex"];
			modelObject.AddMember(rj::StringRef("lengthIndex"), i.m_lengthIndex, modeldoc.GetAllocator())["lengthIndex"];
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
			return true;
		}
		printf("Writing json failed\n");
		return false;
	}

	/*
	bool Converter::binaryReader(std::string fileOut)
	{
	std::ifstream readFile(fileOut, std::ios::binary | std::ios::in);
	if (readFile)
	{
	while (!EOF)
	{
	//	if (char )
	}
	readFile.seekg(jop::m_startPos); //find the last } in the file


	std::cout << "startpos: " << jop::m_startPos << ", size of jsonbin: " << jopmodel::m_sizeofjsonbin << std::endl;
	int n;
	int count = 0;
	for (size_t i = 0; i < jop::sizeofjsonbin; i++)
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
	*/

	void Converter::pushReflections(jop::Material &jopmat, aiColor3D col, int refTypeIndex)
	{
		jopmat.m_reflections[refTypeIndex * 4 + 0] = col.r;
		jopmat.m_reflections[refTypeIndex * 4 + 1] = col.g;
		jopmat.m_reflections[refTypeIndex * 4 + 2] = col.b;
		jopmat.m_reflections[refTypeIndex * 4 + 3] = 1.0;
	}

	void Converter::getMaterials(const aiScene* scene, jop::Model& model)
	{
		aiColor3D col;

		printf("Materials: %d\nMeshes: %d\n", scene->mNumMaterials, scene->mNumMeshes);
		for (size_t j = 0; j < scene->mNumMaterials; j++)
		{
			jop::Material jopmaterial;

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
				jopmaterial.m_shininess = sh;
			}
			float rf;
			if (aiMat.Get(AI_MATKEY_REFLECTIVITY, rf) == aiReturn_SUCCESS)
			{
				jopmaterial.m_reflectivity = rf;
			}

			jop::Texture joptexture;

			auto getFlags = [&aiMat, &joptexture](const aiTextureType aiType, const int ind, aiTextureFlags flags)
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
					joptexture.m_wrapmode = jop::Texture::Repeat::Basic;
					break;
				case aiTextureMapMode_Clamp:
					joptexture.m_wrapmode = jop::Texture::Repeat::ClampEdge;
					break;
				case aiTextureMapMode_Decal:
					joptexture.m_wrapmode = jop::Texture::Repeat::ClampBorder;
					break;
				case aiTextureMapMode_Mirror:
					joptexture.m_wrapmode = jop::Texture::Repeat::Mirrored;
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
						joptexture.m_texturePath = path.C_Str();
						joptexture.m_type = jop::Material::Map::Diffuse;
						jopmaterial.m_textures.push_back(joptexture);
					}
				}

				// Specular
				if (aiMat.GetTextureCount(aiTextureType_SPECULAR))
				{
					aiString path;
					aiMat.GetTexture(aiTextureType_SPECULAR, 0, &path);

					if (path.length)
					{
						joptexture.m_texturePath = path.C_Str();
						joptexture.m_type = jop::Material::Map::Specular;
						jopmaterial.m_textures.push_back(joptexture);
					}
				}

				// Gloss
				if (aiMat.GetTextureCount(aiTextureType_SHININESS))
				{
					aiString path;
					aiMat.GetTexture(aiTextureType_SHININESS, 0, &path);

					if (path.length)
					{
						joptexture.m_texturePath = path.C_Str();
						joptexture.m_type = jop::Material::Map::Gloss;
						jopmaterial.m_textures.push_back(joptexture);
					}
				}

				// Emission
				if (aiMat.GetTextureCount(aiTextureType_EMISSIVE))
				{
					aiString path;
					aiMat.GetTexture(aiTextureType_EMISSIVE, 0, &path);

					if (path.length)
					{
						joptexture.m_texturePath = path.C_Str();
						joptexture.m_type = jop::Material::Map::Emission;
						jopmaterial.m_textures.push_back(joptexture);
					}
				}

				// Reflection
				if (aiMat.GetTextureCount(aiTextureType_REFLECTION))
				{
					aiString path;
					aiMat.GetTexture(aiTextureType_REFLECTION, 0, &path);

					if (path.length)
					{
						joptexture.m_texturePath = path.C_Str();
						joptexture.m_type = jop::Material::Map::Reflection;
						jopmaterial.m_textures.push_back(joptexture);
					}
				}

				// Opacity
				if (aiMat.GetTextureCount(aiTextureType_OPACITY))
				{
					aiString path;
					aiMat.GetTexture(aiTextureType_OPACITY, 0, &path);

					if (path.length)
					{
						joptexture.m_texturePath = path.C_Str();
						joptexture.m_type = jop::Material::Map::Opacity;
						jopmaterial.m_textures.push_back(joptexture);
					}
				}
			}
			model.m_materials.push_back(jopmaterial);
		}
	}

	void Converter::getMeshes(const aiScene* scene, jop::Model& model)
	{
		unsigned int totalSize = 0;

		for (size_t j = 0; j < scene->mNumMeshes; ++j)
		{
			jop::Mesh jopmesh;
			jopmesh.m_start = totalSize;
			aiMesh* mesh = scene->mMeshes[j];
			if (!mesh->mVertices)
			{
				continue;
			}

			//VERTICES
			jopmesh.m_vertexBuffer.resize
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
					reinterpret_cast<aiVector3D&>(jopmesh.m_vertexBuffer[vertIndex]) = aiVector3D(pos.x, pos.y, pos.z);
					vertIndex += sizeof(aiVector3D);
					meshSize += sizeof(aiVector3D);
				}

				// Tex coordinates
				if (mesh->HasTextureCoords(0))
				{
					auto& tc = mesh->mTextureCoords[0][k];
					reinterpret_cast<aiVector2D&>(jopmesh.m_vertexBuffer[vertIndex]) = aiVector2D(tc.x, tc.y);
					vertIndex += sizeof(aiVector2D);
					meshSize += sizeof(aiVector2D);
				}

				// Normal
				if (mesh->HasNormals())
				{
					auto& norm = mesh->mNormals[k];
					reinterpret_cast<aiVector3D&>(jopmesh.m_vertexBuffer[vertIndex]) = aiVector3D(norm.x, norm.y, norm.z);
					vertIndex += sizeof(aiVector3D);
					meshSize += sizeof(aiVector3D);
				}

				// Tangents
				if (mesh->HasTangentsAndBitangents())
				{
					auto& tang = mesh->mTangents[k], bitang = mesh->mBitangents[k];
					reinterpret_cast<aiVector3D&>(jopmesh.m_vertexBuffer[vertIndex]) = aiVector3D(tang.x, tang.y, tang.z);
					vertIndex += sizeof(aiVector3D);
					meshSize += sizeof(aiVector3D);

					reinterpret_cast<aiVector3D&>(jopmesh.m_vertexBuffer[vertIndex]) = aiVector3D(bitang.x, bitang.y, bitang.z);
					vertIndex += sizeof(aiVector3D);
					meshSize += sizeof(aiVector3D);
				}

				// Color
				if (mesh->HasVertexColors(0))
				{
					auto& col = mesh->mColors[0][k];
					reinterpret_cast<aiColor4D&>(jopmesh.m_vertexBuffer[vertIndex]) = aiColor4D(col.r, col.g, col.b, col.a);
					vertIndex += sizeof(aiColor4D);
					meshSize += sizeof(aiColor4D);
				}
			}
			jopmesh.m_length = meshSize;
			totalSize += meshSize;


			// Indices
			const auto elemSize = 4;
			if (mesh->HasFaces())
			{
				unsigned int indexSize = 0;
				jopmesh.m_startIndex = totalSize;
				jopmesh.m_indexBuffer.resize(elemSize * mesh->mNumFaces * 3);

				for (std::size_t j = 0, bufIndex = 0; j < mesh->mNumFaces; ++j, bufIndex += elemSize * 3)
				{
					auto& face = mesh->mFaces[j];

					switch (elemSize)
					{
					case sizeof(unsigned char) :
						jopmesh.m_indexBuffer[bufIndex] = static_cast<unsigned char>(face.mIndices[0]);
						jopmesh.m_indexBuffer[bufIndex + sizeof(unsigned char)] = static_cast<unsigned char>(face.mIndices[1]);
						jopmesh.m_indexBuffer[bufIndex + sizeof(unsigned char) * 2] = static_cast<unsigned char>(face.mIndices[2]);
						break;

					case sizeof(unsigned short int) :
						reinterpret_cast<unsigned short int&>(jopmesh.m_indexBuffer[bufIndex]) = static_cast<unsigned short int>(face.mIndices[0]);
						reinterpret_cast<unsigned short int&>(jopmesh.m_indexBuffer[bufIndex + sizeof(unsigned short int)]) = static_cast<unsigned short int>(face.mIndices[1]);
						reinterpret_cast<unsigned short int&>(jopmesh.m_indexBuffer[bufIndex + sizeof(unsigned short int) * 2]) = static_cast<unsigned short int>(face.mIndices[2]);
						break;

					case sizeof(unsigned int) :
						reinterpret_cast<unsigned int&>(jopmesh.m_indexBuffer[bufIndex]) = face.mIndices[0];
						reinterpret_cast<unsigned int&>(jopmesh.m_indexBuffer[bufIndex + sizeof(unsigned int)]) = face.mIndices[1];
						reinterpret_cast<unsigned int&>(jopmesh.m_indexBuffer[bufIndex + sizeof(unsigned int) * 2]) = face.mIndices[2];
					}
					indexSize += elemSize;
				}
				totalSize += indexSize;
				jopmesh.m_lengthIndex = indexSize;
			}
			//COMPONENTS
			jopmesh.m_components = jopmesh.Position
				| (mesh->HasTextureCoords(0)         * jopmesh.TexCoords)
				| (mesh->HasNormals()                * jopmesh.Normal)
				| (mesh->HasTangentsAndBitangents()  * jopmesh.Tangents)
				| (mesh->HasVertexColors(0)          * jopmesh.Color)
				;

			model.m_meshes.push_back(jopmesh);
		}
	}

	int Converter::conversion(int argc, char* argv[])
	{
		if (argc > 1)
		{
			std::string pathIn = argv[1];
			jop::Converter conv;

			//read old model file with assimp
			Assimp::Importer imp;
			printf("Loading model...\n");
			const aiScene *scene = imp.ReadFile(pathIn, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_OptimizeGraph | aiProcess_RemoveRedundantMaterials);
			if (!scene) {
				printf("Unable to load mesh: %s\n", imp.GetErrorString());
				return false;
			}
			printf("Successfully loaded file\n");

			jop::Model model;
			std::string fileOut = "model.jop";

			conv.getMaterials(scene, model);
			conv.getMeshes(scene, model);
			if (conv.jsonWriter(model, fileOut) && conv.binaryWriter(model, fileOut))
			{
				printf("Model converted successfully\n");
				return 0;
			}
			printf("Model conversion failed\n");
		}
		return 1;
	}

}