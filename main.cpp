#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Window.hpp"
#include "GameObj3D.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "parametric-3d/Parametric3DShape.hpp"
#include "CubeData.hpp"
#include "Textures.hpp"

#include <iostream>
#include <vector>
using namespace std;

// Globals
int u_transform, u_pv, u_frame, u_light_pos, u_light_color;
int moveFront = 0, moveRight = 0;
float mouseX = 0, mouseY = 0;

void keyCallback(GLFWwindow *_, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        moveFront = 1;
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        moveFront = 0;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        moveFront = -1;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        moveFront = 0;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        moveRight = 1;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        moveRight = -1;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
}

static void cursorPositionCallback(GLFWwindow *_, double x, double y)
{
    mouseX = 2.0 * ((float)x / Window::width) - 1;
    mouseY = 2.0 * (1 - ((float)y / Window::height)) - 1;
}

int main()
{
    // init window
    Window::init(800, 800, "my window");

    glfwSetKeyCallback(Window::window, keyCallback);
    glfwSetCursorPosCallback(Window::window, cursorPositionCallback);

    // init objects
    Model3D sphereModel = Parametric3DShape::generate(ParametricLine::halfCircle, 50, 50);
    GameObj3D sphere(sphereModel);
    sphere.translate(0, 0, 0.0);
    sphere.scale(0.3, 0.3, 0.3);

    Model3D cubeModel(CubeData::positions, CubeData::normals, CubeData::uvs, CubeData::indices);
    GameObj3D cube(cubeModel);
    cube.translate(0, 0, 2.0);
    cube.scale(0.3, 0.3, 0.3); 
    cube.rotate(45, 45, 45);

    // light
    glm::vec3 lightPos = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

    const vector<string> texture_files{ "./../textures/gold.jpg", "./../textures/grass2.jpg" };
    // load textures
    vector<unsigned int> textures = Textures::loadTextures(texture_files);

    // create shader
    ShaderProgram sp("./../shader/vertex.vert", "./../shader/frag.frag");
    u_transform = glGetUniformLocation(sp.id, "u_transform");
    u_pv = glGetUniformLocation(sp.id, "u_pv");
    u_frame = glGetUniformLocation(sp.id, "u_frame");
    u_light_pos = glGetUniformLocation(sp.id, "u_light_pos");
    u_light_color = glGetUniformLocation(sp.id, "u_light_color");
    auto u_texture = glGetUniformLocation(sp.id, "u_texture");
    glUniform1i(u_texture, 0);    // 0th unit
    glActiveTexture(GL_TEXTURE0); // active 0th unit

    sp.use();

    // game loop
    while (!Window::isClosed())
    {
        // update camera
        Camera::rotation = glm::vec3(90 * mouseY, -90 * mouseX, 0);
        Camera::moveFront(moveFront * 0.1);
        Camera::moveRight(moveRight * 0.1);

        // update uniforms
        glUniformMatrix4fv(u_pv, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix() * Camera::getViewMatrix()));
        glUniform1i(u_frame, 1);
        glUniform3fv(u_light_pos, 1, glm::value_ptr(lightPos));
        glUniform3fv(u_light_color, 1, glm::value_ptr(lightColor));

        // draw
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(sphere.getTransform()));
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        sphere.draw();

        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(cube.getTransform()));
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        cube.draw();

        // update the scene
        Window::refresh();
    }

    Window::terminate();

    return 0;
}