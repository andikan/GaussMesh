//
//  Mesh.cpp
//  GaussMesh
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Mesh_structure.h"
#include "Triangulation.h"

using namespace std;

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
		this->vertexBuffer = NULL;
		this->pNum = 0;
        //glDeleteBuffers(1, &this->VBid);
	}
}

void Mesh::loadEdgeFromMouse(vector<glm::vec3> point)
{
	// Allocate buffer for each point: pos
	this->release();
	this->pNum = (int)(point.size());
	this->vertexBuffer = new float[this->pNum*3];
	// Load data into buffer in the form of "pos.x, pos.y, pos.z, pos.x, ..."
	for(uint i = 0; i < this->pNum; i++)
	{
        this->vertexBuffer[i*3] = (float)point[i].x;
        this->vertexBuffer[i*3+1] = (float)point[i].y;
        this->vertexBuffer[i*3+2] = (float)point[i].z;
    }
    this->drawMode = GL_POINTS;
}

void Mesh::loadAndSortPointSet()
{
    this->ps = new PointSet();
	for(uint i = 0; i < this->pNum; i++)
    {
		this->ps->addPoint(new Point(i, this->vertexBuffer[i*3], this->vertexBuffer[i*3+1], this->vertexBuffer[i*3+2]));
    }
	// Start with pointSet sorted by x
	this->ps->sortPSet(X);
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
	this->vertexBuffer = new float[this->pNum*3];
	// Load data into buffer in the form of "pos, color, normal, pos, color, normal, ..."
	int vbp = 0;
	list<Edge*>::iterator eit;
	for(eit = edgeList.begin(); eit != edgeList.end(); eit++)
	{
		// Point1
		for(int i = 0; i < 3; i++)
			this->vertexBuffer[vbp++] = (*eit)->p1->p[i];
		// Point2
		for(int i = 0; i < 3; i++)
			this->vertexBuffer[vbp++] = (*eit)->p2->p[i];
	}
    this->drawMode = GL_LINES;
}

void Mesh::delaunayTriangulation()
{
    cout << "start triangulation" << endl;
    CDT(*(this->ps));
}

void Mesh::constraintBound()
{
    cout << "start constraintBound" << endl;
    boundConstraint(*(this->ps));
}

