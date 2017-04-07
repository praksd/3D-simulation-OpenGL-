//#include <iostream>
//#include <cmath>
//#include <fstream>
//#include <vector>
#include <bits/stdc++.h>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
//srand (time(NULL));
struct VAO {
	GLuint VertexArrayID;
	GLuint VertexBuffer;
	GLuint ColorBuffer;

	GLenum PrimitiveMode;
	GLenum FillMode;
	int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

GLuint programID;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void quit(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
	struct VAO* vao = new struct VAO;
	vao->PrimitiveMode = primitive_mode;
	vao->NumVertices = numVertices;
	vao->FillMode = fill_mode;

	// Create Vertex Array Object
	// Should be done after CreateWindow and before any other GL calls
	glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
	glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
	glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

	glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
	glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
	glVertexAttribPointer(
			0,                  // attribute 0. Vertices
			3,                  // size (x,y,z)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
	glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
	glVertexAttribPointer(
			1,                  // attribute 1. Color
			3,                  // size (r,g,b)
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

	return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
	GLfloat* color_buffer_data = new GLfloat [3*numVertices];
	for (int i=0; i<numVertices; i++) {
		color_buffer_data [3*i] = red;
		color_buffer_data [3*i + 1] = green;
		color_buffer_data [3*i + 2] = blue;
	}

	return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
	// Change the Fill Mode for this object
	glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

	// Bind the VAO to use
	glBindVertexArray (vao->VertexArrayID);

	// Enable Vertex Attribute 0 - 3d Vertices
	glEnableVertexAttribArray(0);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

	// Enable Vertex Attribute 1 - Color
	glEnableVertexAttribArray(1);
	// Bind the VBO to use
	glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

	// Draw the geometry !
	glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/

float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;
float person_posx=-4.5,person_posz=-4.5,person_posy=1;
int block_pos=99,t;
int lives=3;
bool jump=false;
float eyeview=-9,targetview=9,upview=9, targetx=0,targety=0,targetz=0;
bool advenview=false,above=false,followview=false;
char direction='n';
int person_rotate=0;
double curx,cury;
bool mousedrag=false;
int a[50] = {99, 98, 97, 96, 87, 88, 64, 86, 77, 76, 75, 74, 84, 94, 66, 65, 55, 45, 35, 25, 15, 5, 54, 53, 52, 49, 48, 47, 46, 44, 43, 42, 50, 40, 36, 26, 16, 6, 30, 20, 10, 31, 21, 11, 62, 72, 51, 78};
int b[100];
float rot_ang = 0 ;
int x = 0, y = 0, z = 0; 
float w1 = 4.5, w2 = 4.5, r1 = 0, r2 = 1;
/* Executed when a regular key is pressed/released/held-down */
/* Prefered for Keyboard events */
void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
	b[41] = 1;
	for(t=0; t<49; t++)
	{ 
		b[a[t]] = 1;
	}
  if(action == GLFW_REPEAT || action==GLFW_PRESS)
	{
		switch(key)
		{
							
			case GLFW_KEY_RIGHT:
					r1 = 0; r2 = 1; rot_ang = -90;
					person_posz++;
					block_pos--;
					if(b[block_pos]==0)
						{	
							cout<<"fallen into pit \nlost life"<<endl;
							block_pos=99;
							person_posy=0;
							lives--;
						}
				cout<<"block "<<block_pos<<endl;

				break;

			case GLFW_KEY_LEFT:
				
					r1= 0; r2 = 1; rot_ang = 90;
					person_posz--;
					block_pos++;
					if(b[block_pos]==0)
						{	
							cout<<"fallen into pit \nlost life"<<endl;
							block_pos=99;
							person_posy=0;
							lives--;
						}

				
				cout<<"block "<<block_pos<<endl;
				break;

			case GLFW_KEY_UP:
				
					r1 = 1; r2 = 0; rot_ang = -90;
					person_posx++;
					block_pos-=10;
					if(b[block_pos]==0)
						{	
							cout<<"fallen into pit \nlost life"<<endl;
							block_pos=99;
							person_posy=0;
							lives--;
						}
				
				cout<<"block "<<block_pos<<endl;
				break;

			case GLFW_KEY_DOWN:
				
					r1 = 1; r2 = 0; rot_ang = 90;
					person_posx--;
					block_pos+=10;
					if(b[block_pos]==0)
						{	
							cout<<"fallen into pit \nlost life"<<endl;
							block_pos=99;
							person_posy=0;
							lives--;
						}
		
				cout<<"block "<<block_pos<<endl;
				break;

			
			case GLFW_KEY_A:
			eyeview=-9;
			targetview=9;
			upview=9;
			advenview=false;
			followview=false;
			targetx=targetz=targety=0;
			break;

			case GLFW_KEY_S:
			eyeview=-1;
			targetview=15;
			upview=1;
			advenview=false;
			followview=false;
			targetx=targetz=targety=0;
			break;

			case GLFW_KEY_R:
			if(eyeview<9 && upview==9)
				eyeview++;
			else if(eyeview==9 && upview>-9)
				upview--;
			else if(upview==-9 && eyeview>-9)
				eyeview--;
			else if(upview<9)
				upview++;
			targetx=targety=targetz=0;
			advenview=false;
			followview=false;
			break;

			case GLFW_KEY_T:
			advenview=true;
			eyeview=person_posx+0.5;
			targetview=person_posy+2;
			upview=person_posz;
			targetx=person_posx+2;
			targety=0;
			targetz=person_posz;
			followview=false;
			break;

			case GLFW_KEY_F:
			followview=true;
			eyeview=person_posx-2;
			targetview=person_posy+3;
			upview=person_posz;
			targetx=person_posx+2;
			targety=0;
			targetz=person_posz;
			advenview=false;
			break;



		}
	}
}

/* Executed for character input (like in text boxes) */
void keyboardChar (GLFWwindow* window, unsigned int key)
{
	switch (key) {
		case 'Q':
		case 'q':
			quit(window);
			break;
		default:
			break;
	}
}

/* Executed when a mouse button is pressed/released */
void mouseButton (GLFWwindow* window, int button, int action, int mods)
{
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (action == GLFW_RELEASE)
			{
				mousedrag=false;
			}
			else
			{
				mousedrag=true;
			}
			break;
		
		default:
			break;
	}
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (GLFWwindow* window, int width, int height)
{
	int fbwidth=width, fbheight=height;
	/* With Retina display on Mac OS X, GLFW's FramebufferSize
	   is different from WindowSize */
	glfwGetFramebufferSize(window, &fbwidth, &fbheight);

	GLfloat fov = 45.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) fbwidth, (GLsizei) fbheight);

	// set the projection matrix as perspective
	/* glMatrixMode (GL_PROJECTION);
	   glLoadIdentity ();
	   gluPerspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1, 500.0); */
	// Store the projection matrix in a variable for future use
	// Perspective projection for 3D views
	Matrices.projection = glm::perspective (fov, (GLfloat) fbwidth / (GLfloat) fbheight, 0.1f, 500.0f);

	// Ortho projection for 2D views
	//	Matrices.projection = glm::ortho(-8.0f, 8.0f, -8.0f, 8.0f, 0.1f, 500.0f);
}

VAO *triangle, *rectangle, *board[105], *person, *obstacle[100];

// Creates the triangle object used in this sample code
void createTriangle ()
{
	/* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

	/* Define vertex array as used in glBegin (GL_TRIANGLES) */
	static const GLfloat vertex_buffer_data [] = {
		0, 1,0, // vertex 0
		-1,-1,0, // vertex 1
		1,-1,0, // vertex 2
	};

	static const GLfloat color_buffer_data [] = {
		1,0,0, // color 0
		0,1,0, // color 1
		0,0,1, // color 2
	};

	// create3DObject creates and returns a handle to a VAO that can be used later
	triangle = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
}

VAO* createPerson(float width,float length,float height,float r,float g, float b)
{
	GLfloat vertex_buffer_data [] = {
		-width/2,-length/2,-height/2, // vertex 1
		width/2,-length/2,-height/2, // vertex 2
		width/2, 1.5*length,height/2, // vertex 3

		width/2, 1.5*length,height/2, // vertex 3
		-width/2, 1.5*length,-height/2, // vertex 4
		-width/2,-length/2,-height/2 , // vertex 1

		width/2,1.5*length,-height/2,
		-width/2,1.5*length,-height/2,  
		-width/2,1.5*length,height/2,

		-width/2,1.5*length,height/2,
		width/2,1.5*length,height/2,
		width/2,1.5*length,-height/2,

		width/2,1.5*length,height/2,
		width/2,1.5*length,-height/2,
		width/2,-length/2,-height/2,

		width/2,-length/2,-height/2,
		width/2,-length/2,height/2,
		width/2,1.5*length,height/2,



		-width/2,-length/2,-height/2,
		width/2,-length/2,-height/2,
		width/2,-length/2,height/2,

		width/2,-length/2,height/2,
		-width/2,-length/2,height/2,
		-width/2,-length/2,-height/2,

		width/2,1.5*length,height/2,
		width/2,-length/2,height/2,
		-width/2,-length/2,height/2,

		-width/2,-length/2,height/2,
		-width/2,1.5*length,height/2,
		width/2,1.5*length,height/2,

		-width/2,-length/2,-height/2,
		-width/2,1.5*length,-height/2, 
		-width/2,1.5*length,height/2,

		-width/2,1.5*length,height/2,
		-width/2,-length/2,height/2,
		-width/2,-length/2,-height/2,
	};
	GLfloat color_buffer_data [200];
	for(int i=0;i<36;i++)
	{
		color_buffer_data[3*i]=r;
		color_buffer_data[(3*i)+1]=g;
		color_buffer_data[(3*i)+2]=b;
	}
	return create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}



VAO* createObstacle(float width,float length,float height,float r,float g,float b)
{
	return createPerson(width,length,height,r,g,b);
}
// Creates the rectangle object used in this sample code
VAO* createBoard (float width,float length, float height,float r, float g, float b)
{
	// GL3 accepts only Triangles. Quads are not supported
	GLfloat vertex_buffer_data [] = {
		-width/2,-length/2,-height/2, // vertex 1
		width/2,-length/2,-height/2, // vertex 2
		width/2, length/2,height/2, // vertex 3

		width/2, length/2,height/2, // vertex 3
		-width/2, length/2,-height/2, // vertex 4
		-width/2,-length/2,-height/2 , // vertex 1

		width/2,length/2,-height/2,
		-width/2,length/2,-height/2,  
		-width/2,length/2,height/2,

		-width/2,length/2,height/2,
		width/2,length/2,height/2,
		width/2,length/2,-height/2,

		width/2,length/2,height/2,
		width/2,length/2,-height/2,
		width/2,-length/2,-height/2,

		width/2,-length/2,-height/2,
		width/2,-length/2,height/2,
		width/2,length/2,height/2,



		-width/2,-length/2,-height/2,
		width/2,-length/2,-height/2,
		width/2,-length/2,height/2,

		width/2,-length/2,height/2,
		-width/2,-length/2,height/2,
		-width/2,-length/2,-height/2,

		width/2,length/2,height/2,
		width/2,-length/2,height/2,
		-width/2,-length/2,height/2,

		-width/2,-length/2,height/2,
		-width/2,length/2,height/2,
		width/2,length/2,height/2,

		-width/2,-length/2,-height/2,
		-width/2,length/2,-height/2, 
		-width/2,length/2,height/2,

		-width/2,length/2,height/2,
		-width/2,-length/2,height/2,
		-width/2,-length/2,-height/2,
	};

	GLfloat color_buffer_data []={

		0,0.6,0,
		0,0.2,0,
		0,1,0,

		0,1,0,
		0.4,1,0.4,
		0,0.6,0,

		0,0.6,0,
		0,0.2,0,
		0,1,0,

		0,1,0,
		0.4,1,0.4,
		0,0.6,0,

		0,0.6,0,
		0,0.2,0,
		0,1,0,

		0,1,0,
		0.4,1,0.4,
		0,0.6,0,

		0,0.6,0,
		0,0.2,0,
		0,1,0,

		0,1,0,
		0.4,1,0.4,
		0,0.6,0,

		0,0.6,0,
		0,0.2,0,
		0,1,0,

		0,1,0,
		0.4,1,0.4,
		0,0.6,0,

		0,0.6,0,
		0,0.2,0,
		0,1,0,

		0,1,0,
		0.4,1,0.4,
		0,0.6,0,


	};

	// create3DObject creates and returns a handle to a VAO that can be used later
	return create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}

float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
	// clear the color and depth in the frame buffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the loaded shader program
	// Don't change unless you know what you are doing
	glUseProgram (programID);

