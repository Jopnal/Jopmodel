// Jopnal Model C++ Application
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

//////////////////////////////////////////////

#ifndef	JOPM_CONVERTER_HPP
#define	JOPM_CONVERTER_HPP

//Headers
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include <jopmodel/Model.hpp>
#include <jopmodel/Material.hpp>
#include <jopmodel/Texture.hpp>
#include <jopmodel/Mesh.hpp>

#include <fstream>
#include <iostream>

//////////////////////////////////////////////

namespace jopm
{
	class Converter
	{
	public:
		Converter();
		~Converter();


		bool binaryWriter(Model& model, const std::string fileOut);

		bool jsonWriter(Model& model, const std::string fileOut);

		bool binaryReader(std::string const fileOut);

		void pushReflections(Material &jopmat, const aiColor3D col, const int refTypeIndex);

		void getMaterials(const aiScene* scene, const Model& model);
		
		void getMeshes(const aiScene* scene, Model& model);

		/// \brief Model converter
		///
		/// This will convert a model file into jopmodel format
		/// argv[1]: model file to convert
		/// argv[2]: (Optional) name for the new model file
		///
		/// \param argc Argc from main()
		/// \param argv Argv[] from main()
		///
		static int conversion(int argc, char* argv[]);

	};
}
#endif