#include "worker.h"
#include <QThread>
#include <QDebug>
#include <algorithm>
#include <cmath> // For log function

Worker::Worker(QObject *parent) : QObject(parent), m_chartIndex(-1), m_speed(50)
{
}

void Worker::setData(const QVector<int> &data)
{
    m_data = data;
}

void Worker::setAlgorithm(const QString &algorithmName)
{
    m_algorithmName = algorithmName;
}

void Worker::setChartIndex(int index)
{
    m_chartIndex = index;
}

void Worker::setSpeed(int speed)
{
    // Clamp speed between 1 and 100 to prevent invalid delay values
    speed = qBound(1, speed, 100);
    m_speed.storeRelease(speed);
}

void Worker::process()
{
    qDebug() << "Worker" << m_chartIndex << ": Starting process with algorithm" << m_algorithmName;
    runAlgorithm();
    emit finished();
    qDebug() << "Worker" << m_chartIndex << ": Finished process";
}

void Worker::runAlgorithm()
{
    if (m_algorithmName == "Bubble Sort") {
        bubbleSort();
    }
    else if (m_algorithmName == "Selection Sort") {
        selectionSort();
    }
    else if (m_algorithmName == "Insertion Sort") {
        insertionSort();
    }
    else if (m_algorithmName == "Merge Sort") {
        mergeSort();
    }
    else if (m_algorithmName == "Quick Sort") {
        quickSort();
    }
    else if (m_algorithmName == "Heap Sort") {
        heapSort();
    }
    else if (m_algorithmName == "Counting Sort") {
        countingSort();
    }
    else if (m_algorithmName == "Shell Sort") {
        shellSort();
    }
    else if (m_algorithmName == "Radix Sort") {
        radixSort();
    }
    else if (m_algorithmName == "Bucket Sort") {
        bucketSort();
    }
    else if (m_algorithmName == "Intro Sort") {
        introSort(); // Simplified version
    }
    else {
        qWarning() << "Worker" << m_chartIndex << ": Unknown algorithm" << m_algorithmName;
    }
}

//////////////////////////
// Bubble Sort
//////////////////////////
void Worker::bubbleSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    // Delay multiplier for O(n²) algorithms
    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 3; // Longer delay for O(n²)

    for (int i = 0; i < n - 1 && !QThread::currentThread()->isInterruptionRequested(); ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1 && !QThread::currentThread()->isInterruptionRequested(); ++j) {
            if (m_data[j] > m_data[j + 1]) {
                qSwap(m_data[j], m_data[j + 1]);
                swapped = true;

                emit progress(m_chartIndex, m_data);

                QThread::msleep(delay);
            }
        }
        if (!swapped) break;
    }
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

//////////////////////////
// Selection Sort
//////////////////////////
void Worker::selectionSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 4; // Longer delay for O(n²)

    for (int i = 0; i < n - 1 && !QThread::currentThread()->isInterruptionRequested(); ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n && !QThread::currentThread()->isInterruptionRequested(); ++j) {
            if (m_data[j] < m_data[min_idx]) {
                min_idx = j;
            }
        }

        if (min_idx != i) {
            qSwap(m_data[i], m_data[min_idx]);

            emit progress(m_chartIndex, m_data);

            QThread::msleep(delay);
        }
    }
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

//////////////////////////
// Insertion Sort
//////////////////////////
void Worker::insertionSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 4; // Longer delay for O(n²)

    for (int i = 1; i < n && !QThread::currentThread()->isInterruptionRequested(); ++i) {
        int key = m_data[i];
        int j = i - 1;

        while (j >= 0 && m_data[j] > key && !QThread::currentThread()->isInterruptionRequested()) {
            m_data[j + 1] = m_data[j];
            j--;

            emit progress(m_chartIndex, m_data);

            QThread::msleep(delay);
        }
        m_data[j + 1] = key;

        emit progress(m_chartIndex, m_data);

        QThread::msleep(delay);
    }
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

//////////////////////////
// Merge Sort
//////////////////////////
void Worker::mergeSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    mergeSortRecursive(0, n - 1);
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

