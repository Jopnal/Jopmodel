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

		char* meshName;
		int type; //triangles, lines etc.
		uint32_t components = 0; //bitfield
		int startLocation = 0;
		int length = 0; //bytes
	};

	class Textures
	{
	public:


		Textures(){}
		~Textures(){}

		std::string texturePath;
		int type = 0; //diffuse, specular etc.
		int wrapmode = 0; //repeat, clamp etc.
	};

	class Material
	{
	public:


		Material()
		{
		}
		~Material(){};

		char* materialName;
		float reflections[16];// = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		float shininess = 1.0f;
		float reflectivity = 0.0f;
		std::vector<Textures> textures;
	};

	class Model
	{
	public:

		Model(){};
		~Model(){};

		const char* modelName;
		std::vector<Material> materials;
		std::vector<Mesh> meshes;
	};


	size_t startPos = 0;
	size_t sizeofjson = 0;
	size_t sizeofjsonbin = 0;
	size_t sizeofmeshes = 0;

	class jsonData
	{
	public:
		jsonData()
		{


		}

		~jsonData()
		{
		}

		//how to do dynamic rapidjson array
		const char* model = "{ \"name\":\"modelName\", \"materialCount\":0, \"meshCount\":0, \"materials\":[], \"meshes\":[]  }";

		const char* materialData = "{ \"materialName\":\"materialnamehere\", \"textureCount\":0,\"Material\" :{ \"reflections\":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], \"shininess\":0, \"reflectivity\":0, \"textures\":[] }}";
		const char* textureData = "{ \"texturePath\": \"\" \"Texture\":{ \"type\":0, \"wrapmode\":0  }}";

		const char* meshData = "{ \"meshName\":{  \"type\":0, \"components\":0, \"start\":0, \"length\":0 }}";

		//old
		//const char* data2 = "{ \"materials\":{ \"materialName\":{ \"reflections\":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], \"shininess\":0, \"reflectivity\":0, \"textures\":[] }}}";
	};

	//old
	//const char* mainData = "{ \"name\":\"\", \"materials\":{ \"materialName\":{ \"reflections\":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0], \"shininess\":0, \"reflectivity\":0 }}}";

	int reflectionCount = 16;

	int dataStartPosition;
	std::vector<int> mainDataVector;
	std::vector<unsigned char*> dataPacket;
	std::vector<size_t> dataPacketSize;

	//keep this for extendability
	std::vector<Model> models;
}

bool binaryWriter(std::string fileName, int modelsIndex);
bool jsonWriter();



//old jsonwriter
/*
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
*/



