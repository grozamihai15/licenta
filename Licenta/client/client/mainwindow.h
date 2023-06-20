#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void populateComboBox();

    void on_listaRute_activated(int index);

private:
    Ui::MainWindow *ui;
    QLabel *raspunsServerLabel;
    QComboBox *listaRute;
};
#endif // MAINWINDOW_H
