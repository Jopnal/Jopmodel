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
    unsigned int g_binaryWriter = 0u;
    unsigned int g_binaryLastSize = 0u;

    Converter::Converter() :
        m_searchLoc(),
        m_modelName(),
        m_embedTex(false),
        m_centered(true),
        m_globalBB(),
        m_textures()
    {
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

    void Converter::getMaterials(const aiScene* scene, Model& model, FileSystem& fs)
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
                joptexture.m_texStart = g_binaryWriter;

                // Diffuse
                if (aiMat.GetTextureCount(aiTextureType_DIFFUSE))
                {
                    aiMat.GetTexture(aiTextureType_DIFFUSE, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {

                        joptexture.m_texturePath = fs.getTexture(path.C_Str());
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
                        joptexture.m_texturePath = fs.getTexture(path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Specular);
                    }
                }

                // Gloss
                if (aiMat.GetTextureCount(aiTextureType_SHININESS))
                {
                    aiMat.GetTexture(aiTextureType_SHININESS, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {
                        joptexture.m_texturePath = fs.getTexture(path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Gloss);
                    }
                }

                // Emission
                if (aiMat.GetTextureCount(aiTextureType_EMISSIVE))
                {
                    aiMat.GetTexture(aiTextureType_EMISSIVE, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {
                        joptexture.m_texturePath = fs.getTexture(path.C_Str());
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
                        joptexture.m_texturePath = fs.getTexture(path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Reflection);
                    }
                }

                // Opacity
                if (aiMat.GetTextureCount(aiTextureType_OPACITY))
                {
                    aiMat.GetTexture(aiTextureType_OPACITY, 0, &path);

                    if (path.length && !textureExists(m_textures, path.C_Str()))
                    {
                        joptexture.m_texturePath = fs.getTexture(path.C_Str());
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

                            joptexture.m_texturePath = fs.getTexture(path.C_Str());
                            joptexture.m_type = static_cast<int>(map);
                        }
                    }
                }
                joptexture.m_texLength = g_binaryLastSize;
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
        unsigned int totalSize = g_binaryWriter;

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

    unsigned int Converter::processAssimpArgs(const FileSystem& fs)
    {
        std::vector<unsigned int> impArgs = {
            aiProcess_CalcTangentSpace,
            aiProcess_JoinIdenticalVertices,
            aiProcess_OptimizeMeshes,
            aiProcess_RemoveComponent,
            aiProcess_RemoveRedundantMaterials,
            aiProcess_SortByPType,
            aiProcess_Triangulate,
            aiProcess_ValidateDataStructure
        };

        if (fs.m_optimizeGraph)
            impArgs.push_back(aiProcess_OptimizeGraph);

        unsigned int t_impArgs = 0;
        for (auto& i : impArgs)
            t_impArgs |= i;

        return t_impArgs;
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
            FileSystem fs;

            if (!fs.sortArgs(argc, argv))
                return false;

            if (!std::strcmp(fs.sortPaths(argc, argv).c_str(), ".jopm"))
                return false;

            std::string absFile(fs.m_absOutputPath + ".jopm");
            conv.m_modelName = fs.m_modelName;
            conv.m_searchLoc = fs.m_absOutputPath;


            //Setup Assimp
            if (fs.m_verbose)
                Assimp::DefaultLogger::set(new detail::Logger);
            Assimp::Importer imp;
            unsigned int comps = aiComponent_ANIMATIONS | aiComponent_BONEWEIGHTS | aiComponent_CAMERAS | aiComponent_LIGHTS;
            imp.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, comps);
            imp.SetPropertyInteger(AI_CONFIG_GLOB_MEASURE_TIME, 1);

            //read old model file with assimp
            std::cout << "Loading model..." << std::endl;
            const aiScene *scene = imp.ReadFile(std::string(argv[1]), conv.processAssimpArgs(fs));
            if (!scene)
            {
                SetConsoleTextAttribute(consoleHandle, 4u);
                std::cout << "Unable to load mesh: " << imp.GetErrorString() << std::endl;
                SetConsoleTextAttribute(consoleHandle, 7u);
                if (fs.m_verbose)
                    Assimp::DefaultLogger::kill();
                return false;
            }

            SetConsoleTextAttribute(consoleHandle, 7u);
            if (fs.m_verbose)
                Assimp::DefaultLogger::kill();
            Model model;

            conv.getMaterials(scene, model, fs);
            conv.getMeshes(scene, model);

            if (fs.m_verbose)
                std::cout << "json and binary will be written to: " << absFile << std::endl;

            if (conv.jsonWriter(*scene, model, absFile) && conv.binaryWriter(model, absFile))
            {
                std::cout << "Model converted successfully\n" << std::endl;
                return true;
            }
            std::cout << "Model conversion failed\n" << std::endl;
            return false;
        }
        else
            std::cout << "Expected an argument, type \"jopmodel -help\" to see instructions." << std::endl;
        return false;
    }
}