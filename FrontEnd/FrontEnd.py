# ___ Importing Modules ___
import subprocess
import numpy as np
import tkinter as tk
from tkinter import ttk, filedialog
from numpy import asarray
from PIL import Image
from copy import deepcopy
import threading
import getpass
import time
import os

# user-created modules
from settings import Settings

# ___ Functions ___
def run_snp_cpp(w, h):

    print("PYTHON>>>" + os.getcwd())

    # issue with OSError and subprocess.run()
    executable = "..\\Debug\\CPP_019_Noise_Reduction_Program_v3.exe"
    f_in = "noisy.txt"
    f_out = "filtered.txt"

    subprocess.run([executable, f_in, f_out, str(w), str(h)])

def save_as_txt(f_path):
    img = Image.open(f_path)
    img.convert("L")

    w, h = img.size

    img = asarray(img)

    # haven't resolved issue of where to save txt file (i.e current directory)
    np.savetxt("noisy.txt", img, delimiter = ' ', fmt = '%u')

    print([w, h])
    return w, h

def save_output(o_f_path):
    filtered = np.loadtxt("filtered.txt", dtype='uint8', delimiter=',')
    filtered = Image.fromarray(filtered)
    filtered.convert('L')
    filtered.save(o_f_path)


class NoiseReduction:
    def __init__(self):
        self.file_path = ""
        self.raw_data = None
        self.copy_raw_data = None
        self.save_directory = ""
        self.save_path = ""
        self.raw_data_exists = False
        self.img_w = 0
        self.img_h = 0

    def get_file(self):
        """User searches and selects image file to be analyzed, the file is then opened"""

        # opens file browsing window and obtains file path for selected image
        self.file_path = filedialog.askopenfilename(initialdir=f'C:\\Users\\{user}\\Documents',
                                                    title='Select an Image to De-Noise',
                                                    filetypes=settings.supported_files)

        # checks for successful file acquisition
        if self.file_path:
            print(f"PYTHON>>> File Opened: {self.file_path}")
            self.raw_data_exists = True

        else:
            self.raw_data_exists = False
            print("PYTHON>>> No file was opened.")

    def save_output_file(self):
        """Pop-up window which allows user to save output file"""

        def directory_select():
            """Opens window to select desired file destination"""

            self.save_directory = filedialog.askdirectory(initialdir=f'C:\\Users\\{user}\\Documents',
                                                          title='Select Destination Folder')

            folder_path["text"] = self.save_directory

        def proceed_with_save():
            """Takes user's directory and file name and saves output"""

            self.save_path = self.save_directory + "\\" + file_name.get()

            try:
                save_output(self.save_path)
                print("PYTHON>>> Output successfully saved.")

            except ValueError:
                print("PYHON>>> Unable to save output.")
                return

        # Toplevel widget
        save_config = tk.Toplevel()

        # Button for selecting directory
        dest_dir = ttk.Button(save_config, text='Select Folder', command=directory_select)
        dest_dir.grid(row=0, column=0, padx=10, pady=10, sticky="WE")

        # file name entry
        file_name = ttk.Entry(save_config)  # input for file name
        file_name.grid(row=1, column=0, padx=10, pady=10, sticky="WE")

        # proceed button
        save = ttk.Button(save_config, text='Proceed', command=proceed_with_save)  # proceed with de-noising
        save.grid(row=2, column=0, padx=10, pady=10, sticky="WE")

        # label to show path to directory
        folder_path = ttk.Label(save_config, text=self.save_directory)
        folder_path.grid(row=3, column=0, padx=10, pady=10, sticky="WE")

    def apply_denoise(self):
        """Applies selected de-noising algorithm to image."""

        # de-noising functions
        def blocking_code():
            start_time = time.time()

            # status indicator goes red to reflect system processing state
            status['background'] = 'red'
            status['text'] = 'Processing...'

            # disables buttons to avoid simultaneous de-noising
            file_input["state"] = "disabled"
            denoise_action["state"] = "disabled"

            if selected_mode == "Salt & Pepper" and settings.mode_activations[selected_mode] == 1:
                save_output = True

                run_snp_cpp(self.img_w, self.img_h)

            elif selected_mode == "Gaussian" and settings.mode_activations[selected_mode] == 1:
                # insert algorithm function
                save_output = True
                print("PYTHON>>> Gaussian")

            elif selected_mode == "Poisson" and settings.mode_activations[selected_mode] == 1:
                # insert algorithm function
                save_output = True
                print("PYTHON>>> Poisson")

            elif selected_mode == "Adam's Custom Algorithm" and settings.mode_activations[selected_mode] == 1:
                # insert algorithm function
                save_output = True
                print("PYTHON>>> Custom")

            else:
                save_output = False
                print("PYTHON>>> Invalid mode or no mode was selected.")
                status['background'] = 'white'  # default status color
                status['text'] = 'Status'  # default status text

            # total time taken to de-noise image
            elapsed = round((time.time() - start_time), 2)
            print(f"PYTHON>>> Process complete at time {elapsed}s")

            # re-enables buttons
            file_input["state"] = "normal"
            denoise_action["state"] = "normal"

            if save_output:
                # indicates de-noising has successfully completed
                status['background'] = 'green'
                status['text'] = 'Complete!'

                self.save_output_file()

        # checks for selected mode in ComboBox widget
        selected_mode = denoise_type_select.get()

        if selected_mode in settings.available_modes and self.raw_data_exists:

            # TEMPORARY
            self.img_w, self.img_h = save_as_txt(self.file_path)

            # creates new thread to execute de-noising through
            denoise_thread = threading.Thread(target=blocking_code, daemon=True)
            denoise_thread.start()

        elif not self.raw_data_exists:
            print("PYTHON>>> Cannot apply de-noise without image file...")
            return

        elif selected_mode not in settings.available_modes:
            print("PYTHON>>> Cannot apply de-noise with invalid mode")
            


