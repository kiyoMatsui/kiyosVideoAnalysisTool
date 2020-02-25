#ifndef SIMPLEDISPLAY_H
#define SIMPLEDISPLAY_H

#include <chrono>
#include <memory>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include "Mk01/engine.h"

class simpleDisplay : public QOpenGLWidget, public QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit simpleDisplay(QWidget *parent = nullptr);
  void setEngine(Mk01::engine* ptr);
  ~simpleDisplay() override;

protected:
  void resizeGL(int w, int h) override;
  void paintGL() override;
  void initializeGL() override;

signals:


public slots:
  void setPlayFlag();

private:
  int rgbTexture;
  GLuint textureId;
  GLsizei mVideoWidth;
  GLsizei mVideoHeight;
  double fpsInterval;
  double elapsedTime;
  bool playFlag;
  std::chrono::time_point<std::chrono::steady_clock> lastUpdate;
  std::chrono::time_point<std::chrono::steady_clock> mNow;
  std::unique_ptr<QOpenGLTexture> mTexture;
  std::unique_ptr<QOpenGLShader> mVertexShader;
  std::unique_ptr<QOpenGLShader> mFragmentShader;
  QOpenGLShaderProgram* mShaderProgram;
  Mk01::engine* pePtr;
  Mk01::Buffer* printBuf;
  int videoStreamIndex;
};

#endif // SIMPLEDISPLAY_H
