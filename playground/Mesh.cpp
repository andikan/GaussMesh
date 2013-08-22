#include "CDT.h"
#include "Ellipse.h"
#ifndef GLFW_DEFINED
#define GLFW_DEFINED
#include <GL\glfw.h>
#endif

using namespace std;

void Mesh::loadEdgeFromMouse(vector<float> point)
{
	// Allocate buffer for each point: pos, color, normal
	this->release();
	this->pNum = point.size() / 3;
	this->vertexBuffer = new float[this->pNum*9];
	// Load data into buffer in the form of "pos, color, normal, pos, color, normal, ..."
	int vbp = 0;
	for(uint i = 0; i < this->pNum; i++)
	{
		for(int j = 0; j < 3; j++)
			this->vertexBuffer[vbp++] = point[i*3+j];
		for(int j = 0; j < 3; j++)
			this->vertexBuffer[vbp++] = 1;
		this->vertexBuffer[vbp++] = 1;
		this->vertexBuffer[vbp++] = 0;
		this->vertexBuffer[vbp++] = 0;
	}
	// Load model infomation into opengl buffer
	glGenBuffers(1, &this->VBid);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBid);	// Bind VBid to ARRAY_BUFFER
	// Allocate space on GPU and copy data to there
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*this->pNum*9, this->vertexBuffer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbound ARRAY_BUFFER
	this->assm = GL_LINE_LOOP;
}

void Mesh::loadTriangleFromPointSet(const PointSet& ps)
{
	// Allocate buffer for each vertex of trangle: pos, color, normal(3*3*3 floats)
	this->release();
	this->pNum = ps.triSet.size() * 3;
	this->vertexBuffer = new float[this->pNum*9];
	// Load data into buffer in the form of "pos, color, normal, pos, color, normal, ..."
	int vbp = 0;
	list<Triangle*>::const_iterator it;
	for(it = ps.triSet.begin(); it != ps.triSet.end(); it++)
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
				this->vertexBuffer[vbp++] = (*it)->point[i]->p[j];
			this->vertexBuffer[vbp++] = 0.5f;
			this->vertexBuffer[vbp++] = 0.5f;
			this->vertexBuffer[vbp++] = 0.5f;
			for(int j = 0; j < 3; j++)
				this->vertexBuffer[vbp++] = (*it)->point[i]->n[j];
		}
	}
	// Load model infomation into opengl buffer
	glGenBuffers(1, &this->VBid);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBid);	// Bind VBid to ARRAY_BUFFER
	// Allocate space on GPU and copy data to there
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*this->pNum*9, this->vertexBuffer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbound ARRAY_BUFFER
	this->assm = GL_TRIANGLES;
}

void Mesh::loadEdgeFromPointSet(const PointSet& ps)
{
	list<Edge*> edgeList;
	list<Point*>::const_iterator pit;
	NeighborLit nit;
	for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
	{
		for(nit = (*pit)->neighbor.begin(); nit != (*pit)->neighbor.end(); nit++)
			edgeList.push_back(nit->e);
	}
	edgeList.unique();
	// Allocate buffer for each vertex of trangle: pos, color, normal(3*3*3 floats)
	this->release();
	this->pNum = edgeList.size() * 2;
	this->vertexBuffer = new float[this->pNum*9];
	// Load data into buffer in the form of "pos, color, normal, pos, color, normal, ..."
	int vbp = 0;
	list<Edge*>::iterator eit;
	for(eit = edgeList.begin(); eit != edgeList.end(); eit++)
	{
		// Point1
		for(int i = 0; i < 3; i++)
			this->vertexBuffer[vbp++] = (*eit)->p1->p[i];
		this->vertexBuffer[vbp++] = ((*eit)->type == 1) ? 0.0f : 1.0f;
		this->vertexBuffer[vbp++] = ((*eit)->type == 1) ? 0.0f : 1.0f;
		this->vertexBuffer[vbp++] = 1.0f;
		this->vertexBuffer[vbp++] = 0.0f;
		this->vertexBuffer[vbp++] = 0.0f;
		this->vertexBuffer[vbp++] = 1.0f;
		// Point2
		for(int i = 0; i < 3; i++)
			this->vertexBuffer[vbp++] = (*eit)->p2->p[i];
		this->vertexBuffer[vbp++] = ((*eit)->type == 1) ? 0.0f : 1.0f;
		this->vertexBuffer[vbp++] = ((*eit)->type == 1) ? 0.0f : 1.0f;
		this->vertexBuffer[vbp++] = 1.0f;
		this->vertexBuffer[vbp++] = 0.0f;
		this->vertexBuffer[vbp++] = 0.0f;
		this->vertexBuffer[vbp++] = 1.0f;
	}
	// Load model infomation into opengl buffer
	glGenBuffers(1, &this->VBid);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBid);	// Bind VBid to ARRAY_BUFFER
	// Allocate space on GPU and copy data to there
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*this->pNum*9, this->vertexBuffer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	// Unbound ARRAY_BUFFER
	this->assm = GL_LINES;
}

Mesh::Mesh()
{
	this->vertexBuffer = NULL;
	this->assm = GL_TRIANGLES;
}

Mesh::~Mesh()
{
}

void Mesh::release()
{
	if(this->vertexBuffer != NULL)
	{
		delete this->vertexBuffer;
		glDeleteBuffers(1, &this->VBid);
		this->vertexBuffer = NULL;
		this->pNum = 0;
	}
}

void Mesh::runCDT()
{
	PointSet *ps = new PointSet();
	for(uint i = 0; i < this->pNum; i++)
		ps->addPoint(new Point(i, this->vertexBuffer[i*9], this->vertexBuffer[i*9+1]));
	// Start with pointSet sorted by x
	ps->sortPSet(X);
	this->ps = ps;
	
	printf("Start DT\n");
	CDT(*ps);
	#ifdef MESHSHADER
	this->loadEdgeFromPointSet(*ps);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->shader->render(GL_ARRAY_BUFFER, this->VBid, this->assm, 0, this->pNum, true);
	glfwSwapBuffers();
	printf("End DT\n");
	system("pause");
	#endif
	
	printf("Start Constrained\n");
	boundConstraint(*ps);
	#ifdef MESHSHADER
	this->loadEdgeFromPointSet(*ps);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->shader->render(GL_ARRAY_BUFFER, this->VBid, this->assm, 0, this->pNum, true);
	glfwSwapBuffers();
	printf("End Constrained\n");
	system("pause");
	#endif
	
	printf("Creating Spine\n");
	int baseID = pruneAndSpine(*ps);
	#ifdef MESHSHADER
	this->loadEdgeFromPointSet(*ps);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->shader->render(GL_ARRAY_BUFFER, this->VBid, this->assm, 0, this->pNum, true);
	glfwSwapBuffers();
	printf("End Creating\n");
	system("pause");
	#endif
	
	printf("Bubbling Up\n");
	vector<Point*> spine = getSpine(*ps, baseID);
	bubbleUp(*ps, spine, 1);
	bubbleUp(*ps, spine, -1);
	#ifdef MESHSHADER
	this->loadEdgeFromPointSet(*ps);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->shader->render(GL_ARRAY_BUFFER, this->VBid, this->assm, 0, this->pNum, true);
	glfwSwapBuffers();
	printf("End Bubbling\n");
	system("pause");
	#endif

	cleanSpineBase(*ps, spine);
	setAllPointNormal(*ps);
	printf("Finished\n");
	
	this->loadTriangleFromPointSet(*ps);
}