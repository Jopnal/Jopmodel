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




#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\pointer.h>

#include <iostream>
#include <conio.h>
#include <fstream>
#include <vector>


//std::vector<uint8_t> vertexBuffer();
namespace jopmodel{
	int startPos;
	int size;

	int dataStartPosition;
	std::vector<unsigned char*> dataPacket;
	std::vector<size_t> dataPacketSize;

}

class Mesh
{
public:
	Mesh(char* meshName, int type, int start, int length)
	{
		this->meshName = meshName;
		this->type = type;
		this->startLocation = start;
		this->length = length;
	}
	~Mesh();

private:
	char* meshName;
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


bool jsonWriter(const char* data, rapidjson::Document &doc, const char* fileName)
{
	rapidjson::StringBuffer buff;
	rapidjson::Writer<rapidjson::StringBuffer> vraitteri(buff);
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
		switch (data[i])
		{
		case '{':
		case '}':
		{
			file << std::endl;
			file << data[i];
			file << std::endl;
			break;
		}
		default:
		{
			file << data[i];
			break;
		}
		}
	}
	file.close();
	return true;
}

bool binaryWriter(const char* fileName, int nums[7])
{
	std::ofstream writeFile(fileName, std::ios::out | std::ios::app | std::ios::ate);
	if (writeFile)
	{
		std::cout << writeFile.tellp() << std::endl;
		//binFile.seekp(0, std::ios::end); //kirjoitus-osoitin tiedoston loppuun -> append?
		jopmodel::startPos = writeFile.tellp();
		std::cout << writeFile.tellp() << std::endl;

		for (size_t i = 0; i < 7; i++)
		{
			writeFile.write((char*)&nums[i], sizeof(int)); //write data to file
			//binFile.seekp(jopmodel::startPos+i); //<- should move by itself
		}
		jopmodel::size = (int)writeFile.tellp() - jopmodel::startPos;
		writeFile.close();
	}
	return true;
}

bool binaryReader(const char* fileName)
{
	int startpos = jopmodel::startPos;
	int size = jopmodel::size;

	int n;

	std::ifstream readFile(fileName, std::ios::binary | std::ios::in);
	if (readFile)
	{
		readFile.seekg(startpos); //lukuosoitin binäärien alkuun

		for (size_t i = 0; i < 7; i++)
		{
			readFile.read((char*)&n, sizeof(int));	//read
			std::cout << n << std::endl;			//print
			//binFile.seekg(startpos += i);			//move reading position
		}
		readFile.close();
	}
	return true;
}

int main(int argc, char* argv[])
{
	std::string str(argv[0]); //polku .exeen

	std::vector<Mesh*> globmeshes;
	std::vector<Material*> globmaterials;

	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************



	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************


	//const char* jiisoni2 = "{   \"hello\":\"world\",   \"t\":true,   \"f\":false,   \"n\":null,   \"i\":123,   \"pi\":3.1416,   \"a\":[1, 2, 3, 4]   }";


	//muuttujat
	char* pt1 = "DOOMGUY";

	char* fileName = "data.jop";

	int wat[] = { 3, 5, 7 };
	float reflections[] = { 1.0, 0.5, 0.2, 1.0, 0.0, 0.0, 0.0, 1.0 };

	//initialize
	const char* data = "{ \"name\":\"\", \"Model\":{\"position\":[1,2,3], \"reflections\":[1,2,3,4,5,6,7,8] }}";

	rapidjson::Document doc;
	doc.Parse(data);

	//nimi
	rapidjson::Value& v = doc["name"];
	v = rapidjson::StringRef(pt1);
	//v.SetString(rapidjson::StringRef(pt1));


	//position
	rapidjson::Value& xyz = doc["Model"]["position"];
	for (size_t i = 0; i < 3; i++)
	{
		xyz[i] = wat[i];
	}

	rapidjson::Value& ref = doc["Model"]["reflections"];
	for (size_t i = 0; i < 8; i++)
	{
		ref[i] = reflections[i];
	}


	//read old model file with assimp

	/*
	Assimp::Importer imp;
	const aiScene* scene = imp.ReadFile()
	if (!scene)
	{
	printf("Importing model failed!");
	return 0;
	}
	*/

	//**********

	//globmeshes.push_back(&Mesh("DOOMGUY", 2, 0, 8));
	//Model model(globmeshes);

	//**********

	
	int nums[7] = { 3, 5, 7, 6, 2, 1, 5 };

	//write data to file
	jsonWriter(data, doc, fileName);
	binaryWriter(fileName, nums);
	binaryReader(fileName);

	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************












	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************


	argv[argc] = "wat";
	argc++;

	std::cout << "\nMäärä: argc = " << argc << "\n" << std::endl;
	for (size_t i = 0; i < argc; i++)
	{
		std::cout << "argv at " << i << " == " << argv[i] << "\n" << std::endl;
	}

	_getch();
	return 1;

}