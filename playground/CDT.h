#include <list>
#include <vector>
#include <map>
#ifndef GLEW_DEFINED
#define GLEW_DEFINED
#include <GL\glew.h>
#endif

//#define STEPBYSTEPCDT
//#define STEPBYSTEPBUBBLE
//#define MESHSHADER

struct Neighbor;
class Point;
class Edge;
class Triangle;
class PointSet;
#ifdef MESHSHADER
class Shader;
#endif

typedef std::list<Neighbor>::iterator NeighborLit;
typedef unsigned int uint;

struct Neighbor
{
	public:
		Neighbor(Point* point, Edge* edge, float _theta):p(point), e(edge), theta(_theta)
		{
			tL = NULL;
			tR = NULL;
			ticket = 0;
		}
		Point* p;
		Edge* e;
		Triangle* tL, *tR;
		float theta;
		int ticket;
};

class Point
{
	public:
		Point(int _id, float x, float y, float z = 0);
		~Point();
		Edge* isLink(const Point* p2);
		void beAdd(PointSet* ps);
		NeighborLit addNeighbor(Edge* edge);
		bool addTriNeighbor(Triangle* triangle);
		void removeNeighbor(NeighborLit it);
		void removeTriangle(NeighborLit it, Triangle *tri);
		
		NeighborLit findNeighbor(Point* point);
		NeighborLit nextNeighbor(NeighborLit it, int d);
		Point* findConvexHull(bool clockwise, const Point* pp);
		void calNormal();

		float p[3];
		float n[3];
		int id;
		PointSet* parent;
		std::list<Neighbor> neighbor;	  // clockwise
		std::list<NeighborLit> triCandidate;
		std::list<Triangle*> triNeighbor;
};

enum EDGETYPE {Internal, External};
class Edge
{
	public:
		Edge(Point* _p1, Point* _p2);
		void beLink();
		void unLink();
		void setType(int _type);
		
		int type;
		Point *p1, *p2;
		NeighborLit e1, e2;	// e1: iterator of p1; e2: iterator of p2
};

enum TRITYPE {J, S, T};
// J: no external edge S: one external edge; T: two external edge;
class Triangle
{
	public:
		Triangle(Edge* e1, Edge* e2, Edge* e3);
		bool beLink();
		void unLink();
		void remove(Edge *e);
		void beAdd(PointSet *ps, std::list<Triangle*>::iterator it);
		void vote(int score);
		Edge* oppositeEdge(Point* p);
		Point* oppositePoint(Edge* e);
		void calNormal();

		int type;
		Point* point[3];
		Edge* edge[3];
		float midPoint[3];
		float normal[3];
		PointSet* parent;
		std::list<Triangle*>::iterator tit;
};

enum SORTTYPE {X, Y, ID};
class PointSet
{
	public:
		PointSet();
		~PointSet();
		void split(PointSet& ps1, PointSet& ps2);
		void merge(PointSet& ps1, PointSet& ps2);
		void clear();
		void release();
		void sortPSet(int sort);		// 0: sort by x; 1: sort by y;
		void addPoint(Point* p);
		void addTriangle(Triangle* t);

		bool isClockwise();

		std::list<Point*> pSet;
		std::list<Triangle*> triSet;
};

class Mesh
{
	public:
		GLuint pNum;
		GLuint VBid;
		GLfloat *vertexBuffer;
		GLuint assm;
		#ifdef MESHSHADER
		Shader* shader;
		#endif
		PointSet* ps;

		void loadEdgeFromMouse(std::vector<float> point);
		void loadTriangleFromPointSet(const PointSet& ps);
		void loadEdgeFromPointSet(const PointSet& ps);
		void runCDT();
		void release();

		Mesh();
		~Mesh();
};

//***Comparator***//
bool comparePointX(Point* p1, Point* p2);
bool comparePointY(Point* p1, Point* p2);
bool comparePointID(Point* p1, Point* p2);

//***Function for calculating***//
float length(float* p1, float* p2);
float lengthSquare(float* p1, float* p2);
float orientation(float* center, float* point);
float circumcircle(float* p1, float* p2, float* p3, float* c);

//***Function for CDT***//
void link(Point* p1, Point* p2);
bool linkTest(Point* pB, Point* pC);
void CDT(PointSet& ps);
void generalTrianglize(Point* p1, Point* p2, std::list<Point*> half, float theta12, float theta21);
void edgeConstraint(Point* p1, Point* p2);
void boundConstraint(PointSet& ps);
NeighborLit examBaseCandidate(NeighborLit base, NeighborLit first, NeighborLit second, int LR);

//***Header for Bubble up***//
std::vector<Point*> getSpine(PointSet &ps, int baseID);
void cleanSpineBase(PointSet &ps, std::vector<Point*> spine);
int pruneAndSpine(PointSet& ps);
void bubbleUp(PointSet& ps, std::vector<Point*>& spine, int upsideDown);
void setAllPointNormal(PointSet &ps);


template <class TClass>
typename std::list<TClass>::iterator loopIterator(std::list<TClass>& root, typename std::list<TClass>::iterator it, int s)
{
	if(it != --(root.end()) && s > 0)
		return ++it;
	else if(it != root.begin() && s < 0)
		return --it;
	else if(s > 0)
		return root.begin();
	return --(root.end());
}

#ifdef MESHSHADER
class Shader
{
	public:
		Shader(const char * VSPath, const char * FSPath);
		~Shader();
		bool addUniformMat4fv(const char *name, GLsizei count, GLfloat *data);
		bool addUniformVec3fv(const char *name, GLsizei count, GLfloat *data);
		bool addUniformVec4fv(const char *name, GLsizei count, GLfloat *data);
		bool addUniformFloat(const char *name, GLsizei count, GLfloat *data);
		void render(GLenum bufferType, GLuint VBid, GLenum polygonMode,
					GLint first, GLsizei count, bool frame = false);

		GLuint pID;
		std::map<GLuint, std::pair<GLsizei , GLfloat*> > uniformMat4;
		std::map<GLuint, std::pair<GLsizei , GLfloat*> > uniformVec3;
		std::map<GLuint, std::pair<GLsizei , GLfloat*> > uniformVec4;
		std::map<GLuint, std::pair<GLsizei , GLfloat*> > uniformFloat;
};
#endif