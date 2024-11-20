//RC: g++ -o /home/paul/NeoVimProjects/temp/temp app.cpp glad/glad.c -I./glad -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "include/glm/glm.hpp"
#include "include/glm/gtc/matrix_transform.hpp"
#include "include/glm/gtc/type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#include "Shader.h"

#include <iostream>
#include <unordered_map>
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>

    unsigned int trunk[6];
    unsigned int leaves[6];

std::array<float, 120> makeRectangleEBO(const std::array<float, 3> location, const std::array<float, 3> dimensions, float textureSize);
void genRectangleEBO(const std::array<float, 120> vertices, const std::array<int, 3> location, unsigned int textures[6]);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void init();
int loadTexture(std::string name, std::string pathToImg);
Shader* ourShader = nullptr;
unsigned int textures[6];

void stepPlayerPhysics(float step, float momentum, std::vector<std::vector<std::vector<std::vector<float>>>>& world);

float generateHeight(int x, int z, int seed);
std::vector<std::vector<std::vector<std::vector<float>>>> createWorld(
    std::vector<std::vector<std::vector<std::vector<float>>>>& world,
    const std::array<float, 2> genTL, const std::array<float, 2> genBR);  //world[+/-][x][+/-][z][y] 

 float noise(float x, float z, int seed);
std::mt19937& getRNG(int seed);
void loadWorld(std::vector<std::vector<std::vector<std::vector<float>>>>& world, std::array<float, 120> block);
void drawTree(std::array<float, 3> location, unsigned int trunk[], unsigned int leaves[], std::array<float, 120> block);

std::unordered_map<std::string, int> keySwitches;
std::unordered_map<std::string, bool> keyHeld;

std::atomic<bool> is_thread_running(false); // Atomic flag to track if a thread is running

const unsigned int SCR_WIDTH = 845;
const unsigned int SCR_HEIGHT = 480;

GLFWmonitor* monitor;
const GLFWvidmode* mode;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
                        //
bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float roll =     0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  90.0f;

int main()
{
    init();
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Window Title", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------
    //Shader ourShader("shaders/shader.vs", "shaders/shader.fs");
    ourShader = new Shader("shaders/shader.vs", "shaders/shader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    unsigned int VAO, VBO, EBO; //Vertex buffer object; Vertex array object, elemnt buffer object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    std::vector<std::vector<std::vector<std::vector<float>>>> world;
    createWorld(world, {100, 100}, {-100, -100});
    std::array<float, 120> block = makeRectangleEBO({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);
    glBufferData(GL_ARRAY_BUFFER, block.size() * sizeof(float), block.data(), GL_STATIC_DRAW);

    unsigned int indices[] = {
      // Face 1
      0, 1, 2,  0, 2, 3,
      // Face 2
      4, 5, 6,  4, 6, 7,
      // Face 3
      8, 9, 10, 8, 10, 11,
      // Face 4
      12, 13, 14, 12, 14, 15,
      // Face 5
      16, 17, 18, 16, 18, 19,
      // Face 6
      20, 21, 22, 20, 22, 23
    };

//unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO); 

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, block.size() * sizeof(float), block.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  //texture attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

    const char* pathToImgs[] {
      "img/grass.png",
      "img/grassSide.png",
      "img/dirt.png",
      "img/grassSide.png",
      "img/grassSide.png",
      "img/grassSide.png"
    };
    for (int i=0; i<6; i++) textures[i] = loadTexture("grassBlock", pathToImgs[i]);


    const char* pathToLogImgs[] {
      "img/oak_log_top.png",
      "img/oak_log.png",
      "img/oak_log_top.png",
      "img/oak_log.png",
      "img/oak_log.png",
      "img/oak_log.png",
    };
    for (int i=0; i<6; i++) trunk[i] = loadTexture("log", pathToLogImgs[i]);




    // render loop
    // ----------------------------------------------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        // render
        // ------
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClearColor(0.529f, 0.808f, 0.9254f, 1.0f); //rgba(122,168,254,255)
        glClearColor(122.0f / 255.0f, 168.0f / 255.0f, 254.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);


        //Camera
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw) * cos(glm::radians(pitch))); // convert to radians first
        direction.y = sin(glm::radians(pitch)) * cos(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw));

        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader->setMat4("view", view);

        //transformations
        unsigned int transformLoc = glGetUniformLocation(ourShader->ID,"transform");
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3(1.0f, 0.0f, 0.0f);

        //glm::mat4 view = glm::mat4(1.0f);
        // note that we’re translating the scene in the reverse direction
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        ourShader->setMat4("projection", projection);

        int modelLoc = glGetUniformLocation(ourShader->ID, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        int viewLoc = glGetUniformLocation(ourShader->ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projectionLoc = glGetUniformLocation(ourShader->ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));


        // render the meshes
        ourShader->use();
        glBindVertexArray(VAO);
        
        

        
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, grassTexture);
        //ourShader->setInt("texture1", 0);
        
        //if (cameraPos[0]+100
          //createWorld(world, {100, 100}, {-100, -100});

        loadWorld(world, block);
        float momentum = 1;
        if (keySwitches["M"] == 1) stepPlayerPhysics(deltaTime, momentum, world);
        drawTree({0, 10, 0}, trunk, leaves, block);
        
        /*
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, crateTexture);
        ourShader.setInt("texture1", 0);
        makeRectangle({-0.5f, -0.5f, -0.5f}, {1.0f, -1.0f, -1.0f}, 1.0f);
        for(unsigned int i = 0; i < 10; i++) {
          glm::mat4 model = glm::mat4(1.0f);
          model = glm::translate(model, cubePositions[i]);
          float angle = 20.0f * i;
          //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
          model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), cubePositions[i]);//glm::vec3(0.5f, 1.0f, 0.0f));
          //model = glm::scale(model, glm::vec3(1*i, 1*i, 1*i));
          ourShader.setMat4("model", model);
          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        */


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawTree(std::array<float, 3> location, unsigned int trunk[], unsigned int leaves[], std::array<float, 120> block) {
  float x = location[0];
  float y = location[1];
  float z = location[2];
  for (int i = 0; i<5; i++) { //Drawing Trunk
   genRectangleEBO(block, {x, y+i, z}, trunk); 
  }
}

