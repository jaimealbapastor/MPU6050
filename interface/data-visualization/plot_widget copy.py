from PySide6.QtCore import QDateTime, Qt
from PySide6.QtGui import QPainter
from PySide6.QtWidgets import QHBoxLayout, QHeaderView, QSizePolicy, QTableView, QWidget
from PySide6.QtCharts import QChart, QChartView, QLineSeries, QDateTimeAxis, QValueAxis

from table_model import CustomTableModel


class PlotWidget(QWidget):
    def __init__(self, data):
        QWidget.__init__(self)

        self.model = CustomTableModel(data)
        self.table_view = QTableView()
        self.table_view.setModel(self.model)

        # QTableView Headers
        self.table_view.resizeColumnsToContents()  # TODO  relocate

        self.x_min = None
        self.x_max = None
        self.y_min = None
        self.y_max = None

        # Creating QChart
        self.chart = QChart()
        self.chart.setAnimationOptions(QChart.AllAnimations)
        self.add_series("Ax", [0, 1])
        self.add_series("Ay", [0, 2])
        self.add_series("Az", [0, 3])
        self.add_axis()

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
        size.setHorizontalStretch(4)
        self.chart_view.setSizePolicy(size)
        self.main_layout.addWidget(self.chart_view)

        # Set the layout to the QWidget
        self.setLayout(self.main_layout)

    def add_series(self, name, columns):
        # Create QLineSeries
        self.series = QLineSeries()
        self.series.setName(name)

        # Filling QLineSeries
        for i in range(self.model.rowCount()):
            # Getting the data

            x = self.model.index(i, columns[0]).data()
            if not x.replace(".", "", 1).isdigit():
                continue

            x = float(x)
            y = float(self.model.index(i, columns[1]).data())

            if not self.x_min or x < self.x_min:
                self.x_min = x
            if not self.x_max or x > self.x_max:
                self.x_max = x
            if not self.y_min or y < self.y_min:
                self.y_min = y
            if not self.y_max or y > self.y_max:
                self.y_max = y

            self.series.append(x, y)

        self.chart.addSeries(self.series)

        # Getting the color from the QChart to use it on the QTableView
        color_name = self.series.pen().color().name()
        self.model.color = f"{color_name}"

    def add_axis(self):
        # Setting X-axis
        self.axis_x = QDateTimeAxis()

        # Convert float values to QDateTime objects
        x_min_datetime = QDateTime.fromMSecsSinceEpoch(int(self.x_min))
        x_max_datetime = QDateTime.fromMSecsSinceEpoch(int(self.x_max))

        self.axis_x.setRange(x_min_datetime, x_max_datetime)
        self.axis_x.setTickCount(20)
        self.axis_x.setFormat("ss .zzz")
        self.axis_x.setTitleText("Time")
        self.chart.addAxis(self.axis_x, Qt.AlignBottom)
        self.series.attachAxis(self.axis_x)

        # Setting Y-axis
        self.axis_y = QValueAxis()
        self.axis_y.setTickCount(20)
        self.axis_y.setRange(self.y_min, self.y_max)
        self.axis_y.setLabelFormat("%.2f")
        self.axis_y.setTitleText("Values")
        self.chart.addAxis(self.axis_y, Qt.AlignLeft)
        self.series.attachAxis(self.axis_y)
