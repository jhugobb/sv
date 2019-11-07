// Author: Marc Comino 2019

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <QMainWindow>
#include <glm/glm.hpp>


namespace Ui {
class MainWindow;
}

namespace gui {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  virtual void show();

 private slots:
  /**
   * @brief on_actionQuit_triggered Closes the application.
   */
  void on_actionQuit_triggered();

  /**
   * @brief on_actionLoad_triggered Opens a file dialog to load a PLY mesh.
   */
  void on_actionLoad_triggered();

  /**
   * @brief on_MaxSlider_R_valueChanged Changes the maximum value of the 
   * transfer function for the Red component.
   */
  void on_MaxSlider_R_valueChanged(int value);

  /**
   * @brief on_MinSlider_R_valueChanged Changes the minimum value of the 
   * transfer function for the Red component.
   */
  void on_MinSlider_R_valueChanged(int value);

  /**
   * @brief on_MaxSlider_G_valueChanged Changes the maximum value of the 
   * transfer function for the Green component.
   */
  void on_MaxSlider_G_valueChanged(int value);

  /**
   * @brief on_MinSlider_G_valueChanged Changes the minimum value of the 
   * transfer function for the Green component.
   */
  void on_MinSlider_G_valueChanged(int value);

  /**
   * @brief on_MaxSlider_B_valueChanged Changes the maximum value of the 
   * transfer function for the Blue component.
   */
  void on_MaxSlider_B_valueChanged(int value);
  
  /**
   * @brief on_MinSlider_B_valueChanged Changes the minimum value of the 
   * transfer function for the Blue component.
   */
  void on_MinSlider_B_valueChanged(int value);

  /**
   * @brief on_x_light_valueChanged Changes the x coordinate of the light position.
   */
  void on_x_light_valueChanged(double arg1);

  /**
   * @brief on_y_light_valueChanged Changes the y coordinate of the light position.
   */
  void on_y_light_valueChanged(double arg1);

  /**
   * @brief on_z_light_valueChanged Changes the z coordinate of the light position.
   */
  void on_z_light_valueChanged(double arg1);

private:
  Ui::MainWindow *ui_;
  std::vector<int> minT;
  std::vector<int> maxT;
  glm::vec3 lightPos;
};

}  //  namespace gui

#endif  //  MAIN_WINDOW_H_
