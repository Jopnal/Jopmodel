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

#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>

#include <iostream>
#include <conio.h>

int main(int argc, char* argv[])
{
	std::string str(argv[0]); //polku .exeen

	Assimp::Importer kek;

	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************

	// 1. Parse a JSON string into DOM.
	const char* jiisoni1 = "{\"project\":\"rapidjson\",\"stars\":10}";
	rapidjson::Document dee;
	dee.Parse(jiisoni1);

	// 2. Modify it by DOM.    
	rapidjson::Value& sss = dee["stars"];
	sss.SetInt(sss.GetInt() + 1);    
	// 3. Stringify the DOM    
	rapidjson::StringBuffer puhveri;
	rapidjson::Writer<rapidjson::StringBuffer> vraitteri(puhveri);
	dee.Accept(vraitteri);
	// Output {"project":"rapidjson","stars":11}  
	std::cout << puhveri.GetString() << std::endl;

	//**********************************************************************************************
	//**********************************************************************************************
	//**********************************************************************************************

	const char* jiisoni2 = "{   \"hello\":\"world\",   \"t\":true,   \"f\":false,   \"n\":null,   \"i\":123,   \"pi\":3.1416,   \"a\":[1, 2, 3, 4]   }";
	rapidjson::Document dee2;
	dee2.Parse(jiisoni2);

	assert(dee2.HasMember("hello"));
	assert(dee2["hello"].IsString());
	printf("hello = %s\n", dee2["hello"].GetString());

	assert(dee2["t"].IsBool());
	printf("%s\n", dee2["t"].GetBool() ? "true" : "false");

	assert(dee2["i"].IsNumber());// In this case, IsUint()/IsInt64()/IsUInt64() also return true.
	assert(dee2["i"].IsInt());
	printf("i = %d\n", dee2["i"].GetInt());// Alternative (int)document["i"]
	assert(dee2["pi"].IsNumber());
	assert(dee2["pi"].IsDouble());
	printf("pi = %g\n", dee2["pi"].GetDouble());

	if (dee2["hello"] == dee2["n"]) {};        // Compare values
	if (dee2["hello"] == "world") {};          // Compare value with literal string
	if (dee2["i"] != 123) {};                  // Compare with integers
	if (dee2["pi"] != 3.14) {};                // Compare with double.

	rapidjson::StringBuffer puhveri2;
	rapidjson::Writer<rapidjson::StringBuffer> vraitteri2(puhveri2);
	dee2.Accept(vraitteri2);
	std::cout << puhveri2.GetString() << std::endl;

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