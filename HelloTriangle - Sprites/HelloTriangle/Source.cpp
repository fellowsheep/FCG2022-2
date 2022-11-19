/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 01/08/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

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

//using namespace glm;


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupGeometry();
int setupSprite();
int setupSprite(int nFrames, int nAnimations, float &dx, float &dy);
int setupGeometry3D();

GLuint generateTexture(string filePath);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Sprites!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	


	// Compilando e buildando o programa de shader
	Shader shader("../shaders/hello_triangle.vs", "../shaders/hello_triangle.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupSprite();

	float dx, dy;
	GLuint VAODino = setupSprite(5, 1, dx, dy);

	//cout << dx << "  " << dy << endl;
	
	GLuint texID = generateTexture("../textures/desert-100.jpg");
	GLuint texID2 = generateTexture("../textures/flaming_meteor.png");
	GLuint texID3 = generateTexture("../textures/dinoanda.png");
	GLuint texID4 = generateTexture("../textures/piterodatilo.png");
	GLuint texID5 = generateTexture("../textures/lava5.png");

	//Outros atributos do dino
	glm::vec3 posDino;
	posDino.x = 100;
	posDino.y = 100;
	int iFrame = 0;
	int iAnimation = 0;

	
	//Ativa o shader que vai ser usado
	glUseProgram(shader.ID);

	//Associa a variável que armazenará o buffer de textura
	//glUniform1i(glGetUniformLocation(shader.ID, "texBuffer"), 0);
	shader.setInt("texBuffer", 0);

	//Matriz de projeção
	glm::mat4 projection = glm::mat4(1); //matriz identidade
	projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	//Matriz de view
	glm::mat4 view = glm::mat4(1);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	shader.setMat4("view", glm::value_ptr(view));

	//Habilita teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);


	//Habilita um buffer de textura
	glActiveTexture(GL_TEXTURE0);

	//Habilita a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//variáveis de controle do meteoro
	float xMeteoro = 400.0;
	float yMeteoro = 850.0;

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);


		//Viewport 
		glViewport(0, 0, width, height);

		//Desenho do fundo

		//Atualiza a matriz de modelo (transform do objeto)
		glm::mat4 model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(400, 300, 0));
		model = glm::scale(model, glm::vec3(800.0f, 600.0f, 1.0f));
		shader.setMat4("model", glm::value_ptr(model));
		
		shader.setVec2("offsets", 0, 0);
		
		//Chamada de desenho
		glBindVertexArray(VAO); //conecta o buffer de geometria (VA0)
		glBindTexture(GL_TEXTURE_2D, texID); //conecta a textura desejada
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//-------------

		//Desenho do sprite de meteoro
		//Atualiza a matriz de modelo (transform do objeto)
		model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(xMeteoro, yMeteoro, 0));
		model = glm::scale(model, glm::vec3(64.0f, 64.0f, 1.0f));
		shader.setMat4("model", glm::value_ptr(model));
		shader.setVec2("offsets", 0, 0);
		//Chamada de desenho
		glBindTexture(GL_TEXTURE_2D, texID2); //conecta a textura desejada
		glDrawArrays(GL_TRIANGLES, 0, 6);

		if (yMeteoro > 0.0)
		{
			yMeteoro -= 2.0;
		}
		else
		{
			//reseta a posição, mas sorteando um novo x
			xMeteoro = 64 + rand() % 673; // 64 a 736
			yMeteoro = 850;
		}

		/////////////

		//Desenho do sprite do dinossauro
		//Atualiza a matriz de modelo (transform do objeto)
		model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(posDino.x, posDino.y, 0));
		model = glm::scale(model, glm::vec3(120.0f, 72.0f, 1.0f));
		shader.setMat4("model", glm::value_ptr(model));
		
		//Passar para o shader os deslocamentos na coord de textura
		shader.setVec2("offsets", iFrame * dx, iAnimation * dy);
		
		//Chamada de desenho
		glBindVertexArray(VAODino);
		glBindTexture(GL_TEXTURE_2D, texID3); //conecta a textura desejada
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Desenho do sprite do pterossauro
		//Atualiza a matriz de modelo (transform do objeto)
		model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(700, 500, 0));
		model = glm::scale(model, glm::vec3(88.0f, 64.0f, 1.0f));
		shader.setMat4("model", glm::value_ptr(model));

		//Passar para o shader os deslocamentos na coord de textura
		shader.setVec2("offsets", iFrame * dx, iAnimation * dy);

		//Chamada de desenho
		glBindVertexArray(VAODino);
		glBindTexture(GL_TEXTURE_2D, texID4); //conecta a textura desejada
		glDrawArrays(GL_TRIANGLES, 0, 6);

		iFrame = (iFrame + 1) % 5; //incrementar o contador de Frames

		
		//cout << iFrame << endl;
		

		glBindVertexArray(0); //"unbind do VAO" 

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = { //Ex3)
		-0.5, -0.5, 0.0, //v0
		 0.5, -0.5, 0.0, //v1
		 0.0, 0.5, 0.0,  //v2
		 //outro triangulo vai aqui
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0); 

	return VAO;
}

