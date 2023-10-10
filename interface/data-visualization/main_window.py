from PySide6.QtCore import Slot
from PySide6.QtGui import QAction, QKeySequence, QIcon
from PySide6.QtWidgets import QMainWindow, QStackedWidget, QMenu
from port_widget import PortWidget
from terminal_widget import TerminalWidget


class MainWindow(QMainWindow):
    def __init__(self, plot_widget):
        QMainWindow.__init__(self)
        self.setWindowTitle("Accelerometer information")

        # Create a stacked widget as the central widget
        self.central_stack = QStackedWidget()
        self.setCentralWidget(self.central_stack)

        # Widgets
        self.plot_widget = plot_widget
        self.port_widget = PortWidget()
        self.terminal_widget = TerminalWidget(self.port_widget)
        self.central_stack.addWidget(self.plot_widget)
        self.central_stack.addWidget(self.terminal_widget)

        # Menu
        self.menu = self.menuBar()
        self.view_menu = self.menu.addMenu("View")

        # View menu

        # Plot view
        show_plot_action = QAction("Plot", self)
        show_plot_action.triggered.connect(self.show_plot_widget)
        self.view_menu.addAction(show_plot_action)

        # Terminal view
        show_terminal_action = QAction("Terminal", self)
        show_terminal_action.triggered.connect(self.show_terminal_widget)
        self.view_menu.addAction(show_terminal_action)

        # Settings menu
        self.settings_menu = self.menu.addMenu("Settings")

        # Select port
        self.port_menu = QMenu("Select Port", self)
        self.settings_menu.addMenu(self.port_menu)
        self.port_menu.aboutToShow.connect(self.show_ports)

        self.ports_in_menu = []

        # Exit QAction
        exit_action = QAction("Exit", self)
        exit_action.setShortcut(QKeySequence.Quit)
        exit_action.triggered.connect(self.close)
        self.settings_menu.addAction(exit_action)

        # Status Bar
        self.status = self.statusBar()
        self.status.showMessage("Data loaded and plotted")

        # Window dimensions
        geometry = self.screen().availableGeometry()
        self.setFixedSize(geometry.width() * 0.8, geometry.height() * 0.7)

    @Slot()
    def show_plot_widget(self):
        self.central_stack.setCurrentIndex(0)  # Show the main widget
        self.plot_widget.show()
        self.terminal_widget.hide()

    @Slot()
    def show_terminal_widget(self):
        self.central_stack.setCurrentIndex(
            self.central_stack.indexOf(self.terminal_widget)
        )
        self.plot_widget.hide()
        self.terminal_widget.show()

    @Slot()
    def show_ports(self):
        for port_action in self.ports_in_menu:
            self.ports_in_menu.remove(port_action)
            port_action.deleteLater()

        ports = self.port_widget.serial_ports()

        if not ports:
            port_action = QAction("No ports found...", self)
            port_action.setDisabled(True)
            self.port_menu.addAction(port_action)
            self.ports_in_menu.append(port_action)
            return

        for port_name in ports:
            port_action = QAction(port_name, self)
            port_action.triggered.connect(lambda: self.select_port(port_action))
            port_action.setCheckable(True)

            if port_name == self.port_widget.selected_port:
                port_action.setChecked(True)

            self.status.showMessage(f"Connected to {port_name}")

            self.port_menu.addAction(port_action)
            self.ports_in_menu.append(port_action)

    @Slot()
    def select_port(self, port_action: QAction):
        if port_action.isChecked():
            port_name = port_action.text()
            self.port_widget.connect_port(port_name)
            port_action.setChecked(True)
        else:
            self.port_widget.disconnect_port()
            port_action.setChecked(False)
