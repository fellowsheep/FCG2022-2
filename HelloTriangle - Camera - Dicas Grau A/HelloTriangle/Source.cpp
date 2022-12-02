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

#define MAX_X 4
#define MAX_Y 4
#define MAX_Z 4

#define MAX_CORES 10

//using namespace glm;


// Protótipo da função de callback de teclado e de mouse
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Protótipos das funções
int setupGeometry();
int setupGeometry3D();
int setupVoxel();
void inicializarGrid();
void desenharGrid();

void adicionarVoxel();

void updateCameraPos(GLFWwindow* window);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Variáveis globais para controle de câmera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); 
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0; //para calcular o quanto que o mouse deslocou
float yaw = -90.0, pitch = 0.0; //rotação em x e y

/// Sugestão de estrutura para o trabalho
         //x //y //z
int grid[MAX_X][MAX_Y][MAX_Z];
int indiceCor = 2; //vermelho
glm::vec3 posVoxelCursor = glm::vec3(0, 0, 0);

glm::vec4 paletaCores[MAX_CORES]{
	glm::vec4(0.0, 0.0 ,0.0 ,1.0), //preto
	glm::vec4(1.0, 1.0 ,1.0 ,1.0), //branco
	glm::vec4(1.0, 0.0 ,0.0 ,1.0), //vermelho
	glm::vec4(0.0, 1.0 ,0.0 ,1.0), //verde
	glm::vec4(0.0, 0.0 ,1.0 ,1.0), //azul
	glm::vec4(1.0, 1.0 ,0.0 ,1.0), //amarelo
	glm::vec4(0.0, 1.0 ,1.0 ,1.0), //ciano
	glm::vec4(1.0, 0.0 ,1.0 ,1.0), //magenta
	glm::vec4(0.5, 0.5 ,0.5 ,1.0), //cor do cursor
	glm::vec4(1.0, 1.0 ,1.0 ,0.1), //cor voxel "vazio"
};

GLuint VAO_Voxel;
// Compilando e buildando o programa de shader
Shader shader;

// indices de tipos de voxels: -1 vazio, 0 preto 1 vermelho....
//////////////////////////////////////////

// Função MAIN
int main()
{
	srand((int)glfwGetTime());

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola 3D", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	//Desabilitando o cursor do mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	
	//Inicializando o objeto da classe Shader
	shader = Shader("../shaders/hello_triangle.vs", "../shaders/hello_triangle.fs");

	// Gerando o identificador do buffer de geometria do voxel
	VAO_Voxel= setupVoxel();
	
	//Habilitar a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(shader.ID);

	//Matriz de projeção PERSPECTIVA
	glm::mat4 projection = glm::mat4(1); //matriz identidade
	//projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
	projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));
	
	//Matriz de view
	glm::mat4 view = glm::mat4(1);
	//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),// Posição (ponto) 
		   glm::vec3(0.0f, 0.0f, 0.0f),// Target (ponto, não vetor)  dir = target - pos                
		   glm::vec3(0.0f, 1.0f, 0.0f)); // Up (vetor)
	shader.setMat4("view", glm::value_ptr(view));

	//Habilita teste de profundidade
	glEnable(GL_DEPTH_TEST);

	glLineWidth(10);
	glPointSize(20);

	inicializarGrid();

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();
		updateCameraPos(window);

		//Atualiza a matriz de view, afinal a camera pode mudar de posição e orientação
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", glm::value_ptr(view));

		// Limpa o buffer de cor
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Viewport 
		glViewport(0, 0, width, height);

		//Desenhar a grid
		desenharGrid();
		
		

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO_Voxel);
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

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		int cursorX = rand() % MAX_X;
		int cursorY = rand() % MAX_Y;
		int cursorZ = rand() % MAX_Z;

		posVoxelCursor = glm::vec3(cursorX, cursorY, cursorZ);
		adicionarVoxel();
	}

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		indiceCor = 3; //verde
	}
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

int setupGeometry3D()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
		//Base da pirâmide: 2 triângulos
		//x    y    z    r    g    b
		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		-0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		 -0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		  0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		  0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		  //
		  -0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		   0.0,  0.5,  0.0, 1.0, 1.0, 0.0,
		   0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		  -0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		   0.0,  0.5,  0.0, 1.0, 0.0, 1.0,
		  -0.5, -0.5,  0.5, 1.0, 0.0, 1.0,
		  -0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		   0.0,  0.5, 0.0, 1.0, 1.0, 0.0,
		   0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		   0.5, -0.5, 0.5, 0.0, 1.0, 1.0,
		   0.0,  0.5,  0.0, 0.0, 1.0, 1.0,
		   0.5, -0.5, -0.5, 0.0, 1.0, 1.0,
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	return VAO;
}

void updateCameraPos(GLFWwindow* window)
{
	float cameraSpeed = 0.05f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	//Precisamos também atualizar o cameraUp!! Pra isso, usamos o Up do  
	//mundo (y), recalculamos Right e depois o Up
	glm::vec3 right = glm::normalize(glm::cross(cameraFront,
		glm::vec3(0.0, 1.0, 0.0)));
	cameraUp = glm::normalize(glm::cross(right, cameraFront));
}

