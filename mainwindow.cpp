#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDateTime>
#include <QtCharts/QValueAxis>
#include <QRandomGenerator>
#include <QThread>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);

    // Timer initialization
    milliseconds = 0;
    seconds = 0;
    minutes = 0;

    connect(&timer, &QTimer::timeout, this, &MainWindow::process);

    // Speed slider UI setup
    ui->speedSlider->setRange(0, 100);
    ui->speedSlider->setValue(100);

    //shared vector to be sorted
    sharedRandomVector = generateRandomVector(100);

    // Set up charts
    setupChartsGrid();

    // Connect signals to slots
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::on_startButton_clicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::on_stopButton_clicked);
    connect(ui->speedSlider, &QAbstractSlider::valueChanged, this, &MainWindow::on_speedSlider_valueChanged);
    connect(ui->clearAlgoButton, &QPushButton::clicked, this, &MainWindow::on_clearAlgoButton_clicked);
}

MainWindow::~MainWindow()
{
    on_stopButton_clicked();
    delete ui;
}


// RESET, START, STOP BUTTONS
void MainWindow::on_resetButton_clicked()
{
    // In on_stopButton_clicked() or on_resetButton_clicked()
    ui->comboBox->setEnabled(true);
    ui->speedSlider->setEnabled(true);
    // Timer
    timer.stop();
    milliseconds = 0;
    seconds = 0;
    minutes = 0;
    timer.setInterval(0);
    ui->millisecLCD->display(milliseconds);
    ui->secLCD->display(seconds);
    ui->minLCD->display(minutes);

    for (int i = 0; i < 6; ++i) {
        if (threads[i]) {
            threads[i]->requestInterruption();
            threads[i]->quit();
            threads[i]->wait();
            threads[i] = nullptr;
            workers[i] = nullptr;
        }
        chartOccupied[i] = false;
        assignedAlgorithms[i].clear();
        if (!barSeries[i]->barSets().isEmpty()) {
            barSeries[i]->clear();
        }
        charts[i]->setTitle("");
    }
}
void MainWindow::on_startButton_clicked()
{
    ui->comboBox->setEnabled(false);
    ui->speedSlider->setEnabled(false);
    // Timer
    timer.start();

    activeWorkers = 0; // Reset activeWorkers before starting

    // Start sorting in threads
    for (int i = 0; i < 6; ++i) {
        if (chartOccupied[i] && !assignedAlgorithms[i].isEmpty() && threads[i] == nullptr) {
            // Create a new thread
            threads[i] = new QThread;

            // Create a worker instance
            workers[i] = new Worker;
            QVector<int> dataCopy = sharedRandomVector;
            workers[i]->setData(dataCopy); // Use a copy of your shared vector
            workers[i]->setAlgorithm(assignedAlgorithms[i]);
            workers[i]->setChartIndex(i); // We'll add this method to keep track of the chart index
            workers[i]->setSpeed(ui->speedSlider->value()); // Pass the speed value

            // Move the worker to the thread
            workers[i]->moveToThread(threads[i]);

            // Connect signals and slots between worker and main thread
            connect(threads[i], &QThread::started, workers[i], &Worker::process);
            connect(workers[i], &Worker::progress, this, &MainWindow::updateChartDuringSort, Qt::QueuedConnection);

            connect(workers[i], &Worker::finished, threads[i], &QThread::quit);
            connect(workers[i], &Worker::finished, workers[i], &Worker::deleteLater);
            connect(threads[i], &QThread::finished, threads[i], &QThread::deleteLater);

            // When thread is finished, reset pointers
            connect(threads[i], &QThread::finished, this, [this, i]() {
                threads[i] = nullptr;
                workers[i] = nullptr;
                activeWorkers--;

                finishSorting(i);
                if (activeWorkers == 0) {
                    // Stop the timer
                    timer.stop();

                    // Re-enable UI elements
                    ui->comboBox->setEnabled(true);
                }
            });

            // Start the thread
            threads[i]->start();
            activeWorkers++; // Increment activeWorkers when we actually start a worker
        }
    }
}


