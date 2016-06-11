#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>
//#include <gtc/type_ptr.hpp>

#include "load_funcs.hpp"

using namespace std;

GLFWwindow * window;

glm::vec3 position = glm::vec3( 3, 3, 3 );
float horizontalAngle = 3.92;
float verticalAngle = 2.35f - 3.14f;
float initialFoV = 45.0f;
float speed = 3.f; // 3 units / second
float mouseSpeed = 0.005f;

struct node
{
    glm::vec3 pos;   // z will be 0
    glm::vec3 velocity;   // z will be 0
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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glEnable(GL_PROGRAM_POINT_SIZE);

    return 0;
}

int main(int argc, const char * argv[])
{
    std::vector< glm::vec3 > vertices;

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

    int particleNum=200;
    int win_width,win_height;
    std::vector<glm::vec3> posSet;
    std::vector<glm::vec3> colorSet;
    std::vector<glm::vec3> velocitySet;

    glfwGetWindowSize(window,&win_width,&win_height);
    for(int i=0;i<particleNum;i++)
    {
        particle p;
        p.pos=glm::vec3(  2*(rand()%win_width-win_width/2)/(float)win_width, 2*(rand()%win_height-win_height/2)/(float)win_height,  0  );

        if(std::find(posSet.begin(),posSet.end(),p.pos)  !=  posSet.end()){
            i--;
        }
        else{
            p.velocity=glm::vec3(  (rand()%10-5)/(float)5000,  (rand()%10-5)/(float)5000,  0  );
            p.color=glm::vec3( rand()%100/(float)100,rand()%100/(float)100,rand()%100/(float)100);
            posSet.push_back(p.pos);
            colorSet.push_back(p.color);
            velocitySet.push_back(p.velocity);

            particleSet.push_back(p);
        }
    }


    GLuint vertexArray;
    glGenVertexArrays(1,&vertexArray);
    glBindVertexArray(vertexArray);

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
        double cursorPosX,cursorPosY;

        glfwGetCursorPos(window, &cursorPosX, &cursorPosY);
        cursorPosX=(2*cursorPosX - win_width) / win_width;
        cursorPosY=(win_height - 2*cursorPosY ) / win_height;
        // cout<<cursorPosX<<","<<cursorPosY<<endl;
        group cursorGroup;
        glm::vec3 cursorPoint=glm::vec3(cursorPosX,cursorPosY,0);

        cursorGroup.push_back(cursorPoint);
        for(int i=0;i<particleNum;i++){
            if(abs(posSet[i].x-cursorPosX) + abs(posSet[i].y-cursorPosY) <= 0.2)
                cursorGroup.push_back(posSet[i]);
        }
        lineSet.push_back(cursorGroup);

        for(int i=0;i<particleNum;i++){
            // border collision check
            if(std::abs(posSet[i].x+velocitySet[i].x)>1)
                velocitySet[i].x=-velocitySet[i].x;
            if(std::abs(posSet[i].y+velocitySet[i].y)>1)
                velocitySet[i].y=-velocitySet[i].y;
            posSet[i].x=posSet[i].x+velocitySet[i].x;
            posSet[i].y=posSet[i].y+velocitySet[i].y;
            // line them up
            group g;
            g.push_back(posSet[i]);
            for(int j=i+1;j<particleNum;j++){
                if(abs(posSet[i].x-posSet[j].x) + abs(posSet[i].y-posSet[j].y) <= 0.1)
                    g.push_back(posSet[j]);
            }
            lineSet.push_back(g);
        }

        glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, posSet.size() * sizeof(glm::vec3), &posSet[0], GL_STATIC_DRAW);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(ProgramID);

        glm::mat4 MVP=glm::mat4(1.0f);

        glUniformMatrix4fv(MVPID,1,GL_FALSE,&MVP[0][0]);
        glUniform1i(RenderID,choose);

        // vertex position
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void *)0);

        // vertex color
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER,colorBuffer);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void *)0);

        glDrawArrays(GL_POINTS, 0, (GLuint) posSet.size());

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
    glDeleteProgram(ProgramID);
    glDeleteVertexArrays(1,&vertexArray);
    glfwTerminate();
}

