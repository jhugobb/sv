// Author: Marc Comino 2019

#include <glwidget.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "./volume.h"
#include "./volume_io.h"

namespace {

const float kFieldOfView = 60;
const float kZNear = 0.1;
const float kZFar = 10;

const char kVertexShaderFile[] = "../shaders/raycast.vert";
const char kFragmentShaderFile[] = "../shaders/raycast.frag";

const int kVertexAttributeIdx = 0;
const int kNormalAttributeIdx = 1;

bool ReadFile(const std::string filename, std::string *shader_source) {
  std::ifstream infile(filename.c_str());

  if (!infile.is_open() || !infile.good()) {
    std::cerr << "Error " + filename + " not found." << std::endl;
    return false;
  }

  std::stringstream stream;
  stream << infile.rdbuf();
  infile.close();

  *shader_source = stream.str();
  return true;
}

}  // namespace

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent), initialized_(false), width_(0.0), height_(0.0) {
  setFocusPolicy(Qt::StrongFocus);
  minT.reserve(3);
  maxT.reserve(3);
  minT[0] = 0.3;
  minT[1] = 0.3;
  minT[2] = 0.3;

  maxT[0] = 0.7;
  maxT[1] = 0.7;
  maxT[2] = 0.7;
  lightPos = glm::vec3(-2,3,-2);
}

GLWidget::~GLWidget() {}

bool GLWidget::LoadVolume(const QString &path) {
  std::unique_ptr<data_representation::Volume> vol =
      std::make_unique<data_representation::Volume>();

  if (data_representation::ReadFromDicom(path.toUtf8().constData(),
                                         vol.get())) {
    vol_.reset(vol.release());
    camera_.UpdateModel(cube_->min_, cube_->max_);

    return true;
  }

  return false;
}

void GLWidget::setMinThreshold_R(int value) {
  minT[0] = (float) value / 100.0f;
  updateGL();
}

void GLWidget::setMaxThreshold_R(int value) {
  maxT[0] = (float) value / 100.0f;
  updateGL();
}

void GLWidget::setMinThreshold_G(int value) {
  minT[1] = (float) value / 100.0f;
  updateGL();
}

void GLWidget::setMaxThreshold_G(int value) {
  maxT[1] = (float) value / 100.0f;
  updateGL();
}

void GLWidget::setMinThreshold_B(int value) {
  minT[2] = (float) value / 100.0f;
  updateGL();
}

void GLWidget::setMaxThreshold_B(int value) {
  maxT[2] = (float) value / 100.0f;
  updateGL();
}

void GLWidget::changeLightPos(glm::vec3 new_pos){
  lightPos = new_pos;
  updateGL();
}


void GLWidget::initializeGL() {
  glewInit();

  glEnable(GL_NORMALIZE);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

  std::string vertex_shader, fragment_shader;
  bool res = ReadFile(kVertexShaderFile, &vertex_shader) &&
             ReadFile(kFragmentShaderFile, &fragment_shader);

  if (!res) exit(0);

  cube_ = std::make_unique<data_representation::Cube>();
  program_ = std::make_unique<QOpenGLShaderProgram>();
  program_->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                    vertex_shader.c_str());
  program_->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                    fragment_shader.c_str());
  program_->bindAttributeLocation("vertex", kVertexAttributeIdx);
  program_->bindAttributeLocation("normal", kNormalAttributeIdx);
  program_->link();

  initialized_ = true;
}

void GLWidget::resizeGL(int w, int h) {
  if (h == 0) h = 1;
  width_ = w;
  height_ = h;

  camera_.SetViewport(0, 0, w, h);
  camera_.SetProjection(kFieldOfView, kZNear, kZFar);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    camera_.StartRotating(event->x(), event->y());
  }
  if (event->button() == Qt::RightButton) {
    camera_.StartZooming(event->x(), event->y());
  }
  updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  camera_.SetRotationX(event->y());
  camera_.SetRotationY(event->x());
  camera_.SafeZoom(event->y());
  updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    camera_.StopRotating(event->x(), event->y());
  }
  if (event->button() == Qt::RightButton) {
    camera_.StopZooming(event->x(), event->y());
  }
  updateGL();
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Up) camera_.Zoom(-1);
  if (event->key() == Qt::Key_Down) camera_.Zoom(1);

  if (event->key() == Qt::Key_Left) camera_.Rotate(-1);
  if (event->key() == Qt::Key_Right) camera_.Rotate(1);

  if (event->key() == Qt::Key_W) camera_.Zoom(-1);
  if (event->key() == Qt::Key_S) camera_.Zoom(1);

  if (event->key() == Qt::Key_A) camera_.Rotate(-1);
  if (event->key() == Qt::Key_D) camera_.Rotate(1);

  if (event->key() == Qt::Key_R) {
    std::string vertex_shader, fragment_shader;
    bool res = ReadFile(kVertexShaderFile, &vertex_shader) &&
               ReadFile(kFragmentShaderFile, &fragment_shader);

    if (!res) exit(0);

    program_ = std::make_unique<QOpenGLShaderProgram>();
    program_->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                      vertex_shader.c_str());
    program_->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                      fragment_shader.c_str());
    program_->bindAttributeLocation("vertex", kVertexAttributeIdx);
    program_->bindAttributeLocation("normal", kNormalAttributeIdx);
    program_->link();
  }

  updateGL();
}

void GLWidget::paintGL() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (initialized_) {
    camera_.SetViewport();

    Eigen::Matrix4f projection = camera_.SetProjection();
    Eigen::Matrix4f view = camera_.SetView();
    Eigen::Matrix4f model = camera_.SetModel();

    program_->bind();

    GLuint minT_location_R = program_->uniformLocation("minT_R");
    glUniform1f(minT_location_R, minT[0]);
    GLuint maxT_location_R = program_->uniformLocation("maxT_R");
    glUniform1f(maxT_location_R, maxT[0]);

    GLuint minT_location_G = program_->uniformLocation("minT_G");
    glUniform1f(minT_location_G, minT[1]);
    GLuint maxT_location_G = program_->uniformLocation("maxT_G");
    glUniform1f(maxT_location_G, maxT[1]);

    GLuint minT_location_B = program_->uniformLocation("minT_B");
    glUniform1f(minT_location_B, minT[2]);
    GLuint maxT_location_B = program_->uniformLocation("maxT_B");
    glUniform1f(maxT_location_B, maxT[2]);

    GLuint l_pos_location = program_->uniformLocation("lightPos");
    glUniform3f(l_pos_location, lightPos.x, lightPos.y, lightPos.z);

    GLuint projection_location = program_->uniformLocation("projection");
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.data());

    GLuint view_location = program_->uniformLocation("view");
    glUniformMatrix4fv(view_location, 1, GL_FALSE, view.data());

    GLuint model_location = program_->uniformLocation("model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, model.data());

    if (vol_ != nullptr) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_3D, vol_->GetTextureId());

      GLint volume = program_->uniformLocation("volume");
      glUniform1i(volume, 0);
    }

    cube_->Render();

    glDisable(GL_BLEND);
  }
}
