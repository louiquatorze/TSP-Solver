
import pyqtgraph as pg
from PySide6.QtWidgets import QCheckBox

class TSPDisplay(pg.PlotWidget):
    def __init__(self):
        super().__init__()
        
        # 1. Background & Styling
        self.setBackground('#191919')  # Very dark gray (matches QPalette Base)
        self.showGrid(x=True, y=True, alpha=0.15) # Subtle grid
        self.setAspectLocked(True)
        
        # 2. Axis Styling
        # Sets the axis lines and tick labels to a muted silver
        axis_pen = pg.mkPen(color='#888888', width=1)
        for axis in ['left', 'bottom']:
            self.getAxis(axis).setPen(axis_pen)
            self.getAxis(axis).setTextPen('#bbbbbb')
        
        # 3. Plot Items with High Contrast
        # The 'Current Best' path in Electric Cyan
        self.path_line = pg.PlotDataItem(
            pen=pg.mkPen(color='#00d4ff', width=2),
            antialias=False,     # High performance boost: skips pixel smoothing
            downsamplingMethod='subsample', # Only draws what's visible
            autoDownsample=True, # Automatically reduces points when zoomed out
            clipToView=True      # Only draws the segments currently inside the window
        )

        self.opt_path_line = pg.PlotDataItem(
            pen=pg.mkPen(color='#2ecc71', width=2),
            antialias=False,     # High performance boost: skips pixel smoothing
            downsamplingMethod='subsample', # Only draws what's visible
            autoDownsample=True, # Automatically reduces points when zoomed out
            clipToView=True      # Only draws the segments currently inside the window
        )
        
        # The Cities in "Sunset Orange" for maximum pop
        self.city_scatter = pg.ScatterPlotItem(
            pen=pg.mkPen(None), # No border around dots
            brush=pg.mkBrush('#ff9f43'),
            pxMode=True,
            size=4, 
        )

        # Button to hide/show opt path
        self.opt_path_btn = QCheckBox(parent=self)
        self.opt_path_btn.move(35, 10)
        self.opt_path_btn.setChecked(True)
        self.opt_path_btn.clicked.connect(lambda: self.opt_path_line.setVisible(self.opt_path_btn.isChecked()))
        self.opt_path_btn.setStyleSheet("""
            QCheckBox {
                font-size: 12px;
                color: #eee;
            }
            QCheckBox::indicator {
                width: 46px;
                height: 20px;
                border-radius: 10px;
                border: 1px solid #444;
            }
            QCheckBox::indicator:unchecked {
                background-color: #222;
            }
            QCheckBox::indicator:checked {
                background-color: #2ecc71;
            }
            /* The sliding "knob" */
            QCheckBox::indicator:unchecked:pressed, QCheckBox::indicator:checked:pressed {
                background-color: #555;
            }
        """)

        self.addItem(self.path_line)
        self.addItem(self.opt_path_line)
        self.addItem(self.city_scatter)

        # 4. Interaction
        self.setMenuEnabled(False) # Clean look, no right-click menu
        self.setMouseEnabled(x=True, y=True)
    
    def set_cities(self, cities):
        if cities is not None:
            self.city_scatter.setData(cities[:, 0], cities[:, 1])
        else:
            self.city_scatter.setData([])

    def set_path(self, path):
        if path is not None:
            self.path_line.setData(path[:, 0], path[:, 1])
        else:
            self.path_line.setData([])

    def set_opt_path(self, opt_path):
        if opt_path is not None:
            self.opt_path_line.setData(opt_path[:, 0], opt_path[:, 1])
        else:
            self.opt_path_line.setData([])