#include <glfw-shell/glfw-shell.h>
#include <glisy/texture.h>
#include <glisy/uniform.h>
#include <glisy/program.h>
#include <glisy/shader.h>
#include <glisy/math.h>
#include <glisy/vao.h>
#include <glisy/gl.h>

#include <SOIL/SOIL.h>
#include <stdlib.h>
#include <stdio.h>
#include <fs/fs.h>

#ifndef IMAGE_PATH
#define IMAGE_PATH "baboon.png"
#endif

GlisyUniform uTexture;
GlisyTexture texture;
GlisyProgram program;
GlisyShader fragment;
GlisyShader vertex;

GLFWwindow *window;
const char *name = "test";
int height = 640;
int width = 640;

void
Render() {
  static GlisyVAOAttribute position;
  static GlisyVAO vao;
  static GLboolean initialized = GL_FALSE;
  static vec2 vertices[] = {
    +4.0f, -4.0f,
    -0.0f, +4.0f,
    -4.0f, -4.0f,
  };

  if (!initialized) {
    // init vao
    memset(&position, 0, sizeof(position));
    glisyVAOInit(&vao);

    // set position attribute buffer options
    position.buffer.data = vertices;
    position.buffer.type = GL_FLOAT;
    position.buffer.size = sizeof(position);
    position.buffer.usage = GL_STATIC_DRAW;
    position.buffer.dimension = 2;
  }

  initialized = GL_TRUE;

  glisyVAOPush(&vao, &position);
  glisyVAOUpdate(&vao, 0);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glisyVAOUnbind(&vao);
}

int
main (void) {
  GLFW_SHELL_CONTEXT_INIT(3, 2);
  GLFW_SHELL_WINDOW_INIT(window, width, height);

  // initialize program
  glisyProgramInit(&program);
  glisyShaderInit(&vertex, GL_VERTEX_SHADER, fs_read("./vertex.glsl"));
  glisyShaderInit(&fragment, GL_FRAGMENT_SHADER, fs_read("./fragment.glsl"));
  glisyProgramAttachShader(&program, &vertex);
  glisyProgramAttachShader(&program, &fragment);
  glisyProgramLink(&program);
  glisyProgramBind(&program);

  // load image and initialize texture
  {
    printf("SOIL_load_image: %s\n", IMAGE_PATH);
    int width, height;
    unsigned char *image = SOIL_load_image(IMAGE_PATH, &width, &height, 0, SOIL_LOAD_AUTO);

    if (image && width && height) {
      // intialize texture
      GLuint shape[2] = { width, height };
      glisyTextureInit(&texture, GL_TEXTURE_2D);
      glisyTextureUpdate(&texture, (GLvoid **) image, shape, 0, 0, 0);
      glisyTextureUnbind(&texture);

      // clean up
      SOIL_free_image_data(image);

      // initialize texture uniform
      glisyUniformInit(&uTexture, "image", GLISY_UNIFORM_INT, 0);
      glViewport(0, 0, width, height);
    }
  }

  // start loop
  GLFW_SHELL_RENDER(window, { Render(); });
  return 0;
}
