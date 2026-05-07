import os
from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel, QPushButton, QTabWidget, QFileSystemModel, QListView
from PySide6.QtCore import Signal, QDir

class CreateTab(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout(self)
        layout.addWidget(QLabel("TODO: Create your own TSPs"))
        layout.addStretch(1)