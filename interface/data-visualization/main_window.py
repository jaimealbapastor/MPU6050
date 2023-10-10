from PySide6.QtCore import Slot
from PySide6.QtGui import QAction, QKeySequence
from PySide6.QtWidgets import QMainWindow, QDialog, QStackedWidget
from plot_widget import PlotWidget
from serial_connect_widget import SerialConnectWidget


class MainWindow(QMainWindow):
    def __init__(self, plot_widget):
        QMainWindow.__init__(self)
        self.setWindowTitle("Accelerometer information")

        # Create a stacked widget as the central widget
        self.central_stack = QStackedWidget()
        self.setCentralWidget(self.central_stack)

        # Add the plot widget to the stacked widget
        self.plot_widget = plot_widget
        self.central_stack.addWidget(self.plot_widget)

        # Menu
        self.menu = self.menuBar()
        self.file_menu = self.menu.addMenu("Options")

        # Exit QAction
        exit_action = QAction("Exit", self)
        exit_action.setShortcut(QKeySequence.Quit)
        exit_action.triggered.connect(self.close)

        # Serial Connect QAction
        self.serial_connect_widget = SerialConnectWidget()
        self.central_stack.addWidget(self.serial_connect_widget)
        self.serial_widget_index = self.central_stack.indexOf(
            self.serial_connect_widget
        )

        show_plot_action = QAction("Plot", self)
        show_plot_action.triggered.connect(self.show_plot_widget)
        self.file_menu.addAction(show_plot_action)

        show_serial_connect_action = QAction("Serial Connect", self)
        show_serial_connect_action.triggered.connect(self.show_serial_connect_widget)
        self.file_menu.addAction(show_serial_connect_action)

        self.serial_connect_widget.hide()

        # Status Bar
        self.status = self.statusBar()
        self.status.showMessage("Data loaded and plotted")

        # Window dimensions
        geometry = self.screen().availableGeometry()
        self.setFixedSize(geometry.width() * 0.8, geometry.height() * 0.7)

    @Slot()
    def show_serial_connect_widget(self):
        self.central_stack.setCurrentIndex(self.serial_widget_index)
        self.plot_widget.hide()
        self.serial_connect_widget.show()

    @Slot()
    def show_plot_widget(self):
        self.central_stack.setCurrentIndex(0)  # Show the main widget
        self.plot_widget.show()
        self.serial_connect_widget.hide()
