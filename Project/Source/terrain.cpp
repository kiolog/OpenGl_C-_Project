#ifndef _TERRAIN_
#define _TERRAIN_
#include "../Externals/Include/Include.h"
#include "utilities.cpp"
#include "FastNoise.cpp"
#include <iostream>
#include <iomanip>
// For random
#include <random>
#include <ctime>

using namespace std;
using namespace glm;

class Terrains;
class Area;
class HeightMapGenerator;
class TerrainProgram;


static class TerrainProgram : public Program{
	struct {
		GLuint projectionMatrix;
		GLuint viewMatrix;
		GLuint lightPosition;
		GLuint modelMatrix;
		GLuint Plane;
		GLuint winterFraction;
		GLuint ShadowMatrix;
	}uniforms;
public:
	TerrainProgram(){};
	~TerrainProgram(){};

	void initialize(){
		loadShaders("terrain.vs.glsl","terrain.fs.glsl");
		uniforms.projectionMatrix = glGetUniformLocation(getProgramID(), "projectionMatrix");
		uniforms.viewMatrix		  = glGetUniformLocation(getProgramID(), "viewMatrix");
		uniforms.lightPosition    = glGetUniformLocation(getProgramID(), "lightPosition");
		uniforms.modelMatrix	  = glGetUniformLocation(getProgramID(), "modelMatrix");
		uniforms.Plane 			  = glGetUniformLocation(getProgramID(), "m_v4Plane");
		uniforms.winterFraction	  = glGetUniformLocation(getProgramID(), "winterFraction");
		uniforms.ShadowMatrix = glGetUniformLocation(getProgramID(), "shadow_matrix");
		cout << "Uniform Locations\n";
		cout << "\tprojectionMatrix :" << uniforms.projectionMatrix << endl;
		cout << "\tviewMatrix :" << uniforms.viewMatrix << endl;
		cout << "\tlightPosition :" << uniforms.lightPosition << endl;
		cout << "\tmodelMatrix :"  << uniforms.modelMatrix << endl;
		cout << "\tPlane :" << uniforms.Plane << endl;
	};

	void loadUniforms(Camera& _camera, mat4 modelMatrix,vec3 lightPosition, float winterFraction){
		glUniformMatrix4fv(uniforms.projectionMatrix, 1, GL_FALSE, value_ptr(_camera.getProjectionMatrix()));
		glUniformMatrix4fv(uniforms.viewMatrix, 1, GL_FALSE, value_ptr(_camera.getViewMatrix()));
		glUniformMatrix4fv(uniforms.modelMatrix, 1, GL_FALSE, value_ptr(modelMatrix));
		printf("LightPosition : %d\n", uniforms.lightPosition);
		glUniform3fv(uniforms.lightPosition, 1, value_ptr(lightPosition));
		glUniform1f(uniforms.winterFraction, winterFraction);
	};

	void loadUniforms(mat4 &_ViewMatrix,mat4 &_ProjMatrix, mat4 &modelMatrix,vec4 &_v4Plane,vec3 _v3LightPosition, float winterFraction, mat4 &_ShadowMatrix, GLuint _iShadowMap) {
		glEnable(GL_CLIP_DISTANCE0);
		glUniformMatrix4fv(uniforms.projectionMatrix, 1, GL_FALSE, value_ptr(_ProjMatrix));
		glUniformMatrix4fv(uniforms.viewMatrix, 1, GL_FALSE, value_ptr(_ViewMatrix));
		glUniformMatrix4fv(uniforms.modelMatrix, 1, GL_FALSE, value_ptr(modelMatrix));
		glUniformMatrix4fv(uniforms.ShadowMatrix, 1, GL_FALSE, value_ptr(_ShadowMatrix));
		
		glUniform3fv(uniforms.lightPosition, 1, value_ptr(_v3LightPosition));
		glUniform4fv(uniforms.Plane, 1, value_ptr(_v4Plane));
		glUniform1f(uniforms.winterFraction, winterFraction);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _iShadowMap);
	};
};