void stepPlayerPhysics(float step, float momentum, std::vector<std::vector<std::vector<std::vector<float>>>>& world) {
  int x = std::round(cameraPos[0]);
  int z = std::round(cameraPos[2]+3);
  if (cameraPos[1] > std::ceil(world[x < 0 ? 0: 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)])+2){

    std::cout << world[x < 0 ? 0: 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)]+2 << std::endl;
    cameraPos[1] += -100*step * momentum;
  }
  if (cameraPos[1] < std::ceil(world[x < 0 ? 0: 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)])+2){ 
    std::cout << "WORLD:  " << std::round(world[x < 0 ? 0: 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)]) << std::endl;
    std::cout << "PLAYER:  " << cameraPos[1] << std::endl;
    cameraPos[1] = std::ceil(world[x < 0 ? 0: 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)])+2;
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

std::vector<std::vector<std::vector<std::vector<float>>>> createWorld(
    std::vector<std::vector<std::vector<std::vector<float>>>>& world,
    const std::array<float, 2> genTL, const std::array<float, 2> genBR) { //world[+/-][x][+/-][z][y] 
  
  const float WORLD_HEIGHT = 128;

    int i = 0;
    if (world.size() < 2) {
      world.push_back({});//Negative x coordinates
      world.push_back({});//Positive x coordinates
    }
    for (int x = genBR[0]; x <= genTL[0]; ++x) {
      if (world[x < 0 ? 0 : 1].size() < std::abs(x)+1) world[x < 0 ? 0 : 1].resize(std::abs(x)+1);
      //if (x < 0)  world[0][std::abs(x)] = std::vector<std::vector<float>>();//.push_back({});//-x coords
      //if (x >= 0) world[1][std::abs(x)] = std::vector<std::vector<float>>();//.push_back({});//+x coords
      if (world[x < 0 ? 0 : 1][std::abs(x)].empty()) { world[x < 0 ? 0 : 1][std::abs(x)].resize(2);}
      //world[x < 0 ? 0 : 1][std::abs(x)].resize(2);
      //if (world[x < 0 ? 0 : 1][std::abs(x)][0].empty()) world[x < 0 ? 0 : 1][std::abs(x)][0] = std::vector<float>();//Positive x coordinates
      //if (world[x < 0 ? 0 : 1][std::abs(x)][1].empty()) world[x < 0 ? 0 : 1][std::abs(x)][1] = std::vector<float>();//Positive x coordinates
      //if (world[x < 0 ? 0 : 1][std::abs(x)][z < 0 ? 0 : 1].empty()) { world[x < 0 ? 0 : 1][std::abs(x)][z < 0 ? 0 : 1].resize(1);}
                                             
      for (int z = genBR[1]; z <= genTL[1]; ++z) {
        // Get the height based on the noise function
        float height = std::round(static_cast<float>(generateHeight(x, z, 42)));
        // Clamp the height to the maximum WORLD_HEIGHT
        //height = std::min(height, WORLD_HEIGHT - 1);
        //Add each height for  each x,z coords
        int xType = x < 0 ? 0 : 1;  int zType = z < 0 ? 0 : 1; 

        if (world[xType][std::abs(x)][zType].size() < std::abs(z)+1) world[xType][std::abs(x)][zType].resize(std::abs(z)+1);
        if (!world[xType][std::abs(x)][zType][std::abs(z)]) world[xType][std::abs(x)][zType][std::abs(z)] = height;//-/+z coords
        }
        i++;
        std::cout << i << std::endl;
    }
    return world;
}