void Worker::mergeSortRecursive(int left, int right)
{
    if (left >= right || QThread::currentThread()->isInterruptionRequested()) {
        return;
    }

    int mid = left + (right - left) / 2;
    mergeSortRecursive(left, mid);
    mergeSortRecursive(mid + 1, right);

    // Merge two halves
    int n1 = mid - left + 1;
    int n2 = right - mid;

    QVector<int> L(n1);
    QVector<int> R(n2);

    for (int i = 0; i < n1; ++i)
        L[i] = m_data[left + i];
    for (int j = 0; j < n2; ++j)
        R[j] = m_data[mid + 1 + j];

    int i = 0, j = 0, k = left;

    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 2; // Medium delay for O(n log n)

    while (i < n1 && j < n2 && !QThread::currentThread()->isInterruptionRequested()) {
        if (L[i] <= R[j]) {
            m_data[k] = L[i];
            i++;
        }
        else {
            m_data[k] = R[j];
            j++;
        }
        k++;

        emit progress(m_chartIndex, m_data);

        QThread::msleep(delay);
    }

    while (i < n1 && !QThread::currentThread()->isInterruptionRequested()) {
        m_data[k++] = L[i++];
    }

    while (j < n2 && !QThread::currentThread()->isInterruptionRequested()) {
        m_data[k++] = R[j++];
    }
}

//////////////////////////
// Quick Sort
//////////////////////////
void Worker::quickSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    quickSortRecursive(0, n - 1);
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

void Worker::quickSortRecursive(int low, int high)
{
    if (low < high && !QThread::currentThread()->isInterruptionRequested()) {
        int pi = partition(low, high);

        emit progress(m_chartIndex, m_data);

        int speed = m_speed.loadAcquire();
        speed = qBound(1, speed, 100);
        int baseDelay = 101 - speed; // Ensures baseDelay >=1
        int delay = baseDelay * 2; // Medium delay for O(n log n)

        QThread::msleep(delay);

        quickSortRecursive(low, pi - 1);
        quickSortRecursive(pi + 1, high);
    }
}

int Worker::partition(int low, int high)
{
    int pivot = m_data[high];
    int i = low - 1;

    for (int j = low; j <= high - 1 && !QThread::currentThread()->isInterruptionRequested(); j++) {
        if (m_data[j] < pivot) {
            i++;
            qSwap(m_data[i], m_data[j]);
        }
    }
    qSwap(m_data[i + 1], m_data[high]);
    return (i + 1);
}

//////////////////////////
// Heap Sort
//////////////////////////
void Worker::heapSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    // Build heap
    for (int i = n / 2 - 1; i >= 0 && !QThread::currentThread()->isInterruptionRequested(); i--) {
        heapify(n, i);
    }

    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 2; // Medium delay for O(n log n)

    // One by one extract elements
    for (int i = n - 1; i > 0 && !QThread::currentThread()->isInterruptionRequested(); i--) {
        qSwap(m_data[0], m_data[i]);

        emit progress(m_chartIndex, m_data);

        QThread::msleep(delay);

        heapify(i, 0);
    }
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

void Worker::heapify(int n, int i)
{
    int largest = i; // Initialize largest as root
    int l = 2 * i + 1; // left = 2*i + 1
    int r = 2 * i + 2; // right = 2*i + 2

    // If left child exists and is greater than root
    if (l < n && m_data[l] > m_data[largest]) {
        largest = l;
    }

    // If right child exists and is greater than largest so far
    if (r < n && m_data[r] > m_data[largest]) {
        largest = r;
    }

    // If largest is not root
    if (largest != i) {
        qSwap(m_data[i], m_data[largest]);

        heapify(n, largest);
    }
}

//////////////////////////
// Counting Sort
//////////////////////////
void Worker::countingSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    int max = *std::max_element(m_data.begin(), m_data.end());
    int min = *std::min_element(m_data.begin(), m_data.end());
    int range = max - min + 1;

    QVector<int> count(range, 0);
    QVector<int> output(n);

    for (int i = 0; i < n && !QThread::currentThread()->isInterruptionRequested(); i++) {
        count[m_data[i] - min]++;
    }

    for (int i = 1; i < range && !QThread::currentThread()->isInterruptionRequested(); i++) {
        count[i] += count[i - 1];
    }

    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 1; // Shorter delay for O(n)

    for (int i = n - 1; i >= 0 && !QThread::currentThread()->isInterruptionRequested(); i--) {
        output[count[m_data[i] - min] - 1] = m_data[i];
        count[m_data[i] - min]--;

        emit progress(m_chartIndex, output);

        QThread::msleep(delay);
    }

    m_data = output;
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

