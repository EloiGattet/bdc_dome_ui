import tkinter as tk
from PIL import Image, ImageTk
import os

ICONS_PATH = os.path.join(os.path.dirname(__file__), "icons")
ACQ_PATH = os.path.join(os.path.dirname(__file__), "acquisitions")

class ProjectsWindow(tk.Toplevel):
    def __init__(self, master=None, windowed=False):
        super().__init__(master)
        self.title("Voir les acquisitions")
        self.configure(bg="#212121")
        if windowed:
            self.geometry("800x480")
        else:
            self.attributes('-fullscreen', True)
        self._load_icons()
        self._build_ui()

    def _load_icons(self):
        self.icon_retour = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "IconeRetour.png")).resize((60, 60)))
        self.icon_export = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "download.png")).resize((50, 50)))
        self.icon_trash = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "corbeille.png")).resize((50, 50)))

    def _build_ui(self):
        # Titre en haut
        label_title = tk.Label(self, text="Acquisitions", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 28, "bold"))
        label_title.place(relx=0.5, y=30, anchor="n")

        # Bouton retour en haut à gauche
        btn_retour = tk.Button(self, image=self.icon_retour, bg="#212121", activebackground="#212121", borderwidth=0, relief="flat", command=self.destroy, cursor="tcross", highlightthickness=0)
        btn_retour.place(x=10, y=10, anchor="nw")

        # Frame central pour la liste
        frame = tk.Frame(self, bg="#212121")
        frame.place(relx=0.5, rely=0.5, anchor="center")

        # Listbox des acquisitions
        if not os.path.exists(ACQ_PATH):
            os.makedirs(ACQ_PATH)
        self.listbox = tk.Listbox(frame, width=32, height=10, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16), selectbackground="#33B5E5", selectforeground="#212121", borderwidth=0, highlightthickness=1, relief="flat")
        self.listbox.pack(padx=20, pady=20)
        self._populate_listbox()

        # Boutons en bas
        btn_export = tk.Button(self, text="Exporter USB", image=self.icon_export, compound="left", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"), relief="flat", borderwidth=0, activebackground="#33B5E5", activeforeground="#FFF3AE", cursor="tcross")
        btn_export.place(relx=0.3, rely=0.92, anchor="s")
        btn_trash = tk.Button(self, text="Supprimer", image=self.icon_trash, compound="left", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"), relief="flat", borderwidth=0, activebackground="#33B5E5", activeforeground="#FFF3AE", cursor="tcross")
        btn_trash.place(relx=0.7, rely=0.92, anchor="s")

    def _populate_listbox(self):
        self.listbox.delete(0, tk.END)
        dossiers = [d for d in os.listdir(ACQ_PATH) if os.path.isdir(os.path.join(ACQ_PATH, d))]
        for d in sorted(dossiers):
            self.listbox.insert(tk.END, d) 