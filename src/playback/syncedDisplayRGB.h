#ifndef SYNCEDDISPLAYRGB_H
#define SYNCEDDISPLAYRGB_H

#include <chrono>
#include <memory>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include "Mk03/engineContainer.h"


class syncedDisplayRGB final : public QOpenGLWidget, public QOpenGLFunctions
{
  Q_OBJECT

public:
  explicit syncedDisplayRGB(QWidget *parent = nullptr);
  void setEngine(Mk03::engineContainer<>* ptr);
  ~syncedDisplayRGB() override;
  void slotAudioPtsXBase_ms(int64_t ptsXBase);

protected:
  void resizeGL(int w, int h) override;
  void paintGL() override;
  void initializeGL() override;

signals:
  void emitProgressPtsXBase(int64_t pts);


public slots:
  void setPlayFlag(bool flag);

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
  Mk03::engineContainer<>* pePtr;
  Mk03::videoBuffer* printBuf;
  int64_t audioPtsXBase_ms;
};

#endif // SYNCEDDISPLAYRGB_H
