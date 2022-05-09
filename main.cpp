#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>

#include "glad/glad.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.h"

using namespace glm;

#define GLSL(src) #src
#define MODE_CUBE
//#define MODE_QUAD

static void error_callback(int error, const char *description) {
  fputs(description, stderr);
}
 
void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

std::stringstream get_file(std::string path){
    std::stringstream result;
    std::ifstream stream(path);
    std::string line;
    while(getline(stream,line))
        result << line << "\n";
    return result;
}

GLuint createShader(std::string& shader,GLenum type){
    GLuint id = glCreateShader(type);
    const char* shader_c_string = shader.c_str();
    glShaderSource(id,1,&shader_c_string,nullptr);
    glCompileShader(id);

    GLint result;
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    if(result == GL_FALSE){
        GLint length;
        glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
        char *message = (char*)alloca(sizeof(char)*length);
        glGetShaderInfoLog(id,length,&length,message);
        std::cout << "Failed to compile "<<(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader"<<std::endl;
        std::cout << message << std::endl;
    }

    return id;
}

// camera
Camera camera(glm::vec3(0.0f, 0.0f, -30.0f));
float lastX = 2.0f;
float lastY = 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
int frame = 0;

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 600;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetWindowShouldClose(window, true);
    }
        
    float speed = 2.0;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.MovementSpeed = 10.0;;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime * speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime * speed);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int main(void) {
  GLFWwindow *window;



  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

  window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Voxel-Renderer", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable( GL_DEBUG_OUTPUT );
  glDebugMessageCallback( MessageCallback, 0 );

  glEnable(GL_DEPTH_TEST);

    const int width = 200;
    const int height = 50;
    const int depth = 200;

#ifdef MODE_QUAD
  glEnable(GL_PROGRAM_POINT_SIZE);
  
    // Create a vertex buffer for rendering point primitives
    const uint32_t no_of_vertices = width*height*depth;
    uint32_t *vertices = new uint32_t[no_of_vertices * 2];

    for (int i=0; i < no_of_vertices; i++) {

      uint32_t x = (i%width);
      uint32_t y = ((i/(width*depth))) << 11;
      uint32_t z = ((i/width)%depth) << 21;
      uint32_t pos = x|y|z;

      uint32_t r = uint32_t(float(std::rand()) / float(RAND_MAX) * 32.0); // 5 bits 0-31
      uint32_t g = uint32_t(float(std::rand()) / float(RAND_MAX) * 64.0) << 5; // 6 bits 0-63
      uint32_t b = uint32_t(float(std::rand()) / float(RAND_MAX) * 32.0) <<11; // 5 bits 0-31
      uint32_t albedo = r|g|b;

      vertices[i*2] = pos;
      vertices[i*2+1] = albedo;
    }


    GLuint vertex_array;
    GLuint vertex_buffer; //Use the vertex buffer for all drawing
    glCreateVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glCreateBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * 2 * no_of_vertices, vertices, GL_STATIC_DRAW);
    glVertexAttribIPointer(0, 2, GL_INT, 2 * sizeof(uint32_t), (void *)0);
    glEnableVertexAttribArray(0);
    //create and attach shaders
    std::string vs_source = get_file("./shaders/quad.vert").str(),
    fs_source = get_file("./shaders/quad.frag").str();
#endif

