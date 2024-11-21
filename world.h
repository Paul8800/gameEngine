#ifndef WORLD_H
#define WORLD_H

#include "include/glad/glad.h"
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <array>
#include <random>


class world
{
public:

    unsigned int ID;

    std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<float>>>>>> coordinates; //6D array, 3 for (x,y,z) + 3 for (neg/pos) versions
    
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    world(const std::array<float, 2> genTL, const std::array<float, 2> genBR) {
      genWorld(genTL, genBR);
    }

    

    float getBlock(int x, int  y, int z) {
      //resizeWorld(x, y, z);
      //checksize(x, y, z);
      if ((x < 0 ? 0 : 1) < coordinates.size() &&
       (std::abs(x))   < coordinates[x < 0 ? 0 : 1].size() &&
       (y < 0 ? 0 : 1) < coordinates[x < 0 ? 0 : 1][std::abs(x)].size() &&
       (std::abs(y))   < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1].size() &&
       (z < 0 ? 0 : 1) < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)].size() &&
       (std::abs(z))   < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)][z < 0 ? 0 : 1].size()
       ){
      return coordinates[x<0 ? 0:1][std::abs(x)][y<0 ? 0:1][std::abs(y)][z<0 ? 0:1][std::abs(z)];
      }
      return -1.0f;
    }

    void setBlock(int x, int  y, int z, unsigned int block) {
      resizeWorld(x, y, z);
       if ((x < 0 ? 0 : 1) < coordinates.size() &&
         (std::abs(x))   < coordinates[x < 0 ? 0 : 1].size() &&
         (y < 0 ? 0 : 1) < coordinates[x < 0 ? 0 : 1][std::abs(x)].size() &&
         (std::abs(y))   < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1].size() &&
         (z < 0 ? 0 : 1) < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)].size() &&
         (std::abs(z))   < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)][z < 0 ? 0 : 1].size()
         ){
      coordinates[x<0 ? 0:1][std::abs(x)][y<0 ? 0:1][std::abs(y)][z<0 ? 0:1][std::abs(z)] = block;}                //Runtime Error here
    }

    void resizeWorld(int& x, int&  y, int& z) {
      /*if ((x < 0 ? 0 : 1) < coordinates.size() &&
         (std::abs(x))   < coordinates[x < 0 ? 0 : 1].size() &&
         (y < 0 ? 0 : 1) < coordinates[x < 0 ? 0 : 1][std::abs(x)].size() &&
         (std::abs(y))   < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1].size() &&
         (z < 0 ? 0 : 1) < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)].size() &&
         (std::abs(z))   < coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)][z < 0 ? 0 : 1].size()
         ){
      return;
      }*/
      if ((x < 0 ? 0 : 1)   >= coordinates.size()) {
        coordinates.resize(2);
      } if ((std::abs(x))   >= coordinates[x < 0 ? 0 : 1].size()) {
        coordinates[x < 0 ? 0 : 1].resize(std::abs(x)+1);
      } if ((y < 0 ? 0 : 1) >= coordinates[x < 0 ? 0 : 1][std::abs(x)].size()) {
        coordinates[x < 0 ? 0 : 1][std::abs(x)].resize(2);
      } if ((std::abs(y))   >= coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1].size()) {
        coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1].resize(std::abs(y)+1);
      } if ((z < 0 ? 0 : 1) >= coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)].size()) {
        coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)].resize(2);
      } if ((std::abs(z))   >= coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)][z < 0 ? 0 : 1].size()) {
        coordinates[x < 0 ? 0 : 1][std::abs(x)][y < 0 ? 0 : 1][std::abs(y)][z < 0 ? 0 : 1].resize(std::abs(z)+1);
      }

    }

    void setTree(int x, int  y, int z) {
      for (int i=0; i < 2; i++) {
        for (int j=0; j < 5; j++) {
          for (int k=0; k < 5; k++) {
            setBlock(x+j-2, y+i+3, z+k-2, 3);
          }
        }
      }
      for (int i=0; i < 2; i++) {
        for (int j=0; j < 3; j++) {
          for (int k=0; k < 3; k++) {
            setBlock(x+j-1, y+i+5, z+k-1, 3);
          }
        }
      }
      for (int i=0; i < 5; i++) setBlock(x, y+i, z, 2); // trunk
    }

    void genWorld(std::array<float, 2> genTL, std::array<float, 2> genBR){
      const float WORLD_HEIGHT = 1;

      std::mt19937 gen(42); 
      std::uniform_int_distribution<> dist(1, 1000); // Uniform distribution between 1 and 1000


      int i = 0;
      for (int x = genBR[0]; x <= genTL[0]; ++x) {
        for (int y = 0; y <= WORLD_HEIGHT; ++y) {
          for (int z = genBR[1]; z <= genTL[1]; ++z) {
            float height = std::round(static_cast<float>(generateHeight(x, z, 42)));
            int xType = x < 0 ? 0 : 1;  int zType = z < 0 ? 0 : 1; 

            setBlock(x, static_cast<int>(height), z, 1);// Set grass at height of terain

            if (dist(gen) == 1000) setTree(x, height+1, z);
            }
            i++;
            std::cout << i << std::endl;
        }
      }
    }
    
    std::mt19937& getRNG(int seed) {
      static std::mt19937 rng(seed);
      return rng;
    }

    float generateHeight(int x, int z, int seed) {
      float scale = 0.1f; // Adjust the scale to change the frequency of the noise
      float persistence = 0.5f; // Control the roughness of the terrain
      int octaves = 6; // Number of layers of noise
      
      float total = 0.0f;
      float frequency = 0.25f;
      float amplitude = 5.0f;
      float maxValue = 0.0f;  // Used for normalizing result to 0.0 - 1.0

      for (int i = 0; i < octaves; ++i) {
          total += noise(x * scale * frequency, z * scale * frequency, seed + i) * amplitude;
          
          maxValue += amplitude;
          
          amplitude *= persistence;
          frequency *= 2.0f;
      }

      float height = total / maxValue;
      float maxHeight = 64.0f; // Maximum height of the terrain
      return std::max(0.0f, height * maxHeight); // Ensure height is not below 0
    }

// Perlin noise function
  float noise(float x, float z, int seed) {
      std::mt19937 generator(seed); // Seed random generator
      std::uniform_real_distribution<float> distribution(0.0f, 0.25f);

      // Simple random noise
      return distribution(generator) * (0.5f * (std::sin(x) + std::cos(z)) + 0.5f);
  }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
     
    }
};
#endif
