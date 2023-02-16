#include "framework.h"
#include <time.h>       
 
 
// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char* const vertexSource = R"(
	#version 330				// Shader 3.3
	precision highp float;		// normal floats, makes no difference on desktop computers
 
	uniform mat4 MVP;			// uniform variable, the Model-View-Projection transformation matrix
	layout(location = 0) in vec2 vp;	// Varying input: vp = vertex position is expected in attrib array 0
 
	void main() {
		gl_Position = vec4(vp.x, vp.y, 0, 1) * MVP;		// transform vp from modeling space to normalized device space
	}
)";
 
// fragment shader in GLSL
const char* const fragmentSource = R"(
	#version 330			// Shader 3.3
	precision highp float;	// normal floats, makes no difference on desktop computers
	
	uniform vec3 color;		// uniform variable, the color of the primitive
	out vec4 outColor;		// computed color of the current pixel
 
	void main() {
		outColor = vec4(color, 1);	// computed color is the color of the primitive
	}
)";
 
 
 
GPUProgram gpuProgram; 
const int totalpoints = 100;  ///No of dots a not consist of
 
class Graph
{
	unsigned int nodesVao;     
	unsigned int nodesVbo;     
	unsigned int edgesVao;
	unsigned int edgesVbo;
	vec2 noOfNodes[50];   //Vector2 array to sotre 50 nodess
	vec2 pointsArray[totalpoints]; ////Array to store no of dots a node contained
	vec2 noOfEdges[122];           //// Vector2 array to store edges
	int connectedNodes[122];       ///Array used to connect the nodes to edges
	int graph[50][2];
 
public:
 
 
 
	void nodeCreation()
	{
		glGenVertexArrays(1, &nodesVao);
		glBindVertexArray(nodesVao);
 
		glGenBuffers(1, &nodesVbo);
		glBindBuffer(GL_ARRAY_BUFFER, nodesVbo);
 
		for (int i = 0; i < totalpoints;i++)
		{
			pointsArray[i] = vec2(cosf((2 * M_PI * i) / totalpoints), sinf((2 * M_PI * i) / totalpoints));
 
			noOfNodes[(i)/ 2] = vec2((((-1) - 1) * ((float)rand() / RAND_MAX)) + 1, ///I got help to randomly generate the x and y coordinate
				(((-1) - 1) * ((float)rand() / RAND_MAX)) + 1);                        //// form the internet and source is https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
		}
 
 
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * totalpoints, noOfNodes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}
 
