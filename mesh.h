#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

//Default values
const string shape         = "square";

class Mesh {
  public:
    //vars
    

    mesh() {
      //vars
    }
    float vertices[] = {
    // Vertex coordinates (x, y, z)      // Texture coordinates (u, v)
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left back
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // Bottom-right back
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right back
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right back (duplicate)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left back
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left back (duplicate)

    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left front
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right front
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right front
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // Top-right front (duplicate)
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // Top-left front
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left front (duplicate)

    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Top-left left
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-left back
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Bottom-left back
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Bottom-left back (duplicate)
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left front
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Top-left front

    // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Top-right front
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right back
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Bottom-right back
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Bottom-right back (duplicate)
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-right front
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Top-right front

    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Bottom-left back
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // Bottom-right back
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right front
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // Bottom-right front (duplicate)
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // Bottom-left front
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // Bottom-left back (duplicate)

    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // Top-left back
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // Top-right back
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Top-right front
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // Top-right front (duplicate)
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, // Top-left front
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f  // Top-left back (duplicate)
    };    

    std::array<float, 180> makeRectangle(const std::array<float, 3> location, const std::array<float, 3> dimensions, float textureSize) {
    float corners[] = { 
        location[0], location[1], location[2],                                             // top-left front
        location[0], location[1], location[2] - dimensions[2],                             // top-left back
        location[0] + dimensions[0], location[1], location[2] - dimensions[2],             // top-right back
        location[0] + dimensions[0], location[1], location[2],                             // top-right front
        location[0], location[1] - dimensions[1], location[2],                             // bottom-left front
        location[0], location[1] - dimensions[1], location[2] - dimensions[2],             // bottom-left back
        location[0] + dimensions[0], location[1] - dimensions[1], location[2] - dimensions[2], // bottom-right back
        location[0] + dimensions[0], location[1] - dimensions[1], location[2]              // bottom-right front
    };

    float textureCorners[] = { 
        0.0f, 0.0f,
        0.0f, textureSize,  
        textureSize, textureSize,
        textureSize, 0.0f,
    };

    std::array<float, 180> vertices = {};
    int face_indices[] = {
        0, 1, 2, 2, 3, 0,
        1, 5, 6, 6, 2, 1,
        7, 6, 5, 5, 4, 7,
        4, 0, 3, 3, 7, 4,
        4, 5, 1, 1, 0, 4,
        3, 2, 6, 6, 7, 3
    };

    int j = 0;
    int ti = 0;
    for (int i = 0; i < 36; i++) {
        int index = face_indices[i];
        vertices[j] = corners[index * 3];
        vertices[j + 1] = corners[index * 3 + 1];
        vertices[j + 2] = corners[index * 3 + 2];
        if((i) % 3 != 0 || i-1 == 0) {
          vertices[j + 3] = textureCorners[(ti % 4) * 2]; // % 4 for texture coordinates
          vertices[j + 4] = textureCorners[(ti % 4) * 2 + 1]; // % 4 for texture coordinates
        }
               
        if((i+1) % 3 == 0 && i != 0 && j+5+4 <= 180){
          vertices[j + 5 + 3] = textureCorners[(ti % 4) * 2]; // % 4 for texture coordinates
          vertices[j + 5 + 4] = textureCorners[(ti % 4) * 2 + 1]; // % 4 for texture coordinates
          ti--;
        }

        j += 5;
        ti++;
        //if (i % 6 == 0) ti = 0;
    }

    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);   // for std::arrays
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    return vertices;
}
  void genRectangleEBO(const std::array<float, 120> vertices, const std::array<int, 3> location) {
    unsigned int indices[] = {
      // Back face
      0, 1, 2, // Triangle 1
      2, 3, 0, // Triangle 2

      // Front face
      4, 5, 6, // Triangle 1
      6, 7, 4, // Triangle 2

      // Left face
      8, 9, 10, // Triangle 1
      10, 11, 8, // Triangle 2

      // Right face
      12, 13, 14, // Triangle 1
      14, 15, 12, // Triangle 2

      // Bottom face
      16, 17, 18, // Triangle 1
      18, 19, 16, // Triangle 2

      // Top face
      20, 21, 22, // Triangle 1
      22, 23, 20  // Triangle 2
    };


    unsigned int EBO;
    glGenBuffers(1, &EBO); 

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 translation(location[0], location[1], location[2]);
    model = glm::translate(model, translation);

    ourShader->setMat4("model", model);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glDrawElemets(GL_TRIANGLES, 0, 36);
  }

    void texture(std::vector<string> sides) {
      //pass
    }
    

}

#endif
