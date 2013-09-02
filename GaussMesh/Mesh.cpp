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
    this->release();
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
        {
			this->vertexBuffer[vbp++] = (*eit)->p1->p[i];
        }
		// Point2
		for(int i = 0; i < 3; i++)
        {
			this->vertexBuffer[vbp++] = (*eit)->p2->p[i];
        }
	}
    this->drawMode = GL_LINES;
}

void Mesh::loadTriangleFromPointSet(const PointSet& ps)
{
	// Allocate buffer for each vertex of trangle: pos, color, normal(3*3*3 floats)
	this->release();
	this->pNum = (int)ps.triSet.size() * 3;
	this->vertexBuffer = new float[this->pNum*3];
	// Load data into buffer in the form of "pos, color, normal, pos, color, normal, ..."
	int vbp = 0;
	list<Triangle*>::const_iterator it;
	for(it = ps.triSet.begin(); it != ps.triSet.end(); it++)
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
            {
				this->vertexBuffer[vbp++] = (*it)->point[i]->p[j];
            }
        }
	}
	this->drawMode = GL_TRIANGLES;
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
    this->ps->sortPSet(ID);
    
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
            {
				if((*it)->edge[i]->type == Internal){
					currentEdge = (*it)->edge[i];
					endPoint->p[0] = ((*it)->edge[i]->p1->p[0] + (*it)->edge[i]->p2->p[0]) / 2;
					endPoint->p[1] = ((*it)->edge[i]->p1->p[1] + (*it)->edge[i]->p2->p[1]) / 2;
					radius = pow(pow((*it)->edge[i]->p1->p[0] - (*it)->edge[i]->p2->p[0], 2) + pow((*it)->edge[i]->p1->p[1] - (*it)->edge[i]->p2->p[1], 2), 0.5f) / 2;
				}
            }
			pList.push_back((*it)->oppositePoint(currentEdge));
			pList.push_back(currentEdge->p1);
			pList.push_back(currentEdge->p2);
			while(1)
            {
				eList.push_back(currentEdge);
				bool stop = false;
				for(int i = 0; i < pList.size(); i++)	// stop condition, some vertex lies outside the semicircle
                {
					if(pow(pow(endPoint->p[0] - pList[i]->p[0], 2) + pow(endPoint->p[1] - pList[i]->p[1], 2), 0.5f) > radius)
                    {
						stop = true;
						break;
					}
                }
				delTri = (currentEdge->e1->tL == delTri) ? currentEdge->e1->tR : currentEdge->e1->tL;
				if(stop)
                {
					for(int i = pList.size() - 1; i >= 0; i--)
                    {
						if(pList[i] == currentEdge->p1)
							pList.erase(pList.begin() + i);
                    }
					for(int i = pList.size() - 1; i >= 0; i--)
                    {
						if(pList[i] == currentEdge->p2)
							pList.erase(pList.begin() + i);
                    }
					eList.pop_back();
					break;
				}
				if(delTri->type == J){					// stop condition, advanced to junction triangle
					int i;
					for(i = 0; i < visitedJTriangleList.size(); i++)
                    {
						if(delTri == visitedJTriangleList[i])
							break;
                    }
					if(i == visitedJTriangleList.size())
                    {
						visitedJTriangleList.push_back(delTri);
                    }
					for(i = 0; i < spine.size(); i++)
                    {
						if(endPoint->p[0] == spine[i]->p[0] && endPoint->p[1] == spine[i]->p[1])
							break;
                    }
                    
					Point *preEndPoint = new Point(endPoint->id, endPoint->p[0], endPoint->p[1], 0);
                    
					for(int j = 0; j < 3; j++)
                    {
						endPoint->p[j] = delTri->midPoint[j];
                    }
					if(i != spine.size()){	// visit J triangle twice
						for(int i = pList.size() - 1; i >= 0; i--)
                        {
							if(pList[i] == currentEdge->p1)
								pList.erase(pList.begin() + i);
                        }
						for(int i = pList.size() - 1; i >= 0; i--)
                        {
							if(pList[i] == currentEdge->p2)
								pList.erase(pList.begin() + i);
                        }
						spine.erase(spine.begin() + i, spine.begin() + i + 2);
					} else{
						pList.push_back(delTri->oppositePoint(currentEdge));
						for(int i = 0; i < 3; i++)
                        {
							if(delTri->edge[i]->type == Internal && delTri->edge[i] != currentEdge){
								Point* tmp = new Point(ps.pSet.size(), 0, 0, 0);
								for(int j = 0; j < 3; j++)
                                {
									tmp->p[j] = (delTri->edge[i]->p1->p[j] + delTri->edge[i]->p2->p[j]) / 2;
                                }
								spine.push_back(tmp);
								spine.push_back(endPoint);
							}
                        }
					}
					break;
				} else{									// edge advancing
					int currentEdgeIndex;
					for(int i = 0; i < 3; i++)
                    {
						if(delTri->edge[i]->type == Internal){
							if(delTri->edge[i] != currentEdge){
								currentEdgeIndex = i;
								for(int j = 0; j < 3; j++)
                                {
									endPoint->p[j] = (delTri->edge[i]->p1->p[j] + delTri->edge[i]->p2->p[j]) / 2;
                                }
								radius = pow(pow(delTri->edge[i]->p1->p[0] - delTri->edge[i]->p2->p[0], 2) + pow(delTri->edge[i]->p1->p[1] - delTri->edge[i]->p2->p[1], 2), 0.5f) / 2;
							} else
								pList.push_back(delTri->oppositePoint(delTri->edge[i]));
						}
                    }
					currentEdge = delTri->edge[currentEdgeIndex];
				}
			}
			pListList.push_back(pList);
			eListList.push_back(eList);
			endPointList.push_back(endPoint);
		}
	}
	for(int i = 0; i < eListList.size(); i++)
    {
		for(int j = 0; j < eListList[i].size(); j++)
        {
			eListList[i][j]->unLink();
        }
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
            {
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
            }
			spine.push_back(tmp[0]);
			spine.push_back(tmp[1]);
		} else if((*it)->type == TRITYPE::J){
			int i;
			for(i = 0; i < visitedJTriangleList.size(); i++)
            {
				if((*it) == visitedJTriangleList[i])
					break;
            }
			if(i == visitedJTriangleList.size()){
				Point* tmp = new Point(ps.pSet.size(), 0, 0, 0);
				for(int j = 0; j < 3; j++)
                {
					tmp->p[j] = (*it)->midPoint[j];
                }
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
    {
		delEdgeList[i]->unLink();
    }
	for(int i = 0; i < spine.size(); i += 2){
		list<Point*>::const_iterator pit;
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
        {
			if((*pit)->p[0] == spine[i]->p[0] && (*pit)->p[1] == spine[i]->p[1]){
				spine[i] = (*pit);
				break;
			}
        }
		if(pit == ps.pSet.end()){
			spine[i]->id = ps.pSet.size();
			ps.addPoint(spine[i]);
		}
		for(pit = ps.pSet.begin(); pit != ps.pSet.end(); pit++)
        {
			if((*pit)->p[0] == spine[i + 1]->p[0] && (*pit)->p[1] == spine[i + 1]->p[1]){
				spine[i + 1] = (*pit);
				break;
			}
        }
		if(pit == ps.pSet.end()){
			spine[i + 1]->id = ps.pSet.size();
			ps.addPoint(spine[i + 1]);
		}
		link(spine[i], spine[i + 1]);
		NeighborLit it;
		for(it = spine[i]->neighbor.begin(); it != spine[i]->neighbor.end(); it++)
        {
			it->e->setType(External);
        }
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


void Mesh::loadP2tPoints(vector<p2t::Point*> polyline)
{
    this->release();
//	this->pNum = (int)(polyline.size());
    this->ps = new PointSet();
    for(uint i = 0; i < polyline.size(); i++)
    {
		this->ps->addPoint(new Point(i, (float)polyline[i]->x, (float)polyline[i]->y, 0.0));
    }
}


void Mesh::addP2tTriangles(vector<p2t::Triangle*> triangles)
{
    Edge* edgesArray[triangles.size()][3];
    
    for(uint i=0; i<triangles.size(); i++)
    {
        p2t::Triangle t = *triangles[i];
        p2t::Point a = *t.GetPoint(0);
        p2t::Point b = *t.GetPoint(1);
        p2t::Point c = *t.GetPoint(2);
        Point* pa = NULL;
        Point* pb = NULL;
        Point* pc = NULL;
        
        // setting point
        list<Point*>::iterator pit;
        int num = 0;
        for(pit = this->ps->pSet.begin(); pit != this->ps->pSet.end(); pit++)
        {
            
            if((double)((*pit)->p[0]) == a.x && (double)((*pit)->p[1]) == a.y)
            {
                pa = *pit;
                num++;
            }
            if((double)((*pit)->p[0]) == b.x && (double)((*pit)->p[1]) == b.y)
            {
                pb = *pit;
                num++;
            }
            if((double)((*pit)->p[0]) == c.x && (double)((*pit)->p[1]) == c.y)
            {
                pc = *pit;
                num++;
            }
            
            
        }
        
        Point* p1 = NULL;
        Point* p2 = NULL;
        
        for(int j=0; j<3; j++)
        {
            if(j == 0)
            {
                p1 = pb;
                p2 = pc;
            }
            if(j == 1)
            {
                p1 = pc;
                p2 = pa;
            }
            if(j == 2)
            {
                p1 = pa;
                p2 = pb;
            }
            
            // check edge is exist or not
            bool existEdge = false;
            list<Triangle*>::const_iterator triangleIt;
            for(triangleIt = this->ps->triSet.begin(); triangleIt != this->ps->triSet.end(); triangleIt++)
            {
                for(int i=0; i<3; i++) // check three edges
                {
                    Point* edgeP1 = (*triangleIt)->edge[i]->p1;
                    Point* edgeP2 = (*triangleIt)->edge[i]->p2;
                    
                    if((edgeP1->p[0] == p1->p[0] && edgeP1->p[1] == p1->p[1]) && (edgeP2->p[0] == p2->p[0] && edgeP2->p[1] == p2->p[1]))
                        existEdge = true;
                    
                    if((edgeP1->p[0] == p2->p[0] && edgeP1->p[1] == p2->p[1]) && (edgeP2->p[0] == p1->p[0] && edgeP2->p[1] == p1->p[1]))
                        existEdge = true;
                }
            }
            
            // if edge does not exist, link it
            if(!existEdge)
            {
                // link 
                Edge* edge = new Edge(p1, p2);
                edge->beLink();
                edgesArray[i][j] = edge;
                
                
                // Check the triangle candidate list
                // It is no matter we check the list of p1 or p2
                // If a candidate p' of p1 is linked to p2, it is also a candidate of p2, vice versa.
                list<NeighborLit>::iterator it;
                list<Triangle*> triList;
                for(it = p1->triCandidate.begin(); it != p1->triCandidate.end(); it++)
                {
                    Edge* e2 = (*it)->e;
                    Edge* e3 = (*it)->p->isLink(p2);
                    if(e3 != NULL)
                    {
                        Triangle* tri = new Triangle(edge, e2, e3);
                        triList.push_back(tri);
                    }
                }
                list<Triangle*>::iterator tit;
                for(tit = triList.begin(); tit != triList.end(); tit++)
                {
                    if((*tit)->beLink())// || (*tit)->midPoint[2] != 0)
                        p1->parent->addTriangle(*tit);
                    else
                        delete *tit;
                }
            }
        }
    }
    
    
    
    // setting the externel edges
    for(uint i=0; i<triangles.size(); i++)
    {
        p2t::Triangle t = *triangles[i];
        for(int j=0; j<3; j++)
        {
            if(t.constrained_edge[j]){
                edgesArray[i][j]->setType(External);
            }
        }
    }
}


std::vector<Point*> Mesh::getSkeletonPointSet(PointSet &ps)
{
    vector<Point*> skeletonPoints;
    list<Triangle*>::const_iterator it;
    Triangle* prevJointTriangle = NULL;
    Triangle* currentTriangle = NULL;
    int pointNum = 1;
    
    
    for(it = ps.triSet.begin(); it != ps.triSet.end(); it++)
	{
        if((*it)->type == TRITYPE::J)
        {
            prevJointTriangle = *it;
            currentTriangle = *it;
            findNextSkeletonPoint(skeletonPoints, currentTriangle, currentTriangle, prevJointTriangle, MERGETYPE::NONE);
            break;
        }
    }
    
    return skeletonPoints;
}

void Mesh::findNextSkeletonPoint(std::vector<Point*> &skeletonPoints, Triangle* currentTriangle, Triangle* prevTriangle,
                                 Triangle* prevJointTriangle, int prevMergeType)
{
    if(currentTriangle == prevTriangle && currentTriangle->type == TRITYPE::J)    // first triangle
    {
        Edge* edges[3];
        vector<int> nearbyJointEdges;
        for(int i=0; i<3; i++)
        {
            edges[i] = currentTriangle->edge[i];
            if((edges[i]->e1->tR != currentTriangle && edges[i]->e1->tR->type == TRITYPE::J) ||
               (edges[i]->e1->tL != currentTriangle && edges[i]->e1->tL->type == TRITYPE::J))
            {
                nearbyJointEdges.push_back(i);
            }
        }
        
        if(nearbyJointEdges.size() == 2) // center joint triangle, this is JJJ
        {
            float midPoint[3];
            Triangle* triangle1;
            Triangle* triangle2;
            
            // get triangle 1
            if(edges[nearbyJointEdges[0]]->e1->tR != currentTriangle)
                triangle1 = edges[nearbyJointEdges[0]]->e1->tR;
            else if(edges[nearbyJointEdges[0]]->e1->tL != currentTriangle)
                triangle1 = edges[nearbyJointEdges[0]]->e1->tL;
            // get triangle 2
            if(edges[nearbyJointEdges[1]]->e1->tR != currentTriangle)
                triangle2 = edges[nearbyJointEdges[1]]->e1->tR;
            else if(edges[nearbyJointEdges[1]]->e1->tL != currentTriangle)
                triangle2 = edges[nearbyJointEdges[1]]->e1->tL;
            
            
            // find mid point
            for(int i=0; i<3; i++)
                midPoint[i] = (currentTriangle->midPoint[i] + triangle1->midPoint[i] + triangle2->midPoint[i])/3;
            
            // find next triangle from triangle1
            for(int i=0; i<3; i++)
            {
                if(triangle1->edge[i] != edges[nearbyJointEdges[0]])
                {
                    Triangle* nextTriangle = NULL;
                    if(triangle1->edge[i]->e1->tR != triangle1)
                        nextTriangle = triangle1->edge[i]->e1->tR;
                    else if(triangle1->edge[i]->e1->tL != triangle1)
                        nextTriangle = triangle1->edge[i]->e1->tL;
                    
                    
                    if(nextTriangle->type != TRITYPE::T)
                    {
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (triangle1->edge[i]->p1->p[0]+triangle1->edge[i]->p2->p[0])/2 , (triangle1->edge[i]->p1->p[1]+triangle1->edge[i]->p2->p[1])/2, (triangle1->edge[i]->p1->p[2]+triangle1->edge[i]->p2->p[2])/2));
                        
                        findNextSkeletonPoint(skeletonPoints, nextTriangle, triangle1, prevJointTriangle, MERGETYPE::JJJ);
                    }
                }
            }
            // find next triangle from triangle2
            for(int i=0; i<3; i++)
            {
                if(triangle2->edge[i] != edges[nearbyJointEdges[1]])
                {
                    Triangle* nextTriangle = NULL;
                    if(triangle2->edge[i]->e1->tR != triangle2)
                        nextTriangle = triangle2->edge[i]->e1->tR;
                    else if(triangle2->edge[i]->e1->tL != triangle2)
                        nextTriangle = triangle2->edge[i]->e1->tL;
                    
                    
                    if(nextTriangle->type != TRITYPE::T)
                    {
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (triangle2->edge[i]->p1->p[0]+triangle2->edge[i]->p2->p[0])/2 , (triangle2->edge[i]->p1->p[1]+triangle2->edge[i]->p2->p[1])/2, (triangle2->edge[i]->p1->p[2]+triangle2->edge[i]->p2->p[2])/2));
                        
                        findNextSkeletonPoint(skeletonPoints, nextTriangle, triangle2, prevJointTriangle, MERGETYPE::JJJ);
                    }
                }
            }
            
            // find next triangle from current triangle
            for(int i=0; i<3; i++)
            {
                if(currentTriangle->edge[i] != edges[nearbyJointEdges[0]] && currentTriangle->edge[i] != edges[nearbyJointEdges[1]])
                {
                    Triangle* nextTriangle = NULL;
                    if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tR;
                    else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tL;
                    
                    if(nextTriangle->type != TRITYPE::T)
                    {
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (currentTriangle->edge[i]->p1->p[0]+currentTriangle->edge[i]->p2->p[0])/2 , (currentTriangle->edge[i]->p1->p[1]+currentTriangle->edge[i]->p2->p[1])/2, (currentTriangle->edge[i]->p1->p[2]+currentTriangle->edge[i]->p2->p[2])/2));
                        
                        findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, MERGETYPE::JJJ);
                    }
                }
            }
        }
        else if(nearbyJointEdges.size() == 1) // side joint triangle, need to find JJJ
        {
            Triangle* triangle1;
            int existNearbyJointTriangleNum = 0;
            float midPoint[3];
            
            // get triangle 1
            if(edges[nearbyJointEdges[0]]->e1->tR != currentTriangle)
                triangle1 = edges[nearbyJointEdges[0]]->e1->tR;
            else if(edges[nearbyJointEdges[0]]->e1->tL != currentTriangle)
                triangle1 = edges[nearbyJointEdges[0]]->e1->tL;
            
            // search nearby joint triangle
            for(int i=0; i<3; i++)
            {
                if(triangle1->edge[i] != edges[nearbyJointEdges[0]])
                {
                    Triangle* nextTriangle = NULL;
                    if(triangle1->edge[i]->e1->tR != triangle1)
                        nextTriangle = triangle1->edge[i]->e1->tR;
                    else if(triangle1->edge[i]->e1->tL != triangle1)
                        nextTriangle = triangle1->edge[i]->e1->tL;
                    
                    if(nextTriangle->type == TRITYPE::J)
                        existNearbyJointTriangleNum++;
                }
                
                if(currentTriangle->edge[i] != edges[nearbyJointEdges[0]])
                {
                    Triangle* nextTriangle = NULL;
                    if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tR;
                    else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tL;
                    
                    if(nextTriangle->type == TRITYPE::J)
                        existNearbyJointTriangleNum++;
                }
            }
            
            // exist JJJ
            if(existNearbyJointTriangleNum > 0)
            {
                findNextSkeletonPoint(skeletonPoints, triangle1, currentTriangle, prevJointTriangle, MERGETYPE::NONE);
            }
            else // this is JJ
            {
                // find mid point
                for(int i=0; i<3; i++)
                {
                    midPoint[i] = (currentTriangle->midPoint[i] + triangle1->midPoint[i])/2;
                }
                
                // find next triangle from triangle1
                for(int i=0; i<3; i++)
                {
                    if(triangle1->edge[i] != edges[nearbyJointEdges[0]])
                    {
                        Triangle* nextTriangle = NULL;
                        if(triangle1->edge[i]->e1->tR != triangle1)
                            nextTriangle = triangle1->edge[i]->e1->tR;
                        else if(triangle1->edge[i]->e1->tL != triangle1)
                            nextTriangle = triangle1->edge[i]->e1->tL;
                        
                        if(nextTriangle->type != TRITYPE::T)
                        {
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (triangle1->edge[i]->p1->p[0]+triangle1->edge[i]->p2->p[0])/2 , (triangle1->edge[i]->p1->p[1]+triangle1->edge[i]->p2->p[1])/2, (triangle1->edge[i]->p1->p[2]+triangle1->edge[i]->p2->p[2])/2));
                            
                            findNextSkeletonPoint(skeletonPoints, nextTriangle, triangle1, prevJointTriangle, MERGETYPE::JJ);
                        }
                    }
                }
                
                // find next triangle from current triangle
                for(int i=0; i<3; i++)
                {
                    if(currentTriangle->edge[i] != edges[nearbyJointEdges[0]])
                    {
                        Triangle* nextTriangle = NULL;
                        if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                            nextTriangle = currentTriangle->edge[i]->e1->tR;
                        else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                            nextTriangle = currentTriangle->edge[i]->e1->tL;
                        
                        if(nextTriangle->type != TRITYPE::T)
                        {
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (currentTriangle->edge[i]->p1->p[0]+currentTriangle->edge[i]->p2->p[0])/2 , (currentTriangle->edge[i]->p1->p[1]+currentTriangle->edge[i]->p2->p[1])/2, (currentTriangle->edge[i]->p1->p[2]+currentTriangle->edge[i]->p2->p[2])/2));
                            
                            findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, MERGETYPE::JJ);
                        }
                    }
                }
            }
        }
        else // without nearby triangle, need to find JSJ
        {
            // find JSJ
            int existJSJ = MERGETYPE::NONE;
            for(int i=0; i<3; i++)
            {
                Triangle* nextTriangle = NULL;
                if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                    nextTriangle = currentTriangle->edge[i]->e1->tR;
                else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                    nextTriangle = currentTriangle->edge[i]->e1->tL;
                
                if(nextTriangle->type != TRITYPE::T)
                {
                    for(int j=0; j<3; j++)
                    {
                        Triangle* nextNextTriangle = NULL;
                        
                        if(nextTriangle->edge[j]->type == Internal)
                        {
                            if(nextTriangle->edge[j]->e1->tR != nextTriangle)
                                nextNextTriangle = nextTriangle->edge[j]->e1->tR;
                            else if(nextTriangle->edge[j]->e1->tL != nextTriangle)
                                nextNextTriangle = nextTriangle->edge[j]->e1->tL;
                            
                            if(nextNextTriangle->type == TRITYPE::J && nextNextTriangle != currentTriangle)
                                existJSJ = MERGETYPE::PREJSJ;
                        }
                    }
                }
            }
            
            // find next triangle from current triangle
            for(int i=0; i<3; i++)
            {
                Triangle* nextTriangle = NULL;
                if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                    nextTriangle = currentTriangle->edge[i]->e1->tR;
                else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                    nextTriangle = currentTriangle->edge[i]->e1->tL;
                
                if(nextTriangle->type != TRITYPE::T)
                {
                    findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, existJSJ);
                }
            }
        }
        
        return;
    }
    
    else if(currentTriangle != prevTriangle && currentTriangle->type == TRITYPE::J)
    {
        prevJointTriangle = currentTriangle;
        
        Edge* edges[3];
        vector<int> nearbyJointEdges;
        for(int i=0; i<3; i++)
        {
            edges[i] = currentTriangle->edge[i];
            if((edges[i]->e1->tR != currentTriangle && edges[i]->e1->tR->type == TRITYPE::J) ||
               (edges[i]->e1->tL != currentTriangle && edges[i]->e1->tL->type == TRITYPE::J))
            {
                nearbyJointEdges.push_back(i);
            }
        }
        
        if(nearbyJointEdges.size() == 2) // center joint triangle, this is JJJ
        {
            float midPoint[3];
            Triangle* triangle1;
            Triangle* triangle2;
            
            // get triangle 1
            if(edges[nearbyJointEdges[0]]->e1->tR != currentTriangle)
                triangle1 = edges[nearbyJointEdges[0]]->e1->tR;
            else if(edges[nearbyJointEdges[0]]->e1->tL != currentTriangle)
                triangle1 = edges[nearbyJointEdges[0]]->e1->tL;
            // get triangle 2
            if(edges[nearbyJointEdges[1]]->e1->tR != currentTriangle)
                triangle2 = edges[nearbyJointEdges[1]]->e1->tR;
            else if(edges[nearbyJointEdges[1]]->e1->tL != currentTriangle)
                triangle2 = edges[nearbyJointEdges[1]]->e1->tL;
            
            
            // find mid point
            for(int i=0; i<3; i++)
                midPoint[i] = (currentTriangle->midPoint[i] + triangle1->midPoint[i] + triangle2->midPoint[i])/3;
            
            // find next triangle from triangle1
            for(int i=0; i<3; i++)
            {
                if(triangle1->edge[i] != edges[nearbyJointEdges[0]])
                {
                    Triangle* nextTriangle = NULL;
                    if(triangle1->edge[i]->e1->tR != triangle1)
                        nextTriangle = triangle1->edge[i]->e1->tR;
                    else if(triangle1->edge[i]->e1->tL != triangle1)
                        nextTriangle = triangle1->edge[i]->e1->tL;
                    
                    
                    if(nextTriangle->type != TRITYPE::T)
                    {
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (triangle1->edge[i]->p1->p[0]+triangle1->edge[i]->p2->p[0])/2 , (triangle1->edge[i]->p1->p[1]+triangle1->edge[i]->p2->p[1])/2, (triangle1->edge[i]->p1->p[2]+triangle1->edge[i]->p2->p[2])/2));
                        
                        if(prevTriangle != triangle1 || prevTriangle->type == TRITYPE::J)
                            findNextSkeletonPoint(skeletonPoints, nextTriangle, triangle1, prevJointTriangle, MERGETYPE::JJJ);
                    }
                }
            }
            // find next triangle from triangle2
            for(int i=0; i<3; i++)
            {
                if(triangle2->edge[i] != edges[nearbyJointEdges[1]])
                {
                    Triangle* nextTriangle = NULL;
                    if(triangle2->edge[i]->e1->tR != triangle2)
                        nextTriangle = triangle2->edge[i]->e1->tR;
                    else if(triangle2->edge[i]->e1->tL != triangle2)
                        nextTriangle = triangle2->edge[i]->e1->tL;
                    
                    
                    if(nextTriangle->type != TRITYPE::T)
                    {
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (triangle2->edge[i]->p1->p[0]+triangle2->edge[i]->p2->p[0])/2 , (triangle2->edge[i]->p1->p[1]+triangle2->edge[i]->p2->p[1])/2, (triangle2->edge[i]->p1->p[2]+triangle2->edge[i]->p2->p[2])/2));
                        
                        if(prevTriangle != triangle2 || prevTriangle->type == TRITYPE::J)
                            findNextSkeletonPoint(skeletonPoints, nextTriangle, triangle2, prevJointTriangle, MERGETYPE::JJJ);
                    }
                }
            }
            
            
            // find next triangle from current triangle
            for(int i=0; i<3; i++)
            {
                if(currentTriangle->edge[i] != edges[nearbyJointEdges[0]] && currentTriangle->edge[i] != edges[nearbyJointEdges[1]])
                {
                    Triangle* nextTriangle = NULL;
                    if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tR;
                    else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tL;
                    
                    
                    skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                    skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (currentTriangle->edge[i]->p1->p[0]+currentTriangle->edge[i]->p2->p[0])/2 , (currentTriangle->edge[i]->p1->p[1]+currentTriangle->edge[i]->p2->p[1])/2, (currentTriangle->edge[i]->p1->p[2]+currentTriangle->edge[i]->p2->p[2])/2));
                    
                    if(nextTriangle != prevTriangle)
                        findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, MERGETYPE::JJJ);
                }
            }
            

        }
        else if(nearbyJointEdges.size() == 1) // side joint triangle, need to find JJJ
        {
            Triangle* triangle1 = NULL;
            int existNearbyJointTriangleNum = 0;
            float midPoint[3];
            
            // get triangle 1
            if(edges[nearbyJointEdges[0]]->e1->tR != currentTriangle)
                triangle1 = edges[nearbyJointEdges[0]]->e1->tR;
            else if(edges[nearbyJointEdges[0]]->e1->tL != currentTriangle)
                triangle1 = edges[nearbyJointEdges[0]]->e1->tL;
            
            if(prevTriangle == triangle1) // this is JJ
            {
                // find mid point
                for(int i=0; i<3; i++)
                    midPoint[i] = (currentTriangle->midPoint[i] + triangle1->midPoint[i])/2;
                
                Edge* prevEdge;
                // find prev edge
                for(int i=0; i<3; i++)
                {
                    for(int j=0; j<3; j++)
                    {
                        if(prevTriangle->edge[i] == currentTriangle->edge[j])
                            prevEdge = currentTriangle->edge[j];
                    }
                }
                
                for(int i=0; i<3; i++)
                {
                    if(prevTriangle->edge[i] != prevEdge)
                    {
                        Triangle* nextTriangle = NULL;
                        if(prevTriangle->edge[i]->e1->tR != prevTriangle)
                            nextTriangle = prevTriangle->edge[i]->e1->tR;
                        else if(prevTriangle->edge[i]->e1->tL != prevTriangle)
                            nextTriangle = prevTriangle->edge[i]->e1->tL;
                        
                        if(nextTriangle->type != TRITYPE::T)
                        {
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (prevTriangle->edge[i]->p1->p[0]+prevTriangle->edge[i]->p2->p[0])/2 , (prevTriangle->edge[i]->p1->p[1]+prevTriangle->edge[i]->p2->p[1])/2, (prevTriangle->edge[i]->p1->p[2]+prevTriangle->edge[i]->p2->p[2])/2));
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                        }
                    }
                }
                
                
                // find next triangle from current triangle
                for(int i=0; i<3; i++)
                {
                    if(currentTriangle->edge[i] != edges[nearbyJointEdges[0]])
                    {
                        Triangle* nextTriangle = NULL;
                        if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                            nextTriangle = currentTriangle->edge[i]->e1->tR;
                        else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                            nextTriangle = currentTriangle->edge[i]->e1->tL;
                        
                        if(nextTriangle->type != TRITYPE::T)
                        {
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (currentTriangle->edge[i]->p1->p[0]+currentTriangle->edge[i]->p2->p[0])/2 , (currentTriangle->edge[i]->p1->p[1]+currentTriangle->edge[i]->p2->p[1])/2, (currentTriangle->edge[i]->p1->p[2]+currentTriangle->edge[i]->p2->p[2])/2));
                            
                            findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, MERGETYPE::JJ);
                        }
                    }
                }
            }
            else
            {
                Edge* prevEdge;
                // find prev edge
                for(int i=0; i<3; i++)
                {
                    for(int j=0; j<3; j++)
                    {
                        if(prevTriangle->edge[i] == currentTriangle->edge[j])
                            prevEdge = currentTriangle->edge[j];
                    }
                }
                
                for(int i=0; i<3; i++)
                {
                    if(currentTriangle->edge[i] != prevEdge)
                    {
                        Triangle* nextTriangle = NULL;
                        if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                            nextTriangle = currentTriangle->edge[i]->e1->tR;
                        else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                            nextTriangle = currentTriangle->edge[i]->e1->tL;
                        
                        if(nextTriangle->type != TRITYPE::T)
                        {
                            findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, MERGETYPE::JJ);
                        }
                    }
                }
            }
        }
        else // without nearby joint triangle, need to find JSJ
        {
            Edge* prevEdge;
            // find prev edge
            for(int i=0; i<3; i++)
            {
                for(int j=0; j<3; j++)
                {
                    if(prevTriangle->edge[i] == currentTriangle->edge[j])
                        prevEdge = currentTriangle->edge[j];
                }
            }
            
            // find JSJ
            int existJSJ = MERGETYPE::NONE;
            for(int i=0; i<3; i++)
            {
                if(currentTriangle->edge[i] != prevEdge)
                {
                    Triangle* nextTriangle = NULL;
                    if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tR;
                    else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tL;
                    
                    if(nextTriangle->type != TRITYPE::T)
                    {
                        
                        for(int j=0; j<3; j++)
                        {
                            Triangle* nextNextTriangle = NULL;
                            
                            if(nextTriangle->edge[j]->type == Internal)
                            {
                                if(nextTriangle->edge[j]->e1->tR != nextTriangle)
                                    nextNextTriangle = nextTriangle->edge[j]->e1->tR;
                                else if(nextTriangle->edge[j]->e1->tL != nextTriangle)
                                    nextNextTriangle = nextTriangle->edge[j]->e1->tL;
                                
                                if(nextNextTriangle->type == TRITYPE::J && nextNextTriangle != currentTriangle)
                                    existJSJ = MERGETYPE::PREJSJ;
                            }
                        }
                    }
                }
            }
            
            // find next triangle from current triangle
            for(int i=0; i<3; i++)
            {
                if(currentTriangle->edge[i] != prevEdge)
                {
                    Triangle* nextTriangle = NULL;
                    if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tR;
                    else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tL;
                    
                    if(nextTriangle->type != TRITYPE::T)
                    {
                        findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, existJSJ);
                    }
                }
            }
        }
        
        return;
    }
    else if(currentTriangle->type == TRITYPE::S)
    {
        if(prevTriangle->type == TRITYPE::J)
        {
            Edge* edges[3];
            vector<int> nearbyJointEdges;
            for(int i=0; i<3; i++)
            {
                edges[i] = currentTriangle->edge[i];
                
                if(edges[i]->type == Internal)
                {
                    if((edges[i]->e1->tR != currentTriangle && edges[i]->e1->tR->type == TRITYPE::J) ||
                       (edges[i]->e1->tL != currentTriangle && edges[i]->e1->tL->type == TRITYPE::J))
                    {
                        nearbyJointEdges.push_back(i);
                    }
                }
            }
            
            // exist JSJ
            if(nearbyJointEdges.size() == 2)
            {
                if(prevMergeType == MERGETYPE::PREJSJ || prevMergeType == MERGETYPE::NONE)
                {
                    float midPoint[3];
                    Triangle* triangle1;
                    Triangle* triangle2;
                    
                    // get triangle 1
                    if(edges[nearbyJointEdges[0]]->e1->tR != currentTriangle)
                        triangle1 = edges[nearbyJointEdges[0]]->e1->tR;
                    else if(edges[nearbyJointEdges[0]]->e1->tL != currentTriangle)
                        triangle1 = edges[nearbyJointEdges[0]]->e1->tL;
                    // get triangle 2
                    if(edges[nearbyJointEdges[1]]->e1->tR != currentTriangle)
                        triangle2 = edges[nearbyJointEdges[1]]->e1->tR;
                    else if(edges[nearbyJointEdges[1]]->e1->tL != currentTriangle)
                        triangle2 = edges[nearbyJointEdges[1]]->e1->tL;
                    
                    
                    // find mid point
                    for(int i=0; i<3; i++)
                        midPoint[i] = (currentTriangle->midPoint[i] + triangle1->midPoint[i] + triangle2->midPoint[i])/3;
                    
                    
                    // find next triangle from triangle1
                    for(int i=0; i<3; i++)
                    {
                        if(triangle1->edge[i] != edges[nearbyJointEdges[0]])
                        {
                            Triangle* nextTriangle = NULL;
                            if(triangle1->edge[i]->e1->tR != triangle1)
                                nextTriangle = triangle1->edge[i]->e1->tR;
                            else if(triangle1->edge[i]->e1->tL != triangle1)
                                nextTriangle = triangle1->edge[i]->e1->tL;
                            
                            
                            if(nextTriangle->type != TRITYPE::T)
                            {
                                skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                                skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (triangle1->edge[i]->p1->p[0]+triangle1->edge[i]->p2->p[0])/2 , (triangle1->edge[i]->p1->p[1]+triangle1->edge[i]->p2->p[1])/2, (triangle1->edge[i]->p1->p[2]+triangle1->edge[i]->p2->p[2])/2));
                                
                                if(prevTriangle != triangle1)
                                    findNextSkeletonPoint(skeletonPoints, nextTriangle, triangle1, prevJointTriangle, MERGETYPE::JSJ);
                            }
                        }
                    }
                    // find next triangle from triangle2
                    for(int i=0; i<3; i++)
                    {
                        if(triangle2->edge[i] != edges[nearbyJointEdges[1]])
                        {
                            Triangle* nextTriangle = NULL;
                            if(triangle2->edge[i]->e1->tR != triangle2)
                                nextTriangle = triangle2->edge[i]->e1->tR;
                            else if(triangle2->edge[i]->e1->tL != triangle2)
                                nextTriangle = triangle2->edge[i]->e1->tL;
                            
                            
                            if(nextTriangle->type != TRITYPE::T)
                            {
                                skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, midPoint[0], midPoint[1], midPoint[2]));
                                skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (triangle2->edge[i]->p1->p[0]+triangle2->edge[i]->p2->p[0])/2 , (triangle2->edge[i]->p1->p[1]+triangle2->edge[i]->p2->p[1])/2, (triangle2->edge[i]->p1->p[2]+triangle2->edge[i]->p2->p[2])/2));
                                
                                if(prevTriangle != triangle2)
                                    findNextSkeletonPoint(skeletonPoints, nextTriangle, triangle2, prevJointTriangle, MERGETYPE::JSJ);
                            }
                        }
                    }
                }
                else // prevMergeType == JJ/JSJ/JJJ
                {
                    // find prev edge
                    Edge* prevEdge;
                    for(int i=0; i<3; i++)
                    {
                        for(int j=0; j<3; j++)
                        {
                            if(prevTriangle->edge[i] == currentTriangle->edge[j])
                                prevEdge = currentTriangle->edge[j];
                        }
                    }
                    
                    // find next triangle from current triangle
                    for(int i=0; i<3; i++)
                    {
                        if(currentTriangle->edge[i] != prevEdge && currentTriangle->edge[i]->type == Internal)
                        {
                            Triangle* nextTriangle = NULL;
                            if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                                nextTriangle = currentTriangle->edge[i]->e1->tR;
                            else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                                nextTriangle = currentTriangle->edge[i]->e1->tL;
                            
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (prevEdge->p1->p[0]+prevEdge->p2->p[0])/2 , (prevEdge->p1->p[1]+prevEdge->p2->p[1])/2, (prevEdge->p1->p[2]+prevEdge->p2->p[2])/2));
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (currentTriangle->edge[i]->p1->p[0]+currentTriangle->edge[i]->p2->p[0])/2 , (currentTriangle->edge[i]->p1->p[1]+currentTriangle->edge[i]->p2->p[1])/2, (currentTriangle->edge[i]->p1->p[2]+currentTriangle->edge[i]->p2->p[2])/2));
                            
                            if(prevTriangle != nextTriangle)
                                findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, MERGETYPE::NONE);
                        }
                    }
                    
                }
            }
            else if(nearbyJointEdges.size() == 1)
            {                
                
                if(prevMergeType == MERGETYPE::NONE)
                {
                    // set prev triangle edge
                    for(int i=0; i<3; i++)
                    {
                        Triangle* nextTriangle = NULL;
                        if(prevTriangle->edge[i]->e1->tR != prevTriangle)
                            nextTriangle = prevTriangle->edge[i]->e1->tR;
                        else if(prevTriangle->edge[i]->e1->tL != prevTriangle)
                            nextTriangle = prevTriangle->edge[i]->e1->tL;
                        
                        if(nextTriangle->type != TRITYPE::T)
                        {
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, prevTriangle->midPoint[0], prevTriangle->midPoint[1], prevTriangle->midPoint[2]));
                            skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (prevTriangle->edge[i]->p1->p[0]+prevTriangle->edge[i]->p2->p[0])/2 , (prevTriangle->edge[i]->p1->p[1]+prevTriangle->edge[i]->p2->p[1])/2, (prevTriangle->edge[i]->p1->p[2]+prevTriangle->edge[i]->p2->p[2])/2));
                        }
                    }
                }
                
                // find prev edge
                Edge* prevEdge;
                for(int i=0; i<3; i++)
                {
                    for(int j=0; j<3; j++)
                    {
                        if(prevTriangle->edge[i] == currentTriangle->edge[j])
                            prevEdge = currentTriangle->edge[j];
                    }
                }
                
                // find next triangle from current triangle
                for(int i=0; i<3; i++)
                {
                    if(currentTriangle->edge[i] != prevEdge && currentTriangle->edge[i]->type == Internal)
                    {
                        Triangle* nextTriangle = NULL;
                        if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                            nextTriangle = currentTriangle->edge[i]->e1->tR;
                        else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                            nextTriangle = currentTriangle->edge[i]->e1->tL;
                        
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (prevEdge->p1->p[0]+prevEdge->p2->p[0])/2 , (prevEdge->p1->p[1]+prevEdge->p2->p[1])/2, (prevEdge->p1->p[2]+prevEdge->p2->p[2])/2));
                        skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (currentTriangle->edge[i]->p1->p[0]+currentTriangle->edge[i]->p2->p[0])/2 , (currentTriangle->edge[i]->p1->p[1]+currentTriangle->edge[i]->p2->p[1])/2, (currentTriangle->edge[i]->p1->p[2]+currentTriangle->edge[i]->p2->p[2])/2));
                        
                        if(prevTriangle != nextTriangle)
                            findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, MERGETYPE::NONE);
                    }
                }
            }
        }
        else  // prev triangle is not Joint
        {
            // find prev edge
            Edge* prevEdge;
            for(int i=0; i<3; i++)
            {
                for(int j=0; j<3; j++)
                {
                    if(prevTriangle->edge[i] == currentTriangle->edge[j])
                        prevEdge = currentTriangle->edge[j];
                }
            }
            
            // find next triangle from current triangle
            for(int i=0; i<3; i++)
            {
                if(currentTriangle->edge[i] != prevEdge && currentTriangle->edge[i]->type == Internal)
                {
                    Triangle* nextTriangle = NULL;
                    if(currentTriangle->edge[i]->e1->tR != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tR;
                    else if(currentTriangle->edge[i]->e1->tL != currentTriangle)
                        nextTriangle = currentTriangle->edge[i]->e1->tL;
                    
                    skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (prevEdge->p1->p[0]+prevEdge->p2->p[0])/2 , (prevEdge->p1->p[1]+prevEdge->p2->p[1])/2, (prevEdge->p1->p[2]+prevEdge->p2->p[2])/2));
                    skeletonPoints.push_back(new Point((int)skeletonPoints.size()+1, (currentTriangle->edge[i]->p1->p[0]+currentTriangle->edge[i]->p2->p[0])/2 , (currentTriangle->edge[i]->p1->p[1]+currentTriangle->edge[i]->p2->p[1])/2, (currentTriangle->edge[i]->p1->p[2]+currentTriangle->edge[i]->p2->p[2])/2));
                    findNextSkeletonPoint(skeletonPoints, nextTriangle, currentTriangle, prevJointTriangle, MERGETYPE::NONE);
                }
            }
            
        }

        return;
    }
    else if(currentTriangle->type == TRITYPE::T)
    {
        return;
    }    
}


