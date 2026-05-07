
from PySide6.QtWidgets import (QGroupBox, QVBoxLayout, QFormLayout, 
                             QLabel, QFrame)
from PySide6.QtCore import Qt

class MetadataSection(QGroupBox):
    def __init__(self):
        super().__init__("Dataset Metadata")
        
        # Main layout for the metadata box
        layout = QVBoxLayout(self)
        
        # Form layout for the "Label: Value" pairs
        self.form = QFormLayout()
        self.form.setLabelAlignment(Qt.AlignRight)
        self.form.setFormAlignment(Qt.AlignLeft | Qt.AlignTop)

        # Labels to hold the dynamic data
        self.lbl_name = QLabel("---")
        self.lbl_dimension = QLabel("0")
        self.lbl_opt = QLabel("Not Found")
        self.lbl_comment = QLabel("---")
        
        # Adding some light styling to make the values pop
        value_style = "font-weight: bold; color: #3498db;"
        for lbl in [self.lbl_name, self.lbl_dimension, self.lbl_comment, self.lbl_opt]:
            lbl.setStyleSheet(value_style)

        # Add rows to the form
        self.form.addRow("Name:", self.lbl_name)
        self.form.addRow("Dimension:", self.lbl_dimension)
        self.form.addRow("Opt. Tour:", self.lbl_opt)
        self.form.addRow("Comment", self.lbl_comment)

        layout.addLayout(self.form)

    def set_data(self, name="---", dimension=0, has_opt=False, comment="---"):
        """Updates the labels with actual TSP info."""
        self.lbl_name.setText(name)
        self.lbl_dimension.setText(str(dimension))
        self.lbl_comment.setText(comment)
        
        if has_opt:
            self.lbl_opt.setText("Linked")
            self.lbl_opt.setStyleSheet("font-weight: bold; color: #27ae60;")
        else:
            self.lbl_opt.setText("Missing")
            self.lbl_opt.setStyleSheet("font-weight: bold; color: #e74c3c;")