void MainWindow::on_stopButton_clicked()
{
    // Timer
    timer.stop();
    for (int i = 0; i < 6; ++i) {
        if (threads[i]) {
            threads[i]->requestInterruption();
        }
    }
}

// SPEED SLIDER
void MainWindow::on_speedSlider_valueChanged(int value)
{
    ui->speedLCD->display(value);

    if (value > 0) {
        int newInterval = 100 / value; // Adjust as needed for speed control
        timer.setInterval(newInterval);
    }
    // Update speed in all active workers
    for (int i = 0; i < 6; ++i) {
        if (workers[i]) {
            workers[i]->setSpeed(value);
        }
    }
}

void MainWindow::on_comboBox_activated(int index)
{
    QString algorithm = ui->comboBox->itemText(index);

    // Check if the placeholder is selected
    if (algorithm == "Select an algorithm") {
        return; // Do nothing if the placeholder is selected
    }

    // Use the shared random vector (which has 500 unique values)
    QVector<int> &values = sharedRandomVector;

    // Find the first available chart slot that is empty
    for (int i = 0; i < 6; ++i) {
        if (!chartOccupied[i]) {
            // Assign the algorithm to this chart and mark it as occupied
            chartOccupied[i] = true;

            // Store the assigned algorithm name
            assignedAlgorithms[i] = algorithm;

            // Update the chart with the selected algorithm and shared values
            updateChart(i, values, algorithm);

            // Update the chart title
            charts[i]->setTitle(QString("Algorithm: %1").arg(algorithm));

            break; // Break after assigning one chart
        }
    }
}
void MainWindow::updateChart(int index, const QVector<int> &values, const QString &algorithm)
{
    if (index < 0 || index >= 6) {
        qWarning() << "Invalid chart index:" << index;
        return;
    }

    // Retrieve the bar sets for the series
    QList<QBarSet*> sets = barSeries[index]->barSets();

    // If no bar sets exist, create one
    if (sets.isEmpty()) {
        QBarSet *set = new QBarSet(algorithm);
        set->setColor(Qt::blue); // Assign a consistent color
        barSeries[index]->append(set);

        // Initialize the set with values
        for (const int &value : values) {
            *set << value;
        }
    }
    else {
        QBarSet *set = sets.first();

        // Update each bar's value
        for (int i = 0; i < values.size(); ++i) {
            if (i < set->count()) {
                set->replace(i, values[i]);
            }
            else {
                *set << values[i];
            }
        }

        // If the new data has fewer elements, remove the excess
        while (set->count() > values.size()) {
            set->remove(set->count() - 1);
        }
    }

    // Adjust axes ranges only if necessary
    QChart *chart = charts[index];

    // Fixing Clazy warning by storing the axis lists in local variables
    const QList<QAbstractAxis *> horizontalAxes = chart->axes(Qt::Horizontal);
    const QList<QAbstractAxis *> verticalAxes = chart->axes(Qt::Vertical);

    QValueAxis *axisX = nullptr;
    QValueAxis *axisY = nullptr;

    if (!horizontalAxes.isEmpty()) {
        axisX = qobject_cast<QValueAxis *>(horizontalAxes.first());
    }
    if (!verticalAxes.isEmpty()) {
        axisY = qobject_cast<QValueAxis *>(verticalAxes.first());
    }

    if (!axisX || !axisY) {
        qWarning() << "Axes not found for chart index:" << index;
        return;
    }

    // Determine new min and max
    int newMin = *std::min_element(values.begin(), values.end());
    int newMax = *std::max_element(values.begin(), values.end());

    // Update Y-axis range if necessary
    if (newMin < axisY->min() || newMax > axisY->max()) {
        axisY->setRange(newMin, newMax);
    }

    // X-axis typically remains constant based on data size
    axisX->setRange(0, values.size());

    // Optionally, hide labels and grid lines if not already done
    axisX->setLabelsVisible(false);
    axisX->setGridLineVisible(false);
    axisY->setLabelsVisible(false);
    axisY->setGridLineVisible(false);
}



