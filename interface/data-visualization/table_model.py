from PySide6.QtCore import Qt, QAbstractTableModel, QModelIndex
from PySide6.QtGui import QColor
from datetime import datetime


class CustomTableModel(QAbstractTableModel):
    def __init__(self, data=None):
        QAbstractTableModel.__init__(self)
        self.load_data(data)

    def load_data(self, data):
        self.input_data = data
        self.input_times = data["T"].values
        self.input_Ax = data["Ax"].values
        self.input_Ay = data["Ay"].values
        self.input_Az = data["Az"].values

        self.column_count = 4
        self.row_count = len(self.input_times)

    def rowCount(self, parent=QModelIndex()):
        return self.row_count

    def columnCount(self, parent=QModelIndex()):
        return self.column_count

    def headerData(self, section, orientation, role):
        if role != Qt.DisplayRole:
            return None
        if orientation == Qt.Horizontal:
            return ("Time [msec]", "Ax", "Ay", "Az")[section]
        else:
            return f"{section}"

    def data(self, index, role=Qt.DisplayRole):
        column = index.column()
        row = index.row()

        if role == Qt.DisplayRole:
            if column == 0:
                date = self.input_times[row]
                return str(date)[:-3]
            elif column == 1:
                ax = self.input_Ax[row]
                return f"{ax:.2f}"
            elif column == 2:
                ay = self.input_Ay[row]
                return f"{ay:.2f}"
            elif column == 3:
                az = self.input_Az[row]
                return f"{az:.2f}"

        elif role == Qt.BackgroundRole:
            return QColor(Qt.white)
        elif role == Qt.TextAlignmentRole:
            return Qt.AlignRight

        return None
