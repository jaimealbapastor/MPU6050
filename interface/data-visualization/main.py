# https://doc.qt.io/qtforpython-6/tutorials/datavisualize/index.html

import sys
import argparse
import pandas as pd

from PySide6.QtWidgets import QApplication
from PySide6.QtCore import QDateTime, QTimeZone
from main_window import MainWindow
from plot_widget import PlotWidget


def read_data(fname):
    # Read the CSV content
    df = pd.read_csv(fname, sep=";", header=0)
    return df


if __name__ == "__main__":
    options = argparse.ArgumentParser()
    test_csv = "C:\\Users\\Jaime\\Documents\\GitHub\\PolyCube\\interface\\data-visualization\\test-acc.csv"
    options.add_argument("-f", "--file", type=str, default=test_csv)
    args = options.parse_args()
    data = read_data(args.file)

    app = QApplication(sys.argv)
    plot_widget = PlotWidget(data)
    window = MainWindow(plot_widget)

    window.show()
    sys.exit(app.exec())
