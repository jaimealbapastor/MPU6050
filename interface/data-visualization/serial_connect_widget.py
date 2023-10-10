from PySide6.QtCore import Slot
from PySide6.QtWidgets import (
    QWidget,
    QVBoxLayout,
    QLabel,
    QLineEdit,
    QPushButton,
)


class SerialConnectWidget(QWidget):
    def __init__(self):
        super().__init__()

        layout = QVBoxLayout()

        self.port_label = QLabel("Serial Port:")
        self.port_line_edit = QLineEdit()
        layout.addWidget(self.port_label)
        layout.addWidget(self.port_line_edit)

        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.connect_to_serial)
        layout.addWidget(self.connect_button)

        self.setLayout(layout)

    @Slot()
    def connect_to_serial(self):
        # Get the selected serial port from the line edit
        selected_port = self.port_line_edit.text()

        # Perform serial port connection and data acquisition here
        # You should use a library like pyserial to manage serial communication
