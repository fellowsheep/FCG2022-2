#pragma once

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"

#include "stb_image.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#define MAP_WIDTH 7
#define MAP_HEIGHT 7

class Tilemap
{
public:
	Tilemap() {}
	void initialize(string mapPath);
	void update();
	void draw();
	void finish();
	inline void setShader(Shader *shader) { this->shader = shader;  }
	inline void setCharacterPos(glm::vec2 pos) { this->pos = pos;  }

protected:
	void loadMap(string mapPath);
	GLuint generateTexture(string filePath);
	int setupSprite();

	//Tileset
	vector <GLuint> tileset; //armazena os IDs de textura dos tiles
	glm::vec2 tileDimensions;

	//Buffer de geometria dos tiles (pode ser o mesmo)
	GLuint VAOTile;

	//Tilemap
	int map[MAP_WIDTH][MAP_HEIGHT]; //ATENÇÃO! Pode ser melhorado com alocação dinâmica

	//Ponteiro para o shader, para poder usá-lo aqui
	Shader* shader;

	//Pos do personagem (indice de qual tile ele está)
	glm::vec2 pos;

};