int setupSprite()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
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

int setupSprite(int nFrames, int nAnimations, float &dx, float &dy)
{
	dx = 1.0f / (float) nFrames;
	dy = 1.0f / (float) nAnimations;

	GLfloat vertices[] = {
		//Base da pirâmide: 2 triângulos
		//x    y    z    r    g    b	s     t
		-0.5, -0.5, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0,
		 0.5, -0.5, 0.0, 1.0, 0.0, 1.0, dx, 0.0,
		-0.5,  0.5, 0.0, 0.0, 1.0, 1.0, 0.0, dy,


		 0.5, -0.5, 0.0, 1.0, 0.0, 1.0, dx, 0.0,
		 0.5,  0.5, 0.0, 0.0, 1.0, 1.0, dx, dy,
		-0.5,  0.5, 0.0, 0.0, 1.0, 1.0, 0.0, dy,
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

int setupGeometry3D()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
		//Base da pirâmide: 2 triângulos
		//x    y    z    r    g    b	s     t
		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0, 0.0, 0.0,
		-0.5, -0.5,  0.5, 0.0, 1.0, 1.0, 0.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0, 1.0, 0.0,
		 -0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 0.0, 1.0,
		  0.5, -0.5,  0.5, 0.0, 1.0, 1.0,1.0, 1.0,
		  0.5, -0.5, -0.5, 1.0, 0.0, 1.0,1.0, 0.0,
		  //
		  -0.5, -0.5, -0.5, 1.0, 1.0, 0.0, 0.0, 0.0,
		   0.0,  0.5,  0.0, 1.0, 1.0, 0.0, 0.5, 1.0,
		   0.5, -0.5, -0.5, 1.0, 1.0, 0.0, 1.0, 0.0,

		  -0.5, -0.5, -0.5, 1.0, 0.0, 1.0, 0.0, 0.0,
		   0.0,  0.5,  0.0, 1.0, 0.0, 1.0, 0.5, 1.0,
		  -0.5, -0.5,  0.5, 1.0, 0.0, 1.0, 1.0, 0.0,

		  -0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 0.0, 0.0,
		   0.0,  0.5, 0.0, 1.0, 1.0, 0.0, 0.5, 1.0,
		   0.5, -0.5, 0.5, 1.0, 1.0, 0.0, 1.0, 0.0,

		   0.5, -0.5, 0.5, 0.0, 1.0, 1.0, 0.0, 0.0,
		   0.0,  0.5,  0.0, 0.0, 1.0, 1.0,0.5, 1.0,
		   0.5, -0.5, -0.5, 0.0, 1.0, 1.0,1.0, 0.0
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

GLuint generateTexture(string filePath)
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

