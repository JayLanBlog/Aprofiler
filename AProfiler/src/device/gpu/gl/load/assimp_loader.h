#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <device/gpu/gl/render/gl_render.h>

#define MAX_BONE_INFLUENCE 4
namespace APROFILER {

	namespace GL {
		struct Drawable
		{
			Primitive* primitive;
			std::vector<int> textures;
		};
		struct Vertex {
			// position
			glm::vec3 Position;
			// normal
			glm::vec3 Normal;
			// texCoords
			glm::vec2 TexCoords;
			// tangent
			glm::vec3 Tangent;
			// bitangent
			glm::vec3 Bitangent;
			//bone indexes which will influence this vertex
			int m_BoneIDs[MAX_BONE_INFLUENCE];
			//weights from each bone
			float m_Weights[MAX_BONE_INFLUENCE];
		};

		class AssimpProxy {
		public:
			ModelRender* model;
			std::string directory;
			AssimpProxy() {
				model = new ModelRender();
			}
			ModelRender* Load(string const& path) {
				Assimp::Importer importer;

				const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
				// check for errors
				if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
				{
					cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
					return model;
				}
				// retrieve the directory path of the filepath 
				directory = path.substr(0, path.find_last_of('/'));

				ProcessNode(scene->mRootNode, scene);

				return model;
			}

			void ProcessNode(aiNode* node, const aiScene* scene) {
				for (unsigned int i = 0; i < node->mNumMeshes; i++)
				{
					// the node object only contains indices to index the actual objects in the scene. 
					// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
					aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
					//meshes.push_back(processMesh(mesh, scene));
					//model->AddPrimtive();
					Process(mesh, scene);
				}
				// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
				for (unsigned int i = 0; i < node->mNumChildren; i++)
				{
					ProcessNode(node->mChildren[i], scene);
				}
			}

