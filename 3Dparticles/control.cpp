#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "control.hpp"

glm::vec3 position = glm::vec3( 3, 3, 3 );
float horizontalAngle = 3.92;
float verticalAngle = 2.35f - 3.14f;
float initialFoV = 45.0f;
float speed = 3.f; // 3 units / second
float mouseSpeed = 0.005f;

void computeMatricesFromInputs(GLFWwindow * window,glm::mat4 & modelMatrix, glm::mat4 & viewMatrix, glm::mat4 projectionMatrix){
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
    
    std::cout<<"The vertical angle is: "<<verticalAngle<<"; The horizontal angle is: "<<horizontalAngle<<std::endl;
    
    float FoV = initialFoV ;//- 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
    modelMatrix=glm::mat4(1.0f);
    projectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
    viewMatrix       = glm::lookAt(
                                   position,           // Camera is here
                                   position+direction, // and looks here : at the same position, plus "direction"
                                   up                  // Head is up (set to 0,-1,0 to look upside-down)
                                   );
    
}