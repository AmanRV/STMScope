import customtkinter
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import serial
import struct

# Global values list
values = np.zeros(312)
highest_v = 0
lowest_v = 0
VPP = 0
AFE_mode = 0
amplitude = 0
x_shift = 0
y_shift = 0

# Configure serial port to recieve data
ser = serial.Serial(
    port='COM5',       #IMPORTANT!!!!! CHANGE THIS TO THE CORRESPONDING COM PORT THE STMSCOPE IS CONNECTED TO, CHECK DEVICE MANAGER
    baudrate=115200,   
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    timeout=0.1  
)

def draw_graph():
    global values  # Ensure the global values are used
    global fig
    global ax
    global canvas

    plt.close()

    #find amplitude and vpp
    VPP = round(np.max(values)-np.min(values), 5)
    amplitude = round(VPP/2, 5)

    # Create a Matplotlib figure and axis
    fig, ax = plt.subplots(figsize=(8, 4))  
    fig.patch.set_facecolor('#2B2B2B')  
    ax.set_facecolor('#2B2B2B') 

    # Set y range based on AFE mode and add shifting
    if int(AFE_mode) == 1:
        ax.set_ylim([-3.3-x_shift, 3.3-x_shift])

    if int(AFE_mode) == 0:
        ax.set_ylim([0-x_shift, 3.3-x_shift])

    # Set x range and add shifting
    ax.set_xlim([0-y_shift, 312-y_shift])
    
    # Plot the data without markers, titles, or labels
    ax.plot(values, linestyle='-', color='white') 
    ax.grid(True, color='gray', linestyle='--', linewidth=0.5) 
    ax.tick_params(axis='both', colors='white')  
    ax.xaxis.label.set_color('white')  
    ax.yaxis.label.set_color('white')  
    ax.title.set_color('white')  

    # Adjust layout to remove extra whitespace
    fig.tight_layout(pad=0.1, h_pad=0.1, w_pad=0.1, rect=[0, 0, 1, 1])

    # Create a canvas for the figure and add it to the Tkinter frame
    canvas = FigureCanvasTkAgg(fig, master=frame)
    canvas.draw()
    canvas.get_tk_widget().place(relx=0.03, rely=0.08, anchor='nw')  # Adjust position as needed

    # Refresh VPP text
    textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 20), width=340, height=20, corner_radius=13, bg_color='#2B2B2B')
    textbox.place(relx=0.753, rely=0.36, anchor="center")
    textbox.insert("0.0", "Peak to Peak Voltage:" + " " + str(VPP) + " V")  # Insert at line 0 character 0

    # Refresh amplitude text
    textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 20), width=340, height=20, corner_radius=13, bg_color='#2B2B2B')
    textbox.place(relx=0.753, rely=0.50, anchor="center")
    textbox.insert("0.0", "Amplitude:" + " " + str(amplitude/2) + " V")  # Insert at line 0 character 0

def button_event():
    global values  # Ensure the global values are used
    if not ser.is_open:
        ser.open()
    
    try:
        print("Listening for data...")
        run = True
        while run:
            if ser.in_waiting >= 624:  # Ensure the entire 624 bytes are available
                data = ser.read(624)  # Read 624 bytes
                values = np.array(struct.unpack('<312H', data)) # Unpack the data into 312 uint16_t values (little-endian format)
                values = values/(4095/3.3) # Convert to voltage values from ADC 0 to 4095 values
                run = False
    except KeyboardInterrupt:
        print("\nStopped by user")
    finally:
        ser.close()
        print("Serial port closed")

    draw_graph()

# AFE switch
def afe_switch():
    global AFE_mode
    AFE_mode = switch_var.get()
    draw_graph()

def draw_new_vals():

    # Refresh Y axis slider text
    textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 20), width=200, height=20, corner_radius=13, bg_color='#2B2B2B', fg_color='#2B2B2B')
    textbox.place(relx=0.753+0.088+0.02, rely=0.60+0.04, anchor="center")
    textbox.insert("0.0", "Scale Y Axis: " + str(round(slider2.get(), 3)))  # Insert at line 0 character 0

    # Refresh X axis slider text
    textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 20), width=200, height=20, corner_radius=13, bg_color='#2B2B2B', fg_color='#2B2B2B')
    textbox.place(relx=0.69+0.02, rely=0.60+0.04, anchor="center")
    textbox.insert("0.0", "Scale X Axis: " + str(round(slider.get(), 3)))  # Insert at line 0 character 0