#ifdef MODE_CUBE
    const uint32_t no_of_instances = width*height*depth;

    std::cout<<"drawing " << no_of_instances << " cubes"<<std::endl;

    //Create a vertex array object that won't be used
    // OpenGL forces us to bind a vertex array object if we want to draw anything
    float vertices[3] = {0.0, 0.0, 0.0};
    GLuint vertex_array;
    GLuint vertex_buffer; //Use the vertex buffer for all drawing
    glCreateVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glCreateBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3, &vertices, GL_STATIC_DRAW);

    std::cout <<"vertex buffer done"<<std::endl;

    //Create instance buffer
    // Index Buffer from Seb Aaltonen: https://twitter.com/SebAaltonen/status/1322819025047605248/photo/2
    const uint32_t num_cube_indices = 3 * 2 * 3; //3 faces, 2 tris per face, 3 verts per tri
    const uint32_t num_cube_vertices = 8;
    const uint32_t cube_indices[num_cube_indices] = {
        0, 2, 1, 2, 3, 1,
        5, 4, 1, 1, 4, 0,
        0, 4, 6, 0, 6, 2, 
    };

    const uint32_t cube_num_indices = no_of_instances * num_cube_indices;
    uint32_t *indices = new uint32_t[cube_num_indices];
    for (uint32_t i = 0; i < cube_num_indices; i++) {
      uint32_t cube = i / num_cube_indices;
      uint32_t cube_local = i % num_cube_indices;
      indices[i] = cube_indices[cube_local] + cube * num_cube_vertices;
    }

    GLuint index_buffer;
    glCreateBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * cube_num_indices, indices, GL_STATIC_DRAW);

    std::cout <<"index buffer done"<<std::endl;

    // Create a storagebuffer for rendering point primitives
    uint32_t *instances = new uint32_t[no_of_instances * 2];

    for (int i = 0; i < no_of_instances; i++) {

      uint32_t x = (i%width);
      uint32_t y = ((i/(width*depth))) << 11;
      uint32_t z = ((i/width)%depth) << 21;
      uint32_t pos = x|y|z;

      uint32_t r = uint32_t(float(std::rand()) / float(RAND_MAX) * 32.0); // 5 bits 0-31
      uint32_t g = uint32_t(float(std::rand()) / float(RAND_MAX) * 64.0) << 5; // 6 bits 0-63
      uint32_t b = uint32_t(float(std::rand()) / float(RAND_MAX) * 32.0) <<11; // 5 bits 0-31
      uint32_t albedo = r|g|b;

      instances[i*2] = pos;
      instances[i*2+1] = albedo;
    }


    GLuint voxel_buffer;
    glCreateBuffers(1, &voxel_buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, voxel_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t) * 2 * no_of_instances, instances, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, voxel_buffer);

    std::cout <<"storage buffer done"<<std::endl;

  
    //create and attach shaders
    std::string vs_source = get_file("./shaders/cube.vert").str(),
    fs_source = get_file("./shaders/cube.frag").str();
#endif
    GLuint program = glCreateProgram();
    GLuint vs = createShader(vs_source,GL_VERTEX_SHADER);
    GLuint fs = createShader(fs_source,GL_FRAGMENT_SHADER);

    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    glUseProgram(program);
    
    GLuint screen_size_location = glGetUniformLocation(program,"screen_size");
    GLuint camera_location = glGetUniformLocation(program,"view_projection_matrix");
    GLuint camera_origin = glGetUniformLocation(program,"camera_origin");
    GLuint inv_proj = glGetUniformLocation(program,"inv_projection");
    GLuint proj = glGetUniformLocation(program,"projection");
    GLuint viewport_location = glGetUniformLocation(program,"viewport");

    glClearColor(0.3f, 0.6f, 0.99f, 1.0f);
    
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view = camera.GetViewMatrix();
        projection = projection*view;
        glUniformMatrix4fv(camera_location, 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(proj, 1, GL_FALSE, &projection[0][0]);
        projection = glm::inverse(projection);
        glUniformMatrix4fv(inv_proj, 1, GL_FALSE, &projection[0][0]);
        glUniform3fv(camera_origin, 1, &camera.Position[0]);
        glUniform2f(screen_size_location, float(SCREEN_WIDTH), float(SCREEN_HEIGHT));
        glUniform4f(viewport_location, 0.0, 0.0, float(SCREEN_WIDTH), float(SCREEN_HEIGHT));
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
#ifdef MODE_QUAD
        glDrawArrays(GL_POINTS, 0, no_of_vertices);
#endif

#ifdef MODE_CUBE
        glDrawElements(GL_TRIANGLES, cube_num_indices, GL_UNSIGNED_INT, 0);
#endif

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        frame++;
        if (frame %60 == 0) {
          std::cout<< deltaTime * 1000.0 <<std::endl; //in microseconds
        }
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwTerminate();
#ifdef MODE_QUAD
  delete vertices;
#endif
#ifdef MODE_CUBE
  delete indices;
#endif

  exit(EXIT_SUCCESS);
}