std::vector<Point*> Mesh::getSpinePoints(PointSet &ps)
{
    cout << "get spine" << endl;
    
    int baseID = ps.pSet.size();
    std::vector<Point*> spine;
    list<Triangle*>::const_iterator it;
	vector<vector<Point*>> pListList;
	vector<vector<Edge*>> eListList;
	vector<Point*> endPointList;
	vector<Point*> toBeLinkList;
	vector<Triangle*> visitedJTriangleList;
	for(it = ps.triSet.begin(); it != ps.triSet.end(); it++)
	{
		if((*it)->type == TRITYPE::T){		// start from T-triangle
			vector<Point*> pList;	// connect to end point
			vector<Edge*> eList;	// edges that need to be removed
			Point *endPoint = new Point(ps.pSet.size(), 0, 0, 0);
			float radius;
			Edge *currentEdge;
			Triangle* delTri = (*it);
			for(int i = 0; i < 3; i++)
				if((*it)->edge[i]->type == Internal){
					currentEdge = (*it)->edge[i];
					endPoint->p[0] = ((*it)->edge[i]->p1->p[0] + (*it)->edge[i]->p2->p[0]) / 2;
					endPoint->p[1] = ((*it)->edge[i]->p1->p[1] + (*it)->edge[i]->p2->p[1]) / 2;
					radius = pow(pow((*it)->edge[i]->p1->p[0] - (*it)->edge[i]->p2->p[0], 2) + pow((*it)->edge[i]->p1->p[1] - (*it)->edge[i]->p2->p[1], 2), 0.5f) / 2;
				}
			pList.push_back((*it)->oppositePoint(currentEdge));
			pList.push_back(currentEdge->p1);
			pList.push_back(currentEdge->p2);
			while(1){
				eList.push_back(currentEdge);
				bool stop = false;
				for(int i = 0; i < pList.size(); i++)	// stop condition, some vertex lies outside the semicircle
					if(pow(pow(endPoint->p[0] - pList[i]->p[0], 2) + pow(endPoint->p[1] - pList[i]->p[1], 2), 0.5f) > radius){
						stop = true;
						break;
					}
				delTri = (currentEdge->e1->tL == delTri) ? currentEdge->e1->tR : currentEdge->e1->tL;
				if(stop){
					for(int i = pList.size() - 1; i >= 0; i--)
						if(pList[i] == currentEdge->p1)
							pList.erase(pList.begin() + i);
					for(int i = pList.size() - 1; i >= 0; i--)
						if(pList[i] == currentEdge->p2)
							pList.erase(pList.begin() + i);
					eList.pop_back();
					break;
				}
				if(delTri->type == J){					// stop condition, advanced to junction triangle
					int i;
					for(i = 0; i < visitedJTriangleList.size(); i++)
						if(delTri == visitedJTriangleList[i])
							break;
					if(i == visitedJTriangleList.size())
						visitedJTriangleList.push_back(delTri);
					for(i = 0; i < spine.size(); i++)
						if(endPoint->p[0] == spine[i]->p[0] && endPoint->p[1] == spine[i]->p[1])
							break;
					Point *preEndPoint = new Point(endPoint->id, endPoint->p[0], endPoint->p[1], 0);
					for(int j = 0; j < 3; j++)
						endPoint->p[j] = delTri->midPoint[j];
					if(i != spine.size()){	// visit J triangle twice
						for(int i = pList.size() - 1; i >= 0; i--)
							if(pList[i] == currentEdge->p1)
								pList.erase(pList.begin() + i);
						for(int i = pList.size() - 1; i >= 0; i--)
							if(pList[i] == currentEdge->p2)
								pList.erase(pList.begin() + i);
						spine.erase(spine.begin() + i, spine.begin() + i + 2);
					} else{
						pList.push_back(delTri->oppositePoint(currentEdge));
						for(int i = 0; i < 3; i++)
							if(delTri->edge[i]->type == Internal && delTri->edge[i] != currentEdge){
								Point* tmp = new Point(ps.pSet.size(), 0, 0, 0);
								for(int j = 0; j < 3; j++)
									tmp->p[j] = (delTri->edge[i]->p1->p[j] + delTri->edge[i]->p2->p[j]) / 2;
								spine.push_back(tmp);
								spine.push_back(endPoint);
							}
					}
					break;
				} else{									// edge advancing
					int currentEdgeIndex;
					for(int i = 0; i < 3; i++)
						if(delTri->edge[i]->type == Internal){
							if(delTri->edge[i] != currentEdge){
								currentEdgeIndex = i;
								for(int j = 0; j < 3; j++)
									endPoint->p[j] = (delTri->edge[i]->p1->p[j] + delTri->edge[i]->p2->p[j]) / 2;
								radius = pow(pow(delTri->edge[i]->p1->p[0] - delTri->edge[i]->p2->p[0], 2) + pow(delTri->edge[i]->p1->p[1] - delTri->edge[i]->p2->p[1], 2), 0.5f) / 2;
							} else
								pList.push_back(delTri->oppositePoint(delTri->edge[i]));
						}
					currentEdge = delTri->edge[currentEdgeIndex];
				}
			}
			pListList.push_back(pList);
			eListList.push_back(eList);
			endPointList.push_back(endPoint);
		}
	}
	for(int i = 0; i < pListList.size(); i++){
		for(int j = 0; j < eListList[i].size(); j++)
			eListList[i][j]->unLink();
	}
	// handle the untouched triangles
	vector<Edge*> delEdgeList;
	for(it = ps.triSet.begin(); it != ps.triSet.end(); it++)
	{
		if((*it)->type == TRITYPE::S){
			Point* tmp[2];
			tmp[0] = new Point(ps.pSet.size(), 0, 0, 0);
			tmp[1] = new Point(ps.pSet.size(), 0, 0, 0);
			int num = 0;
			for(int i = 0; i < 3; i++)
				if((*it)->edge[i]->type == Internal){
					tmp[num]->p[0] = ((*it)->edge[i]->p1->p[0] + (*it)->edge[i]->p2->p[0]) / 2;
					tmp[num]->p[1] = ((*it)->edge[i]->p1->p[1] + (*it)->edge[i]->p2->p[1]) / 2;
					int j;
					for(j = 0; j < delEdgeList.size(); j++)
						if((*it)->edge[i] == delEdgeList[j])
							break;
					if(j == delEdgeList.size()){	// untouched edge
						delEdgeList.push_back((*it)->edge[i]);
						toBeLinkList.push_back(tmp[num]);
						toBeLinkList.push_back((*it)->edge[i]->p1);
						toBeLinkList.push_back(tmp[num]);
						toBeLinkList.push_back((*it)->edge[i]->p2);
					}
					if(num == 0){
						toBeLinkList.push_back(tmp[num]);
						toBeLinkList.push_back((*it)->oppositePoint((*it)->edge[i]));
					}
					num++;
				}
			spine.push_back(tmp[0]);
			spine.push_back(tmp[1]);
		} else if((*it)->type == TRITYPE::J){
			int i;
			for(i = 0; i < visitedJTriangleList.size(); i++)
				if((*it) == visitedJTriangleList[i])
					break;
			if(i == visitedJTriangleList.size()){
				Point* tmp = new Point(ps.pSet.size(), 0, 0, 0);
				for(int j = 0; j < 3; j++)
					tmp->p[j] = (*it)->midPoint[j];
				vector<Point*> tmp_mid;
				tmp_mid.push_back(new Point(ps.pSet.size(), 0, 0, 0));
				tmp_mid.push_back(new Point(ps.pSet.size(), 0, 0, 0));
				tmp_mid.push_back(new Point(ps.pSet.size(), 0, 0, 0));
				int j;
				for(int a = 0; a < 3; a++){
					for(int j = 0; j < 3; j++)
						tmp_mid[a]->p[j] = ((*it)->edge[a]->p1->p[j] + (*it)->edge[a]->p2->p[j]) / 2;
					spine.push_back(tmp);
					spine.push_back(tmp_mid[a]);
					toBeLinkList.push_back(tmp);
					toBeLinkList.push_back((*it)->point[a]);
					for(j = 0; j < delEdgeList.size(); j++)
						if((*it)->edge[a] == delEdgeList[j])
							break;
					if(j == delEdgeList.size()){	// untouched edge
						delEdgeList.push_back((*it)->edge[a]);
						toBeLinkList.push_back(tmp_mid[a]);
						toBeLinkList.push_back((*it)->edge[a]->p1);
						toBeLinkList.push_back(tmp_mid[a]);
						toBeLinkList.push_back((*it)->edge[a]->p2);
					}
				}
			}
		}
	}
	for(int i = 0; i < delEdgeList.size(); i++)
		delEdgeList[i]->unLink();
	for(int i = 0; i < spine.size(); i += 2){
		list<Point*>::const_iterator pit;
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
			if((*pit)->p[0] == spine[i]->p[0] && (*pit)->p[1] == spine[i]->p[1]){
				spine[i] = (*pit);
				break;
			}
		if(pit == ps.pSet.end()){
			spine[i]->id = ps.pSet.size();
			ps.addPoint(spine[i]);
		}
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
			if((*pit)->p[0] == spine[i + 1]->p[0] && (*pit)->p[1] == spine[i + 1]->p[1]){
				spine[i + 1] = (*pit);
				break;
			}
		if(pit == ps.pSet.end()){
			spine[i + 1]->id = ps.pSet.size();
			ps.addPoint(spine[i + 1]);
		}
		link(spine[i], spine[i + 1]);
		NeighborLit it;
		for(it = spine[i]->neighbor.begin(); it != spine[i]->neighbor.end(); it++)
			it->e->setType(External);
	}
	for(int i = 0; i < toBeLinkList.size(); i += 2){
		list<Point*>::const_iterator pit;
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
			if((*pit)->p[0] == toBeLinkList[i]->p[0] && (*pit)->p[1] == toBeLinkList[i]->p[1]){
				toBeLinkList[i] = (*pit);
				break;
			}
		link(toBeLinkList[i], toBeLinkList[i + 1]);
	}
	for(int i = 0; i < pListList.size(); i++){
		list<Point*>::const_iterator pit;
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
			if((*pit)->p[0] == endPointList[i]->p[0] && (*pit)->p[1] == endPointList[i]->p[1])	// same end point in J triangle
				break;
		if(pit == ps.pSet.end()){
			endPointList[i]->id = ps.pSet.size();
			ps.addPoint(endPointList[i]);
			for(int j = 0; j < pListList[i].size(); j++)
				link(endPointList[i], pListList[i][j]);
		} else
			for(int j = 0; j < pListList[i].size(); j++)
				link((*pit), pListList[i][j]);
	}
    
    ps.sortPSet(ID);
	list<Point*>::iterator pit;
	for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
	{
		if((*pit)->id >= baseID)
			spine.push_back(*pit);
	}
    
	return spine;
}