class Area
{
	Image areaImage0;
	Image areaImage1;
	Image areaImage2;

public:
	Area(){};
	~Area(){};
	void loadAreaImg(void){
		areaImage0.load("areaImage0_.png", 1, true);
		areaImage1.load("areaImage1_.png", 1, true);
		areaImage2.load("areaImage2_.png", 1, true);
	}
	float getMountain(float x, float y){
		return areaImage0.getValue(x,y,0,true);
	}
	float getRiver(float x, float y){
		return areaImage1.getValue(x,y,0,true);
	}
	float getHill(float x, float y){
		return areaImage2.getValue(x,y,0,true);	
	}
	
};

class HeightMapGenerator{
	FastNoise noiseGenerator;
	Area areaIndicator;
	bool useImage = false;
	float HEIGHEST = 0;
	float LOWEST = 0;
	float VERTEX_COUNT = 0;

	struct {
		bool ini = false;
		float vNum = 0;
		float * raw   = NULL;
		float * final = NULL;
		float getFinalHeight(float x, float y) {
			if (ini) {
				size_t coord = x * vNum + y;
				if (coord < 0 || coord >= (vNum - 1)*(vNum - 1) * 6)
					return 0;
				if(x==(int)x&&y==(int)y)
					return final[coord];
				else {
					float fx = x - floor(x);
					float fy = y - floor(y);
					x = (int)floor(x); y = (int)floor(y);
					float a = getFinalHeight(x  ,y  );
					float b = getFinalHeight(x+1,y  );
					float c = getFinalHeight(x  ,y+1);
					float d = getFinalHeight(x+1,y+1);
					return mix(mix(a,b,fx),mix(c,d,fx),fy);
				}
			}
			else {
				printf("HeightMapGenerator : GetValue from heightData before initialization.\n");
				exit(-1);
			}
		}
		void allocate(float vN) {
			if (!ini){
				vNum = vN;
				raw      = new float [vNum*vNum];
				final 	 = new float [vNum*vNum];
				ini = true;
			}
		}
		void freeFloatPointer(float* p){if(p){free(p);p=NULL;}};
		void freeMemory(void) {
			if (ini) {
				freeFloatPointer(raw);
				freeFloatPointer(final);
				ini = false;
			}
		}
	}heightData;

	struct{
		float mountainHeight = 0;
		float plainHeight = 0;
		float riverDepth = 0;
		float hillHeight = 0;
		bool ini = false;
	}format;

	void copyImageValue(float* target, Image& img){
		for(float i = 0; i < VERTEX_COUNT ; i++)
           	for(float j = 0; j < VERTEX_COUNT ; j++)
           		target[(size_t)(j * VERTEX_COUNT + i)] = img.getValue( i /VERTEX_COUNT, j / VERTEX_COUNT, true);	
	}

	
	void initializeRawHeightData(float seed = 1337){
		if (useImage){
			Image heightImage;
			heightImage.load("heightmap.png", 1, true);
			copyImageValue(heightData.raw, heightImage);
		}else{
			noiseGenerator.SetSeed(seed);
			noiseGenerator.SetFrequency(5 * 1/(float)VERTEX_COUNT);
			noiseGenerator.SetFractalOctaves(3);
			noiseGenerator.SetFractalGain(0.5);
			noiseGenerator.SetFractalLacunarity(2);	
			float minVal = 1000000;
			float maxVal = -1000000;
			for(float i = 0; i < VERTEX_COUNT ; i++){
            	for(float j = 0; j < VERTEX_COUNT ; j++){
					size_t coord = j * VERTEX_COUNT + i;
            		heightData.raw[coord] = noiseGenerator.GetSimplexFractal(i, j);
            		minVal = glm::min(minVal, heightData.raw[coord]);
            		maxVal = glm::max(maxVal, heightData.raw[coord]);
            	}
        	}
	    	maxVal -= minVal;
			for (int i = 0; i < VERTEX_COUNT*VERTEX_COUNT; ++i){
				heightData.raw[i] -= minVal;
				heightData.raw[i] /= maxVal;
			}
		}
	}

