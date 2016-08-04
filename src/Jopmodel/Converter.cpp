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

//Headers
#include <jopmodel/Converter.hpp>
#include <Jopnal/Graphics/Material.hpp>
#include <Jopnal/Graphics/Mesh.hpp>
#include <Jopnal/Graphics/Texture.hpp>

//////////////////////////////////////////////

namespace jopm
{
    Converter::Converter() :
        m_searchLoc(),
        m_modelName(),
        m_outputDir(),
        m_textureName(),
        m_impArgs(0),
        m_binaryWriter(0),
        m_binaryLastSize(0),
        m_embedTex(false),
        m_centered(true),
        m_globalBB(),
        m_argCalls(),
        m_textures(),
        m_argvNewPath(false),
        m_verbose(false)
    {
        m_argCalls = { "-", "/", "--" };
        m_globalBB = std::make_pair(glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX), glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX));
    }

    Converter::~Converter()
    {
    }

    bool Converter::binaryWriter(const Model& model, const std::string& fileOut)
    {
        std::ofstream writeFile(fileOut, std::ios::binary | std::ios::out | std::ios::app | std::ios::ate);
        if (writeFile.is_open())
        {
            if (m_embedTex)
            {
                for (auto& i : m_textures)
                {
                    if (!i.second.m_texturePath.empty())
                    {
                        i.second.m_texturePath.erase(i.second.m_texturePath.begin(), i.second.m_texturePath.begin() + i.second.m_texturePath.find_last_of('/') + 1);

                        std::ifstream texFile(m_searchLoc + '\\' + i.second.m_texturePath, std::ios::binary | std::ios::in);
                        if (texFile.is_open())
                        {
                            writeFile << texFile.rdbuf();
                            texFile.clear();
                            texFile.close();
                        }
                        else
                            std::cout << "Failed to open file \"" << i.second.m_texturePath << "\" for reading" << std::endl;
                    }
                }
            }
            for (auto& j : model.m_meshes)
            {
                writeFile.write(reinterpret_cast<const char*>(j.m_vertexBuffer.data()), j.m_meshLength);
                writeFile.write(reinterpret_cast<const char*>(j.m_indexBuffer.data()), j.m_meshLengthIndex);
            }
            writeFile.close();
            return true;
        }
        else
            std::cout << "Failed to open file \"" << fileOut << "\" for reading" << std::endl;

        return false;
    }

    bool Converter::processNode(aiNode& node, rapidjson::Value::AllocatorType& alloc, rapidjson::Value& rootChild, rapidjson::Value*& out)
    {
        namespace rj = rapidjson;

        if (!node.mName.length)
            node.mName = "unnamed";

        rootChild.AddMember(rj::StringRef(node.mName.C_Str()), rj::kObjectType, alloc);
        auto& nodeObject = (rootChild.MemberEnd() - 1)->value;

        out = &nodeObject;

        // Transform
        {
            aiVector3D scale, pos; aiQuaternion rot;
            node.mTransformation.Decompose(scale, rot, pos);
            glm::quat rots(rot.w, rot.x, rot.y, rot.z);

            auto& scaleArray = nodeObject.AddMember(rj::StringRef("scale"), rj::kArrayType, alloc)["scale"];
            scaleArray.PushBack(scale.x, alloc);
            scaleArray.PushBack(scale.y, alloc);
            scaleArray.PushBack(scale.z, alloc);

            auto& rotationArray = nodeObject.AddMember(rj::StringRef("rotation"), rj::kArrayType, alloc)["rotation"];
            rotationArray.PushBack(rots.w, alloc);
            rotationArray.PushBack(rots.x, alloc);
            rotationArray.PushBack(rots.y, alloc);
            rotationArray.PushBack(rots.z, alloc);

            auto& positionArray = nodeObject.AddMember(rj::StringRef("position"), rj::kArrayType, alloc)["position"];
            positionArray.PushBack(pos.x, alloc);
            positionArray.PushBack(pos.y, alloc);
            positionArray.PushBack(pos.z, alloc);
        }

        //mesh index
        auto& meshArray = nodeObject.AddMember(rj::StringRef("meshes"), rj::kArrayType, alloc)["meshes"];

        for (size_t i = 0; i < node.mNumMeshes; ++i)
        {
            meshArray.PushBack(node.mMeshes[i], alloc);
        }

        return true;
    }

    bool Converter::makeNodes(aiNode& parentNode, std::vector<Mesh>& meshes, std::vector<Material>& mats, rapidjson::Value::AllocatorType& alloc, rapidjson::Value& root)
    {
        rapidjson::Value* out = nullptr;

        if (!processNode(parentNode, alloc, root, out))
            return false;

        rapidjson::Value* rootChild = nullptr;

        if (parentNode.mNumChildren)
            rootChild = &out->AddMember(rapidjson::StringRef("children"), rapidjson::kObjectType, alloc)["children"];

        for (std::size_t i = 0; i < parentNode.mNumChildren; ++i)
        {
            aiNode& thisNode = *parentNode.mChildren[i];
            if (!makeNodes(thisNode, meshes, mats, alloc, *rootChild))
                return false;
        }
        return true;
    }

    bool Converter::jsonWriter(const aiScene& scene, Model& model, const std::string& fileOut)
    {
        namespace rj = rapidjson;

        rj::Document modeldoc;
        modeldoc.SetObject();

        //MAIN TEXTUREARRAY
        auto& texArray = modeldoc.AddMember(rj::StringRef("textures"), rj::kObjectType, modeldoc.GetAllocator())["textures"];
        for (auto& j : m_textures)
        {
            auto& texObject = texArray.AddMember(rj::StringRef(j.second.m_texturePath.c_str()), rj::kObjectType, modeldoc.GetAllocator())[j.second.m_texturePath.c_str()];

            if (m_embedTex)
            {
                texObject.AddMember(rj::StringRef("start"), j.second.m_texStart, modeldoc.GetAllocator());
                texObject.AddMember(rj::StringRef("length"), j.second.m_texLength, modeldoc.GetAllocator());
            }
            else
            {
                //std::string temp = j.first.substr(j.first.find_last_of('/') + 1, j.first.size())
                texObject.AddMember(rj::StringRef("path"), rj::Value((m_modelName + '/' + j.first.substr(j.first.find_last_of('/') + 1, j.first.size())).c_str(), modeldoc.GetAllocator()), modeldoc.GetAllocator());
            }
            texObject.AddMember(rj::StringRef("wrapmode"), j.second.m_wrapmode, modeldoc.GetAllocator());
            texObject.AddMember(rj::StringRef("srgb"), j.second.m_srgb, modeldoc.GetAllocator());
            texObject.AddMember(rj::StringRef("genmipmaps"), j.second.m_genmipmaps, modeldoc.GetAllocator());
        }

        //GLOBAL AABB
        auto& globalBBarray = modeldoc.AddMember(rj::StringRef("globalbb"), rj::kArrayType, modeldoc.GetAllocator())["globalbb"];
        globalBBarray.PushBack(m_globalBB.first.x, modeldoc.GetAllocator());
        globalBBarray.PushBack(m_globalBB.first.y, modeldoc.GetAllocator());
        globalBBarray.PushBack(m_globalBB.first.z, modeldoc.GetAllocator());
        globalBBarray.PushBack(m_globalBB.second.x, modeldoc.GetAllocator());
        globalBBarray.PushBack(m_globalBB.second.y, modeldoc.GetAllocator());
        globalBBarray.PushBack(m_globalBB.second.z, modeldoc.GetAllocator());

        //MATERIALS
        auto& materialArray = modeldoc.AddMember(rj::StringRef("materials"), rj::kArrayType, modeldoc.GetAllocator())["materials"];
        for (auto& j : model.m_materials)
        {
            materialArray.PushBack(rj::kObjectType, modeldoc.GetAllocator());

            auto& materialObject = materialArray[materialArray.Size() - 1];

            //MATERIAL REFLECTIONS
            auto& reflectionsArray = materialObject.AddMember(rj::StringRef("reflection"), rj::kArrayType, modeldoc.GetAllocator())["reflection"];
            for (int k = 0; k * sizeof(float) < sizeof(j.m_reflections); ++k)
            {
                reflectionsArray.PushBack(j.m_reflections[k], modeldoc.GetAllocator());
            }

            materialObject.AddMember(rj::StringRef("shininess"), j.m_shininess, modeldoc.GetAllocator());
            materialObject.AddMember(rj::StringRef("reflectivity"), j.m_reflectivity, modeldoc.GetAllocator());

            //TEXTUREARRAY IN MATERIAL
            auto& texturesArray = materialObject.AddMember(rj::StringRef("textures"), rj::kObjectType, modeldoc.GetAllocator())["textures"];

            for (auto& l : j.m_keypairs)
            {
                auto& textureObject = texturesArray.AddMember(rj::StringRef(l.first.c_str()), rj::kObjectType, modeldoc.GetAllocator())[l.first.c_str()];
                textureObject.AddMember(rj::StringRef("type"), l.second, modeldoc.GetAllocator());
            }
        }

        //MESHES
        auto& meshArray = modeldoc.AddMember(rj::StringRef("meshes"), rj::kArrayType, modeldoc.GetAllocator())["meshes"];
        for (auto& i : model.m_meshes)
        {
            meshArray.PushBack(rj::kObjectType, modeldoc.GetAllocator());

            auto& modelObject = meshArray[meshArray.Size() - 1];

            modelObject.AddMember(rj::StringRef("material"), i.m_matIndex, modeldoc.GetAllocator());
            modelObject.AddMember(rj::StringRef("type"), i.m_type, modeldoc.GetAllocator());
            modelObject.AddMember(rj::StringRef("components"), i.m_components, modeldoc.GetAllocator());
            modelObject.AddMember(rj::StringRef("start"), i.m_meshStart, modeldoc.GetAllocator());
            modelObject.AddMember(rj::StringRef("length"), i.m_meshLength, modeldoc.GetAllocator());
            modelObject.AddMember(rj::StringRef("startIndex"), i.m_meshStartIndex, modeldoc.GetAllocator());
            modelObject.AddMember(rj::StringRef("lengthIndex"), i.m_meshLengthIndex, modeldoc.GetAllocator());
            modelObject.AddMember(rj::StringRef("sizeIndex"), i.m_meshSizeIndex, modeldoc.GetAllocator());

            //LOCAL BB
            auto& localBBarray = modelObject.AddMember(rj::StringRef("aabb"), rj::kArrayType, modeldoc.GetAllocator())["aabb"];
            localBBarray.PushBack(i.m_localBB.first.x, modeldoc.GetAllocator());
            localBBarray.PushBack(i.m_localBB.first.y, modeldoc.GetAllocator());
            localBBarray.PushBack(i.m_localBB.first.z, modeldoc.GetAllocator());
            localBBarray.PushBack(i.m_localBB.second.x, modeldoc.GetAllocator());
            localBBarray.PushBack(i.m_localBB.second.y, modeldoc.GetAllocator());
            localBBarray.PushBack(i.m_localBB.second.z, modeldoc.GetAllocator());
        }

        scene.mRootNode->mName = "rootnode";
        makeNodes(*scene.mRootNode, model.m_meshes, model.m_materials, modeldoc.GetAllocator(), modeldoc);

        //write json to file
        std::ofstream file(fileOut, std::ofstream::trunc);
        file.close();
        file.open(fileOut, std::ofstream::out);
        if (file.is_open())
        {
            rapidjson::StringBuffer mdbuff;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> mdwriter(mdbuff);
            modeldoc.Accept(mdwriter);

            file << mdbuff.GetString();
            file.close();
            return true;
        }
        std::cout << "Writing json failed\n" << std::endl;
        return false;
    }

    std::string Converter::getTexture(Material& jopmat, const std::string& texPath)
    {
        std::string texLoc;
        std::string textureName = texPath;
        int texFolder = -1;
        bool foundTex = false;

        //get the name of the texture resource
        for (size_t i = 0; i < texPath.size(); ++i)
        {
            if (texPath[i] == '/' || texPath[i] == '\\' || texPath[i] == './' || texPath[i] == '.\\')
                texFolder = i;
        }
        textureName = textureName.substr(texFolder + 1, textureName.size());

        //no need to go in here when embedding
        if (!m_embedTex)
        {
            //Check quickly if there is already a correct folder...
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir(m_outputDir.c_str())) != NULL)
            {
                //...and does it have the correct file...
                while ((ent = readdir(dir)) != NULL)
                {
                    if (std::string(ent->d_name) != "." && std::string(ent->d_name) != "..")
                    {
                        if (ent->d_type == DT_UNKNOWN || ent->d_type == DT_REG)
                        {
                            if (std::string(ent->d_name) == textureName)
                            {
                                //texture found
                                foundTex = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        //...if not, go find the texture location
        if (!foundTex)
        {
            //The Search
            texLoc = findTexture(m_searchLoc, textureName);

            if (!texLoc.empty())
            {
                std::ifstream src(texLoc, std::ios::binary);
                if (m_embedTex)
                {
                    //get the size to write
                    unsigned int temp = m_binaryWriter;
                    m_binaryWriter += static_cast<unsigned int>(src.rdbuf()->pubseekoff(0, src.end));
                    m_binaryLastSize = m_binaryWriter - temp;
                }
                else
                {
                    std::ofstream dest(m_outputDir + '\\' + textureName, std::ios::binary | std::ios::trunc);
                    dest << src.rdbuf();
                    dest.close();
                }
                src.close();
            }
            else
            {
                std::cout << "Failed to find texture: " << textureName << " from " << m_searchLoc << std::endl;
            }
        }
        m_textureName = textureName;
        //Changing the path to be compatible with the Jopnal engine
        return m_modelName + '/' + textureName;
    }

    std::string Converter::findTexture(const std::string& searchDir, const std::string& texName)
    {
        DIR *dir;
        struct dirent *ent;

        //Anyone here?
        if ((dir = opendir(searchDir.c_str())) != NULL)
        {
            //Are we there yet?
            while ((ent = readdir(dir)) != NULL)
            {
                if (std::string(ent->d_name) != "." && std::string(ent->d_name) != "..")
                {
                    switch (ent->d_type)
                    {
                    case DT_UNKNOWN:
                    case DT_REG:
                    {
                        //check file
                        if (std::string(ent->d_name) == texName)
                        {
                            //texture found
                            return searchDir + '\\' + ent->d_name;
                        }
                        break;
                    }
                    //go check the subfolder
                    case DT_DIR:
                    {
                        std::string temp = findTexture(searchDir + '\\' + ent->d_name, texName);
                        if (!temp.empty())
                            return temp;
                    }
                    }
                }
            }
        }
        return "";
    }

    void Converter::getMaterials(const aiScene* scene, Model& model)
    {
        aiColor3D col;

        printf("Materials: %d\nMeshes: %d\n", scene->mNumMaterials, scene->mNumMeshes);
        for (size_t j = 0; j < scene->mNumMaterials; j++)
        {
            Material jopmaterial;

            aiMaterial &aiMat = *scene->mMaterials[j];

            //reflections
            aiMat.Get(AI_MATKEY_COLOR_AMBIENT, col);
            jopmaterial.pushReflections(jopmaterial, col, 0);
            aiMat.Get(AI_MATKEY_COLOR_DIFFUSE, col);
            jopmaterial.pushReflections(jopmaterial, col, 1);
            aiMat.Get(AI_MATKEY_COLOR_SPECULAR, col);
            jopmaterial.pushReflections(jopmaterial, col, 2);
            aiMat.Get(AI_MATKEY_COLOR_EMISSIVE, col);
            jopmaterial.pushReflections(jopmaterial, col, 3);


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

            auto textureExists = [](const std::unordered_map<std::string, Texture>& map, const std::string& path)
            {
                return map.find(path) != map.end();
            };

            Texture joptexture;

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
                    joptexture.m_wrapmode = static_cast<int>(jop::TextureSampler::Repeat::Basic);
                    break;
                case aiTextureMapMode_Clamp:
                    joptexture.m_wrapmode = static_cast<int>(jop::TextureSampler::Repeat::ClampEdge);
                    break;
                case aiTextureMapMode_Decal:
                    joptexture.m_wrapmode = static_cast<int>(jop::TextureSampler::Repeat::ClampBorder);
                    break;
                case aiTextureMapMode_Mirror:
                    joptexture.m_wrapmode = static_cast<int>(jop::TextureSampler::Repeat::Mirrored);
                }
            };

            //Types
            {
                aiString path;
                joptexture.m_texStart = m_binaryWriter;

                // Diffuse
                if (aiMat.GetTextureCount(aiTextureType_DIFFUSE))
                {
                    aiMat.GetTexture(aiTextureType_DIFFUSE, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {

                        joptexture.m_texturePath = getTexture(jopmaterial, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Diffuse);
                        joptexture.m_srgb = true;
                    }
                }

                // Specular
                if (aiMat.GetTextureCount(aiTextureType_SPECULAR))
                {
                    aiMat.GetTexture(aiTextureType_SPECULAR, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {
                        joptexture.m_texturePath = getTexture(jopmaterial, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Specular);
                    }
                }

                // Gloss
                if (aiMat.GetTextureCount(aiTextureType_SHININESS))
                {
                    aiMat.GetTexture(aiTextureType_SHININESS, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {
                        joptexture.m_texturePath = getTexture(jopmaterial, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Gloss);
                    }
                }

                // Emission
                if (aiMat.GetTextureCount(aiTextureType_EMISSIVE))
                {
                    aiMat.GetTexture(aiTextureType_EMISSIVE, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {
                        joptexture.m_texturePath = getTexture(jopmaterial, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Emission);
                        joptexture.m_srgb = true;
                    }
                }

                // Reflection
                if (aiMat.GetTextureCount(aiTextureType_REFLECTION))
                {
                    aiMat.GetTexture(aiTextureType_REFLECTION, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {
                        joptexture.m_texturePath = getTexture(jopmaterial, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Reflection);
                    }
                }

                // Opacity
                if (aiMat.GetTextureCount(aiTextureType_OPACITY))
                {
                    aiMat.GetTexture(aiTextureType_OPACITY, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {
                        joptexture.m_texturePath = getTexture(jopmaterial, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Opacity);
                    }
                }

                if (aiMat.GetTextureCount(aiTextureType_UNKNOWN))
                {
                    for (std::size_t i = 0; i < aiMat.GetTextureCount(aiTextureType_UNKNOWN); ++i)
                    {
                        aiMat.GetTexture(aiTextureType_UNKNOWN, i, &path);
                        if (path.length && !textureExists(m_textures, path.C_Str()))
                        {
                            using A = jop::Material::Attribute;
                            using M = jop::Material::Map;
                            M map;

                            // Diffuse
                            if (strstr(path.C_Str(), "dif"))
                                map = M::Diffuse;

                            // Specular
                            else if (strstr(path.C_Str(), "spec"))
                                map = M::Specular;

                            // Emission
                            else if (strstr(path.C_Str(), "emis"))
                                map = M::Emission;

                            // Reflection
                            else if (strstr(path.C_Str(), "refl"))
                                map = M::Reflection;

                            // Opacity
                            else if (strstr(path.C_Str(), "opa") || strstr(path.C_Str(), "alp"))
                                map = M::Opacity;

                            // Gloss
                            else if (strstr(path.C_Str(), "glo"))
                                map = M::Gloss;

                            // Not identified
                            else
                                continue;

                            joptexture.m_texturePath = getTexture(jopmaterial, path.C_Str());
                            joptexture.m_type = static_cast<int>(map);
                        }
                    }
                }
                joptexture.m_texLength = m_binaryLastSize;
                if (!joptexture.m_texturePath.empty() && std::strcmp(path.C_Str(), ""))
                {
                    m_textures[path.C_Str()] = joptexture;
                    jopmaterial.m_keypairs.emplace_back(joptexture.m_texturePath, joptexture.m_type);
                }
            }
            model.m_materials.push_back(jopmaterial);
        }
    }

    void Converter::getMeshes(const aiScene* scene, Model& model)
    {
        unsigned int totalSize = m_binaryWriter;

        for (size_t j = 0; j < scene->mNumMeshes; ++j)
        {
            Mesh jopmesh;
            jopmesh.m_meshStart = totalSize;
            aiMesh* mesh = scene->mMeshes[j];
            if (!mesh->mVertices)
            {
                continue;
            }

            const unsigned int vertexSize = (sizeof(glm::vec3) +
                sizeof(glm::vec2) * mesh->HasTextureCoords(0) +
                sizeof(glm::vec3) * mesh->HasNormals() +
                sizeof(glm::vec3) * mesh->HasTangentsAndBitangents() * 2 +
                sizeof(jop::Color)     * mesh->HasVertexColors(0)
                );

            jopmesh.m_vertexSize = vertexSize;

            //VERTICES
            jopmesh.m_vertexBuffer.resize(vertexSize * mesh->mNumVertices);

            unsigned int meshSize = 0;

            for (size_t k = 0, vertIndex = 0; k < mesh->mNumVertices; ++k)
            {
                //Material index
                jopmesh.m_matIndex = mesh->mMaterialIndex;

                // Position
                {
                    auto& pos = mesh->mVertices[k];
                    reinterpret_cast<glm::vec3&>(jopmesh.m_vertexBuffer[vertIndex]) = glm::vec3(pos.x, pos.y, pos.z);

                    auto& bb = jopmesh.m_localBB;
                    bb = std::make_pair(
                        glm::vec3(std::min(bb.first.x, pos.x), std::min(bb.first.y, pos.y), std::min(bb.first.z, pos.z)),
                        glm::vec3(std::max(bb.second.x, pos.x), std::max(bb.second.y, pos.y), std::max(bb.second.z, pos.z))
                        );

                    m_globalBB = std::make_pair(
                        glm::vec3(std::min(m_globalBB.first.x, pos.x), std::min(m_globalBB.first.y, pos.y), std::min(m_globalBB.first.z, pos.z)),
                        glm::vec3(std::max(m_globalBB.second.x, pos.x), std::max(m_globalBB.second.y, pos.y), std::max(m_globalBB.second.z, pos.z))
                        );


                    vertIndex += sizeof(glm::vec3);
                    meshSize += sizeof(glm::vec3);
                }

                // Tex coordinates
                if (mesh->HasTextureCoords(0))
                {
                    auto& tc = mesh->mTextureCoords[0][k];
                    reinterpret_cast<glm::vec2&>(jopmesh.m_vertexBuffer[vertIndex]) = glm::vec2(tc.x, tc.y);
                    vertIndex += sizeof(glm::vec2);
                    meshSize += sizeof(glm::vec2);
                }

                // Normal
                if (mesh->HasNormals())
                {
                    auto& norm = mesh->mNormals[k];
                    reinterpret_cast<glm::vec3&>(jopmesh.m_vertexBuffer[vertIndex]) = glm::vec3(norm.x, norm.y, norm.z);
                    vertIndex += sizeof(glm::vec3);
                    meshSize += sizeof(glm::vec3);
                }

                // Tangents
                if (mesh->HasTangentsAndBitangents())
                {
                    auto& tang = mesh->mTangents[k], bitang = mesh->mBitangents[k];
                    reinterpret_cast<glm::vec3&>(jopmesh.m_vertexBuffer[vertIndex]) = glm::vec3(tang.x, tang.y, tang.z);
                    vertIndex += sizeof(glm::vec3);
                    meshSize += sizeof(glm::vec3);

                    reinterpret_cast<glm::vec3&>(jopmesh.m_vertexBuffer[vertIndex]) = glm::vec3(bitang.x, bitang.y, bitang.z);
                    vertIndex += sizeof(glm::vec3);
                    meshSize += sizeof(glm::vec3);
                }

                struct Color
                {
                    float r, g, b, a;
                };

                if (mesh->HasVertexColors(0))
                {
                    auto& col = mesh->mColors[0][k];
                    reinterpret_cast<Color&>(jopmesh.m_vertexBuffer[vertIndex]) = Color{ col.r, col.g, col.b, col.a };
                    vertIndex += sizeof(Color);
                    meshSize += sizeof(Color);
                }
            }
            jopmesh.m_meshLength = meshSize;
            totalSize += meshSize;


            // Indices
            const unsigned int elemSize = mesh->mNumVertices <= 254 ? 1 : mesh->mNumVertices <= 0xFFFE ? 2 : 4;
            if (mesh->HasFaces())
            {
                unsigned int indexSize = 0;
                jopmesh.m_meshStartIndex = totalSize;
                jopmesh.m_indexBuffer.resize(elemSize * mesh->mNumFaces * 3);

                std::vector<unsigned int> u(elemSize * 3 * mesh->mNumFaces);

                for (std::size_t j = 0, bufIndex = 0; j < mesh->mNumFaces; ++j, bufIndex += elemSize * 3)
                {
                    auto& face = mesh->mFaces[j];

                    std::memcpy(&u[j * 3], &face.mIndices[0], elemSize * 3);

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
                    indexSize += elemSize * 3;
                }
                totalSize += indexSize;
                jopmesh.m_meshLengthIndex = indexSize;
                jopmesh.m_meshSizeIndex = elemSize;
            }
            //COMPONENTS
            jopmesh.m_components = jop::Mesh::VertexComponent::Position
                | (mesh->HasTextureCoords(0)         * jop::Mesh::VertexComponent::TexCoords)
                | (mesh->HasNormals()                * jop::Mesh::VertexComponent::Normal)
                | (mesh->HasTangentsAndBitangents()  * jop::Mesh::VertexComponent::Tangents)
                | (mesh->HasVertexColors(0)          * jop::Mesh::VertexComponent::Color)
                ;

            model.m_meshes.push_back(jopmesh);
        }

        //MODEL CENTERING
        if (m_centered)
        {
            const glm::vec3 center(
                0.5f * (m_globalBB.first.x + m_globalBB.second.x),
                0.5f * (m_globalBB.first.y + m_globalBB.second.y),
                0.5f * (m_globalBB.first.z + m_globalBB.second.z)
                );

            if (center == glm::vec3(0.f))
                return;

            for (auto& i : model.m_meshes)
            {
                for (unsigned int j = 0; j < i.m_vertexBuffer.size(); j += i.m_vertexSize)
                {
                    reinterpret_cast<glm::vec3&>(i.m_vertexBuffer[j]) -= center;
                }
                i.m_localBB.first -= center;
                i.m_localBB.second -= center;
            }
            m_globalBB.first -= center;
            m_globalBB.second -= center;
        }
    }

    Converter::pathInfo Converter::sortAPath(const std::string& anyPath)
    {
        pathInfo newInfo;

        for (size_t i = 0; i < anyPath.size(); ++i)
        {
            if (anyPath[i] == '/' || anyPath[i] == '\\' || anyPath[i] == './' || anyPath[i] == '.\\')
                newInfo.lastFolder = i;
            else if (anyPath[i] == '.')
                newInfo.lastDot = i;
            else if (anyPath[i] == ':')
                newInfo.fromRoot = true;
        }
        return newInfo;
    }

    std::string Converter::sortPaths(const int& argc, const char* argv[])
    {
        std::string searchLoc = argv[1];
        std::string modelName = argv[1];
        std::string fileOutPath; //can't initialize yet

        std::string tempRoot;

        //execution path
        {
            std::vector<wchar_t> pathBuffer;
            DWORD copied = 0;
            do {
                pathBuffer.resize(pathBuffer.size() + MAX_PATH);
                copied = GetModuleFileNameW(0, &pathBuffer.at(0), pathBuffer.size());
            } while (copied >= pathBuffer.size());

            pathBuffer.resize(copied);
            tempRoot = std::string(pathBuffer.begin(), pathBuffer.end());
            pathInfo info = sortAPath(tempRoot);

            tempRoot.resize(info.lastFolder);

            if (m_verbose)
                std::cout << "Root path after execution path block is: " << tempRoot << std::endl;
        }
        //~execution path

        //argv[1]
        {
            //find out what kind of path we are given
            pathInfo info = sortAPath(searchLoc);

            //given argv[1] doesn't seem to be a file
            if (info.lastDot == -1)
            {
                std::cout << "Unknown parameters: first argument is not a file\n" << std::endl; //fix so that * can be used for recursion
                return ".jopm";
            }

            modelName = modelName.substr(info.lastFolder + 1, info.lastDot - (info.lastFolder + 1)); //name of the model == folder name to create

            //Cut the model file out
            searchLoc.resize(info.lastFolder == -1 ? 0 : info.lastFolder);

            //A directory structure was given but it doesn't start from root --- konv stuff/model.jopm
            if (info.fromRoot == false)
                searchLoc = tempRoot + '\\' + searchLoc;

            //no directory structure was given, starting from working directory --- konv model.jopm
            if (info.lastFolder == -1)
                searchLoc = tempRoot;

            m_searchLoc = searchLoc;

            if (m_verbose)
            {
                std::cout << "Model name after argv[1] block is: " << modelName << std::endl;
                std::cout << "Search location after argv[1] block is: " << searchLoc << std::endl;
            }
        }
        //~argv[1]

        //argv[2]
        {
            if (m_argvNewPath)
            {
                //find out what kind of path we are given
                fileOutPath = argv[2];
                pathInfo info = sortAPath(fileOutPath);

                //It's going to be a .jopm!
                if (info.lastDot != -1)
                    fileOutPath.resize(info.lastDot);

                if (info.fromRoot == false)
                    fileOutPath = tempRoot + '\\' + fileOutPath;
            }

            else
                fileOutPath = searchLoc;

            if (m_verbose)
            {
                std::string isNewPath = m_argvNewPath ? "true" : "false";
                std::cout << "There is a new location to place the converted model: " << isNewPath << std::endl;
                std::cout << "FileOutPath after argv[2] block is: " << fileOutPath << std::endl;
            }

        }
        //~argv[2]

        //Create directory tree
        //////////////////////////////////////////////////Should redesign this
        fileOutPath += '\\';
        std::string tempPath = fileOutPath;
        for (size_t i = 0; i < fileOutPath.size(); ++i)
        {
            if (fileOutPath[i] == '\\')
            {
                tempPath.resize(i);
                _mkdir(tempPath.c_str());
                tempPath = fileOutPath;
            }
        }
        //Create a directory if not embedded, should check whether there are files to copy
        if (!m_embedTex)
            _mkdir((fileOutPath + modelName).c_str());
        m_outputDir = fileOutPath + modelName;
        m_modelName = modelName;
        return m_outputDir + ".jopm";
    }

    bool Converter::sortArgs(const int& argc, const char* argv[])
    {
        if (!std::strcmp(argv[1], "-h") || !std::strcmp(argv[1], "/h") || !std::strcmp(argv[1], "-help") || !std::strcmp(argv[1], "/help"))
        {
            std::cout <<
                "\nJOPMODEL CONVERTER\n"
                "Use this tool to convert your model files to be compatible with the Jopnal engine.\n"
                "See more in \"Jopnal.net\" and \"github.com/Jopnal\"\n\n"
                "ARGUMENTS:\n\n"
                "REQUIRED:\n"
                "First argument: file to load\n\n"
                "OPTIONAL:\n"

                "Second argument: path to write the new model file.\nWrites a file same name as the original model, ending in \".jopm\".\n"
                "Creates required directories.\n"
                "If not specified, creates the file in to the same directory as the original model.\n\n"

                "-et : --embed-textures = embed textures into the model file, default off\n\n"
                "-cn : --no-collapse    = collapse nodes, default on\n\n"
                "-cc : --no-center      = calculate local center of the object for Jopnal engine, default on\n"
                << std::endl;
            return false;
        }

        std::vector<unsigned int> flags;

        for (unsigned int i = 0; i < argc; ++i)
        {
            auto& a = std::string(argv[i]);
            auto& ac = m_argCalls;

            for (unsigned int j = 0; j < ac.size(); ++j)
            {
                if (argComp(a, ac[j] + "et") || argComp(a, ac[j] + "embed-textures"))
                    m_embedTex = true;
                else if (argComp(a, ac[j] + "cn") || argComp(a, ac[j] + "no-collapse"))
                    flags.push_back(aiProcess_OptimizeGraph);
                else if (argComp(a, ac[j] + "cc") || argComp(a, ac[j] + "no-center"))
                    m_centered = false;
                else if (argComp(a, ac[j] + "v") || argComp(a, ac[j] + "verbose"))
                    m_verbose = true;
                else if (i == 2u)
                {
                    std::cout << argv[i] << " triggered newPath" << std::endl;
                    std::string cen = m_centered ? "true" : "false";
                    std::cout << cen << std::endl;
                    m_argvNewPath = true;
                }
                    
            }
        }

        if (m_verbose)
        {
            std::string emb = m_embedTex ? "true" : "false";
            std::string cln = flags.back() == aiProcess_OptimizeGraph ? "true" : "false";
            std::string cen = m_centered ? "true" : "false";

            std::cout << "Using verbose mode.\n" <<
                "Embedded textures: " << emb <<
                "Collapsed nodes: " << cln <<
                "Centered model: " << cen <<
                std::endl;
        }

        std::vector<unsigned int> impArgs = {
            aiProcess_CalcTangentSpace,
            aiProcess_JoinIdenticalVertices,
            aiProcess_OptimizeGraph,
            aiProcess_OptimizeMeshes,
            aiProcess_RemoveComponent,
            aiProcess_RemoveRedundantMaterials,
            aiProcess_SortByPType,
            aiProcess_Triangulate,
            aiProcess_ValidateDataStructure
        };

        //Specified flags
        for (unsigned int i = 0; i < flags.size(); ++i)
        {
            //find from impArgs
            unsigned int pos = std::find(impArgs.begin(), impArgs.end(), flags[i]) - impArgs.begin();
            if (pos < impArgs.size())
            {
                //remove from use
                impArgs.erase(impArgs.begin() + pos);
                flags.erase(flags.begin() + i);
                --i;
            }
        }
        for (auto& i : impArgs)
            m_impArgs |= i;

        return true;
    }

    bool Converter::argComp(std::string& a, std::string& b)
    {
        if (a.length() == b.length())
        {
            return std::equal(b.begin(), b.end(), a.begin(), [](unsigned char _a, unsigned char _b){return std::tolower(_a) == std::tolower(_b); });
        }
        return false;
    }

    int Converter::conversion(const int argc, const char* argv[])
    {
        if (argc > 1)
        {
            //Terminal
            HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            {
                CONSOLE_SCREEN_BUFFER_INFOEX info;
                std::memset(&info, 0, sizeof(info));
                info.cbSize = sizeof(info);

                CONSOLE_FONT_INFOEX font;
                std::memset(&font, 0, sizeof(font));
                font.cbSize = sizeof(font);

                auto consoleSize = GetLargestConsoleWindowSize(consoleHandle);
                consoleSize.X = static_cast<SHORT>(consoleSize.X * 0.9f);
                consoleSize.Y = static_cast<SHORT>(consoleSize.Y * 0.9f);

                SetConsoleScreenBufferSize(consoleHandle, { consoleSize.X, consoleSize.Y *= 9 });
                ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
            }
            //~Terminal

            Converter conv;

            if (!conv.sortArgs(argc, argv))
                return false;

            std::string pathIn = argv[1];
            std::string fileOut = conv.sortPaths(argc, argv);
            if (!std::strcmp(fileOut.c_str(), ".jopm"))
                return false;

            //Setup Assimp
            if (conv.m_verbose)
                Assimp::DefaultLogger::set(new detail::Logger);
            Assimp::Importer imp;
            unsigned int comps = aiComponent_ANIMATIONS | aiComponent_BONEWEIGHTS | aiComponent_CAMERAS | aiComponent_LIGHTS;
            imp.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, comps);
            imp.SetPropertyInteger(AI_CONFIG_GLOB_MEASURE_TIME, 1);

            //read old model file with assimp
            std::cout << "Loading model..." << std::endl;
            const aiScene *scene = imp.ReadFile(pathIn, conv.m_impArgs);
            if (!scene)
            {
                SetConsoleTextAttribute(consoleHandle, 4);
                std::cout << "Unable to load mesh: " << imp.GetErrorString() << std::endl;
                SetConsoleTextAttribute(consoleHandle, 7);
                if (conv.m_verbose)
                    Assimp::DefaultLogger::kill();
                return false;
            }

            SetConsoleTextAttribute(consoleHandle, 7);
            if (conv.m_verbose)
                Assimp::DefaultLogger::kill();
            Model model;

            conv.getMaterials(scene, model);
            conv.getMeshes(scene, model);

            if (conv.jsonWriter(*scene, model, fileOut) && conv.binaryWriter(model, fileOut))
            {
                std::cout << "Model converted successfully\n" << std::endl;
                return true;
            }
            std::cout << "Model conversion failed\n" << std::endl;
            return false;
        }
        else
            std::cout << "Expected an argument, type \"konv -help\" to see instructions." << std::endl;
        return false;
    }
}