from PySide6.QtCore import QDateTime, Qt, Slot
from PySide6.QtGui import QPainter
from PySide6.QtWidgets import (
    QHBoxLayout,
    QVBoxLayout,
    QHeaderView,
    QSizePolicy,
    QTableView,
    QWidget,
    QScrollBar,
)
from PySide6.QtCharts import QChart, QChartView, QLineSeries, QDateTimeAxis, QValueAxis

from table_model import CustomTableModel
from port_widget import PortWidget
import pandas as pd
import time


class PlotWidget(QWidget):
    def __init__(self, data=None, port_widget: PortWidget = None):
        QWidget.__init__(self)

        self.model = CustomTableModel(data)
        self.table_view = QTableView()
        self.table_view.setModel(self.model)

        # QTableView Headers
        self.table_view.setHorizontalHeader(QHeaderView(Qt.Horizontal))
        self.table_view.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)

        self.x_min = None
        self.x_max = None
        self.y_min = None
        self.y_max = None

        # Creating QChart
        self.chart = QChart()
        self.chart.setAnimationOptions(QChart.NoAnimation)

        # Creating QChartView
        self.chart_view = QChartView(self.chart)
        self.chart_view.setRenderHint(QPainter.Antialiasing)

        # QWidget Layout
        self.main_layout = QHBoxLayout()
        size = QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)

        # Left layout
        size.setHorizontalStretch(1)
        self.table_view.setSizePolicy(size)
        self.main_layout.addWidget(self.table_view)

        # Right Layout
        size.setHorizontalStretch(3)
        self.chart_view.setSizePolicy(size)
        self.main_layout.addWidget(self.chart_view)

        # Set the layout to the QWidget
        self.setLayout(self.main_layout)

        self.series_list = []
        self.axis_x = None
        self.axis_y = None

        self.x_range = 10  # seconds
        self.x_range_start = None
        self.x_range_end = None

        if isinstance(data, pd.DataFrame) and not data.empty:
            # self.model.appendRow(data)
            self.add_series("Ax", populate=True)
            self.add_series("Ay", populate=True)
            self.add_series("Az", populate=True)

        if port_widget:
            self.add_series("θ")
            self.add_series("φ")
            self.add_series("ψ")
            self.port_widget = port_widget
            self.port_widget.data_received.connect(self.update_from_serial)

        self.add_axis()

    def update_row(self, row_i, series_i):
        t = self.model.index(row_i, 0).data()
        # Check if is number (int or float)
        if not t.replace(".", "", 1).isdigit():
            return
        t = float(t)
        y = float(self.model.index(row_i, series_i + 1).data())

        self.update_ranges(t, y)
        self.series_list[series_i].append(t, y)

    def update_series(self):
        if self.model.input_data.empty:
            return

        row_count = self.model.rowCount()
        for s_i in range(len(self.series_list)):
            self.update_row(row_count - 1, s_i)

        self.update_axes()

    def update_axes(self):
        row_count = self.model.rowCount()

        y_min = self.y_min - 0.5
        y_max = self.y_max + 0.5
        self.axis_y.setRange(y_min, y_max)

        if self.x_max >= self.x_range_end:
            self.x_range_end += self.x_range
            # self.x_range_start += self.x_range

            # Update the x-axis range
            new_x_min_datetime = QDateTime.fromMSecsSinceEpoch(int(self.x_range_start))
            new_x_max_datetime = QDateTime.fromMSecsSinceEpoch(int(self.x_range_end))
            self.axis_x.setRange(new_x_min_datetime, new_x_max_datetime)

    def add_series(self, name: str, populate=False):
        series = QLineSeries()
        series.setName(name)

        series_i = len(self.series_list)
        self.series_list.append(series)

        if populate:
            for row_i in range(self.model.rowCount()):
                self.update_row(row_i, series_i)

        self.chart.addSeries(series)

        color_name = series.pen().color().name()
        self.model.color = f"{color_name}"

    def update_ranges(self, x, y):
        if not self.x_min or x < self.x_min:
            self.x_min = x
        if not self.x_max or x > self.x_max:
            self.x_max = x
        if not self.y_min or y < self.y_min:
            self.y_min = y
        if not self.y_max or y > self.y_max:
            self.y_max = y
        if not self.x_range_start:
            self.x_range_start = self.x_min
        if not self.x_range_end:
            self.x_range_end = self.x_max

    def add_axis(self):
        x_min = self.x_min
        if not self.x_min:
            x_min = 0
        x_max = self.x_max
        if not self.x_max:
            x_max = 0
        y_min = self.y_min
        if not self.y_min:
            y_min = 0
        y_max = self.y_max
        if not self.y_max:
            y_max = 0

        self.axis_x = QDateTimeAxis()
        x_min_datetime = QDateTime.fromMSecsSinceEpoch(int(x_min))
        x_max_datetime = QDateTime.fromMSecsSinceEpoch(int(x_max))
        self.axis_x.setRange(x_min_datetime, x_max_datetime)
        self.axis_x.setTickCount(10)
        self.axis_x.setFormat("ss.zzz")
        self.axis_x.setTitleText("Time")
        self.chart.addAxis(self.axis_x, Qt.AlignBottom)

        self.axis_y = QValueAxis()
        self.axis_y.setTickCount(20)
        self.axis_y.setRange(y_min, y_max)
        self.axis_y.setLabelFormat("%.2f")
        self.axis_y.setTitleText("Values")
        self.chart.addAxis(self.axis_y, Qt.AlignLeft)

        for i in range(len(self.series_list)):
            self.series_list[i].attachAxis(self.axis_x)
            self.series_list[i].attachAxis(self.axis_y)

    def clear_data(self):
        row_count = self.model.rowCount()
        for i in range(row_count):
            self.model.removeRow(0)  # Remove the first row until there are no more rows

        self.chart.removeAllSeries()
        self.x_min = None
        self.x_max = None
        self.y_min = None
        self.y_max = None
        self.series_list.clear()
        self.axis_x = None
        self.axis_y = None

    @Slot()
    def update_from_serial(self, data):
        data = data.split(";")

        if len(data) == 3:  # Assuming 3 data values (Ax, Ay, Az)
            data.insert(0, time.time())
            self.model.appendRow(data)
            self.update_series()
        if len(data) == 4:  # Assuming 4 data values (T, Ax, Ay, Az)
            self.model.appendRow(data)
            self.update_series()
