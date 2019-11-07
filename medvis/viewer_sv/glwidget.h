// Author: Marc Comino 2019

#ifndef GLWIDGET_H_
#define GLWIDGET_H_

#include <GL/glew.h>
#include <QGLWidget>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QString>

#include <memory>
#include <glm/glm.hpp>


#include "./camera.h"
#include "./cube.h"
#include "./volume.h"

class GLWidget : public QGLWidget {
  Q_OBJECT

 public:
  explicit GLWidget(QWidget *parent = 0);
  ~GLWidget();

  /**
   * @brief LoadVolume Loads a volume model from the input path.
   * @param filename Path to the stack of images composing the volume model.
   * @return Whether it was able to load the volume.
   */
  bool LoadVolume(const QString &filename);

  void setMinThreshold_R(int value);
  void setMaxThreshold_R(int value);

  void setMinThreshold_G(int value);
  void setMaxThreshold_G(int value);

  void setMinThreshold_B(int value);
  void setMaxThreshold_B(int value);

  void changeLightPos(glm::vec3 new_pos);

 protected:
  /**
   * @brief initializeGL Initializes OpenGL variables and loads, compiles and
   * links shaders.
   */
  void initializeGL();

  /**
   * @brief resizeGL Resizes the viewport.
   * @param w New viewport width.
   * @param h New viewport height.
   */
  void resizeGL(int w, int h);

  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void keyPressEvent(QKeyEvent *event);

 private:
  /**
   * @brief program_ A basic shader program.
   */
  std::unique_ptr<QOpenGLShaderProgram> program_;

  /**
   * @brief camera_ Class that computes the multiple camera transform matrices.
   */
  data_visualization::Camera camera_;

  /**
   * @brief cube_ A cubic mesh used to render the colume.
   */
  std::unique_ptr<data_representation::Cube> cube_;

  /**
   * @brief mesh_ Data structure representing a volume.
   */
  std::unique_ptr<data_representation::Volume> vol_;

  /**
   * @brief initialized_ Whether the widget has finished initializations.
   */
  bool initialized_;

  /**
   * @brief width_ Viewport current width.
   */
  float width_;

  /**
   * @brief height_ Viewport current height.
   */
  float height_;

  /**
   * @brief minT minimum intervals for the transfer function.
   */
  std::vector<float> minT;

  /**
   * @brief maxT maximum intervals for the transfer function.
   */
  std::vector<float> maxT;

  /**
   * @brief lightPos Position of the Light in the scene.
   */
  glm::vec3 lightPos;

 protected slots:
  /**
   * @brief paintGL Function that handles rendering the scene.
   */
  void paintGL();
};

#endif  //  GLWIDGET_H_