# ___ User Interface Code ___
denoise = NoiseReduction()  # creates instance of NoiseReduction class
settings = Settings()

root = tk.Tk()  # main root
root.title(f"Image De-Noise v{settings.build_num}")
root.iconbitmap(default='transparent.ico')  # sets window icon (top left corner)
root.geometry("800x500")
root.minsize(settings.min_window_width, settings.min_window_height)
root.grid_columnconfigure((0, 1), weight=1, uniform='half')
root.grid_rowconfigure(3, weight=1)

# ttk style configurations
button_sty = ttk.Style()
button_sty.configure('my.TButton', font=(settings.def_font, settings.button_font_size))

# frame contains primary interactive widgets
frame1 = tk.Frame(root, highlightbackground='black', highlightthickness=1)
frame1.grid(row=1, columnspan=2, pady=10)

# Button to browse for image file name
file_input = ttk.Button(frame1, text='Browse Files', style='my.TButton', command=denoise.get_file)
file_input.grid(row=0, column=0, padx=10, pady=10)

# Drop down menu for selecting de-noise type
denoise_type_select = ttk.Combobox(frame1, state='readonly', width=25, values=settings.available_modes)
denoise_type_select.grid(row=0, column=1, padx=10, pady=10)
denoise_type_select.set("--Select De-Noise Type--")

# Button to start de-noising algorithm
denoise_action = ttk.Button(frame1, text='Start De-Noise', style='my.TButton', command=denoise.apply_denoise)
denoise_action.grid(row=0, column=2, padx=10, pady=10)

# Button for accessing user alterable settings
user_settings = ttk.Button(root, text='Settings', style='my.TButton')
user_settings.grid(row=0, column=0, padx=10, pady=10, sticky='W')

# Label to denote status of de-noise
status = ttk.Label(root, width=15, borderwidth=1, background='white', text="Status")
status.grid(row=0, column=1, padx=10, pady=10, sticky='E')
status.configure(anchor='center')

# label for before image
before_label = ttk.Label(root, text='Image Before')
before_label.grid(row=2, column=0)
before_label.config(font=("Segoe UI", 12), anchor='center')

# frame to contain before image
before_image = tk.Frame(root, highlightbackground='black', highlightthickness=1)
before_image.grid(row=3, column=0, padx=10, pady=15, sticky='nsew')

# label for after image
after_label = ttk.Label(root, text='Image After')
after_label.grid(row=2, column=1)
after_label.config(font=("Segoe UI", 12), anchor='center')

# frame to contain after image
after_image = tk.Frame(root, highlightbackground='black', highlightthickness=1)
after_image.grid(row=3, column=1, padx=10, pady=15, sticky='nsew')

# ___ Main Program Code ___
if __name__ == "__main__":
    user = getpass.getuser()
    root.mainloop()  # runs UI window

# --------- NOTES ---------------
# ** haven't added output image save successful yet
# ** front end doesn't actually save jpg version of filtered file
# ** delete temporary txt files once complete
# ** be able to use all modes with the C++ exe file
# ** clean up python code (it kinda sucks)
# ** denoise all of a sudden takes too long
#   ** too many calculations made