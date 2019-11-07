// Author: Marc Comino 2018

#include <main_window.h>

#include <QFileDialog>
#include <QMessageBox>

#include "./ui_main_window.h"
#include <iostream>

namespace gui {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  ui_->setupUi(this);
  minT.reserve(3);
  maxT.reserve(3);
  minT[0] = 30;
  minT[1] = 30;
  minT[2] = 30;

  maxT[0] = 70;
  maxT[1] = 70;
  maxT[2] = 70;
  lightPos = glm::vec3(-2,3,-2);
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::show() { QMainWindow::show(); }

void MainWindow::on_actionQuit_triggered() { close(); }

void MainWindow::on_actionLoad_triggered() {
  QString filename = QFileDialog::getExistingDirectory(
      this, "Choose a directory.", ".", QFileDialog::Option::ShowDirsOnly);
  if (!filename.isNull()) {
    if (!ui_->glwidget->LoadVolume(filename))
      QMessageBox::warning(this, tr("Error"),
                           tr("The selected volume could not be opened."));
  }
}

void MainWindow::on_MaxSlider_R_valueChanged(int value)
{
  ui_->glwidget->setMaxThreshold_R(value);
  maxT[0] = value;
  ui_->MaxValue_R->setText(QString::number(value/100.0f));
  if (minT[0] > maxT[0]) {
    ui_->MinSlider_R->setSliderPosition(value);
    minT[0] = value;
  }
}

void MainWindow::on_MinSlider_R_valueChanged(int value)
{
  ui_->glwidget->setMinThreshold_R(value);
  minT[0] = value;
  ui_->MinValue_R->setText(QString::number(value/100.0f));
  if (minT[0] > maxT[0]) {
    ui_->MaxSlider_R->setSliderPosition(value);
    maxT[0] = value;
  }
}

void MainWindow::on_MaxSlider_G_valueChanged(int value)
{
  ui_->glwidget->setMaxThreshold_G(value);
  maxT[1] = value;
  ui_->MaxValue_G->setText(QString::number(value/100.0f));
  if (minT[1] > maxT[1]) {
    ui_->MinSlider_G->setSliderPosition(value);
    minT[1] = value;
  }
}

void MainWindow::on_MinSlider_G_valueChanged(int value)
{
  ui_->glwidget->setMinThreshold_G(value);
  minT[1] = value;
  ui_->MinValue_G->setText(QString::number(value/100.0f));
  if (minT[1] > maxT[1]) {
    ui_->MaxSlider_G->setSliderPosition(value);
    maxT[1] = value;
  }
}

void MainWindow::on_MaxSlider_B_valueChanged(int value)
{
  ui_->glwidget->setMaxThreshold_B(value);
  maxT[2] = value;
  ui_->MaxValue_B->setText(QString::number(value/100.0f));
  if (minT[2] > maxT[2]) {
    ui_->MinSlider_B->setSliderPosition(value);
    minT[2] = value;
  }
}

void MainWindow::on_MinSlider_B_valueChanged(int value)
{
  ui_->glwidget->setMinThreshold_B(value);
  minT[2] = value;
  ui_->MinValue_B->setText(QString::number(value/100.0f));
  if (minT[2] > maxT[2]) {
    ui_->MaxSlider_B->setSliderPosition(value);
    maxT[2] = value;
  }
}

void MainWindow::on_x_light_valueChanged(double arg1)
{
  lightPos.x = arg1;
  ui_->glwidget->changeLightPos(lightPos);
}

void MainWindow::on_y_light_valueChanged(double arg1)
{
  lightPos.y = arg1;
  ui_->glwidget->changeLightPos(lightPos);
}

void MainWindow::on_z_light_valueChanged(double arg1)
{
  lightPos.z = arg1;
  ui_->glwidget->changeLightPos(lightPos);
}

}  //  namespace gui


