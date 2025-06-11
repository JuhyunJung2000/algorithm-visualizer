# Algorithms Visualizer

This is a C++ application built with Qt Studio that visualizes the behavior of different sorting algorithms in real time. The project provides an interactive and intuitive way to observe how sorting algorithms operate internally, making it ideal for both learning and demonstration purposes.

## Features

- Real-time sorting visualization using bar charts.
- Each bar represents a value in the array being sorted.
- Users can select from a list of 10 available algorithms.
- The visualizer runs the sorting logic in a multithreaded environment using worker threads (up to 6 workers).
- Smooth animations for easy understanding of algorithm behavior.


## How It Works

1. The user selects a sorting algorithm from the available list.
2. The application generates a random dataset and displays it as a bar chart.
3. Once started, the algorithm sorts the data visually.
4. Each bar changes position and height dynamically to reflect the sorting process.
5. Multithreading ensures that the UI remains responsive and sorting operations run concurrently without blocking the interface.


