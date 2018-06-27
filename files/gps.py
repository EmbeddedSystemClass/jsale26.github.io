"""
/**
  ******************************************************************************
  * @file    Project2/main.py
  * @author  Joshua Salecich - 43981722
  * @date    01/06/2018
  * @brief   Project 2 GUI Demonstration
  *
  ******************************************************************************
  *
  */
"""

from tkinter import *
import threading
from threading import Lock
import serial
from multiprocessing import Queue
import time

# PARAMETERS
WINDOW_SIZE = 600  # pixels
NUM_CELLS = 201
GRID_LINE_WIDTH = 1  # pixels
SYMBOL_WIDTH = WINDOW_SIZE / NUM_CELLS  # pixels
PEN_SIZE = 2
VELOCITY = 1 / 50
X_OFFSET = 0.2 
Y_OFFSET = 1.25 

# 0-1 : size of a symbol relative to it's cell
SYMBOL_SIZE = 0.5

# COLOURS
GRID_COLOUR = 'light grey'
BORDER_COLOUR = 'black'
DEFAULT_PEN_COLOUR = 'blue'
RAISED_PEN_COLOUR = 'red'
BG_COLOR = 'white'

# OTHER
CELL_SIZE = WINDOW_SIZE / NUM_CELLS

# G CODE SCALING
INCHES = 25.4
MM = 1

# Synchronisation Mechanism
payload_queue = Queue()
read_mutex = Lock()


