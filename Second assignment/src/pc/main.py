from tkinter import ttk
import tkinter as tk
import serial
import sys
import glob

def update_serial_data():
    """Updates the data from the serial"""
    try:
        if arduino.in_waiting > 0:
            msg = arduino.read_until(b';')
            print(msg.decode)
            data = msg.decode().strip().split(":")
            print(data[0])
            if (data[0] == "T"):
                temperature.set(data[1].split(";")[0])
            elif (data[0] == "S"):
                state.set(data[1].split(";")[0])
            elif (data[0] == "N"):
                nOfWashes.set(data[1].split(";")[0])
            print(msg)
            if state.get() == "MAINTENANCE":
                show_maintenance()
            else:
                hide_maintenance()
    except IndexError:
        pass
    root.after(10, update_serial_data)

def show_maintenance():
    """Shows the maintenance button and label"""
    errorLabel.grid(column=1, row=3, sticky=(tk.W, tk.E))
    doneButton.grid(column=1, row=4, sticky=tk.W)
    root.bind("<Return>", maintenance_done)

def hide_maintenance():
    """Hides the maintenance button and label"""
    if doneButton.grid_info() and errorLabel.grid_info():
        doneButton.grid_forget()
        errorLabel.grid_forget()


def maintenance_done():
    """Notifies the arduino that the maintenance is done"""
    hide_maintenance()
    while arduino.in_waiting:
        continue
    arduino.write("SOLVED".encode())

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

ports=serial_ports()

arduino = serial.Serial(port=ports[0], baudrate=9600)


root = tk.Tk()
root.title("PC Control Dashboard")

mainframe = ttk.Frame(root, padding="3 3 12 12")
mainframe.grid(column=0, row=0, sticky=(tk.N, tk.W, tk.E, tk.S))
root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)




errorLabel = ttk.Label(mainframe, text="maintenance required")
doneButton = ttk.Button(mainframe, text="maintenance Done", command=maintenance_done)


nOfWashes = tk.StringVar()
nOfWashes.set(r"N\A")
ttk.Label(mainframe, textvariable=nOfWashes).grid(column=1, row=0, sticky=(tk.W, tk.E))
state = tk.StringVar()
state.set(r"N\A")
ttk.Label(mainframe, textvariable=state).grid(column=1, row=1, sticky=(tk.W, tk.E))
temperature = tk.StringVar()
temperature.set(r"N\A")
ttk.Label(mainframe, textvariable=temperature).grid(column=1, row=2, sticky=(tk.W, tk.E))

ttk.Label(mainframe, text="Number of washes:").grid(column=0, row=0, sticky=(tk.W, tk.E))
ttk.Label(mainframe, text="Current state:").grid(column=0, row=1, sticky=(tk.W, tk.E))
ttk.Label(mainframe, text="Temperature:").grid(column=0, row=2, sticky=(tk.W, tk.E))
errorLabel.grid_remove()
doneButton.grid_remove()

root.after(10, update_serial_data)


root.mainloop()
