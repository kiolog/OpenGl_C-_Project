//house.cpp
#include "../Externals/Include/Include.h"
#include "utilities.cpp"
using namespace std;
using namespace glm;

class House;


class House 
{
	struct {
		GLuint vao;
		GLuint vbo;
		GLuint program;
		GLint um4m;
		GLint um4v;
		GLint um4p;
		GLint tex;
		GLint snow_open;
	}house_program;
	mat4 house_model = rotate(mat4(), radians(GLfloat(-90.0)), vec3(1.0, 0.0, 0.0)) * scale(mat4(), vec3(3.5,3.5,3.5));
	int open;
	int t1;

	Shape *shapes;
	Material *materials;
	const aiScene *scene;
	void loadModel(string path, string t_path)
	{
		scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		materials = (Material*)malloc(sizeof(Material) * scene->mNumMaterials);
		shapes = (Shape*)malloc(sizeof(Shape) * scene->mNumMeshes);

		t1 = scene->mNumMeshes;
		//printf("Mesh Number : %d\n", t1);
		for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
		{
			aiMaterial *material = scene->mMaterials[i];
			Material mater;
			aiString texturePath(t_path);
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == aiReturn_SUCCESS)
			{
				string str = string(texturePath.C_Str());
				char* fullpath = const_cast<char *>(str.c_str());
				TextureData data = loadPNG(fullpath);
				glGenTextures(1, &(mater.diffuse_tex));
				glBindTexture(GL_TEXTURE_2D, mater.diffuse_tex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glGenerateMipmap(GL_TEXTURE_2D);

			}
			else
			{
				TextureData data = loadPNG("dabrovic-sponza/KAMEN-bump.jpg");
				glGenTextures(1, &(mater.diffuse_tex));
				glBindTexture(GL_TEXTURE_2D, mater.diffuse_tex);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data);
				glGenerateMipmap(GL_TEXTURE_2D);
				printf("tex FAIL, %d\n", i);
			}

			materials[i] = mater;

		}

		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			aiMesh *mesh = scene->mMeshes[i];
			Shape shape;
			glGenVertexArrays(1, &shape.vao);
			glBindVertexArray(shape.vao);

			glGenBuffers(1, &shape.vbo_position);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(float), 0, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);

			glGenBuffers(1, &shape.vbo_texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 2 * sizeof(float), 0, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);

			glGenBuffers(1, &shape.vbo_normal);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(float), 0, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
			{
				glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
				glBufferSubData(GL_ARRAY_BUFFER, v * sizeof(float) * 3, sizeof(float) * 3, &mesh->mVertices[v][0]);
				glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
				glBufferSubData(GL_ARRAY_BUFFER, v * sizeof(float) * 2, sizeof(float) * 2, &mesh->mTextureCoords[0][v][0]);
				glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
				glBufferSubData(GL_ARRAY_BUFFER, v * sizeof(float) * 3, sizeof(float) * 3, &mesh->mNormals[v][0]);
			}

			glGenBuffers(1, &shape.ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->mNumFaces * 3 * sizeof(unsigned int), 0, GL_STATIC_DRAW);
			for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.ibo);
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, f * sizeof(unsigned int) * 3, sizeof(unsigned int) * 3, &mesh->mFaces[f].mIndices[0]);
			}

			shape.materialID = mesh->mMaterialIndex;
			shape.drawCount = mesh->mNumFaces * 3;

			shapes[i] = shape;
		}
		aiReleaseImport(scene);
	}
	
public:
	void InitHouseprogram()
	{
		house_program.program = create_program("obj_vertex.glsl", "obj_fragment.glsl");
		house_program.um4p = glGetUniformLocation(house_program.program, "um4p");
		house_program.um4m = glGetUniformLocation(house_program.program, "um4m");
		house_program.um4v = glGetUniformLocation(house_program.program, "um4v");
		house_program.tex = glGetUniformLocation(house_program.program, "tex");
		house_program.snow_open = glGetUniformLocation(house_program.program, "snow_open");
		loadModel("20958_Log_Cabin_v1_NEW.obj", "20958_Log_Cabin_v1_NEW.mtl");
	}
	void DrawHouse(mat4 _ViewMatrix, mat4 _ProjMatrix) {
		glUseProgram(house_program.program);
		glUniformMatrix4fv(house_program.um4m, 1, GL_FALSE, value_ptr(house_model));
		glUniformMatrix4fv(house_program.um4v, 1, GL_FALSE, value_ptr(_ViewMatrix));
		glUniformMatrix4fv(house_program.um4p, 1, GL_FALSE, value_ptr(_ProjMatrix));
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(house_program.tex, 0);
		if(open)
			glUniform1i(house_program.snow_open, 1);
		else 
			glUniform1i(house_program.snow_open, 0);
		//draw
		for (int i = 0; i < t1; ++i)
		{
			glBindVertexArray(shapes[i].vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[i].ibo);
			int id = shapes[i].materialID;
			glBindTexture(GL_TEXTURE_2D, materials[id].diffuse_tex);
			//glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
			glDrawElementsInstanced(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0, 5);
		}
	}
	void DrawHouse(Camera myCamera)
	{
		glUseProgram(house_program.program);

		glUniformMatrix4fv(house_program.um4m, 1, GL_FALSE, value_ptr(house_model));
		glUniformMatrix4fv(house_program.um4v, 1, GL_FALSE, value_ptr(myCamera.getViewMatrix()));
		glUniformMatrix4fv(house_program.um4p, 1, GL_FALSE, value_ptr(myCamera.getProjectionMatrix()));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(house_program.tex, 0);
		//draw
		for (int i = 0; i < t1; ++i)
		{
			glBindVertexArray(shapes[i].vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[i].ibo);
			int id = shapes[i].materialID;
			glBindTexture(GL_TEXTURE_2D, materials[id].diffuse_tex);
			//glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
			glDrawElementsInstanced(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0, 2);
		}
	}
	
	void OpenSnow()
	{
		open = 1;
		glUniform1i(house_program.snow_open, 1);
	}
	void CloseSnow()
	{
		open = 0;
		glUniform1i(house_program.snow_open, 0);
	}
	
	
};