	void initializeOtherHeightData(void){
		for(float i = 0; i < VERTEX_COUNT ; i++)
           	for(float j = 0; j < VERTEX_COUNT ; j++){
           		float mtVal = areaIndicator.getMountain(i/VERTEX_COUNT, j/VERTEX_COUNT);
           		float riVal = areaIndicator.getRiver(i/VERTEX_COUNT, j/VERTEX_COUNT);
           		float hiVal = areaIndicator.getHill(i/VERTEX_COUNT, j/VERTEX_COUNT);
				size_t coord = j * VERTEX_COUNT + i;
				float rawVal = heightData.raw[coord];
     			mtVal *= (0.5 + 0.5 * rawVal) * format.mountainHeight;
           		riVal *= (0.2 + 0.8 * rawVal) * (format.riverDepth - 10);
           		hiVal *= (0.6 + 0.4 * rawVal) * format.hillHeight;
				rawVal *= 0.01 * format.plainHeight +10;
           		heightData.final[coord] = mtVal + riVal + hiVal + rawVal;
           		HEIGHEST = glm::max(HEIGHEST,heightData.final[coord]);
           		LOWEST = glm::min(LOWEST,heightData.final[coord]);
           	}
        heightData.ini = true;
	}
public:

	void setHeightFormat(float mountainH, float plainH, float hillH, float riverD){
		format.mountainHeight = mountainH;
		format.plainHeight = plainH;
		format.riverDepth = riverD;
		format.hillHeight = hillH;
		format.ini = true;
	}

	void initialize(void){
		areaIndicator.loadAreaImg();
		heightData.allocate(VERTEX_COUNT);
		initializeRawHeightData();
		initializeOtherHeightData();
	}
	void reInitialize(float seed) {
		initializeRawHeightData(seed);
		initializeOtherHeightData();
	};
	HeightMapGenerator(){};
	void setParameters(bool _useImage , float vertNum){
		useImage = _useImage;
		VERTEX_COUNT = vertNum;
	}
	~HeightMapGenerator(){
		if (heightData.ini) 
			heightData.freeMemory();
	};
	float getHeight(float x, float y){
		return heightData.getFinalHeight(x, y);
	}

	float getHeighest(void){
		return HEIGHEST;
	}
	float getLowest(void){
		return LOWEST;
	}

	vec4 getFormat(void) {
		if (format.ini) {
			return vec4(format.mountainHeight, format.hillHeight, format.plainHeight, format.riverDepth);
		}else
			return vec4(-1);
	}
	float hillVal(float x, float y){
		return areaIndicator.getHill(x,y);
	}
};

class TerrainGenerator{
	float VERTEX_COUNT = 0;
	float SIZE = 0;

	HeightMapGenerator heightGenerator;
	
	GLuint vao;
    struct {
		float vN = 0;
    	float*  vertices 		= NULL;
    	float*  normals 		= NULL;
    	float*  color 	        = NULL;
    	int*    indices 		= NULL;
		void allocate() {
			vertices  = new float[3 * vN * vN];
			normals   = new float[3 * vN * vN];
			color     = new float[4 * vN * vN];
			indices   = new int[6 * vN * vN];
		};
		template <class T>
    	void freeBuffer(T*& buf){
    		if(!buf){
    			free(buf);
    			buf = NULL;
    		}
		};

    	void freeAllMemoryExceptIndices(void){
    		freeBuffer(vertices);
    		freeBuffer(normals);
    		freeBuffer(color);
		};
		void freeAllMemory(void){
    		freeBuffer(indices);
			freeAllMemoryExceptIndices();
		}
    }buffers;

