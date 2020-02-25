#include "appinfo.h"
#include "simpleDisplay.h"
#include "customExceptions.h"
#include "openglVariables.h"

simpleDisplay::simpleDisplay(QWidget *parent)
  : QOpenGLWidget(parent)
  , rgbTexture(-1)
  , textureId(-1)
  , mVideoWidth(-1)
  , mVideoHeight(-1)
  , fpsInterval(-1)
  , elapsedTime(0.0)
  , playFlag(false)
  , mTexture(nullptr)
  , mVertexShader(nullptr)
  , mFragmentShader(nullptr)
  , mShaderProgram(nullptr)
  , pePtr(nullptr)
  , printBuf(nullptr)
  , videoStreamIndex{-1} {
}

simpleDisplay::~simpleDisplay() {
  makeCurrent();
}

void simpleDisplay::setEngine(Mk01::engine* ptr) {
  assert(ptr);
  pePtr = ptr;
  videoStreamIndex = ptr->getVideoIndex();
  if(videoStreamIndex >= 0) {
      mVideoWidth = ptr->getWidth();
      mVideoHeight = ptr->getHeight();
      fpsInterval = 1.0/ptr->getFps();
    }
}

void simpleDisplay::paintGL() {
  if (playFlag) {
      mNow = std::chrono::steady_clock::now();
      double updateTick = std::chrono::duration<double>(mNow - lastUpdate).count();
      elapsedTime += updateTick;

      if (elapsedTime+updateTick > fpsInterval) {
          Mk01::Buffer* buf = pePtr->getVideoBuffer();
          if(buf) {
              if (printBuf) pePtr->returnVideoBuffer(printBuf);
              printBuf = buf;
              elapsedTime = elapsedTime - fpsInterval ;
           }
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);
        if (printBuf) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mVideoWidth, mVideoHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, printBuf->data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        lastUpdate = mNow;
    }
  update();
}

void simpleDisplay::initializeGL() {
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);

  mVertexShader.reset(new QOpenGLShader(QOpenGLShader::Vertex, this));
  mFragmentShader.reset(new QOpenGLShader(QOpenGLShader::Fragment, this));
  mShaderProgram = new QOpenGLShaderProgram(this);

  if(!(mVertexShader->compileSourceCode(mVertexShaderSimpleDisplay))) throw shaderCompileException();
  if(!(mFragmentShader->compileSourceCode(mFragmentShaderSimpleDisplay))) throw shaderCompileException();

  mShaderProgram->addShader(mFragmentShader.get());
  mShaderProgram->addShader(mVertexShader.get());
  mShaderProgram->bindAttributeLocation("inputVerticies", 0);
  mShaderProgram->bindAttributeLocation("inputTexture", 1);
  mShaderProgram->link();
  mShaderProgram->bind();
  rgbTexture = mShaderProgram->uniformLocation("texture");

  glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, mVertexVerticesSimpleDisplay);
  glVertexAttribPointer(1, 2, GL_FLOAT, 0, 0, mTextureVerticesSimpleDisplay);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  mTexture.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
  mTexture->create();
  textureId = mTexture->textureId();
  glClearColor (0.17, 0.01, 0.89, 0.0);
}

void simpleDisplay::resizeGL(int a, int b) {
  Q_UNUSED(a);
  Q_UNUSED(b);

}


void simpleDisplay::setPlayFlag() {
  if(videoStreamIndex >= 0) {
      playFlag = true;
      lastUpdate = std::chrono::steady_clock::now();
      elapsedTime = fpsInterval;
    }
}


