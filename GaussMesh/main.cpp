//
//  main.cpp
//  GaussMesh
//
//  Created by  Andikan on 13/8/2.
//  Copyright (c) 2013 Andikan. All rights reserved.
//



#include "IncludeLib.h"


int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE,GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
    
    // Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}
    
    // Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
    
    // Initialize variable
    glfwSetWindowTitle( "GaussOrganic Mesh" );
    
    // Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
    glfwSetMousePos( 1024/2, 700/2 );
    
    // Dark blue background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Enable depth test
	glEnable(GL_DEPTH_TEST);
    
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
    
    // Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/TransformVertexShader.vertexshader", "/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/ColorFragmentShader.fragmentshader" );
    
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    
	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexColorID = glGetAttribLocation(programID, "vertexColor");
    
    // Use our shader
    glUseProgram(programID);
    
    
    // Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
	};
    
	    
    GLuint cubevertexbuffer;
	glGenBuffers(1, &cubevertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubevertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    
	GLuint cubecolorbuffer;
	glGenBuffers(1, &cubecolorbuffer);
    // One color for each vertex. They were generated randomly.
    static GLfloat g_color_buffer_data[12*3*3];
    srand((unsigned)time(NULL));
    for (int v = 0; v < 12*3 ; v++){
        g_color_buffer_data[3*v+0] = (float)((rand()%255)+1)/(float)255; // R
        g_color_buffer_data[3*v+1] = (float)((rand()%255)+1)/(float)255; // G
        g_color_buffer_data[3*v+2] = (float)((rand()%255)+1)/(float)255; // B
    }
    glBindBuffer(GL_ARRAY_BUFFER, cubecolorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    
    
    GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
    GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
    
    GLuint spineVertexBuffer;
	glGenBuffers(1, &spineVertexBuffer);
    GLuint spineColorBuffer;
	glGenBuffers(1, &spineColorBuffer);
    
    GLuint jointVertexBuffer;
	glGenBuffers(1, &jointVertexBuffer);
    GLuint jointColorBuffer;
	glGenBuffers(1, &jointColorBuffer);

    
    GLuint axesVertexBuffer;
    glGenBuffers(1, &axesVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, axesVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_axes_vertex_buffer_data), g_axes_vertex_buffer_data, GL_STATIC_DRAW);
    
    GLuint axesColorbuffer;
	glGenBuffers(1, &axesColorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, axesColorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_axes_color_buffer_data), g_axes_color_buffer_data, GL_STATIC_DRAW);
    
    
    // Initialize variable
    bool mouseDrawPress = false;
    bool spineDetect = false;
    bool readContourData = false;
    int readContourDataCount = 0;
    vector<vec3> clickPoint;
    vector<vec3> vertices;
	vector<vec3> colors;
    
    vector<vec3> spineVertice;
    vector<vec3> spineColors;
    
    vector<vec3> jointVertice;
    vector<vec3> jointColors;

    Mesh mesh;
    Mesh findSkeletonMesh;
    GLenum drawMode;
    
    int filenum = 0;
    float thinLine = 1.5;
    float boldLine = 4.5;
    float lineWidth = thinLine;

    
    do{
        // Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        // detect mouse to draw
        if (glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS){
            
            if(!mouseDrawPress)  // refresh a point set
            {
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                spineDetect = false;
            }
            
            
            // get origin mouse position
            int xpos, ypos;
            glfwGetMousePos( &xpos, &ypos );
            cout << "x: " << xpos-512 << ", y: " << 350-ypos << endl;
            
            // modify origin mouse position by decrease 100 times
            double xmodpos, ymodpos;
            xmodpos = (double)(xpos-512)/100;
            ymodpos = (double)(350-ypos)/100;
            cout << "xm: " << xmodpos << ", ym: " << ymodpos << endl;
            
            
            if(clickPoint.size() == 0)
            {
                clickPoint.push_back(vec3( xmodpos, ymodpos, 0));
                vertices.push_back(vec3( xmodpos, ymodpos, 0));
                colors.push_back(vec3(255, 255, 255));
            }
            else
            {
                double distance = pow(pow(xmodpos - clickPoint[clickPoint.size()-1].x, 2) +
                                      pow(ymodpos - clickPoint[clickPoint.size()-1].y, 2), 0.5);
                cout << "distance : " << distance << endl;
                
                if(distance > 0.3){
                    if(vertices.size() % 2 == 0){
                        //clickPoint.push_back(vec3(clickPoint[clickPoint.size() - 1].x, clickPoint[clickPoint.size() - 1].y, 0));
                        vertices.push_back(vec3(vertices[vertices.size() - 1].x, vertices[vertices.size() - 1].y, 0));
                        colors.push_back(vec3(255, 255, 255));
                    }
                    clickPoint.push_back(vec3(xmodpos, ymodpos, 0));
                    vertices.push_back(vec3( xmodpos, ymodpos, 0));
                    colors.push_back(vec3(255, 255, 255));
                }
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
            
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
            
            mouseDrawPress = true;
            drawMode = GL_LINES;
        }
        
        if (glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) == GLFW_RELEASE){
            if(mouseDrawPress){
                cout << "draw down\n";
                mouseDrawPress = false;
                
                mesh.loadEdgeFromMouse(clickPoint);
                // point insert to mesh
                mesh.loadAndSortPointSet();
                // delaunayTriangulation
                // mesh.delaunayTriangulation();
                mesh.loadEdgeFromPointSet(*mesh.ps);
                // constraintBound
                // mesh.constraintBound();
                mesh.loadEdgeFromPointSet(*mesh.ps);
                
                
                vertices.clear();
                colors.clear();
                
                cout << "mesh vertices : " << mesh.pNum << endl;
                for (int i = 0; i < mesh.pNum; i++) {
					vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
					colors.push_back(vec3(255, 255, 255));
                }
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                drawMode = mesh.drawMode;
            }
        }
        
        if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS)
        {
            
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            
            if(!readContourData)
            {
                if( filenum == 1 )
                {
                    filenum = 32;
                }
                
                filenum = filenum - 1;
                // filenum = 6;
                double xpos, ypos;
                int vertexCount;
                vector<Point*> contourpoints;
                
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                cout << "contour points number " << contourpoints.size() << endl;
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                /*
                 * STEP 1: Create CDT and add primary polyline
                 * NOTE: polyline must be a simple polygon. The polyline's points
                 * constitute constrained edges. No repeat points!!!
                 */
                clock_t init_time = clock();
                p2t::CDT* cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();
                clock_t end_time = clock();
                cout << "Elapsed time (sec) = " << (end_time-init_time)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
                map = cdt->GetMap();
                
                
                mesh.loadP2tPoints(polyline);
                findSkeletonMesh.loadP2tPoints(polyline);
                
                mesh.addP2tTriangles(triangles);
                findSkeletonMesh.addP2tTriangles(triangles);
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                
                
                /* find skeleton */
                vector<Point*> spineEdgeSet;
                vector<Point*> jointPointEdgeSet;
                // remove some T triangle, like T-J, T-S
                findSkeletonMesh.removeTerminalTriangleWithJoint(*findSkeletonMesh.ps);
                // findSkeletonMesh.removeTerminalTriangleWithSleeve(*findSkeletonMesh.ps);
                // get edge of points, return vector<Point*>
                // edge 1 = vectorPoints[0], vectorPoints[1]
                // edge 2 = vectorPoints[2], vectorPoints[3]
                // edge 3 = ....
                spineEdgeSet = findSkeletonMesh.getSkeletonPointSet(*findSkeletonMesh.ps);
                jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(spineEdgeSet);
                /* END find skeleton END */
                
                mesh.loadEdgeFromPointSet(*mesh.ps);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                
                //                for (int i = 0; i < findSkeletonMesh.pNum; i++) {
                //                    vertices.push_back(vec3(findSkeletonMesh.vertexBuffer[i*3], findSkeletonMesh.vertexBuffer[i*3+1], findSkeletonMesh.vertexBuffer[i*3+2]));
                //                    colors.push_back(vec3(255, 255, 255));
                //                }
                
                for (int i = 0; i < mesh.pNum; i++) {
                    vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
                    colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                }
                
                for (int i = 0; i < spineEdgeSet.size(); i++)
                {
                    spineVertice.push_back(vec3(spineEdgeSet[i]->p[0], spineEdgeSet[i]->p[1], spineEdgeSet[i]->p[2]));
                    spineColors.push_back(vec3(0.16f, 0.50f, 0.73f));
                }
                //
                //                for (int i = 0; i < jointPointEdgeSet.size(); i++) {
                //                    jointVertice.push_back(vec3(jointPointEdgeSet[i]->p[0], jointPointEdgeSet[i]->p[1], jointPointEdgeSet[i]->p[2]));
                //                    jointColors.push_back(vec3(255, 255, 0));
                //                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                
                // drawMode = GL_LINES;
                drawMode = mesh.drawMode;
                lineWidth = thinLine;
                // drawMode = findSkeletonMesh.drawMode;
                
                readContourData = true;
                readContourDataCount++;
            }


		}
        
        
        if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            
            if(!readContourData)
            {
                if( filenum == 31 )
                {
                    filenum = 0;
                }
                
                filenum = filenum + 1;
                // filenum = 6;
                double xpos, ypos;
                int vertexCount;
                vector<Point*> contourpoints;
                
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                cout << "contour points number " << contourpoints.size() << endl;
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                /*
                 * STEP 1: Create CDT and add primary polyline
                 * NOTE: polyline must be a simple polygon. The polyline's points
                 * constitute constrained edges. No repeat points!!!
                 */
                clock_t init_time = clock();
                p2t::CDT* cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();
                clock_t end_time = clock();
                cout << "Elapsed time (sec) = " << (end_time-init_time)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
                map = cdt->GetMap();
                
                
                mesh.loadP2tPoints(polyline);
                findSkeletonMesh.loadP2tPoints(polyline);
                
                mesh.addP2tTriangles(triangles);
                findSkeletonMesh.addP2tTriangles(triangles);
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                
                
                /* find skeleton */
                vector<Point*> spineEdgeSet;
                vector<Point*> jointPointEdgeSet;
                // remove some T triangle, like T-J, T-S
                findSkeletonMesh.removeTerminalTriangleWithJoint(*findSkeletonMesh.ps);
                // findSkeletonMesh.removeTerminalTriangleWithSleeve(*findSkeletonMesh.ps);
                // get edge of points, return vector<Point*>
                // edge 1 = vectorPoints[0], vectorPoints[1]
                // edge 2 = vectorPoints[2], vectorPoints[3]
                // edge 3 = ....
                spineEdgeSet = findSkeletonMesh.getSkeletonPointSet(*findSkeletonMesh.ps);
                jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(spineEdgeSet);
                /* END find skeleton END */
                
                mesh.loadEdgeFromPointSet(*mesh.ps);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                
//                for (int i = 0; i < findSkeletonMesh.pNum; i++) {
//                    vertices.push_back(vec3(findSkeletonMesh.vertexBuffer[i*3], findSkeletonMesh.vertexBuffer[i*3+1], findSkeletonMesh.vertexBuffer[i*3+2]));
//                    colors.push_back(vec3(255, 255, 255));
//                }
                
                for (int i = 0; i < mesh.pNum; i++) {
                    vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
                    colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                }
                
                for (int i = 0; i < spineEdgeSet.size(); i++)
                {
                    spineVertice.push_back(vec3(spineEdgeSet[i]->p[0], spineEdgeSet[i]->p[1], spineEdgeSet[i]->p[2]));
                    spineColors.push_back(vec3(0.16f, 0.50f, 0.73f));
                }
//
//                for (int i = 0; i < jointPointEdgeSet.size(); i++) {
//                    jointVertice.push_back(vec3(jointPointEdgeSet[i]->p[0], jointPointEdgeSet[i]->p[1], jointPointEdgeSet[i]->p[2]));
//                    jointColors.push_back(vec3(255, 255, 0));
//                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                
                // drawMode = GL_LINES;
                drawMode = mesh.drawMode;
                lineWidth = thinLine;
                // drawMode = findSkeletonMesh.drawMode;

                readContourData = true;
                readContourDataCount++;
            }

            
        }
        
        if(glfwGetKey(GLFW_KEY_TAB) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            if(!readContourData)
            {
                vector<Point*> contourpoints;
                
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                /*
                 * STEP 1: Create CDT and add primary polyline
                 * NOTE: polyline must be a simple polygon. The polyline's points
                 * constitute constrained edges. No repeat points!!!
                 */
                clock_t init_time = clock();
                p2t::CDT* cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();
                clock_t end_time = clock();
                cout << "Elapsed time (sec) = " << (end_time-init_time)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
                map = cdt->GetMap();
                
                
                mesh.loadP2tPoints(polyline);
                findSkeletonMesh.loadP2tPoints(polyline);
                
                mesh.addP2tTriangles(triangles);
                findSkeletonMesh.addP2tTriangles(triangles);
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                
                
                /* find skeleton */
                vector<Point*> spineEdgeSet;
                vector<Point*> jointPointEdgeSet;
                // remove some T triangle, like T-J, T-S
                findSkeletonMesh.removeTerminalTriangleWithJoint(*findSkeletonMesh.ps);
                // findSkeletonMesh.removeTerminalTriangleWithSleeve(*findSkeletonMesh.ps);
                // get edge of points, return vector<Point*>
                // edge 1 = vectorPoints[0], vectorPoints[1]
                // edge 2 = vectorPoints[2], vectorPoints[3]
                // edge 3 = ....
                spineEdgeSet = findSkeletonMesh.getSkeletonPointSet(*findSkeletonMesh.ps);
                spineEdgeSet = findSkeletonMesh.removeRedundantTerminalEdges(spineEdgeSet);
                // findSkeletonMesh.getPipeHole(contourpoints, spineEdgeSet);
                // jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(spineEdgeSet);
                /* END find skeleton END */
                
                mesh.loadEdgeFromPointSet(*mesh.ps);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                
                for (int i = 0; i < mesh.pNum; i++) {
                    vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
                    colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                }
                
                for (int i = 0; i < spineEdgeSet.size(); i++)
                {
                    spineVertice.push_back(vec3(spineEdgeSet[i]->p[0], spineEdgeSet[i]->p[1], spineEdgeSet[i]->p[2]));
                    spineColors.push_back(vec3(0.16f, 0.50f, 0.73f));
                }

//                for (int i = 0; i < jointPointEdgeSet.size(); i++) {
//                    jointVertice.push_back(vec3(jointPointEdgeSet[i]->p[0], jointPointEdgeSet[i]->p[1], jointPointEdgeSet[i]->p[2]));
//                    jointColors.push_back(vec3(0.086f, 0.627f, 0.521f));
//                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                // drawMode = GL_LINES;
                drawMode = mesh.drawMode;
                lineWidth = thinLine;
                // drawMode = findSkeletonMesh.drawMode;
                
                readContourData = true;
                readContourDataCount++;
            }

            
        }
        
        if (glfwGetKey( GLFW_KEY_SPACE ) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            if(!readContourData)
            {
                cout << "press space : get contour edges" << endl;
                vector<Point*> contourpoints;
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                cout << "contour points number " << contourpoints.size() << endl;
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                for (int i = 0; i < contourpoints.size(); i++) {
                    if(i != 0)
                    {
                        vertices.push_back(vec3(contourpoints[i-1]->p[0], contourpoints[i-1]->p[1], 0));
                        colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                        vertices.push_back(vec3(contourpoints[i]->p[0], contourpoints[i]->p[1], 0));
                        colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                    }
                    else
                    {
                        vertices.push_back(vec3(contourpoints[i]->p[0], contourpoints[i]->p[1], 0));
                        colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                        vertices.push_back(vec3(contourpoints[contourpoints.size()-1]->p[0], contourpoints[contourpoints.size()-1]->p[1], 0));
                        colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                    }
                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                // drawMode = GL_LINES;
                drawMode = GL_LINES;
                lineWidth = boldLine;
                // drawMode = findSkeletonMesh.drawMode;
                readContourData = true;
                readContourDataCount++;
            }
            
        }
        
        if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            if(!readContourData)
            {
                vector<Point*> contourpoints;
                
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                /*
                 * STEP 1: Create CDT and add primary polyline
                 * NOTE: polyline must be a simple polygon. The polyline's points
                 * constitute constrained edges. No repeat points!!!
                 */
                clock_t init_time = clock();
                p2t::CDT* cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();
                clock_t end_time = clock();
                cout << "Elapsed time (sec) = " << (end_time-init_time)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
                map = cdt->GetMap();
                
                
                mesh.loadP2tPoints(polyline);
                // findSkeletonMesh.loadP2tPoints(polyline);
                
                mesh.addP2tTriangles(triangles);
                // findSkeletonMesh.addP2tTriangles(triangles);
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                
                
                /* find skeleton */
                vector<Point*> spineEdgeSet;
                vector<Point*> jointPointEdgeSet;
                // remove some T triangle, like T-J, T-S
                // findSkeletonMesh.removeTerminalTriangleWithJoint(*findSkeletonMesh.ps);
                // findSkeletonMesh.removeTerminalTriangleWithSleeve(*findSkeletonMesh.ps);
                // get edge of points, return vector<Point*>
                // edge 1 = vectorPoints[0], vectorPoints[1]
                // edge 2 = vectorPoints[2], vectorPoints[3]
                // edge 3 = ....
                // spineEdgeSet = findSkeletonMesh.getSkeletonPointSet(*findSkeletonMesh.ps);
                // jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(spineEdgeSet);
                /* END find skeleton END */
                
                spineEdgeSet = mesh.getChordalAxisPointSet(*mesh.ps);
                mesh.loadEdgeFromPointSet(*mesh.ps);
                // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                
                for (int i = 0; i < mesh.pNum; i++) {
                    vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
                    colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                }
                
                for (int i = 0; i < spineEdgeSet.size(); i++)
                {
                    spineVertice.push_back(vec3(spineEdgeSet[i]->p[0], spineEdgeSet[i]->p[1], spineEdgeSet[i]->p[2]));
                    spineColors.push_back(vec3(0.16f, 0.50f, 0.73f));
                }

//                for (int i = 0; i < jointPointEdgeSet.size(); i++) {
//                    jointVertice.push_back(vec3(jointPointEdgeSet[i]->p[0], jointPointEdgeSet[i]->p[1], jointPointEdgeSet[i]->p[2]));
//                    jointColors.push_back(vec3(0.086f, 0.627f, 0.521f));
//                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                // drawMode = GL_LINES;
                drawMode = mesh.drawMode;
                lineWidth = thinLine;
                // drawMode = findSkeletonMesh.drawMode;
                
                readContourData = true;
                readContourDataCount++;
            }

            
        }
        
        if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            if(!readContourData)
            {
                vector<Point*> contourpoints;
                
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                /*
                 * STEP 1: Create CDT and add primary polyline
                 * NOTE: polyline must be a simple polygon. The polyline's points
                 * constitute constrained edges. No repeat points!!!
                 */
                clock_t init_time = clock();
                p2t::CDT* cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();
                clock_t end_time = clock();
                cout << "Elapsed time (sec) = " << (end_time-init_time)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
                map = cdt->GetMap();
                
                
                mesh.loadP2tPoints(polyline);
                findSkeletonMesh.loadP2tPoints(polyline);
                
                mesh.addP2tTriangles(triangles);
                findSkeletonMesh.addP2tTriangles(triangles);
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                
                
                /* find skeleton */
                vector<Point*> spineEdgeSet;
                vector<Point*> jointPointEdgeSet;
                // remove some T triangle, like T-J, T-S
                findSkeletonMesh.removeTerminalTriangleWithJoint(*findSkeletonMesh.ps);
                // findSkeletonMesh.removeTerminalTriangleWithSleeve(*findSkeletonMesh.ps);
                // get edge of points, return vector<Point*>
                // edge 1 = vectorPoints[0], vectorPoints[1]
                // edge 2 = vectorPoints[2], vectorPoints[3]
                // edge 3 = ....
                spineEdgeSet = findSkeletonMesh.getSkeletonPointSet(*findSkeletonMesh.ps);
                // spineEdgeSet = findSkeletonMesh.removeRedundantTerminalEdges(spineEdgeSet);
                
                jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(spineEdgeSet);
                // jointPointEdgeSet = findSkeletonMesh.getSimplifiedPointEdgeSet(spineEdgeSet);
                /* END find skeleton END */
                
                mesh.loadEdgeFromPointSet(*mesh.ps);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                
                for (int i = 0; i < jointPointEdgeSet.size()/2; i++) {
                    Point* p1 = jointPointEdgeSet[2*i];
                    Point* p2 = jointPointEdgeSet[2*i+1];
                    
                    double dx = p2->p[0]-p1->p[0];
                    double dy = p2->p[1]-p1->p[1];
                    double width = 12.0;
                    double height = 6.0;
                    glm::vec2 lineVec = vec2(dx*height/sqrt(dx*dx+dy*dy) , dy*height/sqrt(dx*dx+dy*dy));
                    
                    glm::vec2 verticalLineVec = vec2(-dy*width/sqrt(dx*dx+dy*dy) , dx*width/sqrt(dx*dx+dy*dy));
                    
                    Point* p1_r = new Point(1, p1->p[0]+(-lineVec.x)+(verticalLineVec.x), p1->p[1]+(-lineVec.y)+(verticalLineVec.y), 0);
                    Point* p1_l = new Point(1, p1->p[0]+(-lineVec.x)+(-verticalLineVec.x), p1->p[1]+(-lineVec.y)+(-verticalLineVec.y), 0);
                    Point* p2_r = new Point(1, p2->p[0]+(lineVec.x)+(verticalLineVec.x), p2->p[1]+(lineVec.y)+(verticalLineVec.y), 0);
                    Point* p2_l = new Point(1, p2->p[0]+(lineVec.x)+(-verticalLineVec.x), p2->p[1]+(lineVec.y)+(-verticalLineVec.y), 0);
                    
                    vertices.push_back(vec3(p1_r->p[0], p1_r->p[1], p1_r->p[2]));
                    vertices.push_back(vec3(p1_l->p[0], p1_l->p[1], p1_l->p[2]));
                    
                    vertices.push_back(vec3(p2_r->p[0], p2_r->p[1], p2_r->p[2]));
                    vertices.push_back(vec3(p2_l->p[0], p2_l->p[1], p2_l->p[2]));
                    
                    vertices.push_back(vec3(p1_r->p[0], p1_r->p[1], p1_r->p[2]));
                    vertices.push_back(vec3(p2_r->p[0], p2_r->p[1], p2_r->p[2]));
                    
                    vertices.push_back(vec3(p1_l->p[0], p1_l->p[1], p1_l->p[2]));
                    vertices.push_back(vec3(p2_l->p[0], p2_l->p[1], p2_l->p[2]));
                    
                    for(int num=0; num < 6; num++)
                    {
                        colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                    }
                }
                
                
//                for (int i = 0; i < spineEdgeSet.size(); i++)
//                {
//                    spineVertice.push_back(vec3(spineEdgeSet[i]->p[0], spineEdgeSet[i]->p[1], spineEdgeSet[i]->p[2]));
//                    spineColors.push_back(vec3(0, 0, 255));
//                }
                //
                for (int i = 0; i < jointPointEdgeSet.size(); i++) {
                    jointVertice.push_back(vec3(jointPointEdgeSet[i]->p[0], jointPointEdgeSet[i]->p[1], jointPointEdgeSet[i]->p[2]));
                    jointColors.push_back(vec3(0.086f, 0.627f, 0.521f));
                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                // drawMode = GL_LINES;
                drawMode = GL_LINES;
                // drawMode = GL_QUADS;
                lineWidth = thinLine;
                // drawMode = findSkeletonMesh.drawMode;
                
                readContourData = true;
                readContourDataCount++;
            }

        }
        
        if (glfwGetKey( GLFW_KEY_RSHIFT ) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            if(!readContourData)
            {
                vector<Point*> contourpoints;
                
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                /*
                 * STEP 1: Create CDT and add primary polyline
                 * NOTE: polyline must be a simple polygon. The polyline's points
                 * constitute constrained edges. No repeat points!!!
                 */
                clock_t init_time = clock();
                p2t::CDT* cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();
                clock_t end_time = clock();
                cout << "Elapsed time (sec) = " << (end_time-init_time)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
                map = cdt->GetMap();
                
                
                mesh.loadP2tPoints(polyline);
                findSkeletonMesh.loadP2tPoints(polyline);
                
                mesh.addP2tTriangles(triangles);
                findSkeletonMesh.addP2tTriangles(triangles);
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                
                
                /* find skeleton */
                vector<Point*> spineEdgeSet;
                vector<Point*> jointPointEdgeSet;
                // remove some T triangle, like T-J, T-S
                findSkeletonMesh.removeTerminalTriangleWithJoint(*findSkeletonMesh.ps);
                // findSkeletonMesh.removeTerminalTriangleWithSleeve(*findSkeletonMesh.ps);
                // get edge of points, return vector<Point*>
                // edge 1 = vectorPoints[0], vectorPoints[1]
                // edge 2 = vectorPoints[2], vectorPoints[3]
                // edge 3 = ....
                spineEdgeSet = findSkeletonMesh.getSkeletonPointSet(*findSkeletonMesh.ps);
                // spineEdgeSet = findSkeletonMesh.removeRedundantTerminalEdges(spineEdgeSet);
                
//                jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(spineEdgeSet);
//                jointPointEdgeSet = findSkeletonMesh.removeSmallRedundantTerminalEdges(jointPointEdgeSet);
                
                
                jointPointEdgeSet = findSkeletonMesh.getSimplifiedPointEdgeSet(spineEdgeSet);
                jointPointEdgeSet = findSkeletonMesh.removeSmallRedundantTerminalEdges(jointPointEdgeSet);
                // jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(jointPointEdgeSet);
                /* END find skeleton END */
                
                mesh.loadEdgeFromPointSet(*mesh.ps);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                
                for (int i = 0; i < jointPointEdgeSet.size()/2; i++) {
                    Point* p1 = jointPointEdgeSet[2*i];
                    Point* p2 = jointPointEdgeSet[2*i+1];
                    
                    double dx = p2->p[0]-p1->p[0];
                    double dy = p2->p[1]-p1->p[1];
                    double width = 12.0;
                    double height = 6.0;
                    glm::vec2 lineVec = vec2(dx*height/sqrt(dx*dx+dy*dy) , dy*height/sqrt(dx*dx+dy*dy));
                    
                    glm::vec2 verticalLineVec = vec2(-dy*width/sqrt(dx*dx+dy*dy) , dx*width/sqrt(dx*dx+dy*dy));
                    
                    Point* p1_r = new Point(1, p1->p[0]+(-lineVec.x)+(verticalLineVec.x), p1->p[1]+(-lineVec.y)+(verticalLineVec.y), 0);
                    Point* p1_l = new Point(1, p1->p[0]+(-lineVec.x)+(-verticalLineVec.x), p1->p[1]+(-lineVec.y)+(-verticalLineVec.y), 0);
                    Point* p2_r = new Point(1, p2->p[0]+(lineVec.x)+(verticalLineVec.x), p2->p[1]+(lineVec.y)+(verticalLineVec.y), 0);
                    Point* p2_l = new Point(1, p2->p[0]+(lineVec.x)+(-verticalLineVec.x), p2->p[1]+(lineVec.y)+(-verticalLineVec.y), 0);
                    
                    vertices.push_back(vec3(p1_r->p[0], p1_r->p[1], p1_r->p[2]));
                    vertices.push_back(vec3(p1_l->p[0], p1_l->p[1], p1_l->p[2]));
                    
                    vertices.push_back(vec3(p2_r->p[0], p2_r->p[1], p2_r->p[2]));
                    vertices.push_back(vec3(p2_l->p[0], p2_l->p[1], p2_l->p[2]));
                    
                    vertices.push_back(vec3(p1_r->p[0], p1_r->p[1], p1_r->p[2]));
                    vertices.push_back(vec3(p2_r->p[0], p2_r->p[1], p2_r->p[2]));
                    
                    vertices.push_back(vec3(p1_l->p[0], p1_l->p[1], p1_l->p[2]));
                    vertices.push_back(vec3(p2_l->p[0], p2_l->p[1], p2_l->p[2]));
                    
                    for(int num=0; num < 6; num++)
                    {
                        colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                    }
                }
                
                
                //                for (int i = 0; i < spineEdgeSet.size(); i++)
                //                {
                //                    spineVertice.push_back(vec3(spineEdgeSet[i]->p[0], spineEdgeSet[i]->p[1], spineEdgeSet[i]->p[2]));
                //                    spineColors.push_back(vec3(0, 0, 255));
                //                }
                //
                for (int i = 0; i < jointPointEdgeSet.size(); i++) {
                    jointVertice.push_back(vec3(jointPointEdgeSet[i]->p[0], jointPointEdgeSet[i]->p[1], jointPointEdgeSet[i]->p[2]));
                    jointColors.push_back(vec3(0.086f, 0.627f, 0.521f));
                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                // drawMode = GL_LINES;
                drawMode = GL_LINES;
                // drawMode = GL_QUADS;
                lineWidth = thinLine;
                // drawMode = findSkeletonMesh.drawMode;
                
                readContourData = true;
                readContourDataCount++;
            }
            
        }
        
        if (glfwGetKey( GLFW_KEY_ENTER ) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            if(!readContourData)
            {
                vector<Point*> contourpoints;
                
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                /*
                 * STEP 1: Create CDT and add primary polyline
                 * NOTE: polyline must be a simple polygon. The polyline's points
                 * constitute constrained edges. No repeat points!!!
                 */
                clock_t init_time = clock();
                p2t::CDT* cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();
                clock_t end_time = clock();
                cout << "Elapsed time (sec) = " << (end_time-init_time)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
                map = cdt->GetMap();
                
                
                mesh.loadP2tPoints(polyline);
                findSkeletonMesh.loadP2tPoints(polyline);
                
                mesh.addP2tTriangles(triangles);
                findSkeletonMesh.addP2tTriangles(triangles);
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                
                
                /* find skeleton */
                vector<Point*> spineEdgeSet;
                vector<Point*> jointPointEdgeSet;
                // remove some T triangle, like T-J, T-S
                findSkeletonMesh.removeTerminalTriangleWithJoint(*findSkeletonMesh.ps);
                // findSkeletonMesh.removeTerminalTriangleWithSleeve(*findSkeletonMesh.ps);
                // get edge of points, return vector<Point*>
                // edge 1 = vectorPoints[0], vectorPoints[1]
                // edge 2 = vectorPoints[2], vectorPoints[3]
                // edge 3 = ....
                spineEdgeSet = findSkeletonMesh.getSkeletonPointSet(*findSkeletonMesh.ps);
                jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(spineEdgeSet);
                /* END find skeleton END */
                
                mesh.loadEdgeFromPointSet(*mesh.ps);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                
                for (int i = 0; i < mesh.pNum; i++) {
                    vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
                    colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                }
                
//                for (int i = 0; i < spineEdgeSet.size(); i++)
//                {
//                    spineVertice.push_back(vec3(spineEdgeSet[i]->p[0], spineEdgeSet[i]->p[1], spineEdgeSet[i]->p[2]));
//                    spineColors.push_back(vec3(0.75f, 0.22f, 0.16f));
//                }

                for (int i = 0; i < jointPointEdgeSet.size(); i++) {
                    jointVertice.push_back(vec3(jointPointEdgeSet[i]->p[0], jointPointEdgeSet[i]->p[1], jointPointEdgeSet[i]->p[2]));
                    jointColors.push_back(vec3(0.086f, 0.627f, 0.521f));
                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                // drawMode = GL_LINES;
                drawMode = mesh.drawMode;
                lineWidth = thinLine;
                // drawMode = findSkeletonMesh.drawMode;
                
                readContourData = true;
                readContourDataCount++;
            }
            
        }
        
        if (glfwGetKey( GLFW_KEY_BACKSPACE ) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourDataCount++;
                if(readContourDataCount%9 == 0)
                {
                    readContourData = false;
                }
            }
            
            if(!readContourData)
            {
                vector<Point*> contourpoints;
                
                /* p2t data */
                /// Constrained triangles
                vector<p2t::Triangle*> triangles;
                /// Triangle map (include edge outside the contour)
                list<p2t::Triangle*> map;
                /// Polylines (if there are holes in the polygon)
                // vector< vector<p2t::Point*> > polylines;
                /// Polyline
                vector<p2t::Point*> polyline;
                
                
                
                string filenum_str = to_string(filenum);
                string filename ="/Users/andikan/Dev/gauss/GaussMesh/GaussMesh/data/";
                filename.append(filenum_str);
                filename.append(".txt");
                
                cout << "Start processing " << filenum_str << ".txt" << endl;
                
                ContourReader reader(filename);
                contourpoints = reader.getContourPoints();
                
                clickPoint.clear();
                vertices.clear();
                colors.clear();
                
                for(int i=0; i<contourpoints.size(); i++)
                {
                    polyline.push_back(new p2t::Point(contourpoints[i]->p[0], contourpoints[i]->p[1]));
                }
                
                /*
                 * STEP 1: Create CDT and add primary polyline
                 * NOTE: polyline must be a simple polygon. The polyline's points
                 * constitute constrained edges. No repeat points!!!
                 */
                clock_t init_time = clock();
                p2t::CDT* cdt = new p2t::CDT(polyline);
                cdt->Triangulate();
                triangles = cdt->GetTriangles();
                clock_t end_time = clock();
                cout << "Elapsed time (sec) = " << (end_time-init_time)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
                map = cdt->GetMap();
                
                
                mesh.loadP2tPoints(polyline);
                findSkeletonMesh.loadP2tPoints(polyline);
                
                mesh.addP2tTriangles(triangles);
                findSkeletonMesh.addP2tTriangles(triangles);
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                
                
                /* find skeleton */
                vector<Point*> spineEdgeSet;
                vector<Point*> jointPointEdgeSet;
                // remove some T triangle, like T-J, T-S
                findSkeletonMesh.removeTerminalTriangleWithJoint(*findSkeletonMesh.ps);
                // findSkeletonMesh.removeTerminalTriangleWithSleeve(*findSkeletonMesh.ps);
                // get edge of points, return vector<Point*>
                // edge 1 = vectorPoints[0], vectorPoints[1]
                // edge 2 = vectorPoints[2], vectorPoints[3]
                // edge 3 = ....
                spineEdgeSet = findSkeletonMesh.getSkeletonPointSet(*findSkeletonMesh.ps);
                // jointPointEdgeSet = findSkeletonMesh.getJointPointEdgeSet(spineEdgeSet);
                /* END find skeleton END */
                
                mesh.getSpinePoints(*mesh.ps);
                mesh.loadEdgeFromPointSet(*mesh.ps);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                jointVertice.clear();
                jointColors.clear();
                
                for (int i = 0; i < mesh.pNum; i++) {
                    vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
                    colors.push_back(vec3(0.17f, 0.24f, 0.31f));
                }
                
                for (int i = 0; i < spineEdgeSet.size(); i++)
                {
                    spineVertice.push_back(vec3(spineEdgeSet[i]->p[0], spineEdgeSet[i]->p[1], spineEdgeSet[i]->p[2]));
                    spineColors.push_back(vec3(0.16f, 0.50f, 0.73f));
                }

//                for (int i = 0; i < jointPointEdgeSet.size(); i++) {
//                    jointVertice.push_back(vec3(jointPointEdgeSet[i]->p[0], jointPointEdgeSet[i]->p[1], jointPointEdgeSet[i]->p[2]));
//                    jointColors.push_back(vec3(0, 255, 255));
//                }
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointVertice.size() * sizeof(vec3), &jointVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, jointColors.size() * sizeof(vec3), &jointColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                // drawMode = GL_LINES;
                drawMode = mesh.drawMode;
                // drawMode = findSkeletonMesh.drawMode;
                
                readContourData = true;
                readContourDataCount++;
            }

        }
        
        
        
        
        
        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 Projection;
        if(!mouseDrawPress)
        {
            // Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
            Projection = glm::perspective(45.0f, 4.0f / 3.0f, 1.0f, 1000.0f);
        }
        else
        {
            Projection = glm::ortho(-5.12f, 5.12f, -3.50f, 3.50f, 0.1f, 100.0f);
        }
        
        // Camera matrix
        glm::mat4 View;
        if(!mouseDrawPress)
            View = glm::lookAt(glm::vec3(0,0,400), glm::vec3(0,0,0), glm::vec3(0,1,0) );
        else
            View = glm::lookAt(glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0) );
        
        
        
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 Model      = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
        
        // Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        
        
