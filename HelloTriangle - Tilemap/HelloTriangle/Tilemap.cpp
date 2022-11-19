#include "Tilemap.h"

void Tilemap::initialize(string mapPath)
{
	loadMap(mapPath);
	VAOTile = setupSprite();
	pos.x = 0;
	pos.y = 0;
}

void Tilemap::update()
{
	map[(int)pos.y][(int)pos.x] = 6;
}

void Tilemap::draw()
{
	float xi = 200;
	float yi = 200;

	//Percorrer a matriz de tiles (tilemap)
	for (int i = 0; i < MAP_HEIGHT; i++)
	{
		for (int j = 0; j < MAP_WIDTH; j++)
		{
			//Processa cada tile individualmente
			int texIDTile = tileset[map[i][j]]; //pega o indice da textura no tileset 
			
			//Atualiza a matriz de modelo do tile
			glm::mat4 model = glm::mat4(1); //matriz identidade
			model = glm::translate(model, glm::vec3(xi + j * tileDimensions.x, yi + i * tileDimensions.y, 0));
			model = glm::scale(model, glm::vec3(tileDimensions.x, tileDimensions.y, 1.0f));
			shader->setMat4("model", glm::value_ptr(model));
			shader->setVec2("offsets", 0, 0);

			//Chamada de desenho
			glBindVertexArray(VAOTile); //conecta o buffer de geometria (VA0)
			glBindTexture(GL_TEXTURE_2D, texIDTile); //conecta a textura desejada
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0); //"unbind do VAO" 
		}
	}

	//Processa cada tile individualmente
	int texIDTile = tileset[6]; //pega o indice da textura no tileset que representa ocupado

	//Atualiza a matriz de modelo do tile
	glm::mat4 model = glm::mat4(1); //matriz identidade
	model = glm::translate(model, glm::vec3(xi + pos.x * tileDimensions.x, yi + pos.y * tileDimensions.y, 0));
	model = glm::scale(model, glm::vec3(tileDimensions.x, tileDimensions.y, 1.0f));
	shader->setMat4("model", glm::value_ptr(model));
	shader->setVec2("offsets", 0, 0);

	//Chamada de desenho
	glBindVertexArray(VAOTile); //conecta o buffer de geometria (VA0)
	glBindTexture(GL_TEXTURE_2D, texIDTile); //conecta a textura desejada
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0); //"unbind do VAO" 
	
}

void Tilemap::finish()
{
}

void Tilemap::loadMap(string mapPath)
{
	ifstream inputFile(mapPath.c_str());
	if (!inputFile.is_open())
	{
		cout << "Erro ao ler arquivo!";
	}
	else
	{
		int tilesetSize;
		inputFile >> tilesetSize;
		inputFile >> tileDimensions.x >> tileDimensions.y; //largura e altura do tile
		for (int i = 0; i < tilesetSize; i++)
		{
			string tileName;
			inputFile >> tileName; 
			cout << tileName << endl; //debug tempoorário
			GLuint texID = generateTexture(tileName);
			tileset.push_back(texID);
		}
		int mapWidth, mapHeight;
		inputFile >> mapWidth >> mapHeight;
		for (int i = 0; i < mapHeight; i++) //linhas
		{
			for (int j = 0; j < mapWidth; j++) //colunas
			{
				inputFile >> map[i][j];
				cout << map[i][j] << " ";
			}
			cout << endl;
		}

		inputFile.close();
	}
}

GLuint Tilemap::generateTexture(string filePath)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento do arquivo de imagem da textura
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	//Verifica se conseguiu carregar e manda a textura pro OpenGL
	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //4 canais png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data); //liberando o array da imagem

	glBindTexture(GL_TEXTURE_2D, 0); // dar "unbind" 

	return texID;
}

int Tilemap::setupSprite()
{
	GLfloat vertices[] = {
		//Base da pirâmide: 2 triângulos
		//x    y    z    r    g    b	s     t
		-0.5, -0.5, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0,
		 0.5, -0.5, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0,
		-0.5,  0.5, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0,


		 0.5, -0.5, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0,
		 0.5,  0.5, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0,
		-0.5,  0.5, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0,
	};
	GLuint VBO, VAO;
	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//Atributo coordenada de textura (s,t)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	return VAO;
}