//////////////////////////
// Shell Sort
//////////////////////////
void Worker::shellSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 3; // Intermediate delay for Shell Sort

    for (int gap = n / 2; gap > 0 && !QThread::currentThread()->isInterruptionRequested(); gap /= 2) {
        for (int i = gap; i < n && !QThread::currentThread()->isInterruptionRequested(); i++) {
            int temp = m_data[i];
            int j;
            for (j = i; j >= gap && m_data[j - gap] > temp && !QThread::currentThread()->isInterruptionRequested(); j -= gap) {
                m_data[j] = m_data[j - gap];

                emit progress(m_chartIndex, m_data);

                QThread::msleep(delay);
            }
            m_data[j] = temp;

            emit progress(m_chartIndex, m_data);
        }
    }
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

//////////////////////////
// Radix Sort
//////////////////////////
void Worker::radixSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    int max = *std::max_element(m_data.begin(), m_data.end());

    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 1; // Shorter delay for O(n)

    for (int exp = 1; max / exp > 0 && !QThread::currentThread()->isInterruptionRequested(); exp *= 10) {
        QVector<int> output(n);
        QVector<int> count(10, 0);

        for (int i = 0; i < n && !QThread::currentThread()->isInterruptionRequested(); i++) {
            count[(m_data[i] / exp) % 10]++;
        }

        for (int i = 1; i < 10 && !QThread::currentThread()->isInterruptionRequested(); i++) {
            count[i] += count[i - 1];
        }

        for (int i = n - 1; i >= 0 && !QThread::currentThread()->isInterruptionRequested(); i--) {
            output[count[(m_data[i] / exp) % 10] - 1] = m_data[i];
            count[(m_data[i] / exp) % 10]--;

            emit progress(m_chartIndex, output);

            QThread::msleep(delay);
        }

        m_data = output;
    }
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

//////////////////////////
// Bucket Sort
//////////////////////////
void Worker::bucketSort()
{
    int n = m_data.size();
    if (n <= 0) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    int minValue = *std::min_element(m_data.begin(), m_data.end());
    int maxValue = *std::max_element(m_data.begin(), m_data.end());

    int bucketCount = n / 10; // Adjust bucket count as needed
    bucketCount = qMax(bucketCount, 1);
    QVector<QVector<int>> buckets(bucketCount);

    // Distribute input data into buckets
    for (int i = 0; i < n && !QThread::currentThread()->isInterruptionRequested(); ++i) {
        int bucketIndex;
        if (maxValue == minValue) {
            bucketIndex = 0; // All elements are identical; assign to the first bucket
        }
        else {
            bucketIndex = ((m_data[i] - minValue) * (bucketCount - 1)) / (maxValue - minValue);
        }
        buckets[bucketIndex].append(m_data[i]);

        // Optional: Debugging statement
        // qDebug() << "Bucket Sort - Element:" << m_data[i] << "Bucket Index:" << bucketIndex;
    }

    int index = 0;

    int speed = m_speed.loadAcquire();
    speed = qBound(1, speed, 100);
    int baseDelay = 101 - speed; // Ensures baseDelay >=1
    int delay = baseDelay * 1; // Shorter delay for O(n)

    for (int i = 0; i < bucketCount && !QThread::currentThread()->isInterruptionRequested(); ++i) {
        std::sort(buckets[i].begin(), buckets[i].end());

        for (int j = 0; j < buckets[i].size(); ++j) {
            m_data[index++] = buckets[i][j];

            emit progress(m_chartIndex, m_data);

            QThread::msleep(delay);
        }
    }
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

//////////////////////////
// Intro Sort (Simplified)
//////////////////////////
void Worker::introSort()
{
    int n = m_data.size();
    if (n < 2) {
        emit progress(m_chartIndex, m_data);
        return;
    }

    int maxDepth = 2 * log(n);
    introSortRecursive(0, n - 1, maxDepth);
    // Emit final progress signal
    emit progress(m_chartIndex, m_data);
}

void Worker::introSortRecursive(int low, int high, int depthLimit)
{
    if (low < high && !QThread::currentThread()->isInterruptionRequested()) {
        if (depthLimit == 0) {
            heapSort();
            return;
        }

        int pi = partition(low, high);

        emit progress(m_chartIndex, m_data);

        int speed = m_speed.loadAcquire();
        speed = qBound(1, speed, 100);
        int baseDelay = 101 - speed; // Ensures baseDelay >=1
        int delay = baseDelay * 2; // Medium delay for O(n log n)
        QThread::msleep(delay);

        introSortRecursive(low, pi - 1, depthLimit - 1);
        introSortRecursive(pi + 1, high, depthLimit - 1);
    }
}
