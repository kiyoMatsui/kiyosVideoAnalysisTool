#include "syncedDisplay.h"
#include "appinfo.h"
#include "customExceptions.h"
#include "openglVariables.h"

syncedDisplay::syncedDisplay(QWidget* parent)
    : QOpenGLWidget(parent),
      rgbTexture(-1),
      textureId(-1),
      mVideoWidth(-1),
      mVideoHeight(-1),
      fpsInterval(-1),
      elapsedTime(0.0),
      playFlag(false),
      mTexture(nullptr),
      mVertexShader(nullptr),
      mFragmentShader(nullptr),
      mShaderProgram(nullptr),
      pePtr(nullptr),
      printBuf(nullptr),
      audioPtsXBase_ms(0) {}

syncedDisplay::~syncedDisplay() {
  // to get current texture for deletion.
  makeCurrent();
}

void syncedDisplay::setEngine(Mk03::engineContainer<>* ptr) {
  assert(ptr);
  pePtr = ptr;
  mVideoWidth = ptr->getWidth();
  mVideoHeight = ptr->getHeight();
  fpsInterval = 1.0 / ptr->getFps();
  printBuf = nullptr;
  audioPtsXBase_ms = -1;
}

void syncedDisplay::paintGL() {
  mNow = std::chrono::steady_clock::now();
  if (playFlag) {
    double updateTick = std::chrono::duration<double>(mNow - lastUpdate).count();
    elapsedTime += updateTick;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    if (elapsedTime + updateTick > fpsInterval) {
      Mk03::videoBuffer* buf = pePtr->getVideoBuffer();
      if (buf) {
        if (printBuf) pePtr->returnVideoBuffer(printBuf);
        printBuf = buf;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mVideoWidth, mVideoHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     printBuf->buffer.data());
        emit emitProgressPtsXBase(printBuf->ptsXtimeBase_ms);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        elapsedTime -= fpsInterval;
        if (audioPtsXBase_ms != -1) {
          double diff = (int64_t)(audioPtsXBase_ms - printBuf->ptsXtimeBase_ms);
          elapsedTime += diff / 1000.0;
        }
      }
    }
  }
  lastUpdate = mNow;
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  update();
}

void syncedDisplay::initializeGL() {
  initializeOpenGLFunctions();
  glEnable(GL_DEPTH_TEST);

  mVertexShader.reset(new QOpenGLShader(QOpenGLShader::Vertex, this));
  mFragmentShader.reset(new QOpenGLShader(QOpenGLShader::Fragment, this));
  mShaderProgram = new QOpenGLShaderProgram(this);

  if (!(mVertexShader->compileSourceCode(mVertexShaderSimpleDisplay))) throw shaderCompileException();
  if (!(mFragmentShader->compileSourceCode(mFragmentShaderSimpleDisplay))) throw shaderCompileException();

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
  glClearColor(0.17, 0.01, 0.89, 0.0);
}

void syncedDisplay::resizeGL(int a, int b) {
  Q_UNUSED(a);
  Q_UNUSED(b);
}

void syncedDisplay::setPlayFlag(bool flag) {
  playFlag = flag;
  lastUpdate = std::chrono::steady_clock::now();
}

void syncedDisplay::slotAudioPtsXBase_ms(int64_t ptsXBase) {
  audioPtsXBase_ms = ptsXBase;
}