			void Process(aiMesh* mesh, const aiScene* scene) {
				// data to fill
				vector<Vertex> vertices;
				vector<unsigned int> indices;
				vector<Texture> textures;
				for (unsigned int i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex vertex;
					glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
					// positions
					vector.x = mesh->mVertices[i].x;
					vector.y = mesh->mVertices[i].y;
					vector.z = mesh->mVertices[i].z;
					vertex.Position = vector;
					// normals
					if (mesh->HasNormals())
					{
						vector.x = mesh->mNormals[i].x;
						vector.y = mesh->mNormals[i].y;
						vector.z = mesh->mNormals[i].z;
						vertex.Normal = vector;
					}
					// texture coordinates
					if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
					{
						glm::vec2 vec;
						// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
						// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
						vec.x = mesh->mTextureCoords[0][i].x;
						vec.y = mesh->mTextureCoords[0][i].y;
						vertex.TexCoords = vec;
						// tangent
						vector.x = mesh->mTangents[i].x;
						vector.y = mesh->mTangents[i].y;
						vector.z = mesh->mTangents[i].z;
						vertex.Tangent = vector;
						// bitangent
						vector.x = mesh->mBitangents[i].x;
						vector.y = mesh->mBitangents[i].y;
						vector.z = mesh->mBitangents[i].z;
						vertex.Bitangent = vector;
					}
					else
						vertex.TexCoords = glm::vec2(0.0f, 0.0f);

					vertices.push_back(vertex);
				}
				// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
				for (unsigned int i = 0; i < mesh->mNumFaces; i++)
				{
					aiFace face = mesh->mFaces[i];
					// retrieve all indices of the face and store them in the indices vector
					for (unsigned int j = 0; j < face.mNumIndices; j++)
						indices.push_back(face.mIndices[j]);
				}
				// PrimitiveMesh primMesh;
			   //  Primitve* primitve = primMesh.primitve;
				Primitive* primitive = new Primitive();
				primitive->CreateVBO();
				PrimitiveElement* element = new PrimitiveElement();
				element->element_count = 3;
				element->offset = 0;
				element->mStrade = sizeof(Vertex);
				element->mType = ElementType::TYPE_VECTOR3;
				element->sematic = ElementSemantic::SEM_POSITION;
				primitive->AddElement(*element);

				element = new PrimitiveElement();
				element->element_count = 3;
				element->offset = offsetof(Vertex, Normal);
				element->mStrade = sizeof(Vertex);
				element->mType = ElementType::TYPE_VECTOR3;
				element->sematic = ElementSemantic::SEM_NORMAL;
				primitive->AddElement(*element);

				element = new PrimitiveElement();
				element->element_count = 2;
				element->offset = offsetof(Vertex, TexCoords);
				element->mStrade = sizeof(Vertex);
				element->mType = ElementType::TYPE_VECTOR2;
				element->sematic = ElementSemantic::SEM_TEXCOORD;
				primitive->AddElement(*element);

				element = new PrimitiveElement();
				element->element_count = 3;
				element->offset = offsetof(Vertex, Tangent);
				element->mStrade = sizeof(Vertex);
				element->mType = ElementType::TYPE_VECTOR3;
				element->sematic = ElementSemantic::SEM_TANGENT;
				primitive->AddElement(*element);


				element = new PrimitiveElement();
				element->element_count = 3;
				element->offset = offsetof(Vertex, Bitangent);
				element->mStrade = sizeof(Vertex);
				element->mType = ElementType::TYPE_VECTOR3;
				element->sematic = ElementSemantic::SEM_BIGTANGENT;
				primitive->AddElement(*element);
				//TO DO :   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
				//  ¼ÆËãÕâ¸öÆ«ÒÆ(void*)offsetof(Vertex, Normal)
				element = new PrimitiveElement();
				element->element_count = 4;
				element->offset = offsetof(Vertex, m_BoneIDs);
				element->mStrade = sizeof(Vertex);
				element->mType = ElementType::TYPE_INT;
				element->sematic = ElementSemantic::SEM_BONE_ID;
				primitive->AddElement(*element);

				element = new PrimitiveElement();
				element->element_count = 4;
				element->offset = offsetof(Vertex, m_Weights);
				element->mStrade = sizeof(Vertex);
				element->mType = ElementType::TYPE_INT;
				element->sematic = ElementSemantic::SEM_BONE_WEIGHT;
				primitive->AddElement(*element);

				primitive->UploadToVBO(&vertices.data()[0], sizeof(Vertex), vertices.size());

				cout << sizeof(sizeof(vertices[0])) << ":" << sizeof(Vertex) << endl;

				primitive->UploadToIBO(&indices.data()[0], sizeof(unsigned int), indices.size());
				//primitve->vbo->upload_data(vertices.data(), sizeof(Vertex), vertices.size());

				primitive->Make();

				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				vector<int> texturesIds;
				vector<int> diffuseMaps = loadMaterialTextureIds(material, aiTextureType_DIFFUSE, "texture_diffuse");
				texturesIds.insert(texturesIds.end(), diffuseMaps.begin(), diffuseMaps.end());
				vector<int> specularMaps = loadMaterialTextureIds(material, aiTextureType_SPECULAR, "texture_specular");
				texturesIds.insert(texturesIds.end(), specularMaps.begin(), specularMaps.end());
				vector<int> normalMaps = loadMaterialTextureIds(material, aiTextureType_HEIGHT, "texture_normal");
				texturesIds.insert(texturesIds.end(), normalMaps.begin(), normalMaps.end());
				vector<int> heightMaps = loadMaterialTextureIds(material, aiTextureType_AMBIENT, "texture_height");
				texturesIds.insert(texturesIds.end(), heightMaps.begin(), heightMaps.end());
				model->AddPrimtive(primitive, texturesIds);
			}

			vector<int> loadMaterialTextureIds(aiMaterial* mat, aiTextureType type, string typeName) {
				vector<int> textures;
				ResourceManager* resManager = ResourceManager::getResManager();
				for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
					aiString str;
					mat->GetTexture(type, i, &str);
					string fullPath = this->directory + "/" + str.C_Str();
					int id = resManager->load_texture(fullPath, typeName);
					cout << "path : " << fullPath << " , ID : " << id << " ,typeName :" << typeName << endl;
					textures.push_back(id);
				}
				return textures;

			}


		};

	}
}