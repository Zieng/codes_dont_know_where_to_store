#ifndef _LOAD_FUNCS_H
#define _LOAD_FUNCS_H

GLuint load_shaders(const char * vertex_file_path, const char * fragment_file_path);
GLuint load_DDS(const char * imagepath);
bool load_OBJ(const char * objPath, std::vector<glm::vec3> & out_vertex,std::vector<glm::vec2> & out_uv,std::vector<glm::vec3> & out_normal);

#endif