#X shift slider
def slider_event(value):
    global x_shift
    x_shift = slider.get()
    draw_graph()
    draw_new_vals()

# Y shift slider
def slider2_event(value):
    global y_shift
    y_shift = slider2.get()
    draw_graph()
    draw_new_vals()

# Resets the x and y shifts, resets sliders back to 0, and refreshes graph
def reset_button():
    global x_shift
    global y_shift
    slider.set(0)
    slider2.set(0)
    x_shift = 0
    y_shift = 0
    draw_new_vals()
    draw_graph()



# Set appearance mode and color theme for CustomTkinter
customtkinter.set_appearance_mode("System") 
customtkinter.set_default_color_theme("blue")  

# Create the main app window
app = customtkinter.CTk() 
app.geometry("1500x500")
app.title('STMScope Desktop')

# Create a frame for the Matplotlib plot
frame = customtkinter.CTkFrame(master=app, width=890, height=460)
frame.grid(row=0, column=0, padx=20, pady=20, sticky="nsew")

# Create a frame for other controls
frame2 = customtkinter.CTkFrame(master=app, width=500, height=455)
frame2.place(relx=0.63, rely=0.045, anchor="nw")

# Title and measurements ---------
textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 30), width=360, height=50,  corner_radius=13, bg_color='#2B2B2B')
textbox.place(relx=0.760, rely=0.13, anchor="center")
textbox.insert("0.0", "STMScope Desktop")  # Insert at line 0 character 0

textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 20), width=340, height=20, corner_radius=13, bg_color='#2B2B2B')
textbox.place(relx=0.753, rely=0.36, anchor="center")
textbox.insert("0.0", "Peak to Peak Voltage:" + " " + str(VPP) + " V")  # Insert at line 0 character 0

textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 20), width=340, height=20, corner_radius=13, bg_color='#2B2B2B')
textbox.place(relx=0.753, rely=0.50, anchor="center")
textbox.insert("0.0", "Amplitude:" + " " + str(amplitude/2) + " V")  # Insert at line 0 character 0

# Run sweep button --------
button = customtkinter.CTkButton(app, text="Run one sweep", command=button_event)
button.place(relx=0.69, rely=0.25, anchor="center")

# AFE mode switch --------
switch_var = customtkinter.StringVar(value="on")
switch = customtkinter.CTkSwitch(app, text="AFE Mode", command=afe_switch, variable=switch_var, onvalue=1, offvalue=0, bg_color='#2B2B2B')
switch.place(relx=0.80, rely=0.25, anchor="center")


# Sliders -------

#X shift slider
slider = customtkinter.CTkSlider(app, from_=-5, to=5, command=slider_event)
slider.place(relx=0.715, rely=0.68+0.04, anchor="center")

textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 20), width=150, height=20, corner_radius=13, bg_color='#2B2B2B', fg_color='#2B2B2B')
textbox.place(relx=0.69, rely=0.60+0.04, anchor="center")
textbox.insert("0.0", "Shift X Axis: " + str(round(slider.get(), 3)))  # Insert at line 0 character 0

#Y shift slider
slider2 = customtkinter.CTkSlider(app, from_=-312, to=312, command=slider2_event)
slider2.place(relx=0.715+0.15, rely=0.68+0.04, anchor="center")

textbox = customtkinter.CTkTextbox(master=app, font=("Calibri", 20), width=150, height=20, corner_radius=13, bg_color='#2B2B2B', fg_color='#2B2B2B')
textbox.place(relx=0.753+0.088, rely=0.60+0.04, anchor="center")
textbox.insert("0.0", "Shift Y Axis: " + str(round(slider2.get(), 3)))  # Insert at line 0 character 0

# Reset shift button
reset = customtkinter.CTkButton(app, text="Reset Shifts", command=reset_button)
reset.place(relx=0.695, rely=0.83, anchor="center")

# Start app
draw_graph()
app.mainloop()
