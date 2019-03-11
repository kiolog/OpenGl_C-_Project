#include "../Externals/Include/Include.h"
#include "utilities.cpp"

using namespace std;
using namespace glm;
class Lowpolytree;

class Lowpolytree
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
	}tree_program;
	struct {
		GLuint vao;
		GLuint vbo;
		GLuint program;
		GLint um4m;
		GLint um4v;
		GLint um4p;
		GLint tex;
	}Bark_program;
	int tree_num = 17;
	mat4 tree_model = scale(mat4(), vec3(10.0f, 10.0f, 10.0f));
	int open;
	int t1;

	Shape *shapes;
	Material *materials;
	const aiScene *scene;
	void loadModel(string path, string t_path)
	{
		scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		shapes = (Shape*)malloc(sizeof(Shape) * scene->mNumMeshes);
		t1 = scene->mNumMeshes;
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
	void InitTreeprogram()
	{
		tree_program.program = create_program("tree_vertex.glsl", "tree_fragment.glsl");
		tree_program.um4p = glGetUniformLocation(tree_program.program, "um4p");
		tree_program.um4m = glGetUniformLocation(tree_program.program, "um4m");
		tree_program.um4v = glGetUniformLocation(tree_program.program, "um4v");
		tree_program.tex = glGetUniformLocation(tree_program.program, "tex");
		tree_program.snow_open = glGetUniformLocation(tree_program.program, "snow_open");
		Bark_program.program = create_program("bark_vertex.glsl", "bark_fragment.glsl");
		Bark_program.um4p = glGetUniformLocation(Bark_program.program, "um4p");
		Bark_program.um4m = glGetUniformLocation(Bark_program.program, "um4m");
		Bark_program.um4v = glGetUniformLocation(Bark_program.program, "um4v");
		Bark_program.tex = glGetUniformLocation(Bark_program.program, "tex");
		loadModel("lowpolytree.obj", "lowpolytree.mtl");
		
	}
	void DrawTree(mat4 view, mat4 proj)
	{
		glUseProgram(tree_program.program);

		glUniformMatrix4fv(tree_program.um4m, 1, GL_FALSE, value_ptr(tree_model));
		glUniformMatrix4fv(tree_program.um4v, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(tree_program.um4p, 1, GL_FALSE, value_ptr(proj));
		if(open == 1)
			glUniform1i(tree_program.snow_open, 1);
		else
			glUniform1i(tree_program.snow_open, 0);
		//draw
			glBindVertexArray(shapes[0].vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[0].ibo);
			glDrawElementsInstanced(GL_TRIANGLES, shapes[0].drawCount, GL_UNSIGNED_INT, 0, tree_num);
		//draw bark
		glUseProgram(Bark_program.program);

		glUniformMatrix4fv(Bark_program.um4m, 1, GL_FALSE, value_ptr(tree_model));
		glUniformMatrix4fv(Bark_program.um4v, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(Bark_program.um4p, 1, GL_FALSE, value_ptr(proj));

		//draw
			glBindVertexArray(shapes[1].vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[1].ibo);

			glDrawElementsInstanced(GL_TRIANGLES, shapes[1].drawCount, GL_UNSIGNED_INT, 0, tree_num);
	}
	void OpenSnow()
	{
		open = 1;
		glUniform1i(tree_program.snow_open, 1);
	}
	void CloseSnow()
	{
		open = 0;
		glUniform1i(tree_program.snow_open, 0);
	}

};