bool jsonWriter()
{
	namespace rj = rapidjson;
	std::string fileName;
	jopmodel::jsonData jData;

	rapidjson::Document modeldoc;
	modeldoc.Parse(jData.model);
	if (modeldoc.HasParseError())
	{
		std::exit(EXIT_FAILURE);
	}

	rapidjson::Value materials(rapidjson::kArrayType);
	

	rapidjson::Value textures(rapidjson::kArrayType);

 	for (size_t i = 0; i < jopmodel::models.size(); i++) //MODELS
	{
		//rapidjson::Value& mnm = modeldoc["name"];
		//mnm.SetString(rapidjson::StringRef(jopmodel::models[i].modelName));

		modeldoc.AddMember(rj::StringRef("name"), rj::StringRef(jopmodel::models[i].modelName), modeldoc.GetAllocator());


		//rapidjson::Value& mac = modeldoc["materialCount"];
		//mac.SetInt(jopmodel::models.at(i).materials.size()); //how many materials are there 

		//rapidjson::Value& moc = modeldoc["meshCount"];
		//moc.SetInt(jopmodel::models.at(i).meshes.size()); //how many meshes are there

		auto& materialArray = modeldoc.AddMember(rj::StringRef("materials"), rj::kArrayType, modeldoc.GetAllocator())["materials"];
		for (auto& j : jopmodel::models[i].materials) //MATERIALS
		{

			materialArray.PushBack(rj::kObjectType, modeldoc.GetAllocator());

			auto& materialObject = materialArray[materialArray.Size() - 1];

			auto& reflectionsArray = materialObject.AddMember(rj::StringRef("reflection"), rj::kArrayType, modeldoc.GetAllocator())["reflection"];
			for (int k = 0; k * sizeof(float) < sizeof(j.reflections); ++k)
			{
				reflectionsArray.PushBack(j.reflections[k], modeldoc.GetAllocator());
			}

			materialObject.AddMember(rj::StringRef("shininess"), j.shininess, modeldoc.GetAllocator())["shininess"];
			materialObject.AddMember(rj::StringRef("reflectivity"), j.reflectivity, modeldoc.GetAllocator())["reflectivity"];


			auto& texturesArray = materialObject.AddMember(rj::StringRef("textures"), rj::kArrayType, modeldoc.GetAllocator())["textures"];
			for (auto& l : j.textures)
			{
				auto& textureObject = texturesArray.AddMember(rj::StringRef(l.texturePath.c_str()), rj::kObjectType, modeldoc.GetAllocator())[l.texturePath.c_str()];
				textureObject.AddMember(rj::StringRef("type"), l.type, modeldoc.GetAllocator())["type"];
				textureObject.AddMember(rj::StringRef("wrapmode"), l.wrapmode, modeldoc.GetAllocator())["wrapmode"];
			}
		}

		auto& meshArray = modeldoc.AddMember(rj::StringRef("meshes"), rj::kArrayType, modeldoc.GetAllocator())["meshes"];
		for (auto& i : jopmodel::models[i].meshes)
		{
			meshArray.PushBack(rj::kObjectType, modeldoc.GetAllocator());

			auto& modelObject = meshArray[meshArray.Size() - 1];

			modelObject.AddMember(rj::StringRef("type"), 0, modeldoc.GetAllocator())["type"];
			modelObject.AddMember(rj::StringRef("components"), i.components, modeldoc.GetAllocator())["components"];
			modelObject.AddMember(rj::StringRef("start"), i.startLocation, modeldoc.GetAllocator())["start"];
			modelObject.AddMember(rj::StringRef("length"), i.length, modeldoc.GetAllocator())["length"];
		}



		fileName = (std::string)jopmodel::models.at(i).modelName + ".jop";
		//write json to file
		std::ofstream file(fileName);
		if (file.is_open())
		{
			rapidjson::StringBuffer mdbuff;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> mdwriter(mdbuff);
			if (modeldoc.HasParseError())
			{
				std::cout << "PARSE ERROR" << std::endl;
			}
			modeldoc.Accept(mdwriter);

			jData.model = mdbuff.GetString();

			file << jData.model;
			file.close();
			std::cout << "Wrote json from model " << i + 1 << " successfully!" << std::endl;
			binaryWriter(fileName, i);
		}


	}
	return true;


	////nimi
	//rapidjson::Value& v = doc["name"];
	//v = rapidjson::StringRef(model.modelName);
	//
	//
	////materialName here
	//
	//
	//rapidjson::Value& ref = doc["materials"]["materialName"]["reflections"];
	//for (size_t i = 0; i < 16; i++)
	//{
	//ref[i] = reflections[i];
	//}
	//
	//
	//rapidjson::StringBuffer buff;
	//rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buff);
	//doc.Accept(writer);
	//puts(buff.GetString());
	//
	//jopmodel::mainData = buff.GetString();
	//jopmodel::sizeofjson = sizeof(jopmodel::mainData);
	//jopmodel::mainDataVector.push_back((int)jopmodel::mainData);
	//
	////write json to file
	//std::ofstream file(fileName);
	//if (file.is_open())
	//{
	//file << jopmodel::mainData;
	//file.close();
	//return true;
	//}
	//printf("Writing json failed!");
	//return false;

}