	struct {
		float vN = 0;
		bool allocated = false;
		GLuint vboVertex;
		GLuint vboNormal;
		GLuint vboColor;
		GLuint vboIndex;
		void generate(void){
			glGenBuffers(1, &vboVertex);
			glGenBuffers(1, &vboNormal);
			glGenBuffers(1, &vboColor);
			glGenBuffers(1, &vboIndex);
			allocateMemory();
		}
		void bindToVao(GLuint& _vao){
			glBindVertexArray(_vao);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
			glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
			glBindBuffer(GL_ARRAY_BUFFER, vboColor);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex);
			glBindVertexArray(0);
		}
		void allocateMemory(void){
			if(!allocated){
			glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
			glBufferData(GL_ARRAY_BUFFER, 3 * vN * vN * sizeof(GL_FLOAT), NULL, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, vboColor);
			glBufferData(GL_ARRAY_BUFFER, 4 * vN * vN * sizeof(GL_FLOAT), NULL, GL_STREAM_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
			glBufferData(GL_ARRAY_BUFFER, 3 * vN * vN * sizeof(GL_FLOAT), NULL, GL_STREAM_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndex);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (vN-1)*(vN-1) * 6 * sizeof(GLuint), NULL ,GL_STATIC_DRAW);
			}else{
				printf("TerrainGenerator : Allocate memory to vbo when it's already allocated.\n");
				exit(1);
			}
		}
		template <class T>
		void storeDataIntoVbo(GLuint _vbo, GLuint type, int size, T* ptr) {
			glBindBuffer(type, _vbo);
			glBufferSubData(type, 0, size * sizeof(T), ptr);
			// auto bbb = new T[9];
			// glGetBufferSubData(type, 0, sizeof(T) * 9, bbb);
			// for (size_t i = 0; i < 9; i++)
			// 	cout << bbb[i] << " ";
			// cout << endl;
			// for (size_t i = 0; i < 9; i++)
			// 	cout << ptr[i] << " ";
			// cout << endl << endl;
			// free(bbb);
		}
		void storeData(int _vao, float* _position, float* _normals, float* _color, int* _indices) {
			glBindVertexArray(_vao);
			storeDataIntoVbo(vboVertex, GL_ARRAY_BUFFER, 3 * vN * vN, _position);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			storeDataIntoVbo(vboColor, GL_ARRAY_BUFFER, 4 * vN * vN, _color);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
			storeDataIntoVbo(vboNormal, GL_ARRAY_BUFFER, 3 * vN * vN, _normals);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
			storeDataIntoVbo(vboIndex, GL_ELEMENT_ARRAY_BUFFER, 6 * (vN-1) * (vN-1), _indices);
			glBindVertexArray(0);
		}
		void bye(void){
			glDeleteBuffers(1, &vboVertex);
			glDeleteBuffers(1, &vboNormal);
			glDeleteBuffers(1, &vboColor);
			glDeleteBuffers(1, &vboIndex);
		}
	}vbo;
    
    // Height
    void intialHeightGenerator(void){
		heightGenerator.setParameters(false, VERTEX_COUNT);
		float mtH = 0.02*(float)VERTEX_COUNT*glm::sqrt((float)VERTEX_COUNT);
		heightGenerator.setHeightFormat(mtH,2,0.3*mtH ,-0.3*mtH);
		heightGenerator.initialize();
	};
	vec3 getNormal(float x, float z){
		vec3 n = vec3(1);
		float hL = heightGenerator.getHeight(x-1, z);
		float hR = heightGenerator.getHeight(x+1, z);
		float hD = heightGenerator.getHeight(x, z-1);
		float hU = heightGenerator.getHeight(x, z+1);
		n.x = (hL - hR);
		n.y = 2.0f;
		n.z = (hD - hU);
		return normalize(n);
	}
	// Calc Vertex Color
	vec4 vertexColor(const vec3& pos, const vec3& nor) {
		auto randFloat = []() {return static_cast <float> (rand()) / static_cast <float> (RAND_MAX); };
		float distToOrigin = glm::sqrt(pos.x*pos.x + pos.z*pos.z);
		float inRadius = SIZE / 2 - 50;
		float outRadius = SIZE / 2 - 5;
		float distRange = 5;
		float alpha = 255;
		if (distToOrigin > outRadius) {
			alpha = 0;
		}
		else if (distToOrigin > inRadius) {
			float d1 = distToOrigin - inRadius;
			float d2 = outRadius - inRadius;
			if (randFloat() <= d1 / d2 || randFloat() > 0.9)
				alpha = d1 / d2 * 255.0f;
		}


		vec4 colors[6] = {
			vec4(105.0f, 44.0f, 26.0f, alpha)/255.0f,
			vec4(171.0f, 81.0f, 31.0f, alpha)/255.0f,
			vec4(144.0f,216.0f, 77.0f, alpha)/255.0f,
			vec4(118.0f,162.0f, 66.0f, alpha)/255.0f,
			vec4(164.0f,171.0f,155.0f, alpha)/255.0f,
			vec4(245.0f,245.0f,245.0f, alpha)/255.0f
		};
		vec4 format = heightGenerator.getFormat();
		//vec4(mountainHeight, hillHeight, plainHeight, riverDepth);

		float mtTop = heightGenerator.getHeighest();
		float hillHeight = format.y + 5;
		float hillVal = heightGenerator.hillVal(0.5 + pos.x / SIZE, 0.5 + pos.z / SIZE);
		float h = pos.y;
		float thresholds[5] = {
			0,
			0.5,
			hillHeight-5,
			0.8 * hillHeight + 0.2 * mtTop,
			0.4 * hillHeight + 0.6 * mtTop
		};

		
		const float _PI = 3.1415926;
		bool skip = false;
		float r;
		float degToZ = degrees(acos(dot(vec3(0,1,0),nor)));
		


		// Underwater
		if(h < thresholds[0]){
			if (randFloat() > 0.5 && h + 5 > thresholds[0]){
				skip = true;
			}else{
				return colors[0];
			}
		}
		// Riverbank
		if(h < thresholds[1]){
			if(skip){
				return 0.5f * (colors[1] + colors[0]);
			}else {
				if (degToZ < 30)
					skip = true;
				else
					return colors[1];
			}
		}
		// Plain
		if(h < thresholds[2]){
			if(skip){
				if (randFloat() > 0.2)
					return 0.5f * (colors[2] + colors[1]);
				else
					return colors[2];
			}else {
				if(randFloat() > 0.3 && h + 5 > thresholds[2])
					skip = true;
				else if (degToZ > 30)
					skip = true;
				else
					return colors[2];
			}
		}
		// Hills
		if(h < thresholds[3]){
			if(skip){
				if (randFloat() > 0.2)
					return 0.5f * (colors[3] + colors[2]);
				else if (randFloat() > 0.8)
					return colors[3];
				else
					return colors[2];
			}else {
				if (hillVal < 0.3) {
					if (randFloat() > 0.5 && h + 5 > thresholds[3])
						skip = true;
					else if (randFloat() > 0.3 && degToZ > 50)
						skip = true;
					else
						return colors[3];
				}else
					return colors[3];
			}
		}
		// RockMountain
		if(h < thresholds[4]){
			if(skip){
				return 0.5f * (colors[4] + colors[3]);
			}else {
				if(randFloat() > 0.5 && h + 5 > thresholds[4])
					skip = true;
				else if (degToZ > 30)
					return colors[4];
				else
					skip = true;
			}
		}
		// Snow
		if(degToZ < 75){
			return colors[5];
		}else if (randFloat() > 0.2 && h - 5 < thresholds[4]){
			return 0.5f * (colors[4] + colors[5]);
		}else{
			return colors[5];
		}
	
	};

	// Generate Indices
 	void storeQuad1(int* indices, int& pointer, int topLeft, int topRight, int bottomLeft, int bottomRight, bool mixed) {
        indices[pointer++] = topLeft;
        indices[pointer++] = bottomLeft;
        indices[pointer++] = mixed ? topRight : bottomRight;
        indices[pointer++] = topRight;
        indices[pointer++] = mixed ? bottomLeft : topLeft;
		indices[pointer++] = bottomRight;
	}
    void storeQuad2(int* indices, int& pointer, int topLeft, int topRight, int bottomLeft, int bottomRight, bool mixed) {
        indices[pointer++] = topLeft;
        indices[pointer++] = mixed ? bottomRight : bottomLeft;
		indices[pointer++] = topRight;
		indices[pointer++] = bottomRight;
        indices[pointer++] = mixed ? topLeft : topRight;
		indices[pointer++] = bottomLeft;
	}
	void initializeIndiceData(void){
		int cnt = 0;
		int indexCount = (VERTEX_COUNT - 1) * (VERTEX_COUNT - 1) * 6;
        buffers.indices = new int[indexCount];
        int pointer = 0;
        for (int col = 0; col < VERTEX_COUNT - 1; col++) {
            for (int row = 0; row < VERTEX_COUNT - 1; row++) {
                int topLeft = (row * VERTEX_COUNT) + col;
                int topRight = topLeft + 1;
                int bottomLeft = ((row + 1) * VERTEX_COUNT) + col;
                int bottomRight = bottomLeft + 1;
                if (row % 2 == 0) {
                    storeQuad1(buffers.indices, pointer, topLeft, topRight, bottomLeft, bottomRight, col % 2 == 0);
                } else {
                    storeQuad2(buffers.indices, pointer, topLeft, topRight, bottomLeft, bottomRight, col % 2 == 0);
                }
             //    if(cnt < 9){
	            //     for (int k = 1; k >= 0 ; --k){
	            //     	printf("<");
	            //     	for (int m = -3; m < 0; ++m){
	            //     		int offset = -3 * k + m;
	            //     		printf("%2d ", buffers.indices[pointer+offset]);
	            //     		cnt++;
	            //     	}
	            //     	printf(">\n");
	            //     }
	            //     printf("\n");
            	// }
            }
        }
	}
	template<class T>
	void setArrayByVec(float* ptr,int component, const T& v) {
		for (int i = 0; i < component; i++)
			ptr[i] = v[i];
	}
	// Generate Vertex Data
	void initializeVertexData(void){
		int count = VERTEX_COUNT * VERTEX_COUNT;
        int vertexPointer = 0;
        float v_c_1 = (float)VERTEX_COUNT-1;
        float * posIdx = new float[VERTEX_COUNT];
        for(float i=0;i<VERTEX_COUNT;i++)
        	posIdx[(int)i] = (i / v_c_1 - 0.5) * SIZE;
        for(float i=0;i<VERTEX_COUNT;i++){
            for(float j=0;j<VERTEX_COUNT;j++){
            	vec3 pos = vec3(posIdx[(int)i], heightGenerator.getHeight(i , j), posIdx[(int)j]);
                vec3 nor = getNormal(i,j);
				vec4 col = vertexColor(pos, nor);
				setArrayByVec(&buffers.vertices[vertexPointer * 3],3, pos);
				setArrayByVec(&buffers.normals[vertexPointer * 3],3, nor);
				setArrayByVec(&buffers.color[vertexPointer * 4] ,4, col);
				vertexPointer++;
            }
		}
        free(posIdx);
	}
	
