#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Pawspective Client");
    setGeometry(100, 100, 800, 600);
}

MainWindow::~MainWindow() = default;