bool binaryWriter(std::string fileName, int modelsIndex)
{
	std::ofstream writeFile(fileName, std::ios::out | std::ios::app | std::ios::ate);
	if (writeFile)
	{
		jopmodel::startPos = writeFile.tellp(); //size of the json data == where to start writing binary data

		for (size_t i = 0; i < jopmodel::models[modelsIndex].materials.size(); i++) //MATERIALS
		{
			for (size_t j = 0; j < 16; j++)
			{
				writeFile.write((char*)&jopmodel::models[modelsIndex].materials[i].reflections[j], sizeof(float)); //MATERIAL REFLECTIONS
			}
			writeFile.write((char*)&jopmodel::models[modelsIndex].materials[i].shininess, sizeof(float));
			writeFile.write((char*)&jopmodel::models[modelsIndex].materials[i].reflectivity, sizeof(float));

			for (size_t k = 0; k < jopmodel::models[modelsIndex].materials[i].textures.size(); k++) //MATERIAL TEXTURES
			{
				writeFile.write((char*)&jopmodel::models[modelsIndex].materials[i].textures[k].type, sizeof(float));
				writeFile.write((char*)&jopmodel::models[modelsIndex].materials[i].textures[k].wrapmode, sizeof(float));
			}
		}

		for (size_t i = 0; i < jopmodel::models[modelsIndex].meshes.size(); i++) //MESHES
		{
			writeFile.write((char*)&jopmodel::models[modelsIndex].meshes[i].type, sizeof(int));
			writeFile.write((char*)&jopmodel::models[modelsIndex].meshes[i].components, sizeof(uint32_t));
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



void pushReflections(jopmodel::Material mat, aiColor3D col, int refTypeIndex)
{
	mat.reflections[(1 * refTypeIndex) - 1] = col.r;
	mat.reflections[(2 * refTypeIndex) - 1] = col.g;
	mat.reflections[(3 * refTypeIndex) - 1] = col.b;
	mat.reflections[(4 * refTypeIndex) - 1] = 1.0;
}



bool processModel(const aiScene* scene, jopmodel::Model model)
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


	model.modelName = mesh->mName.C_Str();
	/*
	std::string str = mesh->mName.C_Str();

	char* writable = new char[str.size() + 1];
	std::copy(str.begin(), str.end(), writable);
	writable[str.size()] = '\0';

	model.modelName = writable;
	delete[] writable;
	*/
	mesh->mMaterialIndex; //do something with this


	aiColor3D col;
	aiMaterial* mat = scene->mMaterials[0];

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		jopmodel::Material jopmaterial;

		jopmaterial.materialName = "mnhere_pcmodel_";// +i;
		/*
		std::string str2 = "mnhere_pcmodel_" + i;
		char* writable2 = new char[str2.size() + 1];
		std::copy(str2.begin(), str2.end(), writable2);
		writable2[str2.size()] = '\0';
		jopmaterial.materialName = writable2;
		delete[] writable2;
		*/
		mat = scene->mMaterials[i];

		mat->Get(AI_MATKEY_COLOR_AMBIENT, col);
		pushReflections(jopmaterial, col, 1);
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, col);
		pushReflections(jopmaterial, col, 2);
		mat->Get(AI_MATKEY_COLOR_SPECULAR, col);
		pushReflections(jopmaterial, col, 3);
		mat->Get(AI_MATKEY_COLOR_EMISSIVE, col);
		pushReflections(jopmaterial, col, 4);
		mat->Get(AI_MATKEY_SHININESS, jopmaterial.shininess);
		mat->Get(AI_MATKEY_REFLECTIVITY, jopmaterial.reflectivity);
	
		//iterate from diffuse to unknown
		//if there is a texture of that type do something with it
		for (int i = aiTextureType::aiTextureType_DIFFUSE; i < aiTextureType::aiTextureType_UNKNOWN; i++)
		{
			if (mat->GetTextureCount((aiTextureType)i))
			{
				jopmodel::Textures joptexture;

				aiTexture tex;
				aiString path;

				mat->GetTexture((aiTextureType)i, 0, &path);

				if (path.length)
				{
					

					joptexture.texturePath = path.C_Str();
					printf("found texture\n");
				}
				jopmaterial.textures.push_back(joptexture);
			}
		}

		model.materials.push_back(jopmaterial);
	}




	int type = 0;
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		jopmodel::Mesh jopmesh;

		const uint32_t comps = jopmodel::Mesh::Position

			| (mesh->HasTextureCoords(0)         * jopmodel::Mesh::TexCoords)
			| (mesh->HasNormals()                * jopmodel::Mesh::Normal)
			| (mesh->HasTangentsAndBitangents()  * jopmodel::Mesh::Tangents)
			| (mesh->HasVertexColors(0)          * jopmodel::Mesh::Color)
			;

		jopmesh.components = comps;


		//jopmodel::meshes.push_back(std::make_pair(&ResourceManager::getNamedResource<Mesh>("jop_mesh_" + getHex(), vertBuf.data(), vertBuf.size(), comps, indBuf.data(), elemSize, mesh->mNumFaces * 3), mesh->mMaterialIndex));


		model.meshes.push_back(jopmesh);
	}
	jopmodel::models.push_back(model);
	return true;
}

