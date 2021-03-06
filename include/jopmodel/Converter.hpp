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
#define NOMINMAX

//Headers
#include <jopmodel/FileSystem.hpp>
#include <jopmodel/Material.hpp>
#include <jopmodel/Mesh.hpp>
#include <jopmodel/Model.hpp>
#include <jopmodel/Texture.hpp>

//#include <direct.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
//#include <windows.h>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

//////////////////////////////////////////////

namespace jopm
{
    namespace detail
    {
        struct Logger : Assimp::Logger
        {
            Logger()
                : Assimp::Logger(Assimp::Logger::LogSeverity::VERBOSE)
            {}

            bool attachStream(Assimp::LogStream*, unsigned int)
            {
                return true;
            }
            bool detatchStream(Assimp::LogStream*, unsigned int)
            {
                return true;
            }
            void OnDebug(const char* message) override
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN);
                std::cout << "DEBUG: " << message << std::endl;
            }
            void OnInfo(const char* message) override
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED);
                std::cout << "INFO: " << message << std::endl;
            }
            void OnWarn(const char* message) override
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
                std::cout << "WARNING: " << message << std::endl;
            }
            void OnError(const char* message) override
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
                std::cout << "ERROR: " << message << std::endl;
            }
        };
    }

    class Converter
    {
    public:

        /// \brief Model converter
        ///
        /// This will convert a model file into jopmodel format
        /// argv[1]:    model file to convert
        /// argv[2]:    (Optional) path to the new model file
        /// argv[1+]:   (Optional) Options to use during conversion
        ///
        /// \param argc Argc from main()
        /// \param argv Argv[] from main()
        ///
        static int conversion(const int argc, const char* argv[]);

    private:

        Converter();
        ~Converter();

        bool processNode(aiNode& node, rapidjson::Value::AllocatorType& alloc, rapidjson::Value& root, rapidjson::Value*& out);

        bool makeNodes(aiNode& parentNode, std::vector<Mesh>& meshes, std::vector<Material>& mats, rapidjson::Value::AllocatorType& alloc, rapidjson::Value& root);

        bool binaryWriter(const Model& model, const std::string& fileOut);

        bool jsonWriter(const aiScene& scene, Model& model, const std::string& fileOut);

        bool binaryReader(std::string fileOut);

        void getMaterials(const aiScene* scene, Model& model, FileSystem& fs);

        void getMeshes(const aiScene* scene, Model& model);

        unsigned int processAssimpArgs(const FileSystem& fs);

        void getValuesFS(const FileSystem& fs);

        std::string m_outPath;
        std::string m_modelName;
        
        bool m_embedTex;
        bool m_centered;

        unsigned int m_binaryWriter;
        unsigned int m_binaryLastSize;

        //1. minimum BB, 2. maximum BB
        std::pair<glm::vec3, glm::vec3> m_globalBB;
        std::unordered_map<std::string, Texture> m_textures;
    };
}
#endif