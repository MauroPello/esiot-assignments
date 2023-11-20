from tkinter import ttk
import tkinter as tk
import serial

def update_serial_data():
    """Updates the data from the serial"""
    try:
        if arduino.in_waiting > 0:
            msg = arduino.readline()
            data = msg.decode().strip().split(";")
            nOfWashes.set(data[0])
            state.set(data[1])
            temperature.set(data[2])
            print(msg)
            if state.get() == "maintanance":
                show_maintanance()
            else:
                hide_maintanance()
    except IndexError:
        pass
    root.after(100, update_serial_data)

def show_maintanance():
    """Shows the maintanance button and label"""
    errorLabel.grid(column=1, row=3, sticky=(tk.W, tk.E))
    doneButton.grid(column=1, row=4, sticky=tk.W)
    root.bind("<Return>", maintanance_done)

def hide_maintanance():
    """Hides the maintanance button and label"""
    if doneButton.grid_info() and errorLabel.grid_info():
        doneButton.grid_forget()
        errorLabel.grid_forget()


def maintanance_done():
    """Notifies the arduino that the maintanance is done"""
    hide_maintanance()
    while arduino.in_waiting:
        continue
    arduino.write("SOLVED".encode())



arduino = serial.Serial(port="/dev/ttyACM0", baudrate=9600)


root = tk.Tk()
root.title("PC Control Dashboard")

mainframe = ttk.Frame(root, padding="3 3 12 12")
mainframe.grid(column=0, row=0, sticky=(tk.N, tk.W, tk.E, tk.S))
root.columnconfigure(0, weight=1)
root.rowconfigure(0, weight=1)




errorLabel = ttk.Label(mainframe, text="maintanance required")
doneButton = ttk.Button(mainframe, text="Maintanance Done", command=maintanance_done)


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

root.after(100, update_serial_data)


root.mainloop()
