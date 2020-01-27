#ifndef OPENGLVARIABLES_H
#define OPENGLVARIABLES_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

static constexpr GLfloat mVertexVerticesSimpleDisplay[] = {
  -1.0f,  1.0f,     1.0f,  1.0f,
  -1.0f, -1.0f,     1.0f, -1.0f,
};
static constexpr GLfloat mTextureVerticesSimpleDisplay[] = {
  0.0f,  0.0f,    1.0f,  0.0f,
  0.0f,  1.0f,    1.0f,  1.0f,
};
constexpr static const char *mVertexShaderSimpleDisplay =
  "attribute vec4 inputVerticies; \
  attribute vec2 inputTexture;    \
  varying vec2 outputTexture;     \
  void main(void) {               \
    gl_Position = inputVerticies; \
    outputTexture = inputTexture; \
  }";
constexpr static const char *mFragmentShaderSimpleDisplay =
  "varying vec2 outputTexture;   \
  uniform sampler2D texture;     \
  void main(void) {              \
    vec3 rgb;                    \
    rgb = texture2D(texture, outputTexture).rgb; \
    gl_FragColor = vec4(rgb, 1); \
  }";

#endif // OPENGLVARIABLES_H
