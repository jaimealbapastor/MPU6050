import serial
import serial.tools.list_ports

from PySide6.QtCore import Slot
from PySide6.QtWidgets import QWidget


class PortWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.selected_port = None
        self.serial_port = None
        self.baudrate = 9600

    def serial_ports(self):
        ports = list(serial.tools.list_ports.comports())
        return [port.device for port in ports]
        # return ports

    @Slot()
    def connect_port(self, port):
        self.selected_port = port
        self.serial_port = serial.Serial(port=port, baudrate=self.baudrate, timeout=0.1)

    @Slot()
    def disconnect_port(self):
        self.selected_port = None
        msg = None
        if self.serial_port:
            try:
                self.serial_port.close()
                print(f"port {self.serial_port.name} closed")
            except Exception as e:
                print(f"Error closing serial port: {str(e)}")
            finally:
                self.serial_port = None

    def write(self, command):
        if self.serial_port:
            self.serial_port.write(bytes(command, "utf-8"))
            # command.encode("utf-8")

    def read(self) -> str:
        if self.serial_port:
            return self.serial_port.readline().decode("utf-8")

    def __del__(self):
        self.disconnect_port()
