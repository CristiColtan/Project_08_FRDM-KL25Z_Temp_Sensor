import numpy as np
from PySide6.QtWidgets import QMainWindow, QVBoxLayout, QHBoxLayout, QWidget, QGroupBox, QLabel, QPushButton, QLineEdit, QTextEdit
from PySide6.QtGui import QFont, QTextCursor
from PySide6.QtCore import Qt, QTimer
import pyqtgraph as pg
import serial
import threading

class MainWindow(QMainWindow):
    promotie: str = "2024-2025"
    team: list[str] = [
        "Coltan Cristian",
        "Guceanu Bianca",
    ]
    lock = threading.Lock()
    state=1

    def __init__(self):
        super().__init__()
        voltage_values = np.arange(0, 5, 0.1)
        self.t1_voltage = 2
        self.t2_voltage = 2.2
        self.t1 = np.where(voltage_values == self.t1_voltage)[0][0] + 1
        self.t2 = np.where(voltage_values == self.t2_voltage)[0][0] + 1

        #self.ser = serial.Serial(port="COM9", baudrate=14400, timeout=1)
        try:
            self.ser = serial.Serial(port="COM3", baudrate=14400, timeout=1)
        except serial.SerialException as e:
            print(f"Error opening serial port: {e}")
            self.ser = None
        self.current_line = ""
        self.sensor_data = []

        self.read_thread = threading.Thread(target=self.read_from_sensor)
        self.read_thread.start()

        primary_layout = QVBoxLayout()
        secondary_layout = QHBoxLayout()
        tertiary_layout = QVBoxLayout()

        team_box = QGroupBox("Membrii echipei")
        bold_font = QFont()
        bold_font.setBold(True)
        team_box.setFont(bold_font)

        first_member = QLabel(f"Membru 1: {self.team[0]}")
        second_member = QLabel(f"Membru 2: {self.team[1]}")
        team_box_layout = QVBoxLayout()
        team_box_layout.addWidget(first_member, 1)
        team_box_layout.addWidget(second_member, 1)
        team_box.setLayout(team_box_layout)

        control_panel_box = QGroupBox("Actions")
        control_panel_box.setFont(bold_font)

        button1 = QPushButton("Change direction for leds")
        button1.clicked.connect(self.change_led_direction)
        control_panel_box_layout = QVBoxLayout()
        control_panel_box_layout.setSpacing(5)
        control_panel_box_layout.addWidget(button1, 1)
        control_panel_box.setLayout(control_panel_box_layout)

        tertiary_layout.addWidget(team_box, 1)
        tertiary_layout.addWidget(control_panel_box, 1)

        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setLabel('left', 'Voltage (V)')
        self.plot_widget.setLabel('bottom', 'Time (seconds)')
        self.plot_widget.setYRange(0, 5)

        self.regions = [
            pg.LinearRegionItem(values=(0, self.t1_voltage), orientation=pg.LinearRegionItem.Horizontal, brush=pg.mkBrush(0, 255, 0, alpha=50)),  # verde
            pg.LinearRegionItem(values=(self.t1_voltage, self.t2_voltage), orientation=pg.LinearRegionItem.Horizontal, brush=pg.mkBrush(255, 255, 0, alpha=50)),  # galben
            pg.LinearRegionItem(values=(self.t2_voltage, 5), orientation=pg.LinearRegionItem.Horizontal, brush=pg.mkBrush(255, 0, 0, alpha=50)),  # rosu
        ]

        for region in self.regions:
            self.plot_widget.addItem(region)

        secondary_layout.addWidget(self.plot_widget, 3)
        secondary_layout.addLayout(tertiary_layout, 1)

        primary_layout.addLayout(secondary_layout, 4)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)

        debug_box = QGroupBox("Debug")
        debug_box_layout = QVBoxLayout()
        debug_box_layout.addWidget(self.text_edit, 1)
        debug_box.setLayout(debug_box_layout)

        primary_layout.addWidget(debug_box, 1)

        widget = QWidget()
        widget.setLayout(primary_layout)
        self.showMaximized()
        self.setCentralWidget(widget)

        self.timer_read_sensor = QTimer(self)
        self.timer_read_sensor.timeout.connect(self.read_from_sensor)
        self.timer_read_sensor.start(500)

        self.timer_update_plot = QTimer(self)
        self.timer_update_plot.timeout.connect(self.update_sensor_plot)
        self.timer_update_plot.start(500)


    def read_from_sensor(self):
        try:
            data = self.ser.read().decode('utf-8')

            if data == '\n':
                values = self.current_line.split('\r')
                with self.lock:
                    for value_str in values:
                        try:
                            value = float(value_str)
                            self.sensor_data.append(value)
                        except ValueError:
                            print(f"Could not convert value to float: {value_str}")

                self.text_edit.append(f"Received from sensor: {self.current_line}")

                cursor = self.text_edit.textCursor()
                cursor.movePosition(QTextCursor.End)
                self.text_edit.setTextCursor(cursor)

                self.current_line = ""

            else:
                self.current_line += data

        except serial.SerialException:
            self.text_edit.append("Error reading from serial port.")

    # def read_from_sensor(self):
    #     if self.ser:
    #         while True:
    #             with self.lock:
    #                 if self.ser.in_waiting > 0:
    #                     try:
    #                         data = self.ser.read().decode('utf-8')
    #                         self.current_line += data
    #                     except serial.SerialException:
    #                         self.text_edit.append("Error reading from serial port.")


    def update_sensor_plot(self):
        with self.lock:
            print("Sensor data:", self.sensor_data)
            time_values = np.arange(0, len(self.sensor_data))
            self.plot_widget.plot(time_values, self.sensor_data, pen={'color': 'k', 'width': 2})

            for region in self.regions:
                if region not in self.plot_widget.items():
                    self.plot_widget.addItem(region)
    
    def change_led_direction(self):
        if self.ser:
            try:
                if self.state == 1:
                    self.ser.write(b'1')
                    self.state = 0
                else:
                    self.ser.write(b'0')
                    self.state = 1

                self.text_edit.append("Sent command to change LED direction.")
            except serial.SerialException:
                self.text_edit.append("Error writing to serial port.")
        else:
            self.text_edit.append("Serial port not available.")
