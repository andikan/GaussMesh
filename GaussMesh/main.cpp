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
	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
    
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
    vector<vec3> clickPoint;
    vector<vec3> vertices;
	vector<vec3> colors;
    
    vector<vec3> spineVertice;
    vector<vec3> spineColors;

    Mesh mesh;
    GLenum drawMode;
    
    int filenum = 0;

    
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
                mesh.delaunayTriangulation();
                mesh.loadEdgeFromPointSet(*mesh.ps);
                // constraintBound
                mesh.constraintBound();
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
        
        if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS){
            
            if(!spineDetect)
            {
                cout << "key press\n";
                vector<Point*> spinePointset;
                // get spine points
                spinePointset = mesh.getSpinePoints(*mesh.ps);
                mesh.loadEdgeFromPointSet(*mesh.ps);
                spineVertice.clear();
                spineColors.clear();
                
                vertices.clear();
                colors.clear();
                
                for (int i = 0; i < spinePointset.size(); i++)
                {
                    if(i > 0)
                    {
                        spineVertice.push_back(vec3(spinePointset[i-1]->p[0], spinePointset[i-1]->p[1], spinePointset[i-1]->p[2]));
                        spineVertice.push_back(vec3(spinePointset[i]->p[0], spinePointset[i]->p[1], spinePointset[i]->p[2]));
                        spineColors.push_back(vec3(255, 235, 0));
                        spineColors.push_back(vec3(255, 235, 0));
                    }
                    
                }
                
                for (int i = 0; i < mesh.pNum; i++)
                {
					vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
					colors.push_back(vec3(255, 255, 255));
                }
                spineDetect = true;
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
            
            glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
            glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
            
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
            
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);

		}
        
        
        if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS)
        {
            if(readContourData)
            {
                readContourData = false;
            }
            
            if( filenum == 31 )
            {
                filenum = 0;
            }
            
            if(!readContourData)
            {
                filenum = filenum + 1;
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
                    // clickPoint.push_back(vec3(contourpoints[i]->p[0], contourpoints[i]->p[1], contourpoints[i]->p[2]));
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
                cout << "mesh vertex num : " << mesh.ps->pSet.size() << endl;
                mesh.addP2tTriangles(triangles);
                
                
                
                vector<Point*> spinePointset;
                // get spine points
                spinePointset = mesh.getSpinePoints(*mesh.ps);
                // pruneAndSpine(*mesh.ps);
                
                
                // mesh.loadTriangleFromPointSet(*mesh.ps);
                mesh.loadEdgeFromPointSet(*mesh.ps);
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                
                
                vertices.clear();
                colors.clear();
                spineVertice.clear();
                spineColors.clear();
                
                for (int i = 0; i < mesh.pNum; i++) {
                    vertices.push_back(vec3(mesh.vertexBuffer[i*3], mesh.vertexBuffer[i*3+1], mesh.vertexBuffer[i*3+2]));
                    colors.push_back(vec3(255, 255, 255));
                }
                
                for (int i = 0; i < spinePointset.size(); i++)
                {
                    if(i > 0)
                    {
                        spineVertice.push_back(vec3(spinePointset[i-1]->p[0], spinePointset[i-1]->p[1], spinePointset[i-1]->p[2]));
                        spineVertice.push_back(vec3(spinePointset[i]->p[0], spinePointset[i]->p[1], spinePointset[i]->p[2]));
                        spineColors.push_back(vec3(255, 0, 0));
                        spineColors.push_back(vec3(255, 0, 0));
                    }
                    
                }
                
                // mesh.loadEdgeFromMouse(clickPoint);
                // mesh.loadAndSortPointSet();
                // mesh.loadEdgeFromMouse(clickPoint);
                //
                
                // clock_t t1, t2, t3, t4;
                // t1 = clock();
                
                // mesh.delaunayTriangulation();
                // mesh.constraintBound();
                // mesh.loadEdgeFromPointSet(*mesh.ps);
                // 
                
//                t2 = clock();
//                cout << "CDT time : " << (t2-t1)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
//                cout << "after CDT triangle number : " << mesh.ps->triSet.size() << endl;
//                
//                
//                
//                vector<Point*> spinePointset;
//                // get spine points
//                t3 = clock();
//                spinePointset = mesh.getSpinePoints(*mesh.ps);
//                t4 = clock();
//                cout << "PruneSpine time : " << (t4-t3)/(double)(CLOCKS_PER_SEC) << " sec" << endl;
//                cout << "after PruneSpine triangle number : " << mesh.ps->triSet.size() << endl;
//                cout << "Spine point number : " << spinePointset.size() << endl;
//
//                
//                
//                
//                mesh.loadEdgeFromPointSet(*mesh.ps);
//                spineVertice.clear();
//                spineColors.clear();
//                
//                vertices.clear();
//                colors.clear();
//                
//                for (int i = 0; i < spinePointset.size(); i++)
//                {
//                    if(i > 0)
//                    {
//                        spineVertice.push_back(vec3(spinePointset[i-1]->p[0], spinePointset[i-1]->p[1], spinePointset[i-1]->p[2]));
//                        spineVertice.push_back(vec3(spinePointset[i]->p[0], spinePointset[i]->p[1], spinePointset[i]->p[2]));
//                        spineColors.push_back(vec3(255, 0, 0));
//                        spineColors.push_back(vec3(255, 0, 0));
//                    }
//                    
//                }
                
                
                
//                for(unsigned int i=0; i<triangles.size(); i++)
//                {
//                    p2t::Triangle t = *triangles[i];
//                    p2t::Point a = *t.GetPoint(0);
//                    p2t::Point b = *t.GetPoint(1);
//                    p2t::Point c = *t.GetPoint(2);
//                    
//                    vertices.push_back(vec3(b.x, b.y, 0.0f));
//                    vertices.push_back(vec3(c.x, c.y, 0.0f));
//
//                    vertices.push_back(vec3(c.x, c.y, 0.0f));
//                    vertices.push_back(vec3(a.x, a.y, 0.0f));
//
//                    vertices.push_back(vec3(a.x, a.y, 0.0f));
//                    vertices.push_back(vec3(b.x, b.y, 0.0f));
//
//                    
//                    for(int i=0; i<3; i++){
//                        if(t.constrained_edge[i]){
//                            colors.push_back(vec3(255, 0, 255));
//                            colors.push_back(vec3(255, 0, 255));
//                        }
//                        else{
//                            colors.push_back(vec3(255, 255, 255));
//                            colors.push_back(vec3(255, 255, 255));
//                        }
//                    }
//                }
                
                
                                
                
                
                glBindBuffer(GL_ARRAY_BUFFER, spineVertexBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineVertice.size() * sizeof(vec3), &spineVertice[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, spineColorBuffer);
                glBufferData(GL_ARRAY_BUFFER, spineColors.size() * sizeof(vec3), &spineColors[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
                glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
                
                glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
                glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
                
                
                // drawMode = GL_LINES;
                drawMode = mesh.drawMode;

                readContourData = true;
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
            View = glm::lookAt(glm::vec3(50,50,450), glm::vec3(0,0,0), glm::vec3(0,1,0) );
        else
            View = glm::lookAt(glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0) );
        
        
        
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 Model      = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around
        
        // Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        
        
        // Draw axes
        // 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, axesVertexBuffer);
		glVertexAttribPointer( vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        
		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(vertexColorID);
		glBindBuffer(GL_ARRAY_BUFFER, axesColorbuffer);
		glVertexAttribPointer(vertexColorID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
        // Draw axes lines
        glDrawArrays(GL_LINES, 0, 6*3);
        
        

        
        // 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
                              vertexPosition_modelspaceID, // The attribute we want to configure
                              3,                           // size
                              GL_FLOAT,                    // type
                              GL_FALSE,                    // normalized?
                              0,                           // stride
                              (void*)0                     // array buffer offset
                              );
        
		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(vertexColorID);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
                              vertexColorID,               // The attribute we want to configure
                              3,                           // size
                              GL_FLOAT,                    // type
                              GL_FALSE,                    // normalized?
                              0,                           // stride
                              (void*)0                     // array buffer offset
                              );
        
		// Draw the triangleS !
        glPointSize(3.0);
        glEnable(GL_POINT_SMOOTH);
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
        glLineWidth(2.0);
        glPointSize(5.0);
        glEnable(GL_POINT_SMOOTH);
        glDrawArrays(GL_POINTS, 0, (int)spineVertice.size());
        
        
        
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