int setupVoxel()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
		GLfloat vertices[] = {
			//Base do cubo: 2 triângulos
		//x    y    z   
		-0.5, -0.5, -0.5, 
		-0.5, -0.5,  0.5,
		 0.5, -0.5, -0.5, 

		 -0.5, -0.5, 0.5,  
		  0.5, -0.5,  0.5, 
		  0.5, -0.5, -0.5, 

		  //Face de cima: 2 triângulos
		  -0.5, 0.5,  0.5, 
		   0.5, 0.5,  0.5, 
		  -0.5, 0.5, -0.5, 

		  0.5, 0.5,  0.5, 
		  0.5, 0.5, -0.5, 
		 -0.5, 0.5, -0.5, 

		 //Face de frente: 2 triângulos
		 -0.5, -0.5, -0.5, 
		 -0.5,  0.5, -0.5,
		  0.5, -0.5, -0.5, 

		  0.5, -0.5, -0.5, 
		 -0.5,  0.5, -0.5, 
		  0.5,  0.5, -0.5, 

		  //Face de trás: 2 triângulos
		  -0.5,  0.5,  0.5, 
		  -0.5, -0.5,  0.5, 
		   0.5,  0.5,  0.5, 

		   0.5,  0.5,  0.5, 
		  -0.5, -0.5,  0.5, 
		   0.5, -0.5,  0.5, 

		   //Face da esquerda: 2 triângulos
		  -0.5,  0.5, -0.5, 
		  -0.5, -0.5, -0.5, 
		  -0.5, -0.5,  0.5, 

		  -0.5, -0.5,  0.5, 
		  -0.5,  0.5,  0.5, 
		  -0.5,  0.5, -0.5, 

		  //Face da direita: 2 triângulos
		   0.5,  0.5,  0.5, 
		   0.5, -0.5,  0.5, 
		   0.5, -0.5, -0.5, 

		   0.5, -0.5, -0.5, 
		   0.5,  0.5, -0.5, 
		   0.5,  0.5,  0.5, 
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	
	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	return VAO;
}

void inicializarGrid()
{
	//inicializar a grid de voxels com o símbolo -1
	//de voxel "vazio"

	for (int x = 0; x < MAX_X; x++)
		for (int y = 0; y < MAX_Y; y++)
			for (int z = 0; z < MAX_Z; z++)
			{
				grid[x][y][z] = -1;
			}
	
}

void desenharGrid()
{
	//Conectar com o buffer de geometria do voxel
	glBindVertexArray(VAO_Voxel);

	float larguraVoxel = 1.0, alturaVoxel = 1.0, profundidadeVoxel = 1.0;

	for (int x = 0; x < MAX_X; x++)
	{
		for (int y = 0; y < MAX_Y; y++)
		{
			for (int z = 0; z < MAX_Z; z++)
			{
				//Calcular os deslocamentos para aplicar a translação do voxel de acordo com os seus índices
				float dx = x * larguraVoxel;
				float dy = y * alturaVoxel;
				float dz = z * profundidadeVoxel;

				//Definição da matriz de modelo (transf. na geometria)
				glm::mat4 model = glm::mat4(1); //matriz identidade
				model = glm::translate(model, glm::vec3(dx, dy, dz));
				shader.setMat4("model", glm::value_ptr(model));

				if (grid[x][y][z] == -1) //voxel vazio
				{
					//Passa para o shader a cor do voxel vazio na paleta
					shader.setVec4("inputColor", paletaCores[9].r, paletaCores[9].g, paletaCores[9].b, paletaCores[9].a);
				}
				else
				{
					glm::vec4 corVoxel;
					int indiceCorVoxel = grid[x][y][z];
					corVoxel.r = paletaCores[indiceCorVoxel].r;
					corVoxel.g = paletaCores[indiceCorVoxel].g;
					corVoxel.b = paletaCores[indiceCorVoxel].b;
					corVoxel.a = paletaCores[indiceCorVoxel].a;
					shader.setVec4("inputColor", corVoxel.r, corVoxel.g, corVoxel.b, corVoxel.a);
				}

				//desenhar preenchido
				glDrawArrays(GL_TRIANGLES, 0, 36);

				//shader.setVec4("inputColor", 0.0, 0.0, 0.0, 1.0); //cor do contorno 
				//glDrawArrays(GL_LINE_LOOP, 0, 36);

			}
		}
	}

	//glBindVertexArray(0);
	//glBindVertexArray(VAO_Voxel);

	//Desenhar o cursor:
	//ajustar a pos na matriz de modelo e passar pro shader
	glm::mat4 model = glm::mat4(1);
	model = glm::translate(model, glm::vec3(posVoxelCursor.x * larguraVoxel, posVoxelCursor.y * alturaVoxel, posVoxelCursor.z * profundidadeVoxel));
	shader.setMat4("model", glm::value_ptr(model));
	//mandar a cor do cursor pro shader
	shader.setVec4("inputColor", 1.0f, 0.0f, 1.0f, 1.0f);

	//mandar desenhar
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(0); //"unbind do VAO" 

	
}

void adicionarVoxel()
{
	//inserção
	grid[(int)posVoxelCursor.x][(int)posVoxelCursor.y][(int)posVoxelCursor.z] = indiceCor;
}


