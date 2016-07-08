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
#include <jopmodel/Material.hpp>
#include <jopmodel/Mesh.hpp>
#include <jopmodel/Model.hpp>
#include <jopmodel/Texture.hpp>

#include <direct.h>
#include <fstream>
#include <iostream>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <dirent/dirent.h>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

//////////////////////////////////////////////

namespace jopm
{
    class Converter
    {
    public:

        /// \brief Model converter
        ///
        /// This will convert a model file into jopmodel format
        /// argv[1]: model file to convert
        /// argv[2]: (Optional) name for the new model file
        ///
        /// \param argc Argc from main()
        /// \param argv Argv[] from main()
        ///
        static int conversion(const int argc, const char* argv[]);


    private:
        Converter();
        ~Converter();

        bool processNode(aiNode& node, std::vector<Mesh>& meshes, std::vector<Material>& mats, rapidjson::Value::AllocatorType& alloc, rapidjson::Value& root, rapidjson::Value*& out);

        bool makeNodes(aiNode& parentNode, std::vector<Mesh>& meshes, std::vector<Material>& mats, rapidjson::Value::AllocatorType& alloc, rapidjson::Value& root);

        bool binaryWriter(Model& model, std::string fileOut);

        bool jsonWriter(const aiScene& scene, Model& model, std::string fileOut);

        bool binaryReader(std::string fileOut);

        void pushReflections(Material &jopmat, aiColor3D col, int refTypeIndex);

        std::string getTexture(const std::string texPath);

        std::string findTexture(const std::string searchDir, const std::string texName);

        void getMaterials(const aiScene* scene, Model& model);

        std::string sortArgs(const int argc, const char* argv[]);

        void getMeshes(const aiScene* scene, Model& model);

        std::string m_searchLoc;
        std::string m_modelName;
        std::string m_outputDir;
    };
}
#endif