public:
	TerrainGenerator(){};
	void initialize(float vN, float sZ){
		VERTEX_COUNT = vbo.vN = buffers.vN = vN;
		SIZE = sZ;
		srand(static_cast <unsigned> (time(0)));
		buffers.allocate();
		intialHeightGenerator();
		initializeVertexData();
		initializeIndiceData();
		glGenVertexArrays(1, &vao);
		vbo.generate();
		vbo.bindToVao(vao);
		vbo.storeData(vao, buffers.vertices, buffers.normals, buffers.color, buffers.indices);
		buffers.freeAllMemoryExceptIndices();
	}
	~TerrainGenerator(){
		buffers.freeAllMemory();
	};
	void reInitialize(float seed) {
		heightGenerator.reInitialize(seed);
		initializeVertexData();
		vbo.storeData(vao, buffers.vertices, buffers.normals, buffers.color, buffers.indices);
		buffers.freeAllMemoryExceptIndices();
	}
	float getHeight(float x, float y){
		return heightGenerator.getHeight(x , y);
	}
	GLuint getIndicesVbo(void){return vbo.vboIndex;	}
	GLuint getVao(void){return vao;	}
	float getHeighest(void){
		return heightGenerator.getHeighest();
	}
	float getLowest(void){
		return heightGenerator.getLowest();
	}
};