void MainWindow::updateChartDuringSort(int chartIndex, const QVector<int> &data)
{
    // Update the chart at chartIndex with the new data
    updateChart(chartIndex, data, assignedAlgorithms[chartIndex]);
}

void MainWindow::on_clearAlgoButton_clicked()
{
    // Set all chartOccupied values to false to mark all charts as unoccupied
    for (int i = 0; i < 6; ++i) {
        chartOccupied[i] = false;

        if (!barSeries[i]->barSets().isEmpty()) {
            barSeries[i]->clear(); // Clear the chart data
        }

        charts[i]->setTitle("Add an algorithm");
    }
}

// TIMER PROCESSING FUNCTION
void MainWindow::process()
{
    // Update milliseconds
    milliseconds++;

    // Update seconds and minutes as needed
    if (milliseconds == 1000) {
        milliseconds = 0;
        seconds++;
    }

    if (seconds == 60) {
        seconds = 0;
        minutes++;
    }

    // Update LCD Displays
    ui->millisecLCD->display(milliseconds);
    ui->secLCD->display(seconds);
    ui->minLCD->display(minutes);
}

// CHARTS IN GRID
void MainWindow::setupChartsGrid()
{
    QGridLayout *layout = ui->chartsGrid;


    for (int i = 0; i < 6; ++i) {
        // Create a new QChart
        charts[i] = new QChart();
        charts[i]->setTitle("Add an algorithm");
        charts[i]->setBackgroundVisible(false); // Hide the background of the chart
        charts[i]->setMargins(QMargins(0, 0, 0, 0)); // Remove any margins for a clean appearance
        charts[i]->setAnimationOptions(QChart::SeriesAnimations);

        // Create a new QBarSeries and add it to the chart
        barSeries[i] = new QBarSeries();
        charts[i]->addSeries(barSeries[i]);

        // Set up a vertical axis (Y axis) to display values without labels or grid lines
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, 100); // Adjust this range depending on the values to be visualized
        axisY->setLabelsVisible(false); // Hide labels on the Y axis
        axisY->setGridLineVisible(false); // Hide grid lines
        charts[i]->addAxis(axisY, Qt::AlignLeft);
        barSeries[i]->attachAxis(axisY);

        // Set up a horizontal axis (X axis) without labels or grid lines
        QValueAxis *axisX = new QValueAxis();
        axisX->setLabelsVisible(false); // Hide labels on the X axis
        axisX->setGridLineVisible(false); // Hide grid lines
        charts[i]->addAxis(axisX, Qt::AlignBottom);
        barSeries[i]->attachAxis(axisX);

        // Create a QChartView for displaying the chart
        chartViews[i] = new QChartView(charts[i]);
        chartViews[i]->setRenderHint(QPainter::Antialiasing);

        // Add the QChartView to the grid layout
        layout->addWidget(chartViews[i], i / 2, i % 2); // i / 2 determines the row, i % 2 determines the column
    }
}
//Random vecot rot o be sorted generator
QVector<int> MainWindow::generateRandomVector(int size)
{
    QVector<int> values;
    values.reserve(size);
    for (int i = 1; i <= size; ++i) {
        values.append(i);
    }

    // Shuffle the vector using QRandomGenerator
    std::shuffle(values.begin(), values.end(), *QRandomGenerator::global());

    return values;
}

// finish sorting
void MainWindow::finishSorting(int index)
{
    if (index < 0 || index >= 6) {
        return; // Check for valid index
    }

    const QList<QBarSet*> sets = barSeries[index]->barSets(); // Store QList in a variable

    if (!sets.isEmpty()) {
        QBarSet *set = sets.first(); // Safe access

        // Set all bars to green
        set->setColor(QColor(Qt::green));
    }

    // Update the chart title to indicate completion
    charts[index]->setTitle(QString("Algorithm: %1 (Completed)").arg(assignedAlgorithms[index]));
}