	// Eye - Location of camera. Don't change unless you are sure!!
	glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
	// Target - Where is the camera looking at.  Don't change unless you are sure!!
	glm::vec3 target (0, 0, 0);
	// Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
	glm::vec3 up (0, 1, 0);
	if(advenview==true)
	{
		eyeview=person_posx+0.5;
		targetview=person_posy+2;
		upview=person_posz;
		targetx=person_posx+2;
		targety=0;
		targetz=person_posz;
	}
	if(followview==true)
	{
		eyeview=person_posx-2;
			targetview=person_posy+3;
			upview=person_posz;
			targetx=person_posx+2;
			targety=0;
			targetz=person_posz;		
	}
	if(mousedrag==true)
	{
		//cout<<"curx-> "<<curx<<" cury-> "<<cury<<endl;
		if(curx<=300 && cury<=300)
		{
			eyeview=-(300-curx)/10;
			targetview=9;
			upview=-(300-cury)/10;
		}
		else if(curx>300 && cury<=300)
		{
			eyeview=(curx-300)/10;
			targetview=9;
			upview=-(300-cury)/10;
		}
		else if(curx<=300 && cury>300)
		{
			eyeview=-(300-curx)/10;
			targetview=9;
			upview=(cury-300)/10;
		}
		else
		{
			eyeview=(curx-300)/10;
			targetview=9;
			upview=(cury-300)/10;
		}
		//eyeview=curx;
		//targetview=9;
		//upview=cury;
		targetx=targety=targetz=0;
	}
	
