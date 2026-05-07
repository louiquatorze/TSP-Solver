import os
from PySide6.QtWidgets import QWidget, QVBoxLayout, QLabel, QPushButton, QTabWidget, QFileSystemModel, QListView
from PySide6.QtCore import Signal, QDir

class SelectTab(QWidget):
    fileSelected = Signal(str)

    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout(self)
        
        # External Upload
        layout.addWidget(QLabel("<b>External File</b>"))
        self.btn_upload = QPushButton("Upload from Computer...")
        self.btn_upload.clicked.connect(self._browse_external)
        layout.addWidget(self.btn_upload)
        
        layout.addSpacing(10)

        # Integrated Browsers
        layout.addWidget(QLabel("<b>Project Datasets</b>"))
        self.repo_tabs = QTabWidget()
        self.repo_tabs.addTab(self._create_file_browser("data/TSPLIB/tsp"), "TSPLIB")
        self.repo_tabs.addTab(self._create_file_browser("data/own/tsp"), "Own")
        
        layout.addWidget(self.repo_tabs)

    def _create_file_browser(self, relative_path):
        abs_path = os.path.abspath(relative_path)
        os.makedirs(abs_path, exist_ok=True)

        model = QFileSystemModel()
        model.setRootPath(abs_path)
        model.setNameFilters(["*.tsp", "*.csv", "*.txt"])
        model.setNameFilterDisables(False)

        view = QListView()
        view.setModel(model)
        view.setRootIndex(model.index(abs_path))
        # Important: Store model on the view to prevent garbage collection
        view.model_ref = model 
        view.clicked.connect(lambda idx: self._on_file_clicked(model, idx))
        return view

    def _on_file_clicked(self, model, index):
        path = model.filePath(index)
        if not QDir(path).exists():
            self.fileSelected.emit(path)

    def _browse_external(self):
        from PySide6.QtWidgets import QFileDialog
        path, _ = QFileDialog.getOpenFileName(self, "Select TSP Data")
        if path:
            self.fileSelected.emit(path)    