#include "CDT.h"

Triangle::Triangle(Edge* e1, Edge* e2, Edge* e3)
{
	this->type = 0;
	this->edge[0] = e1;
	this->edge[1] = e2;
	this->edge[2] = e3;
	// pi is the opposite point of ei
	for(int i = 0; i < 3; i++)
	{
		if(this->edge[(i+1)%3]->p1 == this->edge[(i+2)%3]->p1 ||
		   this->edge[(i+1)%3]->p1 == this->edge[(i+2)%3]->p2)
			this->point[i] = this->edge[(i+1)%3]->p1;
		else
			this->point[i] = this->edge[(i+1)%3]->p2;
	}
	// Calculate midpoint of triangle
	for(int i = 0; i < 3; i++)
	{
		this->midPoint[i] = 0;
		for(int j = 0; j < 3; j++)
			this->midPoint[i] += this->point[j]->p[i];
		this->midPoint[i] /= 3.0f;
	}
}

void Triangle::beAdd(PointSet* ps, std::list<Triangle*>::iterator it)
{
	this->tit = it;
	this->parent = ps;
}

bool Triangle::beLink()
{
	for(int i = 0; i < 3; i++)
	{
		if(this->point[i]->addTriNeighbor(this) == false)
			return false;
	}
	return true;
}

void Triangle::unLink()
{
	for(int i = 0; i < 3; i++)
	{
		if(this->edge[i] == NULL)
			continue;
		this->edge[i]->p1->removeTriangle(this->edge[i]->e1, this);
		this->edge[i]->p2->removeTriangle(this->edge[i]->e2, this);
	}
	this->parent->triSet.erase(this->tit);
}

void Triangle::remove(Edge *e)
{
	if(this->edge[0] == e)
		this->edge[0] = NULL;
	else if(this->edge[1] == e)
		this->edge[1] = NULL;
	else if(this->edge[2] == e)
		this->edge[2] = NULL;
}

void Triangle::vote(int score)
{
	// Score: Internal edge = 0; External edge = 1; J = 0; S = 1; T = 2;
	this->type += score;
}

Edge* Triangle::oppositeEdge(Point* p)
{
	for(int i = 0; i < 3; i++)
	{
		if(p == this->point[i])
			return this->edge[i];
	}
	return NULL;
}

Point* Triangle::oppositePoint(Edge* e)
{
	for(int i = 0; i < 3; i++)
	{
		if(e == this->edge[i])
			return this->point[i];
	}
	return NULL;
}

void Triangle::calNormal()
{
	float v1[3], v2[3];
	for(int i = 0; i < 3; i++)
	{
		v1[i] = this->point[1]->p[i] - this->point[2]->p[i];
		v2[i] = this->point[0]->p[i] - this->point[2]->p[i];
	}
	this->normal[0] = v1[1]*v2[2] - v1[2]*v2[1];
	this->normal[1] = v1[2]*v2[0] - v1[0]*v2[2];
	this->normal[2] = v1[0]*v2[1] - v1[1]*v2[0];
	// Z+ triangle have Z+ normal, suck solution!!!???
	if(this->midPoint[2] * this->normal[2] < 0)
	{
		// Even if we refine the surface, the Z+ triangles are still Z+
		// So, no need to recalculate midPoint
		for(int i = 0; i < 3; i++)
			this->normal[i] = -normal[i];
	}
}