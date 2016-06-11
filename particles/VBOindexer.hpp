#ifndef _VBOINDEXER_H_
#define _VBOINDEXER_H_


void VBO_indexer(vector<glm::vec3> & in_vertices,
                 vector<glm::vec2> & in_uvs,
                 vector<glm::vec3> & in_normals,
                 vector<glm::vec3> & out_vertices,
                 vector<glm::vec2> & out_uvs,
                 vector<glm::vec3> & out_normals,
                 
                 vector<unsigned short> & indices);


#endif