	Matrices.view = glm::lookAt(glm::vec3(eyeview,targetview,upview), glm::vec3(targetx,targety,targetz), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane
	
	glm::mat4 VP = Matrices.projection * Matrices.view;

	glm::mat4 MVP;	// MVP = Projection * View * Model

	
	int index=0,oindex=0;
	for(float i=4.5;i>=-4.5;i--)
	{
		for(float j=4.5;j>=-4.5;j--)
		{
			Matrices.model = glm::mat4(1.0f);

			glm::mat4 translateBlock = glm::translate (glm::vec3(i, 0, j));        // glTranslatef
			//glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
			Matrices.model =translateBlock;
			MVP = VP * Matrices.model;
			glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

			// draw3DObject draws the VAO given to it using current MVP matrix
			//new addition
			for (t=0; t<49; t++)
			{
			if(index==a[t]) 
			{
				b[index]= 1;
				draw3DObject(board[index]);
			}
			else 
				b[index]= 0;
			}

			index++;		
		}
	}
	

	if(person_posx<0 && person_posz<0)
	{
		w1 = -4.5 - person_posx;
		w2 = -4.5 - person_posz;
		
	}
	else if(person_posx<0 && person_posz>0)
	{
		w1 = -4.5 - person_posx;
		w2 = 4.5 - person_posz;
		
	}
	else if(person_posx>0 && person_posz>0)
	{
		w1 = 4.5 - person_posx;
		w2 = 4.5 - person_posz;
		
	}
	else if(person_posx>0 && person_posz<0)
	{
		w1 = 4.5 - person_posx;
		w2 = -4.5 - person_posz;
		
	}
	
	if(person_posy==0)
	{
		person_posy=1;
		person_posz=-4.5;
		person_posx=-4.5; 
		r1 =0; r2 =1; rot_ang=0;
	}
		
	Matrices.model = glm::mat4(1.0f);

	glm::mat4 translatePerson = glm::translate (glm::vec3(person_posx, person_posy,person_posz ));        // glTranslatef    
	//  glm::mat4 scaleRectangle5 = glm::scale (glm::vec3(1.4, 1.4, 0.0));
	//glm::mat4 tr0= glm::translate (glm::vec3(0, -1, 0));
	glm::mat4 tr= glm::translate (glm::vec3(-person_posx, -1, -person_posz));        // glTranslatef
	glm::mat4 rr = glm::rotate((float)(rot_ang*M_PI/180.0f), glm::vec3(r1,0,r2)); // rotate about vector (-1,1,1)
	glm::mat4 tr1 = glm::translate (glm::vec3(person_posx, 1, person_posz));
	//glm::mat4 tr00= glm::translate (glm::vec3(0, 1, 0));
	Matrices.model *= (  tr1*rr*tr*translatePerson  );
	//Matrices.model *= (  translatePerson  );
	MVP = VP * Matrices.model;
	glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// draw3DObject draws the VAO given to it using current MVP matrix
	draw3DObject(person);
	//cout<<person_posx<<" "<<person_posz<<endl;


	if(person_posy==0)
	{
		person_posy=1;
		person_posz=-4.5;
		person_posx=-4.5;
		w1 = 4.5; w2 = 4.5; 
		r1 =0; r2 =0; rot_ang=0;
	}


//new addition//
	float increments = 1;

	//camera_rotation_angle++; // Simulating camera rotation
	triangle_rotation = triangle_rotation + increments*triangle_rot_dir*triangle_rot_status;
	rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;
}

/* Initialise glfw window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
GLFWwindow* initGLFW (int width, int height)
{
	GLFWwindow* window; // window desciptor/handle

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Game", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval( 1 );

	/* --- register callbacks with GLFW --- */

