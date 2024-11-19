#ifndef SHADER_H
#define SHADER_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class player
{
public:
    unsigned int ID;
    string username;
    float health;
    
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    player(const char* vertexPath, const char* fragmentPath) {
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void newPlayer(string username)
    void use() 
    { 
        glUseProgram(ID); 
    
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const {         
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const { 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const { 
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &value) {
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
     
    }
};
#endif
