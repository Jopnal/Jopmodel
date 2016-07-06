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
    Converter::Converter()
    {
    }

    Converter::~Converter()
    {
    }

    bool Converter::binaryWriter(Model& model, const std::string fileOut)
    {
        std::ofstream writeFile(fileOut, std::ios::binary | std::ios::out | std::ios::app | std::ios::ate);
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

    bool Converter::processNode(aiNode& node, std::vector<Mesh>& meshes, std::vector<Material>& mats, rapidjson::Value::AllocatorType& alloc, rapidjson::Value& rootChild, rapidjson::Value*& out)
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
            positionArray.PushBack(scale.x, alloc);
            positionArray.PushBack(scale.y, alloc);
            positionArray.PushBack(scale.z, alloc);
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

        if (!processNode(parentNode, meshes, mats, alloc, root, out))
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

    bool Converter::jsonWriter(const aiScene& scene, Model& model, const std::string fileOut)
    {
        namespace rj = rapidjson;

        rj::Document modeldoc;
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

            modelObject.AddMember(rj::StringRef("material"), i.m_matIndex, modeldoc.GetAllocator())["material"];
            modelObject.AddMember(rj::StringRef("type"), i.m_type, modeldoc.GetAllocator())["type"];
            modelObject.AddMember(rj::StringRef("components"), i.m_components, modeldoc.GetAllocator())["components"];
            modelObject.AddMember(rj::StringRef("start"), i.m_start, modeldoc.GetAllocator())["start"];
            modelObject.AddMember(rj::StringRef("length"), i.m_length, modeldoc.GetAllocator())["length"];
            modelObject.AddMember(rj::StringRef("startIndex"), i.m_startIndex, modeldoc.GetAllocator())["startIndex"];
            modelObject.AddMember(rj::StringRef("lengthIndex"), i.m_lengthIndex, modeldoc.GetAllocator())["lengthIndex"];
            modelObject.AddMember(rj::StringRef("sizeIndex"), i.m_sizeIndex, modeldoc.GetAllocator())["sizeIndex"];
        }

        //        auto& root = modeldoc.AddMember(rj::StringRef("nodes"), rj::kArrayType, modeldoc.GetAllocator())["nodes"];

        scene.mRootNode->mName = "rootnode";
        makeNodes(*scene.mRootNode, model.m_meshes, model.m_materials, modeldoc.GetAllocator(), modeldoc);


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

    void Converter::pushReflections(Material &jopmat, const aiColor3D col, const int refTypeIndex)
    {
        jopmat.m_reflections[refTypeIndex * 4 + 0] = col.r;
        jopmat.m_reflections[refTypeIndex * 4 + 1] = col.g;
        jopmat.m_reflections[refTypeIndex * 4 + 2] = col.b;
        jopmat.m_reflections[refTypeIndex * 4 + 3] = 1.0;
    }

    std::string Converter::getTexture(const char* argv[], const std::string texPath)
    {
        std::string root = argv[0];
        std::string searchLoc = argv[1];
        std::string newFolder;
        std::string exclude;
        std::string texLoc;
        std::string textureName = texPath;
        //texLoc =/= textureName because needs to have an empty string
        // .empty() fails otherwise
        // can't compare because what if same name / path on both

        int texFolder = 0;
        int lastFolder = -1;
        int lastDot = -1;

        for (size_t i = 0; i < texPath.size(); ++i)
        {
            if (texPath[i] == '/' || texPath[i] == '\\' || texPath[i] == './' || texPath[i] == '.\\')
                texFolder = i;
        }
        textureName = textureName.substr(texFolder + 1, textureName.size()); //works


        for (size_t i = 0; i < searchLoc.size(); ++i)
        {
            if (searchLoc[i] == '/' || searchLoc[i] == '\\' || searchLoc[i] == './' || searchLoc[i] == '.\\')
                lastFolder = i;
            else if (searchLoc[i] == '.')
                lastDot = i;
        }
        if (lastDot == -1)
        {
            printf("Unknown parameters");
            return texLoc;
        }

        exclude = searchLoc.substr(lastFolder + 1, lastDot - (lastFolder + 1)); //folder name to create

        if (lastFolder != -1)
        {
            searchLoc.resize(lastFolder);                                           //path to base model
        }

        else
        {
            for (size_t i = 0; i < root.size(); ++i)
            {
                if (root[i] == '/' || root[i] == '\\' || root[i] == './' || root[i] == '.\\')
                    lastFolder = i;
            }
            root.resize(lastFolder);
            searchLoc = root;
        }

        newFolder = searchLoc + '\\' + exclude;

        //Check quickly if there is already a correct folder...
        DIR *dir;
        struct dirent *ent;
        bool foundTex = false;
        if ((dir = opendir(newFolder.c_str())) != NULL)
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
                            texLoc = newFolder + '\\' + textureName;
                            foundTex = true;
                            break;
                        }
                    }
                }
            }
        }

        //...if not, go find the texture location
        if (!foundTex)
        {
            //The Search
            texLoc = findTexture(searchLoc, textureName);


            if (!texLoc.empty())
            {
                _mkdir(newFolder.c_str());

                std::ifstream src(texLoc, std::ios::binary);
                std::ofstream dest(newFolder + '\\' + textureName, std::ios::binary);
                dest << src.rdbuf();
                src.close();
                dest.close();

                //needs moar safetychecks
            }
            else
            {
                //jop_error
                printf("Failed to find model\n");
            }
        }

        //Changing the path to be compatible with the Jopnal engine
        for (size_t i = 0; i < texLoc.size(); ++i)
        {
            if (texLoc[i] == '\\' || texLoc[i] == './' || texLoc[i] == '.\\')
            {
                texLoc[i] = '/'; //this ok?
            }
        }
        return texLoc;
    }

    std::string Converter::findTexture(const std::string searchDir, const std::string texName)
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

    void Converter::getMaterials(const aiScene* scene, Model& model, const char* argv[])
    {
        aiColor3D col;

        printf("Materials: %d\nMeshes: %d\n", scene->mNumMaterials, scene->mNumMeshes);
        for (size_t j = 0; j < scene->mNumMaterials; j++)
        {
            Material jopmaterial;

            aiMaterial &aiMat = *scene->mMaterials[j];

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

            //srgb && mipmaps?

            //Types
            {
                // Diffuse
                if (aiMat.GetTextureCount(aiTextureType_DIFFUSE))
                {
                    aiString path;
                    aiMat.GetTexture(aiTextureType_DIFFUSE, 0, &path);

                    if (path.length)
                    {
                        joptexture.m_texturePath = getTexture(argv, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Diffuse);
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
                        joptexture.m_texturePath = getTexture(argv, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Specular);
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
                        joptexture.m_texturePath = getTexture(argv, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Gloss);
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
                        joptexture.m_texturePath = getTexture(argv, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Emission);
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
                        joptexture.m_texturePath = getTexture(argv, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Reflection);
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
                        joptexture.m_texturePath = getTexture(argv, path.C_Str());
                        joptexture.m_type = static_cast<int>(jop::Material::Map::Opacity);
                        jopmaterial.m_textures.push_back(joptexture);
                    }
                }

                if (aiMat.GetTextureCount(aiTextureType_UNKNOWN))
                {
                    for (std::size_t i = 0; i < aiMat.GetTextureCount(aiTextureType_UNKNOWN); ++i)
                    {
                        aiString path;
                        aiMat.GetTexture(aiTextureType_UNKNOWN, i, &path);

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

                        joptexture.m_texturePath = getTexture(argv, path.C_Str());
                        joptexture.m_type = static_cast<int>(map);
                        jopmaterial.m_textures.push_back(joptexture);
                    }
                }
            }
            model.m_materials.push_back(jopmaterial);
        }
    }

    void Converter::getMeshes(const aiScene* scene, Model& model)
    {
        unsigned int totalSize = 0;

        for (size_t j = 0; j < scene->mNumMeshes; ++j)
        {
            Mesh jopmesh;
            jopmesh.m_start = totalSize;
            aiMesh* mesh = scene->mMeshes[j];
            if (!mesh->mVertices)
            {
                continue;
            }

            //VERTICES
            jopmesh.m_vertexBuffer.resize
                (
                (sizeof(glm::vec3) +
                sizeof(glm::vec2) * mesh->HasTextureCoords(0) +
                sizeof(glm::vec3) * mesh->HasNormals() +
                sizeof(glm::vec3) * mesh->HasTangentsAndBitangents() * 2 +
                sizeof(jop::Color)     * mesh->HasVertexColors(0)
                )
                * mesh->mNumVertices
                );

            unsigned int meshSize = 0;

            for (size_t k = 0, vertIndex = 0; k < mesh->mNumVertices; ++k)
            {
                //Material index
                jopmesh.m_matIndex = mesh->mMaterialIndex;

                // Position
                {
                    auto& pos = mesh->mVertices[k];
                    reinterpret_cast<glm::vec3&>(jopmesh.m_vertexBuffer[vertIndex]) = glm::vec3(pos.x, pos.y, pos.z);
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
            jopmesh.m_length = meshSize;
            totalSize += meshSize;


            // Indices
            const unsigned int elemSize = mesh->mNumVertices <= 254 ? 1 : mesh->mNumVertices <= 0xFFFE ? 2 : 4;
            if (mesh->HasFaces())
            {
                unsigned int indexSize = 0;
                jopmesh.m_startIndex = totalSize;
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
                jopmesh.m_lengthIndex = indexSize;
                jopmesh.m_sizeIndex = elemSize;
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
    }

    int Converter::conversion(const int argc, const char* argv[])
    {
        if (argc > 1)
        {
            std::string pathIn = argv[1];
            Converter conv;
            std::string fileOut;

            if (argv[1] == "-h" || argv[1] == "/h" || argv[1] == "-help" || argv[1] == "/help")
            {
                printf("konv:\n First argument: file to load\n(Optional) Second argument: filename to write out");
                return 0;
            }

            if (argc > 2)
            {
                fileOut = argv[2];
                fileOut += ".jop";
            }
            else
            {
                fileOut = "model.jop";
            }


            //read old model file with assimp

            Assimp::DefaultLogger::create("LOKI.txt", Assimp::DefaultLogger::VERBOSE); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
            Assimp::DefaultLogger::get()->setLogSeverity(Assimp::DefaultLogger::LogSeverity::VERBOSE);




            Assimp::Importer imp;
            printf("Loading model...\n");
            unsigned int kek = aiComponent_ANIMATIONS | aiComponent_BONEWEIGHTS | aiComponent_CAMERAS | aiComponent_LIGHTS;
            imp.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, kek);
            const aiScene *scene = imp.ReadFile(pathIn, aiProcess_RemoveComponent | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType /*| aiProcess_OptimizeGraph*/ | aiProcess_RemoveRedundantMaterials | aiProcess_ValidateDataStructure);
            if (!scene) {
                printf("Unable to load mesh: %s\n", imp.GetErrorString());
                return false;
            }

            Model model;


            conv.getMaterials(scene, model, argv);
            conv.getMeshes(scene, model);
            if (conv.jsonWriter(*scene, model, fileOut) && conv.binaryWriter(model, fileOut))
            {
                printf("Model converted successfully\n");
                return 0;
            }
            printf("Model conversion failed\n");
        }
        return 1;
    }

}