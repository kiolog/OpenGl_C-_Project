#pragma once

#ifndef MYMESH_H
#define MYMESH_H

#include"../Include.h"
#include "GameObject.h"
#include<vector>
#include<string>

using namespace std;
using namespace glm;

class Mesh:public GameObject
{
public:
	Mesh(string _strFileName, string _strDirectory) {
		printf("Start Import Mesh\n");
		const aiScene *scene = aiImportFile(_strFileName.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		if (scene == NULL) {
			printf("Load File Error!!\n");
		}
		m_strDirectory = _strDirectory;
		InitMesh(scene);
		printf("Finished Import Mesh\n");
		InitMaterial(scene);
		aiReleaseImport(scene);
	}
	Mesh();
	void Render() override{
		for (int i = 0; i < m_iMeshCount; ++i) {
			Shape TraceShape = m_ListShape[i];
			Material TraceMaterial = m_ListMaterial[TraceShape.m_iMaterialID];
			glBindVertexArray(TraceShape.m_iVao);
			//glBindBuffer(GL_ARRAY_BUFFER, 6);
			//glBindTexture(GL_TEXTURE_2D, _Bone->GetTextureType());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_ListMaterial[TraceShape.m_iMaterialID].m_iDiffuse);
			//printf("TextureID : %d\n", _Bone->GetTextureType());

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TraceShape.m_iIndexElement);

			glDrawElements(GL_TRIANGLES, TraceShape.drawCount, GL_UNSIGNED_INT, 0);
		}
	}
private:
	struct Shape
	{
		GLuint m_iVao;
		GLuint m_iPosition;
		GLuint m_iNormal;
		GLuint m_iTexcoord;
		GLuint m_iIndexElement;
		int drawCount;
		int m_iMaterialID;
	};
	struct Material
	{
		GLuint m_iDiffuse;
		vec3 m_v3Ka;
		vec3 m_v3Kd;
		vec3 m_v3Ks;
		float m_fNs;

	};
	void InitMesh(const aiScene *_Scene) {
		m_iMeshCount = _Scene->mNumMeshes;
		int iListCount = _Scene->mNumMeshes;
		//printf("Mesh Number : %d\n", iListCount);
		for (unsigned int i = 0; i < iListCount; ++i)
		{
			//printf("%d\n", i);
			vector<float> ListVertex;
			vector<float> ListTextureCoord;
			vector<float> ListNormal;
			vector<int> ListIndex;
			aiMesh *TraceMesh = _Scene->mMeshes[i];
			Shape NewShape;
			glGenVertexArrays(1, &NewShape.m_iVao);
			glBindVertexArray(NewShape.m_iVao);
			// create 3 vbos to hold data
			int iListCount = TraceMesh->mNumVertices;
			//printf("Vertex Number : %d\n", iListCount);
			//printf("Has Texture Coord ? : %d\n", TraceMesh->HasTextureCoords(0));
			for (unsigned int j = 0; j < iListCount; ++j)
			{
				ListVertex.push_back(TraceMesh->mVertices[j].x);
				ListVertex.push_back(TraceMesh->mVertices[j].y);
				ListVertex.push_back(TraceMesh->mVertices[j].z);

				if (TraceMesh->HasTextureCoords(0)) {
					ListTextureCoord.push_back(TraceMesh->mTextureCoords[0][j].x);
					ListTextureCoord.push_back(TraceMesh->mTextureCoords[0][j].y);

					//printf("TextureCoord : (%f, %f)\n", TraceMesh->mTextureCoords[0][j].x, TraceMesh->mTextureCoords[0][j].y);
				}
				else {
					ListTextureCoord.push_back(0);
					ListTextureCoord.push_back(0);
				}
				//printf("Trace Mesh TextureCoord Size : %d\n", sizeof(TraceMesh->mTextureCoords[3]));
				//ListTextureCoord.push_back(TraceMesh->mTextureCoords[0][j].x);
				//ListTextureCoord.push_back(TraceMesh->mTextureCoords[0][j].y);

				ListNormal.push_back(TraceMesh->mNormals[j].x);
				ListNormal.push_back(TraceMesh->mNormals[j].y);
				ListNormal.push_back(TraceMesh->mNormals[j].z);
			}
			//printf("OKOK\n");
			// create 1 ibo to hold data
			iListCount = TraceMesh->mNumFaces;
			for (unsigned int j = 0; j < iListCount; ++j)
			{
				// mesh->mFaces[f].mIndices[0~2] => index
				ListIndex.push_back(TraceMesh->mFaces[j].mIndices[0]);
				ListIndex.push_back(TraceMesh->mFaces[j].mIndices[1]);
				ListIndex.push_back(TraceMesh->mFaces[j].mIndices[2]);
			}
			// glVertexAttribPointer / glEnableVertexArray calls¡K
			NewShape.m_iMaterialID = TraceMesh->mMaterialIndex;
			NewShape.drawCount = TraceMesh->mNumFaces * 3;
			// Bind Data
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glGenBuffers(1, &NewShape.m_iPosition);
			glBindBuffer(GL_ARRAY_BUFFER, NewShape.m_iPosition);
			glBufferData(GL_ARRAY_BUFFER, ListVertex.size() * sizeof(float), ListVertex.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glGenBuffers(1, &NewShape.m_iNormal);
			glBindBuffer(GL_ARRAY_BUFFER, NewShape.m_iNormal);
			glBufferData(GL_ARRAY_BUFFER, ListNormal.size() * sizeof(float), ListNormal.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

			glGenBuffers(1, &NewShape.m_iTexcoord);
			glBindBuffer(GL_ARRAY_BUFFER, NewShape.m_iTexcoord);
			glBufferData(GL_ARRAY_BUFFER, ListTextureCoord.size() * sizeof(float), ListTextureCoord.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);

			glGenBuffers(1, &NewShape.m_iIndexElement);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NewShape.m_iIndexElement);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				ListIndex.size() * sizeof(float), ListIndex.data(), GL_STATIC_DRAW);

			m_ListShape.push_back(NewShape);
		}
	}
	void InitMaterial(const aiScene *_Scene) {
		TextureData NewTexture = loadPNG("../Scene/DefaultTexture.png");
		glGenTextures(1, &m_iDefaultImage);
		glBindTexture(GL_TEXTURE_2D, m_iDefaultImage);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, NewTexture.width, NewTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NewTexture.data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);