	/* Register function to handle window resizes */
	/* With Retina display on Mac OS X GLFW's FramebufferSize
	   is different from WindowSize */
	glfwSetFramebufferSizeCallback(window, reshapeWindow);
	glfwSetWindowSizeCallback(window, reshapeWindow);

	/* Register function to handle window close */
	glfwSetWindowCloseCallback(window, quit);

	/* Register function to handle keyboard input */
	glfwSetKeyCallback(window, keyboard);      // general keyboard input
	glfwSetCharCallback(window, keyboardChar);  // simpler specific character handling

	/* Register function to handle mouse click */
	glfwSetMouseButtonCallback(window, mouseButton);  // mouse button clicks

	return window;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (GLFWwindow* window, int width, int height)
{
	/* Objects should be created before any other gl function and shaders */
	// Create the models
	//createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
	for(int i=0;i<100;i++)
	{
		board[i]=createBoard (1,1,1,0,0,0);
	}
	
	person=createPerson(1,1,1,0,0,0.5);
	
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (window, width, height);

	// Background color of the scene
	glClearColor (0.0f, 0.0f, 0.8f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	int width = 600;
	int height = 600;
	int score=30;

	GLFWwindow* window = initGLFW(width, height);

	initGL (window, width, height);

	double last_update_time = glfwGetTime(), current_time;

	/* Draw in loop */
	while (!glfwWindowShouldClose(window)) {

		// OpenGL Draw commands
		draw();

		// Swap Frame Buffer in double buffering
		glfwSwapBuffers(window);
		glfwGetCursorPos(window,&curx,&cury);

		// Poll for Keyboard and mouse events
		glfwPollEvents();

		// Control based on time (Time based transformation like 5 degrees rotation every 0.5s)
		current_time = glfwGetTime(); // Time in seconds
		if ((current_time - last_update_time) >= 1) { // atleast 0.5s elapsed since last frame
			// do something every 0.5 seconds ..
			last_update_time = current_time;
		}
		if(lives==0)
		{
			cout<<"LOST THE GAME"<<endl;
			break;
		}
		if(block_pos==41)
		{
			cout<<"YOU WON THE GAME\nYour score is "<<score-(3-lives)*10<<endl;
			break;
		}
	}

	glfwTerminate();
	exit(EXIT_SUCCESS);
}
