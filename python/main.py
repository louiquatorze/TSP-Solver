
import PySide6
import pyqtgraph as pg
import sys

from PySide6.QtGui import QPalette, QColor
from PySide6.QtCore import Qt
from src.controller import Controller

def apply_dark_theme(app):
    app.setStyle("Fusion")
    palette = QPalette()
    # The Sidebar/Window background (Darker)
    palette.setColor(QPalette.Window, QColor(20, 20, 20)) 
    palette.setColor(QPalette.WindowText, Qt.white)
    
    # The File Selection/Input area (Brighter/Mid-gray)
    palette.setColor(QPalette.Base, QColor(45, 45, 45)) 
    palette.setColor(QPalette.AlternateBase, QColor(55, 55, 55))
    
    palette.setColor(QPalette.Text, Qt.white)
    palette.setColor(QPalette.Button, QColor(60, 60, 60))
    palette.setColor(QPalette.ButtonText, Qt.white)
    palette.setColor(QPalette.Highlight, QColor(42, 130, 218))
    app.setPalette(palette)

if __name__ == "__main__":
    controller = None

    try:
        app = pg.mkQApp("TSP Solver")
        app.setStyle("Fusion")

        pg.setConfigOption('background', '#141414') 
        pg.setConfigOption('foreground', '#dcdcdc')
        pg.setConfigOptions(antialias=False)

        apply_dark_theme(app)
        controller = Controller()

        sys.exit(app.exec())
    finally:
        if controller is not None:
            controller.cleanup()