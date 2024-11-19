#ifndef UI_H
#define UI_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

<uiObject> hotbar = ui.createRect({size}, texture="red")
ui.draw()


class ui
{
public:

    const float[] square = {
       // Positions          // Texture Coords
      -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,   // Top-left
      -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // Bottom-left
       0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // Bottom-right
       0.5f,  0.5f, 0.0f,   1.0f, 1.0f    // Top-right
    }

    unsigned int ID;
    
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    ui(const char* shape, const std::array<float, 4> coords, const char* texture) {
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void rect(std::array<float, 4> coords = {0, 0, 100, 100}, char* texture = "") {
        glm::mat4 model = glm::mat4(1.0f);

        glm::vec3 translation(location[0], location[1], location[2]);

        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor, scaleFactor, 1.0f));

        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 1.0f));

        model = glm::translate(model, translation);

        ourShader->setMat4("model", model);
    }
    void draw() 
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
