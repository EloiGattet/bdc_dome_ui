import tkinter as tk
from tkinter import Toplevel
from PIL import Image, ImageTk
import os
import sys
import shutil

ICONS_PATH = os.path.join(os.path.dirname(__file__), "icons")

class MainMenu(tk.Tk):
    def __init__(self, windowed=False, mock=False):
        super().__init__()
        self.title("Dôme Mercurio")
        self.configure(bg="#212121")
        self.windowed = windowed
        self.mock = mock
        if windowed:
            self.geometry("800x480")
            self.resizable(False, False)
        else:
            self.attributes('-fullscreen', True)
        self._load_icons()
        self._build_ui()

    def _load_icons(self):
        self.icon_capture = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "menu_capture.png")).resize((120, 120)))
        self.icon_projects = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "menu_projets.png")).resize((120, 120)))
        self.icon_logo = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "logo_mercurio.png")).resize((110, 60)))
        self.icon_shutdown = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "IconeEteindre.png")).resize((60, 60)))
        self.icon_close = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "IconeAnnuler.png")).resize((60, 60)))

    def _build_ui(self):
        # Titre en haut
        y_title = 40
        y_mock = y_title + 40
        label_title = tk.Label(self, text="Dôme Mercurio RTI", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 32, "bold"))
        label_title.place(relx=0.5, y=y_title, anchor="n")
        if self.mock:
            label_mock = tk.Label(self, text="MODE SIMULATION (MOCK)", bg="#E53935", fg="#FFF", font=("Roboto Mono", 16, "bold"))
            label_mock.place(relx=0.5, y=y_mock, anchor="n")
        # Boutons éteindre/fermer en haut à droite
        btn_shutdown = tk.Button(self, image=self.icon_shutdown, bg="#212121", activebackground="#212121", borderwidth=0, relief="flat", command=self.shutdown, cursor="tcross", highlightthickness=0)
        btn_shutdown.place(relx=1.0, y=10, anchor="ne")
        btn_close = tk.Button(self, image=self.icon_close, bg="#212121", activebackground="#212121", borderwidth=0, relief="flat", command=self.close_app, cursor="tcross", highlightthickness=0)
        btn_close.place(relx=1.0, y=80, anchor="ne")
        # Frame central pour les boutons
        frame = tk.Frame(self, bg="#212121")
        frame.update_idletasks()
        screen_h = 480 if self.windowed else self.winfo_screenheight()
        y_center = (screen_h // 2) + (30 if self.mock else 0)
        frame.place(relx=0.5, y=y_center, anchor="center")

        # Paramètres boutons uniformes
        BTN_W, BTN_H = 190, 190  # Largeur/hauteur stricte réduite
        ICON_SIZE = 120
        FONT_BTN = ("Roboto Mono", 22, "bold")
        FG_BTN = "#FFF3AE"
        BG_BTN = "#212121"
        BORDER_COLOR = "#FFF3AE"
        BORDER_WIDTH = 3

        def make_btn(parent, icon, text, command):
            canvas = tk.Canvas(parent, width=BTN_W, height=BTN_H, bg=BG_BTN, highlightthickness=0, bd=0)
            # Cadre rectangle
            canvas.create_rectangle(4, 4, BTN_W-4, BTN_H-4, outline=BORDER_COLOR, width=BORDER_WIDTH)
            # Image centrée
            canvas.create_image(BTN_W//2, 80, image=icon, anchor="center")
            # Texte centré sous l'image
            canvas.create_text(BTN_W//2, 165, text=text, font=FONT_BTN, fill=FG_BTN)
            # Zone cliquable
            canvas.bind("<Button-1>", lambda e: command())
            canvas.config(cursor="tcross")
            return canvas

        btn_acq = make_btn(frame, self.icon_capture, "Acquisition", self.open_acquisition)
        btn_proj = make_btn(frame, self.icon_projects, "Projets", self.open_projects)
        btn_acq.grid(row=0, column=0, padx=40, pady=20)
        btn_proj.grid(row=0, column=1, padx=40, pady=20)

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
        AcquisitionWindow(self, windowed=self.windowed, mock=self.mock)

    def open_projects(self):
        from projects import ProjectsWindow
        ProjectsWindow(self, windowed=self.windowed, mock=self.mock)

    def shutdown(self):
        self.destroy()
        #os.system("sudo poweroff")

    def close_app(self):
        self.destroy()

if __name__ == "__main__":
    if ("--help" in sys.argv) or ("-h" in sys.argv):
        print("""
Usage: python main.py [options]

Options :
  -w, --windowed    Lance l'interface en mode fenêtré (800x480)
  -m, --mock        Active le mode simulation (mock, sans périphériques)
  --help, -h        Affiche cette aide

Vous pouvez combiner --windowed et --mock pour tester sur Mac/PC.
""")
        exit(0)
    windowed = ("-w" in sys.argv) or ("--windowed" in sys.argv)
    mock = ("--mock" in sys.argv) or ("MOCK=True" in os.environ) or ("-m" in sys.argv)
    app = MainMenu(windowed=windowed, mock=mock)
    app.mainloop() 