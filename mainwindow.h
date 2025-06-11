#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QTimer>
#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include "worker.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:


private slots:
    // slots
    void on_resetButton_clicked();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_speedSlider_valueChanged(int value);
    void on_comboBox_activated(int index);
    void process();
    void on_clearAlgoButton_clicked();
    QVector<int> generateRandomVector(int size);
    void finishSorting(int index);

    void updateChartDuringSort(int chartIndex, const QVector<int> &data);

private:
    Ui::MainWindow *ui;
    // stopwatch variables
    QTimer timer;
    short int milliseconds;
    short int seconds;
    short int minutes;
    //shared vector
    QVector<int> sharedRandomVector;
    // CHARTS
    QChartView *chartViews[6];
    QChart *charts[6];
    QBarSeries *barSeries[6];
    bool chartOccupied[6] = {false, false, false, false, false, false};
    void setupChartsGrid();
    void updateChart(int index, const QVector<int> &values, const QString &algorithm);

    // Keep track of assigned algorithms for each chart
    QString assignedAlgorithms[6];

    // Arrays to store threads and workers for each chart
    int activeWorkers = 0;

    QThread* threads[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    Worker* workers[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

};
#endif // MAINWINDOW_H
