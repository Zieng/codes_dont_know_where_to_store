#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "load_funcs.hpp"
#include "control.hpp"

using namespace std;

GLFWwindow * window;

struct node
{
    glm::vec3 pos;   
    glm::vec3 velocity; 
    glm::vec3 color;
};
typedef struct  node particle;
std::vector<particle> particleSet;

int initWindow()
{
    if (!glfwInit()) {
        std::cout<<"glfw init failed!"<<std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window=glfwCreateWindow(1360, 760, "Beautiful, the color cube!", NULL, NULL);
    if (window==NULL) {
        std::cout<<"create window failed!"<<std::endl;
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental=true;
    if (glewInit()!=GLEW_OK) {
        std::cout<<"Failed to init glew"<<std::endl;
        return -1;
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_PROGRAM_POINT_SIZE);
    
    return 0;
}

glm::vec3 position = glm::vec3( 3, 3, 3);
float horizontalAngle = 3.92f;
float verticalAngle = 2.35f - 3.14f;
float initialFoV = 45.0f;
float speed = 3.f; // 3 units / second
float mouseSpeed = 0.005f;

void computeMatricesFromInputs(glm::mat4 & modelMatrix, glm::mat4 & viewMatrix, glm::mat4 & projectionMatrix){
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;
    
    static double old_xpos,old_ypos;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) {
        horizontalAngle += mouseSpeed * float(old_xpos - xpos);
        verticalAngle   += mouseSpeed * float(old_ypos - ypos );
    }
    old_xpos=xpos;
    old_ypos=ypos;
    
    glm::vec3 direction(
                        cos(verticalAngle) * sin(horizontalAngle),
                        sin(verticalAngle),
                        cos(verticalAngle) * cos(horizontalAngle)
                        );
    
    glm::vec3 right = glm::vec3(
                                sin(horizontalAngle - 3.14f/2.0f),
                                0,
                                cos(horizontalAngle - 3.14f/2.0f)
                                );
    
    glm::vec3 up = glm::cross( right, direction );
    
    // Move forward
    if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position -= right * deltaTime * speed;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS){
        position = glm::vec3(0.0, 0.0, 3.0);
        up = glm::vec3(0.0, 1.0, 0.0);
        horizontalAngle = 3.14;
        verticalAngle = 0.0;
    }
    
    // std::cout<<"The vertical angle is: "<<verticalAngle<<"; The horizontal angle is: "<<horizontalAngle<<std::endl;
    
    float FoV = initialFoV ;//- 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
    modelMatrix=glm::mat4(1.0f);
    projectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
    viewMatrix       = glm::lookAt(
                                   position,           // Camera is here
                                   position+direction, // and looks here : at the same position, plus "direction"
                                   up                  // Head is up (set to 0,-1,0 to look upside-down)
                                   );
}

