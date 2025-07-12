import tkinter as tk
from tkinter import Toplevel
from PIL import Image, ImageTk
import os
import sys
import shutil

ICONS_PATH = os.path.join(os.path.dirname(__file__), "icons")

class MainMenu(tk.Tk):
    def __init__(self, windowed=False):
        super().__init__()
        self.title("Dôme Mercurio")
        self.configure(bg="#212121")
        self.windowed = windowed
        if windowed:
            self.geometry("800x480")
        else:
            self.attributes('-fullscreen', True)
        self._load_icons()
        self._build_ui()

    def _load_icons(self):
        self.icon_capture = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "menu_capture.png")).resize((80, 80)))
        self.icon_projects = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "menu_projets.png")).resize((80, 80)))
        self.icon_logo = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "logo_mercurio.png")).resize((90, 50)))
        self.icon_shutdown = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "IconeEteindre.png")).resize((60, 60)))
        self.icon_close = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "IconeAnnuler.png")).resize((60, 60)))

    def _build_ui(self):
        # Titre en haut
        label_title = tk.Label(self, text="Dôme Mercurio RTI", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 32, "bold"))
        label_title.place(relx=0.5, y=30, anchor="n")

        # Boutons éteindre/fermer en haut à droite
        btn_shutdown = tk.Button(self, image=self.icon_shutdown, bg="#212121", activebackground="#212121", borderwidth=0, relief="flat", command=self.shutdown, cursor="tcross", highlightthickness=0)
        btn_shutdown.place(relx=1.0, y=10, anchor="ne")
        btn_close = tk.Button(self, image=self.icon_close, bg="#212121", activebackground="#212121", borderwidth=0, relief="flat", command=self.close_app, cursor="tcross", highlightthickness=0)
        btn_close.place(relx=1.0, y=80, anchor="ne")

        # Frame central pour les boutons
        frame = tk.Frame(self, bg="#212121")
        frame.place(relx=0.5, rely=0.5, anchor="center")

        btn_style = dict(
            bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 20, "bold"),
            relief="flat", activebackground="#212121", activeforeground="#FFF3AE", cursor="tcross", compound="top", borderwidth=0, highlightthickness=0
        )
        btn_acq = tk.Button(frame, text="Capture", image=self.icon_capture, command=self.open_acquisition, **btn_style)
        btn_proj = tk.Button(frame, text="Acquisitions", image=self.icon_projects, command=self.open_projects, **btn_style)
        btn_acq.grid(row=0, column=0, padx=30, pady=20, ipady=20)
        btn_proj.grid(row=0, column=1, padx=30, pady=20, ipady=20)

        # Logo en bas à gauche
        logo = tk.Label(self, image=self.icon_logo, bg="#212121")
        logo.place(x=10, rely=1.0, anchor="sw")

        # Espace disque libre en bas à droite
        self.label_disk = tk.Label(self, text=self.get_disk_free(), bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12, "bold"))
        self.label_disk.place(relx=1.0, rely=1.0, x=-10, y=-10, anchor="se")
        self.after(10000, self.update_disk_free)  # maj auto toutes les 10s

    def get_disk_free(self):
        total, used, free = shutil.disk_usage("/")
        gb = free // (1024*1024*1024)
        return f"Espace libre : {gb} Go"

    def update_disk_free(self):
        self.label_disk.config(text=self.get_disk_free())
        self.after(10000, self.update_disk_free)

    def open_acquisition(self):
        from acquisition import AcquisitionWindow
        AcquisitionWindow(self, windowed=self.windowed)

    def open_projects(self):
        from projects import ProjectsWindow
        ProjectsWindow(self, windowed=self.windowed)

    def shutdown(self):
        self.destroy()
        #os.system("sudo poweroff")

    def close_app(self):
        self.destroy()

if __name__ == "__main__":
    windowed = ("-w" in sys.argv) or ("--windowed" in sys.argv)
    app = MainMenu(windowed=windowed)
    app.mainloop() 