	void drawNode()
	{
 
		vec3 colorArray[50];    //////Array to store 50 different colors for 50 nodes
 
		for (int i = 0;i < 50;i++)
		{
			colorArray[i] = vec3(
				(0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)))),  // I used this random generator function from the internet and the source is below
				(0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0)))),  //////https://www.delftstack.com/howto/cpp/cpp-random-number-between-0-and-1/
				(0 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0))))   //
			);
		}
 
		for (int i = 0; i < 50; i++)
		{
			int location = glGetUniformLocation(gpuProgram.getId(), "color");
			glUniform3f(location, colorArray[i].x, colorArray[i].y, colorArray[i].z); //Assigning random colors to the nodes
			float MVPtransf[4][4] = { 0.028f, 0, 0, 0,
									  0, 0.028f, 0, 0,
									  0, 0, 0, 0,
									  noOfNodes[i].x, noOfNodes[i].y, 0, 1 };
 
			location = glGetUniformLocation(gpuProgram.getId(), "MVP");	
			glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);	
 
			glBindVertexArray(nodesVao);  
			glDrawArrays(GL_TRIANGLE_FAN, 0, totalpoints);
		}
	}
 
 
	void edgesCreation()
	{
		glGenVertexArrays(1, &edgesVao);
		glBindVertexArray(edgesVao);
		glGenBuffers(1, &edgesVbo);
		glBindBuffer(GL_ARRAY_BUFFER, edgesVbo);
 
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 122, noOfEdges, GL_DYNAMIC_DRAW);
 
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		gpuProgram.create(vertexSource, fragmentSource, "outColor");
 
	}
 
 
	void edgesDrawing()
	{
 
		int location = glGetUniformLocation(gpuProgram.getId(), "color");
		glUniform3f(location, 1.0f, 1.0f, 0.2f);
 
		float MVPtransf[4][4] = { 1, 0, 0, 0,
								  0, 1, 0, 0,
								  0, 0, 1, 0,
								  0, 0, 0, 1 };
 
		location = glGetUniformLocation(gpuProgram.getId(), "MVP");
		glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);
 
		glBindVertexArray(edgesVao);  
		glDrawArrays(GL_LINES, 0, 122);
	}
 
 
 
	void connectingEdgesToNodes()
	{
		for (int i = 0;i < 122;i++)
		{
		  int randomNode =  rand() % 50;
 
		  noOfEdges[i].x = noOfNodes[randomNode].x;
		  noOfEdges[i].y = noOfNodes[randomNode].y;
		  connectedNodes[i] = randomNode;
 
		  while ((i > 0) && (randomNode == connectedNodes[i - 1]))
		  {
			  randomNode = rand() % 50;  
			  noOfEdges[i].x = noOfNodes[randomNode].x;
			  noOfEdges[i].y = noOfNodes[randomNode].y;
			  connectedNodes[i] = randomNode;
		  }
		}
 
		for (int j = 0; j < 122;j += 2)
		{
			if (connectedNodes[j] > connectedNodes[j + 1])
			{
				int tempNode = connectedNodes[j];
				connectedNodes[j] = connectedNodes[j + 1];
				connectedNodes[j + 1] = tempNode;
 
				vec2 tempEdge;
				tempEdge.x = noOfEdges[j].x;
				tempEdge.y = noOfEdges[j].y;
 
				noOfEdges[j].x = noOfEdges[j + 1].x;
				noOfEdges[j].y = noOfEdges[j + 1].y;
				noOfEdges[j + 1].x = tempEdge.x;
				noOfEdges[j + 1].y = tempEdge.y;
			}
		}
	}
 
 
	void placeGraph()
	{
		for (int i = 0;i < 50;i++)
		{
			graph[i][0] = i;
			graph[i][1] = 0;
		}
 
		for (int x = 0; x < 50; x++)
		{
			for (int y = 0; y < 122; y++)
			{
				if (graph[x][0] == connectedNodes[y])
				{
					graph[x][x]++;
				}
			}
		}
		for (int i = 49; i > 0; --i)
		{
			for (int j = 0; j < i; ++j)
			{
				if (graph[j][1] > graph[j + 1][1])
				{
					int node1 = graph[j][0];
					int node2 = graph[j][1];
 
					graph[j][0] = graph[j + 1][0];
					graph[j][1] = graph[j + 1][1];
 
					graph[j + 1][0] = node1;
					graph[j + 1][1] = node2;
 
				}
			}
		}
	}
 
 
 
	void arrangementOfGraphAfterMouceClick()
	{
		vec2 arrangedNodes[50];
		for (int i = 0; i < 49; i++)
		{
			float angle = (2 * 3.14 * i) / 49;
			arrangedNodes[i] = vec2(cosf(angle), sinf(angle));
		}
		arrangedNodes[49] = vec2(0.0, 0.0);
 
		for (int i = 0; i < 50;i++)
		{
			noOfNodes[graph[i][0]].x = arrangedNodes[i].x;
			noOfNodes[graph[i][0]].y = arrangedNodes[i].y;
		}
 
		for (int i = 0;i < 122;i++)
		{
			noOfEdges[i].x = noOfNodes[connectedNodes[i]].x;
			noOfEdges[i].y = noOfNodes[connectedNodes[i]].y;
		}
	}
 
};
 
void onInitialization()
{
	srand(time(NULL));
	glViewport(0, 0, windowWidth, windowHeight);
 
	Graph graph;
 
	graph.nodeCreation();
	graph.drawNode();
	graph.connectingEdgesToNodes();
	graph.edgesCreation();
	graph.placeGraph();
 
	gpuProgram.create(vertexSource, fragmentSource, "outColor");
}
 
 
void onDisplay()
{
	Graph graph;
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, windowWidth, windowHeight);
 
	graph.edgesCreation();
	graph.edgesDrawing();
	graph.drawNode();
 
	glutSwapBuffers();
}
 
 
 
// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY)
{
	if (key == 'd') glutPostRedisplay();         // if d, invalidate display, i.e. redraw
}
 
// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY)
{
}
 
// Move mouse with key pressed
void onMouseMotion(int pX, int pY)
{	// pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;
	printf("Mouse moved to (%3.2f, %3.2f)\n", cX, cY);
}
 
// Mouse click event
void onMouse(int button, int state, int pX, int pY)
{ // pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;
 
	Graph graph;
 
	char* buttonStat;
	switch (state)
	{
	case GLUT_DOWN: 
		buttonStat = "pressed"; 
	    graph.arrangementOfGraphAfterMouceClick();  
	    break;
	case GLUT_UP:  
		buttonStat = "released";
	    break;
	}
 
	switch (button)
	{
	case GLUT_LEFT_BUTTON:   printf("Left button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY);   break;
	case GLUT_MIDDLE_BUTTON: printf("Middle button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY); break;
	case GLUT_RIGHT_BUTTON:  printf("Right button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY);  break;
	}
}
 
// Idle event indicating that some time elapsed: do animation here
void onIdle()
{
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
	long newTime = time;
 
	time = glutGet(GLUT_ELAPSED_TIME);
	newTime = time - newTime;
 
	glutPostRedisplay();
}
 