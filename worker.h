#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QVector>
#include <QAtomicInteger>
#include <QString>
#include <cmath> // For log function

class Worker : public QObject
{
    Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr);

    // Set the data and algorithm to use
    void setData(const QVector<int> &data);
    void setAlgorithm(const QString &algorithmName);
    void setChartIndex(int index);
    void setSpeed(int speed);

signals:
    void progress(int chartIndex, const QVector<int> &data);
    void finished();

public slots:
    void process();

private:
    QVector<int> m_data;
    QString m_algorithmName;
    int m_chartIndex;
    QAtomicInteger<int> m_speed;

    void runAlgorithm();

    // Sorting algorithms
    void bubbleSort();
    void selectionSort();
    void insertionSort();
    void mergeSort();
    void quickSort();
    void heapSort();
    void countingSort();
    void shellSort();
    void radixSort();
    void bucketSort();
    void introSort();    // Simplified version

    // Helper functions for complex sorts
    void mergeSortRecursive(int left, int right);
    void quickSortRecursive(int low, int high);
    int partition(int low, int high); // Corrected declaration
    void heapify(int n, int i);
    void introSortRecursive(int low, int high, int depthLimit);

};

#endif // WORKER_H