int main(int argc, const char * argv[])
{
    int worldScale=10;
    
    if (initWindow()!=0) {
        return -1;
    }
 
    GLuint ProgramID=load_shaders("VertexShader.glsl", "FragmentShader.glsl");
    if (ProgramID==-1) {
        return -1;
    }
    int choose=0;
    glUseProgram(ProgramID);
    GLuint MVPID=glGetUniformLocation(ProgramID,"MVP");
    GLuint RenderID=glGetUniformLocation(ProgramID,"choose");
    
    int particleNum=250;
    int win_width,win_height;
    std::vector<glm::vec3> posSet;
    std::vector<glm::vec3> colorSet;
    std::vector<glm::vec3> velocitySet;
    
    glfwGetWindowSize(window,&win_width,&win_height);
    for(int i=0;i<particleNum;i++)
    {
        particle p;
        p.pos=glm::vec3(  2*(rand()%worldScale-worldScale/2)/(float)worldScale, 2*(rand()%worldScale-worldScale/2)/(float)worldScale,  2*(rand()%worldScale-worldScale/2)/(float)worldScale  );
        // p.pos=glm::vec3(i,i,i);
        // p.pos=glm::vec3(rand()%worldScale,rand()%worldScale,rand()%worldScale);
        // cout<<p.pos.x<<","<<p.pos.y<<","<<p.pos.z<<","<<endl;
        if(std::find(posSet.begin(),posSet.end(),p.pos)  !=  posSet.end()){
            i--;
        }
        else{
            p.velocity=glm::vec3(  (rand()%10-5)/(float)5000,  (rand()%10-5)/(float)5000,  (rand()%10-5)/(float)5000  );
            // cout<<"velocity:"<<p.velocity.x<<","<<p.velocity.y<<","<<p.velocity.z<<endl;
            // p.color=glm::vec3( rand()%100/(float)100,rand()%100/(float)100,rand()%100/(float)100);
            p.color=glm::vec3(1,1,1);
            posSet.push_back(p.pos);
            colorSet.push_back(p.color);
            velocitySet.push_back(p.velocity);

            particleSet.push_back(p);
        }
    }

    static const GLfloat cube_data[] = {
        -1.0f,-1.0f,-1.0f, 
        -1.0f,-1.0f, 1.0f,  //
        -1.0f,-1.0f, 1.0f,
        -1.0f,1.0f, 1.0f,   //
        -1.0f,1.0f, 1.0f,
        -1.0f,1.0f, -1.0f,  //
        -1.0f,1.0f, -1.0f,
        -1.0f,-1.0f,-1.0f,  //
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,  //
        -1.0f,-1.0f,1.0f,
        1.0f,-1.0f,1.0f,  //
        -1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,   //
        -1.0f,1.0f,-1.0f,
        1.0f,1.0f,-1.0f,  //
        1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,1.0f,  //
        1.0f,-1.0f,1.0f,
        1.0f,1.0f,1.0f,   //
        1.0f,1.0f,1.0f,
        1.0f,1.0f,-1.0f,  //
        1.0f,1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
    };
    static const GLfloat cube_color_data[] = {
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,
    };

    GLuint vertexArray;
    glGenVertexArrays(1,&vertexArray);
    glBindVertexArray(vertexArray);
    
    GLuint cubeVertexBuffer;
    glGenBuffers(1,&cubeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,cubeVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(cube_data), cube_data, GL_STATIC_DRAW);

    GLuint cubeColorBuffer;
    glGenBuffers(1,&cubeColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,cubeColorBuffer);
    glBufferData(GL_ARRAY_BUFFER,  sizeof(cube_color_data), cube_color_data, GL_STATIC_DRAW);


    GLuint vertexBuffer;
    glGenBuffers(1,&vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, posSet.size() * sizeof(glm::vec3), &posSet[0], GL_STATIC_DRAW);
    
    GLuint colorBuffer;
    glGenBuffers(1,&colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,colorBuffer);
    glBufferData(GL_ARRAY_BUFFER,colorSet.size() * sizeof(glm::vec3), &colorSet[0], GL_STATIC_DRAW);
    
    do{
        typedef vector<glm::vec3> group;
        vector< group> lineSet;

        for(int i=0;i<particleNum;i++){
            // border collision check
            if(std::abs(posSet[i].x+velocitySet[i].x)>1)
                velocitySet[i].x=-velocitySet[i].x;
            if(std::abs(posSet[i].y+velocitySet[i].y)>1)
                velocitySet[i].y=-velocitySet[i].y;
            if(std::abs(posSet[i].z+velocitySet[i].z)>1)
                velocitySet[i].z=-velocitySet[i].z;
            posSet[i].x=posSet[i].x+velocitySet[i].x;
            posSet[i].y=posSet[i].y+velocitySet[i].y;
            posSet[i].z=posSet[i].z+velocitySet[i].z;
            // prepare line them up
            group g;
            g.push_back(posSet[i]);
            for(int j=i+1;j<particleNum;j++){
                if(abs(posSet[i].x-posSet[j].x) + abs(posSet[i].y-posSet[j].y ) + abs(posSet[i].z-posSet[j].z) <= 0.2)
                    g.push_back(posSet[j]);
            }
            lineSet.push_back(g);
        }

        glm::mat4 modelMatrix,viewMatrix,projectionMatrix;
        computeMatricesFromInputs(modelMatrix,viewMatrix,projectionMatrix);
        glm::mat4 MVP=projectionMatrix * viewMatrix * modelMatrix;

        glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, posSet.size() * sizeof(glm::vec3), &posSet[0], GL_STATIC_DRAW);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(ProgramID);
        
        glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
        glUniform1i(RenderID,choose);
        
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);  

        // vertex position
        glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);

        // vertex color
        glBindBuffer(GL_ARRAY_BUFFER,colorBuffer);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void *)0);
        
        glDrawArrays(GL_POINTS, 0, (GLuint) posSet.size());

        // cube border
        glBindBuffer(GL_ARRAY_BUFFER,cubeVertexBuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
        glBindBuffer(GL_ARRAY_BUFFER,cubeColorBuffer);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void *)0);
        glDrawArrays(GL_LINES, 0, 24);      

        //line them up
        for(int i=0;i<lineSet.size();i++){
            group g=lineSet[i];

            GLuint lineBuffer;
            glGenBuffers(1,&lineBuffer);
            glBindBuffer(GL_ARRAY_BUFFER,lineBuffer);
            glBufferData(GL_ARRAY_BUFFER,g.size() * sizeof(glm::vec3),&g[0],GL_STATIC_DRAW);
            glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);
            glDrawArrays(GL_LINE_LOOP,0,(GLuint) g.size());

            glDeleteBuffers(1,&lineBuffer);
        }
        
        glDisableVertexAttribArray(0);   // AttribArray 必须在调用glDrawArrays之后才能关闭
        glDisableVertexAttribArray(1);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }while (glfwGetKey(window, GLFW_KEY_ESCAPE)!=GLFW_PRESS && glfwWindowShouldClose(window)==0);
    
    glDeleteBuffers(1,&vertexBuffer);
    glDeleteBuffers(1,&colorBuffer);
    glDeleteBuffers(1,&cubeVertexBuffer);
    glDeleteBuffers(1,&cubeColorBuffer);
    glDeleteProgram(ProgramID);
    glDeleteVertexArrays(1,&vertexArray);
    glfwTerminate();
}