class Terrains{
	float SIZE = 100;
	int VERTEX_COUNT = 3;
	mat4 modelMatrix ;
	TerrainGenerator generator;
	GLuint vao;
	GLuint indexVbo;
	float winterFraction;
	int winterFractionDir = -1;

	void setParameters(float vN, float sZ) { VERTEX_COUNT = vN; SIZE = sZ; };
	void toWinter(void){
		winterFraction = (winterFraction < 1) ? winterFraction + 0.001 : 1;
	}
	void toSummer(void){
		winterFraction = (winterFraction > 0) ? winterFraction - 0.001 : 0;
	}

public:
	Terrains(){};
	~Terrains(){};
	bool validPosition(vec3 pos){
		if (pos.y <= generator.getHeight(pos.x,pos.z))
			return false;
		float distToOrigin = glm::length(vec2(pos.xz));
		if (distToOrigin > SIZE / 2 - 5)
			return false;
		return true;
	}
	void initilaze(float vN = 355, float sZ = 600){
		winterFraction = 0.0f;
		VERTEX_COUNT = vN; SIZE = sZ;
		generator.initialize(vN, sZ);
		indexVbo = generator.getIndicesVbo();
		vao = generator.getVao();
	}

	void render(mat4 &_ViewMatrix,mat4 &_ProjMatrix,vec3 _v3LightPosition,vec4 &_v4Plane, mat4 &_ShadowMatrix, GLuint _iShadowMap, TerrainProgram& _program) {
		if(winterFractionDir > 0)
			toWinter();
		else if(winterFractionDir < 0)
			toSummer();
		_program.start();
			_program.loadUniforms(_ViewMatrix, _ProjMatrix,modelMatrix, _v4Plane, _v3LightPosition, winterFraction, _ShadowMatrix, _iShadowMap);
			glBindVertexArray(vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
			glDrawElements(GL_TRIANGLES, (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1) * 6, GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0);
		_program.stop();
	};

	void render(Camera& _camera, TerrainProgram& _program, vec3 lightPosition){
		printf("call Function!!!!!\n");
		if(winterFractionDir > 0)
			toWinter();
		else if(winterFractionDir < 0)
			toSummer();
		_program.start();
			_program.loadUniforms(_camera, modelMatrix, lightPosition, winterFraction);
			glBindVertexArray(vao);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
			glDrawElements(GL_TRIANGLES, (VERTEX_COUNT-1)*(VERTEX_COUNT-1)*6, GL_UNSIGNED_INT, (void*) 0);
			glBindVertexArray(0);
		_program.stop();
	};
	void render() {
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVbo);
		glDrawElements(GL_TRIANGLES, (VERTEX_COUNT - 1)*(VERTEX_COUNT - 1) * 6, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
	}
	float GetTerrainHeight(float x, float y) {
		float fReturnValue = 0;
		float fHalfSize = SIZE * 0.5f;
		if (x < fHalfSize && x>-fHalfSize && y < fHalfSize && y>-fHalfSize) {
			auto convert = [=](float val) {return VERTEX_COUNT * (val / SIZE + 0.5f); };
			fReturnValue = generator.getHeight(convert(x), convert(y));
		}
		else {
			printf("OUtSize Height\n");
		}
		return fReturnValue;
	}
	void reInitialize(float seed) {
		generator.reInitialize(seed);
		indexVbo = generator.getIndicesVbo();
		vao = generator.getVao();
	}
	float GetSize() {
		return SIZE;
	}

	float getHeight(float x, float y) {
		auto convert = [=](float val) {return VERTEX_COUNT * (val / SIZE + 0.5f); };
		return generator.getHeight(convert(x), convert(y));
	}
	float getHeight(Camera& cam) {
		vec3 camPos = cam.getPosition();
		return getHeight(camPos.x,camPos.z);
	}

	void setModelMatrix(mat4 _modelMatrix){
		//modelMatrix = _modelMatrix;
	};

	mat4 getModelMatrix(void){
		return modelMatrix;
	};

	int getSeasonDir(void){
		return winterFractionDir;
	}

	void changeSeasonDirection(int dir){
		winterFractionDir = (dir > 0)? 1 : (dir < 0) ? -1 : 0;
	}

};


#endif