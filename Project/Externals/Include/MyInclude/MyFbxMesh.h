#pragma once

#ifndef MYFBXMESH_H
#define MYFBXMESH_H

#include"../Include.h"
#include "../fbxsdk/fbxsdk.h"
#include <IL/il.h>
#include"GameObject.h"
#include"fbxloader.h"
#include<vector>
#include<string>
using namespace std;
using namespace glm;
using namespace tinyobj;
typedef struct
{
	GLuint vao;
	GLuint vbo;
	GLuint vboTex;
	GLuint ebo;
	int materialId;
	int indexCount;
} MyFbxShape;

typedef struct
{
	GLuint texId;
} MyFbxMaterial;
class MyFbxMesh:public GameObject {
public:
	MyFbxMesh() {
		//printf("Error or not : %d\n", ilLoadImage("D:\GraphicProgramming\Final\Model\zombie_fury.fbm/difusse.jpg"));
	}
	MyFbxMesh(string _strFileName) {
		//bool bValidTexture = ilLoadImage("../Model/Patrik/patrik_skin.jpg");
		//printf("Valid? : %d\n", bValidTexture);
		string err;
		bool bValid = LoadFbx(characterFbx, shapes, materials, err, _strFileName.c_str());
		if (bValid) {
			BindData();
		}
	}
	MyFbxMesh(vector<tinyobj::shape_t> &_shapes, vector<tinyobj::material_t> &_materials, fbx_handles &_characterFbx,float _fPlayedSpeed) {
		characterFbx = _characterFbx;
		shapes = _shapes;
		materials = _materials;
		m_fPlayedSpeed = _fPlayedSpeed;
		BindData();
	}
	void Delete() override {
		for (int i = 0; i < shapes.size(); i++)
		{
			MyFbxShape shape;
			glDeleteVertexArrays(1, &shape.vao);
			glDeleteBuffers(3, &shape.vbo);
		}
		for (int i = 0; i < characterMaterials.size(); i++)
		{
			glDeleteTextures(1, &characterMaterials[i].texId);
		}
	}
	void Render() override {
		timer_cnt += m_fPlayedSpeed;
		//printf("%d\n", timer_cnt);
		vector<shape_t> new_shapes;
		GetFbxAnimation(characterFbx, new_shapes, timer_cnt / 255.f);
		for (unsigned int i = 0; i < characterShapes.size(); ++i)
		{
			glBindVertexArray(characterShapes[i].vao);
			glBindBuffer(GL_ARRAY_BUFFER, characterShapes[i].vbo);
			//printf("Size : %d\n", new_shapes[i].mesh.positions.size());
			glBufferSubData(GL_ARRAY_BUFFER, 0, new_shapes[i].mesh.positions.size() * sizeof(float), new_shapes[i].mesh.positions.data());
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, characterMaterials[characterShapes[i].materialId].texId);
			glDrawElements(GL_TRIANGLES, characterShapes[i].indexCount, GL_UNSIGNED_INT, 0);
		}
	}
	vector<tinyobj::shape_t> GetShape() {
		return shapes;
	}
	vector<tinyobj::material_t> GetMaterial() {
		return materials;
	}
	fbx_handles GetFBX() {
		return characterFbx;
	}

private:
	void BindData() {
		//printf("BindData\n");
		for (int i = 0; i < materials.size(); i++)
		{
			ILuint ilTexName;
			ilGenImages(1, &ilTexName);
			ilBindImage(ilTexName);
			MyFbxMaterial mat;
			//printf("len : %d\n", materials.size());
			printf(materials[i].diffuse_texname.c_str());

			TextureData NewTexture = loadPNG(materials[i].diffuse_texname.c_str());
			if (NewTexture.data != NULL)
			{
				glGenTextures(1, &mat.texId);
				glBindTexture(GL_TEXTURE_2D, mat.texId);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, NewTexture.width, NewTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NewTexture.data);
				glGenerateMipmap(GL_TEXTURE_2D);

				ilDeleteImages(1, &ilTexName);
			}
			else {
				//printf("No Texture!!\n");
			}
			characterMaterials.push_back(mat);
		}

		// For Each Shape (or Mesh, Object)
		for (int i = 0; i < shapes.size(); i++)
		{
			MyFbxShape shape;
			glGenVertexArrays(1, &shape.vao);
			glBindVertexArray(shape.vao);
			glGenBuffers(3, &shape.vbo);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo);
			glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.positions.size() * sizeof(float), shapes[i].mesh.positions.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.normals.size() * sizeof(float), shapes[i].mesh.normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vboTex);
			glBufferData(GL_ARRAY_BUFFER, shapes[i].mesh.texcoords.size() * sizeof(float), shapes[i].mesh.texcoords.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[i].mesh.indices.size() * sizeof(unsigned int), shapes[i].mesh.indices.data(), GL_STATIC_DRAW);
			shape.materialId = shapes[i].mesh.material_ids[0];
			shape.indexCount = shapes[i].mesh.indices.size();
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			characterShapes.push_back(shape);
		}
	}
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	fbx_handles characterFbx;
	vector<MyFbxShape> characterShapes;
	vector<MyFbxMaterial> characterMaterials;
	GLubyte timer_cnt;
	float m_fPlayedSpeed = 1;
};
	
	

#endif