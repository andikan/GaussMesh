#include "CDT.h"

using namespace std;

Edge::Edge(Point* _p1, Point* _p2)
{
	this->p1 = _p1;
	this->p2 = _p2;
	this->type = Internal;
}

void Edge::beLink()
{
	this->e1 = this->p1->addNeighbor(this);
	this->e2 = this->p2->addNeighbor(this);
}

void Edge::unLink()
{
	// tL, tR of e1 and e2 are the same
	Triangle *tL = this->e1->tL;
	Triangle *tR = this->e1->tR;
	// Delete neighbor e1, e2 of p1, p2
	this->p1->removeNeighbor(e1);
	this->p2->removeNeighbor(e2);
	// Delete triangle tL and tR
	if(tL != NULL)
	{
		tL->remove(this);
		tL->unLink();
		delete tL;
	}
	if(tR != NULL)
	{
		tR->remove(this);
		tR->unLink();
		delete tR;
	}
}

void Edge::setType(int _type)
{
	this->type = _type;
	if(e1->tL != NULL)
		this->e1->tL->vote(_type);
	if(e1->tR != NULL)
		this->e1->tR->vote(_type);
}
