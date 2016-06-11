#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void computeMatricesFromInputs(glm::mat4 & modelMatrix, glm::mat4 & viewMatrix, glm::mat4 & projectionMatrix);

#endif
