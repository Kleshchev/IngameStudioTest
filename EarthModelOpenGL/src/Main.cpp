#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "InternalTypes.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Sphere.h"
#include "Circle.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

#define EARTH_ROTATION_SPEED 0.3f
#define SPHERES_RESOLUTION   30

#define EARTH_TEXTURE_PATH "res/textures/earth2048.bmp"
#define MOON_TEXTURE_PATH  "res/textures/moon1024.bmp"

#define SPHERE_SHADER_PATH "res/shaders/sphere.shader"
#define CIRCLE_SHADER_PATH "res/shaders/circle.shader"

#define ROTATION_SPEED_MIN 1.00001f
#define ROTATION_SPEED_MAX 10.0f

#define CAMERA_POS_RESIZE_VALUE 1.05f

GLFWwindow* window;

float rotationSpeed = (ROTATION_SPEED_MIN + ROTATION_SPEED_MAX) / 2;
float spheresScale  = 1;
float moonAngle     = 0;
float ratio;
int   width, height;

glm::mat4 m = glm::mat4(1.0);
glm::mat4 p = glm::mat4(1.0);
glm::mat4 v = glm::mat4(1.0);

const glm::vec3 CAMERA_FRONT     = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 CAMERA_UP        = glm::vec3(0.0f, 1.0f, -1.0f);
const glm::vec4 BACKGROUND_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 3.0f);

void windowInit();
void windowClose();
void openGLInit();

void imGuiInit();
void imGuiNewFrame();
void imGuiPanel();
void imGuiEndFrame();
void imGuiClose();

static void errorCallback(int error, const char* description);
static void keyCallback(GLFWwindow* window,
                        int         key,
                        int         scancode,
                        int         action,
                        int         mods);

void         setVertexLayout();
unsigned int setVertexArray(const Model& model, unsigned int* indicesCount);
void         updateMVP();

int main(void) {
  windowInit();
  openGLInit();
  imGuiInit();

  Sphere sphereModel(1.0f, SPHERES_RESOLUTION, SPHERES_RESOLUTION * 2);
  Circle circleModel(1.0f, SPHERES_RESOLUTION * 2);

  unsigned int sphereIndicesCount;
  unsigned int circleIndicesCount;

  unsigned int sphereVAO = setVertexArray(sphereModel, &sphereIndicesCount);
  unsigned int circleVAO = setVertexArray(circleModel, &circleIndicesCount);

  sphereModel.ClearData();
  circleModel.ClearData();


  Shader circleShader(CIRCLE_SHADER_PATH);
  Shader sphereShader(SPHERE_SHADER_PATH);

  Texture earthTexture(EARTH_TEXTURE_PATH);
  Texture moonTexture(MOON_TEXTURE_PATH);

  glBindVertexArray(0);
  while (!glfwWindowShouldClose(window)) {
    imGuiNewFrame();

    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClearColor(BACKGROUND_COLOR.r,
                 BACKGROUND_COLOR.g,
                 BACKGROUND_COLOR.b,
                 BACKGROUND_COLOR.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateMVP();

    auto earthModel = glm::rotate(m,
                                  (float)glfwGetTime() * EARTH_ROTATION_SPEED,
                                  glm::vec3(0.0f, 0.0f, 1.0f));

    sphereShader.Bind();
    sphereShader.SetUniformMat4f("M", earthModel);
    sphereShader.SetUniformMat4f("V", v);
    sphereShader.SetUniformMat4f("P", p);

    earthTexture.Bind();
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndicesCount, GL_UNSIGNED_INT, (void*)0);

    auto trajectoryModel = glm::mat4(m);

    trajectoryModel = glm::scale(trajectoryModel, glm::vec3(2));
    trajectoryModel =
        glm::rotate(trajectoryModel, moonAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    circleShader.Bind();
    sphereShader.SetUniformMat4f("M", trajectoryModel);
    sphereShader.SetUniformMat4f("V", v);
    sphereShader.SetUniformMat4f("P", p);
    glBindVertexArray(circleVAO);
    glDrawElements(GL_LINES, circleIndicesCount, GL_UNSIGNED_INT, (void*)0);

    auto moonModel = glm::mat4(m);

    moonModel = glm::rotate(moonModel, moonAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    moonModel = glm::rotate(moonModel,
                            (float)glfwGetTime() * rotationSpeed,
                            glm::vec3(0.0f, 0.0f, 1.0f));

    moonModel = glm::translate(moonModel, glm::vec3(2.0f, 0.0f, 0.0f));

    moonModel = glm::rotate(moonModel,
                            (float)glfwGetTime(),
                            glm::vec3(0.0f, 0.0f, 1.0f));

    moonModel = glm::scale(moonModel, glm::vec3(spheresScale / 3.667f));

    sphereShader.Bind();
    sphereShader.SetUniformMat4f("M", moonModel);
    sphereShader.SetUniformMat4f("V", v);
    sphereShader.SetUniformMat4f("P", p);

    moonTexture.Bind();
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndicesCount, GL_UNSIGNED_INT, (void*)0);

    imGuiPanel();
    imGuiEndFrame();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  imGuiClose();

  windowClose();
  exit(EXIT_SUCCESS);
}

void windowInit() {
  glfwSetErrorCallback(errorCallback);
  if (!glfwInit()) exit(EXIT_FAILURE);
  glfwWindowHint(GLFW_DEPTH_BITS, GL_TRUE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  window = glfwCreateWindow(800, 800, "Earth and moon", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyCallback);
  glfwSwapInterval(1);
}

void windowClose() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void openGLInit() {
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_MULTISAMPLE);
}

void imGuiInit() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void imGuiNewFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void imGuiPanel() {
  ImGui::SliderFloat("Rotation Speed",
                     &rotationSpeed,
                     ROTATION_SPEED_MIN,
                     ROTATION_SPEED_MAX);
  ImGui::SliderFloat("Spheres Radius", &spheresScale, 0.1f, 2.0f);
  ImGui::SliderFloat("Moon Angle",
                     &moonAngle,
                     -(float)M_PI / 2,
                     (float)M_PI / 2);
  ImGui::Text("Application average (%.1f FPS)", ImGui::GetIO().Framerate);
}

void imGuiEndFrame() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void imGuiClose() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

static void errorCallback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window,
                        int         key,
                        int         scancode,
                        int         action,
                        int         mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cameraPos /= CAMERA_POS_RESIZE_VALUE;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cameraPos *= CAMERA_POS_RESIZE_VALUE;
}
void setVertexLayout() {
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, pos));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, normal));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, uv));
}

unsigned int setVertexArray(const Model& model, unsigned int* indicesCount) {
  unsigned int vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  auto         indices  = model.GetIndices();
  auto         vertices = model.GetVertices();
  VertexBuffer vb(vertices.data(), vertices.size() * sizeof(Vertex));
  setVertexLayout();
  (*indicesCount) = indices.size();
  IndexBuffer ib(indices.data(), *indicesCount);
  glBindVertexArray(0);
  return vao;
}

void updateMVP() {
  m = glm::mat4(1.0f);
  m = glm::scale(m, glm::vec3(spheresScale));
  m = glm::rotate(m, (float)M_PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
  p = glm::perspective((float)M_PI / 2, ratio, 0.1f, 100.0f);
  v = glm::lookAt(cameraPos, CAMERA_FRONT, CAMERA_UP);
}