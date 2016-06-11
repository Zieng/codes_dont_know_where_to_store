#include <iostream>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>


#include "load_funcs.hpp"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII


GLuint load_shaders(const char * vertex_file_path, const char * fragment_file_path)
{
    //A shader object is used to maintain the source code strings that define a shader.
    GLuint VertexShaderID=glCreateShader(GL_VERTEX_SHADER);  //creates an empty shader object and returns a non-zero value by which it can be referenced
    GLuint FragmentShaderID=glCreateShader(GL_FRAGMENT_SHADER);
    std::cout<<"The vertex shader ID= "<<VertexShaderID<<std::endl;
    std::cout<<"The fragment shader ID= "<<FragmentShaderID<<std::endl;
    
    // open shader file to get its source code
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path,std::ios::in);
    if(VertexShaderStream.is_open()){
        std::string line="";
        while(getline(VertexShaderStream,line)){
            VertexShaderCode+=line+"\n";
        }
        VertexShaderStream.close();
    }
    else{
        std::cout<<"Fatal Error! can't open "<<vertex_file_path<<std::endl;
        return -1;
    }
    
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path,std::ios::in);
    if(FragmentShaderStream.is_open()){
        std::string line="";
        while(getline(FragmentShaderStream,line)){
            FragmentShaderCode+=line+"\n";
        }
        FragmentShaderStream.close();
    }
    else{
        std::cout<<"Fatal Error! can't open "<<fragment_file_path<<std::endl;
        return -1;
    }
    
    GLint Result=GL_FALSE;
    int infoLogLength;
    
    //    std::cout<<"Compiling shader "<<vertex_file_path<<std::endl;
    char const * VertexSourcePointer=VertexShaderCode.c_str();
    glShaderSource(VertexShaderID,1,&VertexSourcePointer,NULL); //sets the source code in shader to the source code in the array of strings specified by string
    glCompileShader(VertexShaderID); //compiles the source code strings that have been stored in the shader object specified by shader.
    
    glGetShaderiv(VertexShaderID,GL_COMPILE_STATUS,&Result);  //returns in Result the value of a parameter for a specific shader object.
    glGetShaderiv(VertexShaderID,GL_INFO_LOG_LENGTH,&infoLogLength);  // similiar, but returns  in infoLogLength.
    char VertexShaderErrorMessage[(const int )infoLogLength+1];
    glGetShaderInfoLog(VertexShaderID,infoLogLength,NULL,VertexShaderErrorMessage);    //returns the information log for the specified shader object.
    //    std::cout<<"The state of compiling vertex shader is: "<<VertexShaderErrorMessage<<std::endl;
    
    //    std::cout<<"Compiling shader "<<fragment_file_path<<std::endl;
    char const * FragmentSourcePointer=FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID,1,&FragmentSourcePointer,NULL);
    glCompileShader(FragmentShaderID);
    
    glGetShaderiv(FragmentShaderID,GL_COMPILE_STATUS,&Result);
    glGetShaderiv(FragmentShaderID,GL_INFO_LOG_LENGTH,&infoLogLength);
    char FragmentShaderErrorMessage[(const int )infoLogLength+1];
    glGetShaderInfoLog(FragmentShaderID,infoLogLength,NULL,FragmentShaderErrorMessage);
    //    std::cout<<"The state of compiling fragment shader is: "<<FragmentShaderErrorMessage<<std::endl;
    
    std::cout<<"Linking program..."<<std::endl;
    GLuint ProgramID=glCreateProgram();   //creates an empty program object and returns a non-zero value by which it can be referenced.
    glAttachShader(ProgramID,VertexShaderID);  //  attach order doesn't matter
    glAttachShader(ProgramID,FragmentShaderID);  //Multiple shader objects of the same type may not be attached to a single program object.
    glLinkProgram(ProgramID);   // once the link is successful, then each shader can do its work
    
    glGetProgramiv(ProgramID,GL_LINK_STATUS,&Result);     // similiar to glGetShaderiv()
    if(!Result){
        glGetProgramiv(ProgramID,GL_INFO_LOG_LENGTH,&infoLogLength);
        char * LinkErrorMessage = new char[(const int )infoLogLength+1];
        glGetProgramInfoLog(ProgramID,infoLogLength,NULL,LinkErrorMessage);
        std::cerr<<LinkErrorMessage<<std::endl;
        delete [] LinkErrorMessage;
    }
    
    //    std::cout<<"The state of linking program is: "<<LinkErrorMessage<<std::endl;
    
    glDeleteShader(VertexShaderID);  //frees the memory and invalidates the name associated with the shader object specified by shader
    glDeleteShader(FragmentShaderID);  // undoes all effect by calling glCreateShader()
    
    return ProgramID;
}