void loadWorld(std::vector<std::vector<std::vector<std::vector<float>>>>& world, std::array<float, 120> block) {
  const int WORLD_WIDTH = 50; //render distances
  const int WORLD_HEIGHT = 128;
  const int WORLD_DEPTH = 50;

    std::mt19937 gen(42); 
    std::uniform_int_distribution<> dist(1, 1000); // Uniform distribution between 1 and 1000
  //createWorld(world, {50, -90}, {-50, -150});

  auto genChunkThread = [&](float chunkx, float chunkz) {
    if (is_thread_running.load()) {
        return; // Return early if a thread is already running
    }
    is_thread_running.store(true);
    
    //std::cout << "STARTING" << std::endl;
    createWorld(world, {100*chunkx, 100*chunkz}, {100*chunkx-100, 100*chunkz-100}); //first {100, 0} {-100, -200}    static{100, 100}, {-100, -100}   should{-100, 100}, {-200, -100}
    //createWorld(world, {50, -50}, {-50, -150}); //first {100, 0} {-100, -200}    static{100, 100}, {-100, -100}   should{-100, 100}, {-200, -100}
    //std::cout << "ENDING" << std::endl;

    is_thread_running.store(false);
    };
  
  for (int x = cameraPos[0]-WORLD_WIDTH; x < cameraPos[0]+WORLD_WIDTH; ++x) {
      for (int z = cameraPos[2]-WORLD_DEPTH; z < cameraPos[2]+WORLD_DEPTH; ++z) {
          // Place cubes from the ground up to the height
          if ((x < 0 ? 0 : 1) < world.size() &&
              (std::abs(x))   < world[x < 0 ? 0 : 1].size() &&
              (z < 0 ? 0 : 1) < world[x < 0 ? 0 : 1][std::abs(x)].size() &&
              (std::abs(z))   < world[x < 0 ? 0 : 1][std::abs(x)][z < 0 ? 0 : 1].size()
              ){

                if (dist(gen) == 1000) drawTree({x, world[x < 0 ? 0 : 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)]+1, z}, trunk, leaves, block);
                for (int y = world[x < 0 ? 0 : 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)]; y < world[x < 0 ? 0 : 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)]+1; ++y) {
                  if (world[x < 0 ? 0 : 1][std::abs(x)][z < 0 ? 0 : 1][std::abs(z)]) genRectangleEBO(block, {x, y, z}, textures); // Your function to create a cube at (x, y, z)
          }} else if (!is_thread_running.load()){
            float chunkx = std::ceil(x/100.0f);
            float chunkz = std::ceil(z/100.0f);
            /*
            std::cout << chunkx << std::endl;
            std::cout << z << std::endl;
            std::cout << chunkz << std::endl;
            std::cout << std::round(cameraPos[0]) << ", " << std::round(cameraPos[1]) << ", " << std::round(cameraPos[2]) << std::endl;
            std::cout << std::round(x) << ", " << std::round(z) << std::endl;
            std::cout << 50*chunkx+50 << ", " << 50*chunkz+50 << " || " << 50*chunkx-50 << ", " << 50*chunkz-50 << std::endl;
            std::cout << "WORLD" << std::endl;*/

            //createWorld(world, {50*chunkx+50, 50*chunkz+50}, {50*chunkx-50, 50*chunkz-50}); //first {100, 0} {-100, -200}    static{50, 50}, {-50, -50}   should{50, 0}, {-50, -100}
            std::thread(genChunkThread, chunkx, chunkz).detach();

            
          }
      }
  }
}