int main(int argc, char* argv[])
{
	std::string str(argv[0]); //polku .exeen

	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************


	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************


	//const char* jiisoni2 = "{   \"hello\":\"world\",   \"t\":true,   \"f\":false,   \"n\":null,   \"i\":123,   \"pi\":3.1416,   \"a\":[1, 2, 3, 4]   }";


	//muuttujat
	//char* pt1 = "DOOMGUY";


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
	const aiScene *scene = imp.ReadFile(modelFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_OptimizeGraph);
	if (!scene) {
		printf("Unable to load mesh: %s\n", imp.GetErrorString());
	}
	else
	{
		printf("Successfully loaded file!\n");
	}

	jopmodel::Model jopmodelbase;

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

	jopmodelbase.modelName = "wqertyuiop"; //temp while getting that to work ^

	if (!processModel(scene, jopmodelbase))
	{
		printf("Handling model failed! (inside processModel())\n");
	}

	//write data to file
	jsonWriter();


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

/*

int main(int, char*[]) {
////////////////////////////////////////////////////////////////////////////
// 1. Parse a JSON text string to a document.

const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";
printf("Original JSON:\n %s\n", json);

Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

#if 0
// "normal" parsing, decode strings to new buffers. Can use other input stream via ParseStream().
if (document.Parse(json).HasParseError())
return 1;
#else
// In-situ parsing, decode strings directly in the source string. Source must be string.
char buffer[sizeof(json)];
memcpy(buffer, json, sizeof(json));
if (document.ParseInsitu(buffer).HasParseError())
return 1;
#endif

printf("\nParsing to document succeeded.\n");

////////////////////////////////////////////////////////////////////////////
// 2. Access values in document.

printf("\nAccess values in document:\n");
assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.

assert(document.HasMember("hello"));
assert(document["hello"].IsString());
printf("hello = %s\n", document["hello"].GetString());

// Since version 0.2, you can use single lookup to check the existing of member and its value:
Value::MemberIterator hello = document.FindMember("hello");
assert(hello != document.MemberEnd());
assert(hello->value.IsString());
assert(strcmp("world", hello->value.GetString()) == 0);
(void)hello;

assert(document["t"].IsBool());     // JSON true/false are bool. Can also uses more specific function IsTrue().
printf("t = %s\n", document["t"].GetBool() ? "true" : "false");

assert(document["f"].IsBool());
printf("f = %s\n", document["f"].GetBool() ? "true" : "false");

printf("n = %s\n", document["n"].IsNull() ? "null" : "?");

assert(document["i"].IsNumber());   // Number is a JSON type, but C++ needs more specific type.
assert(document["i"].IsInt());      // In this case, IsUint()/IsInt64()/IsUInt64() also return true.
printf("i = %d\n", document["i"].GetInt()); // Alternative (int)document["i"]

assert(document["pi"].IsNumber());
assert(document["pi"].IsDouble());
printf("pi = %g\n", document["pi"].GetDouble());

{
const Value& a = document["a"]; // Using a reference for consecutive access is handy and faster.
assert(a.IsArray());
for (SizeType i = 0; i < a.Size(); i++) // rapidjson uses SizeType instead of size_t.
printf("a[%d] = %d\n", i, a[i].GetInt());

int y = a[0].GetInt();
(void)y;

// Iterating array with iterators
printf("a = ");
for (Value::ConstValueIterator itr = a.Begin(); itr != a.End(); ++itr)
printf("%d ", itr->GetInt());
printf("\n");
}

// Iterating object members
static const char* kTypeNames[] = { "Null", "False", "True", "Object", "Array", "String", "Number" };
for (Value::ConstMemberIterator itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
printf("Type of member %s is %s\n", itr->name.GetString(), kTypeNames[itr->value.GetType()]);

////////////////////////////////////////////////////////////////////////////
// 3. Modify values in document.

// Change i to a bigger number
{
uint64_t f20 = 1;   // compute factorial of 20
for (uint64_t j = 1; j <= 20; j++)
f20 *= j;
document["i"] = f20;    // Alternate form: document["i"].SetUint64(f20)
assert(!document["i"].IsInt()); // No longer can be cast as int or uint.
}

// Adding values to array.
{
Value& a = document["a"];   // This time we uses non-const reference.
Document::AllocatorType& allocator = document.GetAllocator();
for (int i = 5; i <= 10; i++)
a.PushBack(i, allocator);   // May look a bit strange, allocator is needed for potentially realloc. We normally uses the document's.

// Fluent API
a.PushBack("Lua", allocator).PushBack("Mio", allocator);
}

// Making string values.

// This version of SetString() just store the pointer to the string.
// So it is for literal and string that exists within value's life-cycle.
{
document["hello"] = "rapidjson";    // This will invoke strlen()
// Faster version:
// document["hello"].SetString("rapidjson", 9);
}

// This version of SetString() needs an allocator, which means it will allocate a new buffer and copy the the string into the buffer.
Value author;
{
//char buffer2[10];
//int len = sprintf(buffer2, "%s %s", "Milo", "Yip");  // synthetic example of dynamically created string.

//author.SetString(buffer2, static_cast<SizeType>(len), document.GetAllocator());
// Shorter but slower version:
// document["hello"].SetString(buffer, document.GetAllocator());

// Constructor version:
// Value author(buffer, len, document.GetAllocator());
// Value author(buffer, document.GetAllocator());
//memset(buffer2, 0, sizeof(buffer2)); // For demonstration purpose.
}
// Variable 'buffer' is unusable now but 'author' has already made a copy.
document.AddMember("author", author, document.GetAllocator());

assert(author.IsNull());        // Move semantic for assignment. After this variable is assigned as a member, the variable becomes null.

////////////////////////////////////////////////////////////////////////////
// 4. Stringify JSON

printf("\nModified JSON with reformatting:\n");
StringBuffer sb;
PrettyWriter<StringBuffer> writer(sb);
document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
puts(sb.GetString());

_getch();

return 0;
}

*/