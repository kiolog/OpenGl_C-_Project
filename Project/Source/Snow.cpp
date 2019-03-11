#include "../Externals/Include/Include.h"
#include "../Externals/Include/glTools.h"
#include "../Externals/Include/particle.h"

using namespace glm;
using namespace std;
class SnowFall {
	struct {
		GLuint vao;
		GLuint vbo;
		GLuint program;
		GLint um4m;
		GLint um4v;
		GLint um4p;
		GLint tex;

	}snow_program;

	GLuint  snowtex;
	CParticle Snow;
	float x, y, z, vx, vy, vz, ax, ay, az;
	vec3 cameraposition;
	int t1, snow_open;
	Shape *shapes;
	//Material *materials;
	const aiScene *scene;
	void loadModel(string path)
	{
		scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
		//materials = (Material*)malloc(sizeof(Material) * scene->mNumMaterials);
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

			/*glGenBuffers(1, &shape.vbo_texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 2 * sizeof(float), 0, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);*/

			glGenBuffers(1, &shape.vbo_normal);
			glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_normal);
			glBufferData(GL_ARRAY_BUFFER, mesh->mNumVertices * 3 * sizeof(float), 0, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);

			for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
			{
				glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_position);
				glBufferSubData(GL_ARRAY_BUFFER, v * sizeof(float) * 3, sizeof(float) * 3, &mesh->mVertices[v][0]);
				//glBindBuffer(GL_ARRAY_BUFFER, shape.vbo_texcoord);
				//glBufferSubData(GL_ARRAY_BUFFER, v * sizeof(float) * 2, sizeof(float) * 2, &mesh->mTextureCoords[0][v][0]);
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

	//--------- 初始化雪花粒子------------//
	int sky_width = 400;
	bool InitSnow()
	{
		for (int i = 0; i < Snow.GetNumOfParticle(); ++i)
		{
			///初始化坐标
			x = (rand() % sky_width ) - (sky_width / 2.0);
			//y = 50 + 0.1f * (rand() % 10) + (cameraposition.y);
			y = (rand() % 90) ;
			z = (rand() % sky_width) - (sky_width / 2.0);
			Snow.SetPosition(i, x, y, z);
			//printf("(%f, %f, %f)\n", x, y, z);
			///初始化速度
			vx = 0.0001 * (rand() % 100);
			vy = (float)(0.0000004 * (rand() % 28000)) + 0.30;
			vz = 0.0001 * (rand() % 100);
			Snow.SetVelocity(i, vx, vy, vz);

			///初始化加速度
			ax = 0;
			ay = 0.000005f;
			az = 0;
			Snow.SetAcceleration(i, ax, ay, az);

		}
		return true;
	}

	//----------------- 更新粒子------------ //
	void UpdateSnow()
	{
		//更新位置 
		x += (vx * 5);
		y -= vy;
		z += (vz * 5);

		// 更新速度 
		vy += ay;

		//如果粒子消失或生命结束 
		if (y <= -3 )
		{
			// 初始化位置 
			x = (rand() % sky_width) - (sky_width / 2.0);
			y =  (rand() % 60) + 100;
			z = (rand() % sky_width) - (sky_width / 2.0);
			//printf("%f, %f, %f\n", cameraposition.x, cameraposition.y, cameraposition.z);
			//初始化速度 
			vx = (float)(0.0001 * (rand() % 100));
			vy = (float)(0.0000004 * (rand() % 28000)) + 0.30;
			vz = 0.0001 * (rand() % 100);

			// 初始化加速度 
			ax = 0;
			ay = 0.000005f;
			az = 0;

		}
	}
public:
	void DrawParticle(mat4 view, mat4 projection)
	{
		if (snow_open)
		{
			glUseProgram(snow_program.program);
			glUniformMatrix4fv(snow_program.um4v, 1, GL_FALSE, value_ptr(view));
			glUniformMatrix4fv(snow_program.um4p, 1, GL_FALSE, value_ptr(projection));
			glBindVertexArray(snow_program.vao);
			mat4 model;
			for (int i = 0; i < Snow.GetNumOfParticle(); ++i)
			{
				// 获得粒子的所有属性 
				Snow.GetAll(i, x, y, z, vx, vy, vz, ax, ay, az);
				model = translate(mat4(), vec3(x, y, z)); //* rotate(mat4(), radians(move), vec3(0.0, 1.0, 0.0));
				glUniformMatrix4fv(snow_program.um4m, 1, GL_FALSE, value_ptr(model));
				for (int i = 0; i < t1; ++i)
				{
					glBindVertexArray(shapes[i].vao);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapes[i].ibo);
					glDrawElements(GL_TRIANGLES, shapes[i].drawCount, GL_UNSIGNED_INT, 0);
				}
				UpdateSnow();
				Snow.SetAll(i, x, y, z, vx, vy, vz, ax, ay, az);
			}
		}
	}

	void InitSnowProgram()
	{
		snow_program.program = create_program("snow_vertex.glsl", "snow_fragment.glsl");
		snow_program.um4p = glGetUniformLocation(snow_program.program, "um4p");
		snow_program.um4m = glGetUniformLocation(snow_program.program, "um4m");
		snow_program.um4v = glGetUniformLocation(snow_program.program, "um4v");
		snow_program.tex = glGetUniformLocation(snow_program.program, "tex");

		Snow.Create(3000);
		//初始化粒子 
		InitSnow();
		loadModel("Sphere.obj");
		
	}
	void setCamera(vec3 camera)
	{
		cameraposition = camera;
	}
	void OpenSnow()
	{
		snow_open = 1;
	}
	void CloseSnow()
	{
		snow_open = 0;
	}
};