// Function to create the world
void init(){
    keySwitches["F11"] = 0;keySwitches["ESCAPE"] = 0;keySwitches["M"] = 1;
    keyHeld["F11"] = false;keyHeld["ESCAPE"] = false;keyHeld["M"] = false;
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, false);

    float globalSpeedMultiplier = 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
      globalSpeedMultiplier = 25;

    float cameraSpeed = 12.5f * deltaTime * globalSpeedMultiplier;
    float verticalSpeed = 15.0f * deltaTime * globalSpeedMultiplier;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cameraPos += glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraPos -= glm::normalize(glm::vec3(cameraFront.x, 0.0f,cameraFront.z)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
      cameraPos -= glm::vec3(0.0f, verticalSpeed, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
      cameraPos += glm::vec3(0.0f, verticalSpeed, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
      std::cout << std::round(cameraPos[0]) << ", " << std::round(cameraPos[1]) << ", " << std::round(cameraPos[2]) << std::endl;



    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keyHeld["M"]) {
      keyHeld["M"] = true;
      keySwitches["M"] += 1;
      if (keySwitches["M"] > 2) keySwitches["M"] = 0;
    } else if (glfwGetKey(window, GLFW_KEY_M) != GLFW_PRESS) {
      keyHeld["M"] = false;
    }


    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && !keyHeld["F11"]) {
      if (keySwitches["F11"] != 1) {
      glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
      keySwitches["F11"]= 1;
      keyHeld["F11"] = true;
    } else {
      glfwSetWindowMonitor(window, NULL, 400, 300, 800, 600, 0);
      keySwitches["F11"] = 0;
    }keyHeld["F11"] = true;
    }else if(glfwGetKey(window, GLFW_KEY_F11) != GLFW_PRESS){keyHeld["F11"] = false;}

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !keyHeld["ESCAPE"]) {
      if (!keySwitches["ESCAPE"]) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        keySwitches["ESCAPE"] = 1;
      } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        keySwitches["ESCAPE"] = 0;
      }
      keyHeld["ESCAPE"] = true;
    } else if (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {keyHeld["ESCAPE"] = false;}


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  if (keySwitches["ESCAPE"] == 1) {firstMouse=true; return;}
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;
  float sensitivity = 0.05f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;
  yaw += xoffset;
  pitch += yoffset;
  if(pitch > 89.0f)
    pitch = 89.0f;
  if(pitch < -89.0f)
    pitch = -89.0f;
  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
fov -= (float)yoffset*5;
if (fov < 1.0f)
  fov = 1.0f;
if (fov > 100.0f)
  fov = 100.0f;
}

int loadTexture(std::string name, std::string pathToImg) {
    unsigned int texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    //Repeats texture on both axis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //texture filtering, how magnifying and minifying affects the img
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //NEAREST / LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // set filtering method, mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //load imgs
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(pathToImg.c_str(), &width, &height, &nrChannels, 0);

    if (data) {
      if (nrChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
      } else if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
      } else {std::cout << "Failed to load texture" << std::endl; }
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}