GLuint load_DDS(const char * imagepath)
{
    
    unsigned char header[124];
    
    FILE *fp;
    
    /* try to open the file */
    fp = fopen(imagepath, "rb");
    if (fp == NULL){
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
        return 0;
    }
    
    /* verify the type of file */
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        fclose(fp);
        return 0;
    }
    
    /* get the surface desc */
    fread(&header, 124, 1, fp);
    
    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width	     = *(unsigned int*)&(header[12]);
    unsigned int linearSize	 = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);
    
    
    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    /* close the file pointer */
    fclose(fp);
    
    //    unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch(fourCC)
    {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            free(buffer);
            return 0;
    }
    
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;
    
    /* load the mipmaps */
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                               0, size, buffer + offset);
        
        offset += size;
        width  /= 2;
        height /= 2;
        
        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if(width < 1) width = 1;
        if(height < 1) height = 1;
        
    }
    
    free(buffer);
    
    return textureID;
}

bool load_OBJ(const char * objPath,std::vector<glm::vec3> & out_vertex,std::vector<glm::vec2> & out_uv, std::vector<glm::vec3> & out_normal)
{
    std::vector<unsigned int > vertexIndices,uvIndices,normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    
    FILE * file=fopen(objPath, "rb");
    if (file==NULL) {
        std::cout<<"open obj file failed!"<<std::endl;
        return false;
    }
    while (1) {
        char lineHeader[128];
        if (fscanf(file, "%s",lineHeader)==EOF) {
            break;
        }
        
        if (strcmp(lineHeader, "v")==0) {
            glm::vec3 newVertex;
            fscanf(file, "%f %f %f\n",&newVertex.x,&newVertex.y,&newVertex.z);
            temp_vertices.push_back(newVertex);
        }
        else if (strcmp(lineHeader, "vt")==0){
            glm::vec2 newUV;
            fscanf(file, "%f %f\n",&newUV.x,&newUV.y);
            newUV.y=-newUV.y;
            temp_uvs.push_back(newUV);
        }
        else if (strcmp(lineHeader, "vn")==0){
            glm::vec3 newNormal;
            fscanf(file, "%f %f %f\n",&newNormal.x,&newNormal.y,&newNormal.z);
            temp_normals.push_back(newNormal);
        }
        else if (strcmp(lineHeader, "f")==0){
            std::string vertex1,vertex2,vertex3;
            unsigned int vertexIndex[3],uvIndex[3],normalIndex[3];
            int matches=fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d",&vertexIndex[0],&uvIndex[0],&normalIndex[0],
                               &vertexIndex[1],&uvIndex[1],&normalIndex[1],
                               &vertexIndex[2],&uvIndex[2],&normalIndex[2]);
            if (matches!=9) {
                std::cout<<"format mis-match"<<std::endl;
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices    .push_back(uvIndex[0]);
            uvIndices    .push_back(uvIndex[1]);
            uvIndices    .push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else{
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
        
    }
    for( unsigned int i=0; i<vertexIndices.size(); i++ ){
        
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int uvIndex = uvIndices[i];
        unsigned int normalIndex = normalIndices[i];
        
        // Get the attributes thanks to the index
        glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
        glm::vec2 uv = temp_uvs[ uvIndex-1 ];
        glm::vec3 normal = temp_normals[ normalIndex-1 ];
        
        // Put the attributes in buffers
        out_vertex.push_back(vertex);
        out_uv     .push_back(uv);
        out_normal .push_back(normal);
    }
    
    return true;
}