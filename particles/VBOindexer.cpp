
#include "VBOindexer.hpp"

struct PackedVertex{
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator < (const PackedVertex that) const{
        return memcmp((void *)this, (void *) &that, sizeof(PackedVertex))>0;
    };
};

void VBO_indexer(vector<glm::vec3> & in_vertices,
                 vector<glm::vec2> & in_uvs,
                 vector<glm::vec3> & in_normals,
                 vector<glm::vec3> & out_vertices,
                 vector<glm::vec2> & out_uvs,
                 vector<glm::vec3> & out_normals,
                 
                 vector<unsigned short> & indices)
{
    map<PackedVertex,unsigned short> indexedVertices;
    
    for (int i=0; i<in_vertices.size(); i++) {
        PackedVertex p={in_vertices[i],in_uvs[i],in_normals[i]};
        bool Indexed=false;
        unsigned short index;
        
        map<PackedVertex, unsigned short>::iterator it=indexedVertices.find(p);
        if (it!=indexedVertices.end()) {
            Indexed=true;
            index=it->second;
        }
        
        if (Indexed) {
            indices.push_back(index);
        }
        else{
            out_vertices.push_back(in_vertices[i]);
            out_uvs.push_back(in_uvs[i]);
            out_normals.push_back(in_normals[i]);
            unsigned short newIndex=(unsigned short)out_vertices.size()-1;
            indices.push_back(newIndex);
            indexedVertices[p]=newIndex;
        }
    }
}