		//printf("%d %d\n", NewTexture.width, NewTexture.height);
		int iListCount = _Scene->mNumMaterials;
		for (unsigned int i = 0; i < iListCount; ++i)
		{
			aiMaterial *TraceMaterial = _Scene->mMaterials[i];
			Material NewMaterial;
			aiString strTexturePath;

			if (TraceMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &strTexturePath) ==
				aiReturn_SUCCESS)
			{
				string strFilePath = m_strDirectory + string(strTexturePath.C_Str());
				//printf("File Path %s\n", strFilePath.c_str());
				// load width, height and data from texturePath.C_Str();
				NewTexture = loadPNG(strFilePath.c_str());
				glGenTextures(1, &NewMaterial.m_iDiffuse);
				glBindTexture(GL_TEXTURE_2D, NewMaterial.m_iDiffuse);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, NewTexture.width, NewTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NewTexture.data);
				//printf("Image Size : %d,%d\n", NewTexture.width, NewTexture.height);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glGenerateMipmap(GL_TEXTURE_2D);

			}
			else
			{
				// load some default image as default_diffuse_tex
				NewMaterial.m_iDiffuse = m_iDefaultImage;
			}

			m_ListMaterial.push_back(NewMaterial);
		}
	}
	
	vector<Shape> m_ListShape;
	vector<Material> m_ListMaterial;
	GLuint m_iDefaultImage;
	string m_strDirectory;
	int m_iMeshCount = 0;
	int m_iKa;
	int m_iKd;
};

#endif // !MESH_H