void genRectangleEBO(const std::array<float, 120> vertices, const std::array<int, 3> location, unsigned int textures[6]) {
  /*
  unsigned int indices[] = {
    // Face 1
    0, 1, 2,  0, 2, 3,
    // Face 2
    4, 5, 6,  4, 6, 7,
    // Face 3
    8, 9, 10, 8, 10, 11,
    // Face 4
    12, 13, 14, 12, 14, 15,
    // Face 5
    16, 17, 18, 16, 18, 19,
    // Face 6
    20, 21, 22, 20, 22, 23
  };*/

  glm::mat4 model = glm::mat4(1.0f);
  glm::vec3 translation(location[0], location[1], location[2]);
  model = glm::translate(model, translation);

  ourShader->setMat4("model", model);
  //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  
    for (int i = 0; i < 6; ++i) {
      glBindTexture(GL_TEXTURE_2D, textures[i]);
      ourShader->setInt("texture1", 0);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int))); // Draw the face
    }
    //glBindVertexArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //glDeleteBuffers(1, &EBO);
}

std::array<float, 120> makeRectangleEBO(const std::array<float, 3> location, const std::array<float, 3> dimensions, float textureSize) {
    float corners[] = { 
        location[0], location[1], location[2],                                                 // top-left front
        location[0], location[1], location[2] - dimensions[2],                                 // top-left back
        location[0] + dimensions[0], location[1], location[2] - dimensions[2],                 // top-right back
        location[0] + dimensions[0], location[1], location[2],                                 // top-right front
        location[0], location[1] - dimensions[1], location[2],                                 // bottom-left front
        location[0], location[1] - dimensions[1], location[2] - dimensions[2],                 // bottom-left back
        location[0] + dimensions[0], location[1] - dimensions[1], location[2] - dimensions[2], // bottom-right back
        location[0] + dimensions[0], location[1] - dimensions[1], location[2]                  // bottom-right front
    };

    float textureCorners[] = {
      // Top Face
      0.0f, 0.0f,  // Bottom left
      1.0f, 0.0f,  // Bottom right
      1.0f, 1.0f,  // Top right
      0.0f, 1.0f,  // Top left

      // Back Face (correct orientation)
      1.0f, 1.0f,  // Bottom left (top of texture)
      1.0f, 0.0f,  // Bottom right (top of texture)
      0.0f, 0.0f,  // Top right (bottom of texture)
      0.0f, 1.0f,  // Top left (bottom of texture)

      // Bottom Face
      0.0f, 1.0f,  // Bottom left
      1.0f, 1.0f,  // Bottom right
      1.0f, 0.0f,  // Top right
      0.0f, 0.0f,  // Top left

      // Front Face (90 degrees clockwise)
      0.0f, 0.0f,  // Bottom left
      0.0f, 1.0f,  // Bottom right
      1.0f, 1.0f,  // Top right
      1.0f, 0.0f,  // Top left

      // Left Face
      0.0f, 0.0f,  // Bottom left
      1.0f, 0.0f,  // Bottom right
      1.0f, 1.0f,  // Top right
      0.0f, 1.0f,  // Top left

      // Right Face (correct orientation)
      0.0f, 1.0f,  // Bottom left (top of texture)
      1.0f, 1.0f,  // Bottom right (top of texture)
      1.0f, 0.0f,  // Top right (bottom of texture)
      0.0f, 0.0f   // Top left (bottom of texture)
      };


    std::array<float, 120> vertices = {};
    int face_indices[] = {
        0, 1, 2, 3, //top
        1, 5, 6, 2, //back
        7, 6, 5, 4, //bottom
        4, 0, 3, 7, //front
        4, 5, 1, 0, //left
        3, 2, 6, 7  //right
    };

    int j = 0;
    int ti = 0;
    for (int i = 0; i < 24; i++) {
        int index = face_indices[i];
        vertices[j] = corners[index * 3];
        vertices[j + 1] = corners[index * 3 + 1];
        vertices[j + 2] = corners[index * 3 + 2];

          vertices[j + 3] = textureCorners[ti];//[(ti % 4) * 2]; // % 4 for texture coordinates
          vertices[j + 4] = textureCorners[ti+1];//[(ti % 4) * 2 + 1]; // % 4 for texture coordinates

        j += 5;
        ti += 2;
        
    }
    return vertices;
}
