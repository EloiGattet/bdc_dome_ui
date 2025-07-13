import tkinter as tk
from PIL import Image, ImageTk
import os
import sys
sys.path.append(os.path.dirname(__file__))
import utils

ICONS_PATH = os.path.join(os.path.dirname(__file__), "icons")

class AcquisitionWindow(tk.Toplevel):
    def __init__(self, master=None, windowed=False, mock=False):
        super().__init__(master)
        self.title("Nouvelle acquisition")
        self.configure(bg="#212121")
        self.mock = mock
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
        if self.mock:
            label_mock = tk.Label(self, text="MODE SIMULATION (MOCK)", bg="#E53935", fg="#FFF", font=("Roboto Mono", 14, "bold"))
            label_mock.place(relx=0.5, y=70, anchor="n")
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
        btn_valider = tk.Button(self, text="Valider", image=self.icon_ok, compound="left", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 18, "bold"), relief="flat", borderwidth=0, activebackground="#33B5E5", activeforeground="#FFF3AE", cursor="tcross", command=self.start_acquisition)
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
    def start_acquisition(self):
        import utils
        self.save_params()
        if self.mock:
            self._mock_acquisition()
        else:
            self._real_acquisition()
        self.destroy()

    def _real_acquisition(self):
        import os, time, subprocess
        print("[ACQ] Début de l'acquisition réelle (TODO: séquence Arduino + gphoto2)")
        # TODO: Séquence réelle (allumage LED, déclenchement photo, etc.)
        # ...
        # À la fin, récupération des photos :
        self._retrieve_photos()

    def _retrieve_photos(self):
        import os, time, glob, shutil
        from tkinter import Toplevel, Label, DoubleVar, StringVar
        from tkinter.ttk import Progressbar
        n_photos = 24
        # Fenêtre de progression
        win = Toplevel(self)
        win.title("Transfert des photos")
        win.geometry("500x200")
        win.configure(bg="#212121")
        label_info = Label(win, text=f"Transfert des photos...", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"))
        label_info.pack(pady=10)
        var_progress = DoubleVar(value=0)
        progress = Progressbar(win, variable=var_progress, maximum=n_photos, length=400)
        progress.pack(pady=10)
        var_status = StringVar()
        label_status = Label(win, textvariable=var_status, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 14))
        label_status.pack(pady=5)
        win.update()
        # Dossier cible
        mur = self.mur.get()
        col = self.colonne.get()
        lig = self.ligne.get()
        base_dir = os.path.join(os.path.dirname(__file__), "acquisitions", f"{mur}_C{col}_L{lig}")
        os.makedirs(base_dir, exist_ok=True)
        print(f"[ACQ] Téléchargement des photos dans {base_dir}")
        # Commande gphoto2 pour rapatrier les images
        try:
            cmd = ["gphoto2", "--get-all-files", "--force-overwrite", f"--filename={base_dir}/photo_%03n.jpg"]
            print(f"[ACQ] Commande : {' '.join(cmd)}")
            subprocess.run(cmd, check=True)
        except Exception as e:
            print(f"[ACQ][ERREUR] Echec du transfert gphoto2 : {e}")
        # Comptage et progressbar
        photos = sorted(glob.glob(os.path.join(base_dir, "photo_*.jpg")))
        for i, path in enumerate(photos, 1):
            var_progress.set(i)
            var_status.set(f"Photo {i} / {n_photos}")
            win.update()
            time.sleep(0.05)
        print(f"[ACQ] {len(photos)} photos transférées.")
        win.destroy()
        from tkinter import messagebox
        messagebox.showinfo("Transfert terminé", f"{len(photos)} photos copiées dans {base_dir}")

    def _mock_acquisition(self):
        import utils, os, time
        from tkinter import Toplevel, Label, DoubleVar, StringVar
        from tkinter.ttk import Progressbar
        mur = self.mur.get()
        col = self.colonne.get()
        lig = self.ligne.get()
        base_dir = os.path.join(os.path.dirname(__file__), "acquisitions", f"{mur}_C{col}_L{lig}")
        os.makedirs(base_dir, exist_ok=True)
        print(f"[MOCK] Création du dossier d'acquisition : {base_dir}")
        n_photos = 24
        start_time = time.time()
        # Fenêtre de progression
        win = Toplevel(self)
        win.title("Acquisition en cours")
        win.geometry("500x200")
        win.configure(bg="#212121")
        label_info = Label(win, text=f"Acquisition en cours...", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"))
        label_info.pack(pady=10)
        var_progress = DoubleVar(value=0)
        progress = Progressbar(win, variable=var_progress, maximum=n_photos, length=400)
        progress.pack(pady=10)
        var_status = StringVar()
        label_status = Label(win, textvariable=var_status, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 14))
        label_status.pack(pady=5)
        var_time = StringVar()
        label_time = Label(win, textvariable=var_time, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_time.pack(pady=5)
        win.update()
        for i in range(1, n_photos+1):
            photo_path = os.path.join(base_dir, f"photo_{i:02d}.jpg")
            print(f"[MOCK] Allumage LED {i} (commande mock)")
            utils.mock_arduino_command(f"LED {i}")
            print(f"[MOCK] Prise de photo {i} / {n_photos} -> {photo_path}")
            utils.mock_take_photo(photo_path)
            var_progress.set(i)
            elapsed = time.time() - start_time
            remaining = (elapsed/i)*(n_photos-i) if i>0 else 0
            var_status.set(f"Photo {i} / {n_photos}")
            var_time.set(f"Temps écoulé : {elapsed:.1f}s | Estimé restant : {remaining:.1f}s")
            win.update()
        total = time.time() - start_time
        print(f"[MOCK] Acquisition simulée terminée en {total:.1f} secondes.")
        win.destroy()
        from tkinter import messagebox
        messagebox.showinfo("Simulation terminée", f"24 photos simulées dans {base_dir}")

    def save_params(self):
        import utils
        utils.save_last_acquisition(self.mur.get(), self.colonne.get(), self.ligne.get()) 