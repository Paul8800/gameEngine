#include <GLFW/glfw3.h>
#include <GLFW/glfw3.h>

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTENT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTENT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow * window = glfwCreateWindow(800, 600, "Title", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std:endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!glfwLoadGLLoader)((GLADloadproc)glfwProcAddress)) {
    std::count << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glViewport(0, 0, 800, 600);


  void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewPort(0, 0, width height);

  }

  glSetFramebufferSizeCallback(window, framebuffer_size_callback);


  while (!glfwWindowShouldClose(window)){
    glfwSwapBuffers(window);
    glfwPollEvents();
    glfwTerminate();
    return 0;
  }

  return 0;
}