//        // Draw axes
//        // 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(vertexPosition_modelspaceID);
//		glBindBuffer(GL_ARRAY_BUFFER, axesVertexBuffer);
//		glVertexAttribPointer( vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
//        
//		// 2nd attribute buffer : colors
//		glEnableVertexAttribArray(vertexColorID);
//		glBindBuffer(GL_ARRAY_BUFFER, axesColorbuffer);
//		glVertexAttribPointer(vertexColorID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
//        // Draw axes lines
//        glLineWidth(1.0);
//        glDrawArrays(GL_LINES, 0, 6*3);
        
        

        
        // 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer( vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        
		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(vertexColorID);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(vertexColorID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        
		// Draw the triangleS !
        glPointSize(3.0);
        glEnable(GL_POINT_SMOOTH);
        glLineWidth(lineWidth);
        glDrawArrays(drawMode, 0, (int)vertices.size());
        // GL_TRIANGLES, GL_POINTS, GL_LINES
        
        
        // Draw spine point
        // 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
		glVertexAttribPointer( vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        
		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(vertexColorID);
		glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
		glVertexAttribPointer(vertexColorID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        // Draw spine point
        glLineWidth(boldLine);
        glDrawArrays(GL_LINES, 0, (int)spineVertice.size());
        
        
        // Draw joint point
        // 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, jointVertexBuffer);
		glVertexAttribPointer( vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        
		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(vertexColorID);
		glBindBuffer(GL_ARRAY_BUFFER, jointColorBuffer);
		glVertexAttribPointer(vertexColorID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        // Draw spine point
        glLineWidth(boldLine);
        // glEnable(GL_POINT_SMOOTH);
        glDrawArrays(GL_LINES, 0, (int)jointVertice.size());

        
        
        
        // glDisableVertexAttribArray
		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexColorID);
        
    
		// Swap buffers
		glfwSwapBuffers();
        
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
          glfwGetWindowParam( GLFW_OPENED ) );
    
    // Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
    
    glDeleteBuffers(1, &spineVertexBuffer);
	glDeleteBuffers(1, &spineColorBuffer);
    
    glDeleteBuffers(1, &cubevertexbuffer);
	glDeleteBuffers(1, &cubecolorbuffer);

    glDeleteBuffers(1, &axesVertexBuffer);
	glDeleteBuffers(1, &axesColorbuffer);
	glDeleteProgram(programID);
    
    
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
    
    
    return 0;
}