"""
Plotter task. Controls everything.
"""
class Plotter(Tk):

    """
    Initialise the canvas.
    """
    def __init__(self):
        Tk.__init__(self)

        self.current_coord = 0
        self.prev_rect = 0
        self.depth_change = 0

        self.current_x = 0
        self.future_x = 0

        self.current_y = 0
        self.future_y = 0

        self.current_z = 0
        self.future_z = 0

        self.gcode_scale = MM
        self.home_flag = 0

        self.canvas = Canvas(
            height=WINDOW_SIZE, width=WINDOW_SIZE,
            bg=BG_COLOR, highlightthickness=GRID_LINE_WIDTH, highlightbackground=BORDER_COLOUR)

        self.coord_label = Label(text="Coordinate: XYZ00000000")

        self.clear_button = Button(text="Clear Frame", command=self.new_board)

        self.gcode_label = Label(text="G-Code: ")

        self.gcode_entry = Entry()

        self.gcode_button = Button(text="Enter", command=self.handle_gcode)

        self.port_label = Label(text="Serial Port:")

        buffer = StringVar(self)
        
        self.port_menu = OptionMenu(self, buffer, "one", "two")

        self.complete_canvas()

        self.new_board()

    """
    Fill the canvas.
    """
    def complete_canvas(self):
        
        self.title("Graphical Plotting Simulator")

        self.port_label.grid(row = 0, column = 0)

        self.port_menu.grid(row = 1, column = 0)
        
        self.coord_label.grid(row = 0, column = 1) 

        self.clear_button.grid(row = 1, column = 1) 
        
        self.gcode_label.grid(row = 0, column = 2)
        
        self.gcode_entry.grid(row = 1, column = 2)

        self.gcode_button.grid(row = 1, column = 3)

        self.canvas.grid(row = 2, column = 1) 

    """
    Clear the canvas.
    """
    def new_board(self):
        
        """
        Clears canvas and game board memory, draws a new board on the canvas
        """
        # delete all objects
        self.canvas.delete('all')

        # make grid
        for i in range(0, NUM_CELLS):
            # vertical
            self.canvas.create_line(
                CELL_SIZE * i, 0,
                CELL_SIZE * i, WINDOW_SIZE,
                width=GRID_LINE_WIDTH, fill=GRID_COLOUR)
            # horizontal
            self.canvas.create_line(
                0, CELL_SIZE * i,
                WINDOW_SIZE, CELL_SIZE * i,
                width=GRID_LINE_WIDTH, fill=GRID_COLOUR)

        self.fill_grid(self.current_x - X_OFFSET,
                      (NUM_CELLS - Y_OFFSET) - self.current_y)

    """
    G code handler.
    """
    def handle_gcode(self):

        recv_gcode = self.gcode_entry.get()
        recv_gcode = recv_gcode.split("G")
        
        x = -1
        y = -1
        z = -1
        
        for index in range(1, len(recv_gcode)):
            
            command = recv_gcode[index].split(" ")
            code_type = int(command[0])

            if (code_type == 20):
                
                self.gcode_scale = INCHES
                continue
            
            elif (code_type == 21):

                self.gcode_scale = MM
                continue
            
            elif (code_type == 28):

                self.home_flag = 1

                for j in range(0, len(command)):
                    
                    if (command[j][0] == 'X'):
                        x = int(command[j][1:]) * self.gcode_scale

                    elif (command[j][0] == 'Y'):
                        y = int(command[j][1:]) * self.gcode_scale

                    elif (command[j][0] == 'Z'):
                        z = int(command[j][1:]) * self.gcode_scale                       

            elif (code_type == 90):
                
                for j in range(0, len(command)):
                    
                    if (command[j][0] == 'X'):
                        x = int(command[j][1:]) * self.gcode_scale

                    elif (command[j][0] == 'Y'):
                        y = int(command[j][1:]) * self.gcode_scale

                    elif (command[j][0] == 'Z'):
                        z = int(command[j][1:]) * self.gcode_scale 
                
            elif (code_type == 91):

                for j in range(0, len(command)):
                    
                    if (command[j][0] == 'X'):
                        x = int((self.current_x / self.gcode_scale +
                            int(command[j][1:]) * self.gcode_scale))

                    elif (command[j][0] == 'Y'):
                        y = int((self.current_y / self.gcode_scale +
                            int(command[j][1:]) * self.gcode_scale))
                        
                    elif (command[j][0] == 'Z'):
                        z = int((self.current_z / self.gcode_scale +
                            int(command[j][1:]) * self.gcode_scale))

            if x == -1:
                x = self.current_x
                
            if (x < 10):
                x = "00" + str(x)
            elif (x > 10 and x < 100):
                x = "0" + str(x)
            elif (x > 100):
                x = str(x)

            if y == -1:
                y = self.current_y
                
            if (y < 10):
                y = "00" + str(y)
            elif (y > 10 and y < 100):
                y = "0" + str(y)
            elif (y > 100):
                y = str(y)


            if z == -1:
                z = self.current_z
                
            if (z < 10):
                z = "0" + str(z)
            elif (z > 10):
                z = str(z)

            self.recv_payload = "XYZ" + str(x) + str(y) + str(z)
            self.manipulate_packet()

    """
    Receive from queue.
    """
    def read_queue(self):

        if payload_queue.empty():
            return

        self.recv_payload = payload_queue.get()

        self.manipulate_packet()

    """
    String manipulate received packet.
    """
    def manipulate_packet(self):
        
        if self.recv_payload[:3] == "XYZ":
            
            self.coord_label.configure(
                        text="Coordinate: " + self.recv_payload)

            self.future_x = int(self.recv_payload[3:6])
            self.future_y = int(self.recv_payload[6:9])
            self.future_z = int(self.recv_payload[9:11])
            self.make_line()

            if self.home_flag == 1:
                self.home_flag = 0

                self.coord_label.configure(
                        text="Coordinate: XYZ00000000")
                    
                self.future_x = 0
                self.future_y = 0
                self.future_z = 0
                self.make_line()
            
            
    """
    Draw line, x first, y second.
    """
    def make_line(self):

        delta_x = self.future_x - self.current_x 

        for i in range(0, abs(delta_x)):

            if delta_x > 0:
                sign = 1
            else:
                sign = -1

            self.fill_grid(self.current_x + i * sign - X_OFFSET,
                           (NUM_CELLS - Y_OFFSET) - self.current_y)
            refresh_gui(self)
            time.sleep(VELOCITY)

        self.current_x = self.future_x

        delta_y = self.future_y - self.current_y

        for j in range(0, abs(delta_y)):

            if delta_y > 0:
                sign = 1
            else:
                sign = -1

            self.fill_grid(self.current_x - X_OFFSET,
                           (NUM_CELLS - Y_OFFSET) - self.current_y - sign * j)
            refresh_gui(self)
            time.sleep(VELOCITY)

        self.current_y = self.future_y

        if not self.current_z == self.future_z:
            if self.current_z == 0:
                self.depth_change = 1  # going down
            else:
                self.depth_change = 2  # going up
        else:
            self.depth_change = 0

        self.current_z = self.future_z

        self.fill_grid(self.current_x - X_OFFSET,
                       (NUM_CELLS - Y_OFFSET) - self.current_y)

        refresh_gui(self)

    """
    Fill grid. Used to simulate lines.
    """
    def fill_grid(self, grid_x, grid_y):

        """
        draw the X symbol at x, y in the grid
        """

        x = self.gtpix(grid_x)
        y = self.gtpix(grid_y)
        
        if self.current_z == 0:
            pen_colour = RAISED_PEN_COLOUR
        else:
            pen_colour = DEFAULT_PEN_COLOUR

        if (self.depth_change == 1 or
            (self.depth_change == 0 and self.current_z == 0)):
            self.canvas.delete(self.prev_rect)

        self.depth_change = 0

        self.prev_rect = self.canvas.create_rectangle(x, y, x + PEN_SIZE,
                                     y + PEN_SIZE, fill=pen_colour,
                                     outline=pen_colour)

    """
    Convert grid coordinate to pixels so handling pixels is simple.
    """
    def gtpix(self, grid_coord):

        pixel_coord = grid_coord * CELL_SIZE + CELL_SIZE / 2
        
        return pixel_coord

"""
Receive payload from terminal through a thread. Send back to main class via
queue.
"""
class ProcessPayload(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        
        while True:
            
            read_mutex.acquire()
            
            read_buffer = serial_handler()
            read_buffer = read_buffer.decode()
            
            read_mutex.release()
            
"""
Receive from terminal.
"""
def serial_handler():
    port = serial.Serial("/dev/tty.usbmodem1421", baudrate=115200, timeout=None)
    return port.read(350)

"""
Update Gui.
"""
def refresh_gui(root):
    root.update_idletasks()
    root.update()


def main():

    receive_payload = ProcessPayload()
    receive_payload.start()
    root = Plotter()

    while True:

        root.read_queue()
        refresh_gui(root)

main()
