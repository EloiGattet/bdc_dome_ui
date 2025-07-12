import tkinter as tk
from PIL import Image, ImageTk
import os
import sys
sys.path.append(os.path.dirname(__file__))
import utils

ICONS_PATH = os.path.join(os.path.dirname(__file__), "icons")

class AcquisitionWindow(tk.Toplevel):
    def __init__(self, master=None, windowed=False):
        super().__init__(master)
        self.title("Nouvelle acquisition")
        self.configure(bg="#212121")
        if windowed:
            self.geometry("800x480")
        else:
            self.attributes('-fullscreen', True)
        self._load_icons()
        self._init_state()
        self._build_ui()

    def _load_icons(self):
        self.icon_retour = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "IconeRetour.png")).resize((60, 60)))
        self.icon_left = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "left_triangle.png")).resize((40, 40)))
        self.icon_right = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "right_triangle.png")).resize((40, 40)))
        self.icon_ok = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "Icon_On_Rapide.png")).resize((60, 60)))

    def _init_state(self):
        last = utils.load_last_acquisition()
        self.mur = tk.StringVar(value=last["mur"] if last and "mur" in last else "Nord")
        self.colonne = tk.IntVar(value=last["colonne"] if last and "colonne" in last else 1)
        self.ligne = tk.IntVar(value=last["ligne"] if last and "ligne" in last else 1)

    def _build_ui(self):
        label_title = tk.Label(self, text="Nouvelle acquisition", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 28, "bold"))
        label_title.place(relx=0.5, y=30, anchor="n")
        btn_retour = tk.Button(self, image=self.icon_retour, bg="#212121", activebackground="#212121", borderwidth=0, relief="flat", command=self.destroy, cursor="tcross", highlightthickness=0)
        btn_retour.place(x=10, y=10, anchor="nw")
        frame = tk.Frame(self, bg="#212121")
        frame.place(relx=0.5, rely=0.5, anchor="center")
        btn_nord = tk.Radiobutton(frame, text="Nord", variable=self.mur, value="Nord", bg="#212121", fg="#FFF3AE", selectcolor="#33B5E5", font=("Roboto Mono", 18, "bold"), indicatoron=0, width=8, relief="flat", activebackground="#33B5E5", activeforeground="#FFF3AE")
        btn_sud = tk.Radiobutton(frame, text="Sud", variable=self.mur, value="Sud", bg="#212121", fg="#FFF3AE", selectcolor="#33B5E5", font=("Roboto Mono", 18, "bold"), indicatoron=0, width=8, relief="flat", activebackground="#33B5E5", activeforeground="#FFF3AE")
        btn_nord.grid(row=0, column=0, padx=10, pady=10)
        btn_sud.grid(row=0, column=1, padx=10, pady=10)
        lbl_col = tk.Label(frame, text="Colonne", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"))
        btn_col_left = tk.Button(frame, image=self.icon_left, bg="#212121", borderwidth=0, relief="flat", command=self.decrement_colonne, cursor="tcross", activebackground="#33B5E5")
        ent_col = tk.Label(frame, textvariable=self.colonne, width=4, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 18, "bold"))
        btn_col_right = tk.Button(frame, image=self.icon_right, bg="#212121", borderwidth=0, relief="flat", command=self.increment_colonne, cursor="tcross", activebackground="#33B5E5")
        lbl_col.grid(row=1, column=0, pady=20, columnspan=2)
        btn_col_left.grid(row=2, column=0, sticky="e", padx=(0,5))
        ent_col.grid(row=2, column=1)
        btn_col_right.grid(row=2, column=2, sticky="w", padx=(5,0))
        lbl_lig = tk.Label(frame, text="Ligne", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"))
        btn_lig_left = tk.Button(frame, image=self.icon_left, bg="#212121", borderwidth=0, relief="flat", command=self.decrement_ligne, cursor="tcross", activebackground="#33B5E5")
        ent_lig = tk.Label(frame, textvariable=self.ligne, width=4, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 18, "bold"))
        btn_lig_right = tk.Button(frame, image=self.icon_right, bg="#212121", borderwidth=0, relief="flat", command=self.increment_ligne, cursor="tcross", activebackground="#33B5E5")
        lbl_lig.grid(row=3, column=0, pady=20, columnspan=2)
        btn_lig_left.grid(row=4, column=0, sticky="e", padx=(0,5))
        ent_lig.grid(row=4, column=1)
        btn_lig_right.grid(row=4, column=2, sticky="w", padx=(5,0))
        btn_valider = tk.Button(self, text="Valider", image=self.icon_ok, compound="left", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 18, "bold"), relief="flat", borderwidth=0, activebackground="#33B5E5", activeforeground="#FFF3AE", cursor="tcross", command=self.valider)
        btn_valider.place(relx=0.5, rely=0.92, anchor="s")

    def decrement_colonne(self):
        if self.colonne.get() > 1:
            self.colonne.set(self.colonne.get() - 1)
    def increment_colonne(self):
        self.colonne.set(self.colonne.get() + 1)
    def decrement_ligne(self):
        if self.ligne.get() > 1:
            self.ligne.set(self.ligne.get() - 1)
    def increment_ligne(self):
        self.ligne.set(self.ligne.get() + 1)
    def valider(self):
        import utils
        utils.save_last_acquisition(self.mur.get(), self.colonne.get(), self.ligne.get())
        self.destroy() 