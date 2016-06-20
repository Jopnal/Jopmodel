#ifndef	JOP_CONVERTER_HPP
#define	JOP_CONVERTER_HPP

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#include "Model.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"

#include <fstream>
#include <iostream>



namespace jop
{
	class Converter
	{
	public:
		Converter();
		~Converter();


		bool binaryWriter(jop::Model& model, std::string fileOut);

		bool jsonWriter(jop::Model& model, std::string fileOut);

		bool binaryReader(std::string fileOut);

		void pushReflections(jop::Material &jopmat, aiColor3D col, int refTypeIndex);

		void getMaterials(const aiScene* scene, jop::Model& model);
		
		void getMeshes(const aiScene* scene, jop::Model& model);

		static int conversion(int argc, char* argv[]);

	};
}
#endif