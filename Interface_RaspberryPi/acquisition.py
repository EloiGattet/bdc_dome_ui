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
        self.windowed = windowed
        if windowed:
            self.geometry("800x480")
            self.resizable(False, False)
        else:
            self.geometry("800x480")
            self.attributes('-fullscreen', True)
            self.overrideredirect(True)
            self.resizable(False, False)
        self._load_icons()
        self._init_state()
        self._build_ui()
        self._check_existing_acquisition()

    def _load_icons(self):
        self.icon_retour = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "IconeRetour.png")).resize((60, 60)))
        self.icon_left = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "left_triangle.png")).resize((40, 40)))
        self.icon_right = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "right_triangle.png")).resize((40, 40)))
        self.icon_ok = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "camera.png")).resize((60, 60)))

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
        btn_retour.place(x=20, y=20, anchor="nw")
        frame = tk.Frame(self, bg="#212121")
        frame.place(relx=0.5, rely=0.5, anchor="center")
        # Bloc mur
        frame_mur = tk.Frame(frame, bg="#212121")
        frame_mur.grid(row=0, column=0, columnspan=3, pady=(0, 10))
        btn_nord = tk.Radiobutton(frame_mur, text="Nord", variable=self.mur, value="Nord", bg="#212121", fg="#FFF3AE", selectcolor="#33B5E5", font=("Roboto Mono", 18, "bold"), indicatoron=0, width=8, relief="flat", activebackground="#33B5E5", activeforeground="#FFF3AE")
        btn_sud = tk.Radiobutton(frame_mur, text="Sud", variable=self.mur, value="Sud", bg="#212121", fg="#FFF3AE", selectcolor="#33B5E5", font=("Roboto Mono", 18, "bold"), indicatoron=0, width=8, relief="flat", activebackground="#33B5E5", activeforeground="#FFF3AE")
        btn_nord.pack(side="left", padx=10)
        btn_sud.pack(side="left", padx=10)
        # Bloc colonne
        lbl_col = tk.Label(frame, text="Colonne", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"))
        lbl_col.grid(row=1, column=0, columnspan=3, pady=(10, 0))
        frame_col = tk.Frame(frame, bg="#212121")
        frame_col.grid(row=2, column=0, columnspan=3, pady=(0, 10))
        btn_col_left = tk.Button(frame_col, image=self.icon_left, bg="#212121", borderwidth=0, relief="flat", command=self.decrement_colonne, cursor="tcross", activebackground="#33B5E5")
        ent_col = tk.Label(frame_col, textvariable=self.colonne, width=4, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 18, "bold"))
        btn_col_right = tk.Button(frame_col, image=self.icon_right, bg="#212121", borderwidth=0, relief="flat", command=self.increment_colonne, cursor="tcross", activebackground="#33B5E5")
        btn_col_left.pack(side="left", padx=(0,5))
        ent_col.pack(side="left")
        btn_col_right.pack(side="left", padx=(5,0))
        # Bloc ligne
        lbl_lig = tk.Label(frame, text="Ligne", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"))
        lbl_lig.grid(row=3, column=0, columnspan=3, pady=(10, 0))
        frame_lig = tk.Frame(frame, bg="#212121")
        frame_lig.grid(row=4, column=0, columnspan=3, pady=(0, 10))
        btn_lig_left = tk.Button(frame_lig, image=self.icon_left, bg="#212121", borderwidth=0, relief="flat", command=self.decrement_ligne, cursor="tcross", activebackground="#33B5E5")
        ent_lig = tk.Label(frame_lig, textvariable=self.ligne, width=4, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 18, "bold"))
        btn_lig_right = tk.Button(frame_lig, image=self.icon_right, bg="#212121", borderwidth=0, relief="flat", command=self.increment_ligne, cursor="tcross", activebackground="#33B5E5")
        btn_lig_left.pack(side="left", padx=(0,5))
        ent_lig.pack(side="left")
        btn_lig_right.pack(side="left", padx=(5,0))
        # Bulle d'avertissement si acquisition existe déjà
        self.label_warning = tk.Label(self, text="", bg="#212121", fg="#E53935", font=("Roboto Mono", 13, "bold"))
        self.label_warning.place(relx=0.5, rely=1.0, y=-10, anchor="s")
        btn_valider = tk.Button(self, text="Valider", image=self.icon_ok, compound="left", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 18, "bold"), relief="flat", borderwidth=0, activebackground="#33B5E5", activeforeground="#FFF3AE", cursor="tcross", command=self.start_acquisition)
        btn_valider.place(relx=0.5, rely=0.92, anchor="s")
        # Mise à jour de l'avertissement à chaque changement
        self.mur.trace_add('write', lambda *a: self._check_existing_acquisition())
        self.colonne.trace_add('write', lambda *a: self._check_existing_acquisition())
        self.ligne.trace_add('write', lambda *a: self._check_existing_acquisition())

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
        # Calcul du dossier cible avec suffixe si besoin
        mur = self.mur.get()
        col = self.colonne.get()
        lig = self.ligne.get()
        base_dir = os.path.join(os.path.dirname(__file__), "acquisitions", f"{mur}_C{col}_L{lig}")
        final_dir = base_dir
        idx = 2
        while os.path.exists(final_dir):
            final_dir = f"{base_dir}_{idx}"
            idx += 1
        self.acq_dir = final_dir
        if self.mock:
            self._mock_acquisition(windowed=self.windowed)
        else:
            self._real_acquisition(windowed=self.windowed)
        # self.destroy() supprimé : la fenêtre ne doit se fermer qu'en cas de succès explicite

    def _show_acquisition_summary(self, base_dir, elapsed, n_photos, windowed=None):
        import os, time
        from tkinter import Toplevel, Label, Button
        if windowed is None:
            windowed = self.windowed
        summary_win = Toplevel(self)
        summary_win.title("Acquisition terminée")
        if windowed:
            summary_win.geometry("800x480")
            summary_win.resizable(False, False)
        else:
            summary_win.geometry("800x480")
            summary_win.attributes('-fullscreen', True)
            summary_win.overrideredirect(True)
            summary_win.resizable(False, False)
        summary_win.configure(bg="#212121")
        Label(summary_win, text="Acquisition terminée !", bg="#212121", fg="#33B5E5", font=("Roboto Mono", 18, "bold")).pack(pady=10)
        Label(summary_win, text=f"Dossier : {os.path.basename(base_dir)}", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 14)).pack(pady=5)
        Label(summary_win, text=f"Durée : {elapsed:.1f} s", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 14)).pack(pady=5)
        Label(summary_win, text=f"Photos : {n_photos}", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 14)).pack(pady=5)
        # Boutons
        def go_menu():
            summary_win.destroy()
            self.destroy()
            if self.master:
                self.master.deiconify()
        def next_acq():
            summary_win.destroy()
            self.ligne.set(self.ligne.get() + 1)
            self._check_existing_acquisition()
            # Affiche le nom du prochain dossier
            next_dir = os.path.join(os.path.dirname(__file__), "acquisitions", f"{self.mur.get()}_C{self.colonne.get()}_L{self.ligne.get()}")
            idx = 2
            final_dir = next_dir
            while os.path.exists(final_dir):
                final_dir = f"{next_dir}_{idx}"
                idx += 1
            Label(self, text=f"Prochaine acquisition : {os.path.basename(final_dir)}", bg="#212121", fg="#33B5E5", font=("Roboto Mono", 13, "bold")).place(relx=0.5, y=370, anchor="n")
        Button(summary_win, text="FIN", bg="#33B5E5", fg="#212121", font=("Roboto Mono", 14, "bold"), command=go_menu, width=12).pack(pady=10)
        Button(summary_win, text="ACQUISITION SUIVANTE", bg="#FFF3AE", fg="#212121", font=("Roboto Mono", 14, "bold"), command=next_acq, width=20).pack(pady=5)

    def _real_acquisition(self, windowed=None):
        import os, time, subprocess, glob, shutil
        from tkinter import Toplevel, Label, DoubleVar, StringVar
        from tkinter.ttk import Progressbar
        if windowed is None:
            windowed = self.windowed
        n_photos = 24
        base_dir = self.acq_dir
        os.makedirs(base_dir, exist_ok=True)
        win = Toplevel(self)
        win.title("Acquisition en cours")
        if windowed:
            win.geometry("800x480")
            win.resizable(False, False)
        else:
            win.geometry("800x480")
            win.attributes('-fullscreen', True)
            win.overrideredirect(True)
            win.resizable(False, False)
        win.configure(bg="#212121")
        label_info = Label(win, text="Acquisition réelle", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"))
        label_info.pack(pady=10)
        var_status = StringVar()
        label_status = Label(win, textvariable=var_status, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 14, "bold"))
        label_status.pack(pady=5)
        win.update()
        # 1/4 Vérification connexion série et appareil photo
        var_status.set("1/4 : Vérification connexion série et appareil photo...")
        bar1 = Progressbar(win, maximum=2, length=400)
        bar1.pack(pady=5)
        label_bar1 = Label(win, text="2/2", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_bar1.pack()
        win.update()
        # Vérif série (pyserial)
        try:
            # TODO: remplacer par vraie vérif pyserial
            time.sleep(0.5)
            bar1['value'] = 1
            label_bar1.config(text="1/2")
            win.update()
        except Exception as e:
            var_status.set(f"Erreur connexion série : {e}")
            label_status.config(fg="#E53935")
            bar1.pack_forget()
            label_bar1.pack_forget()
            return
        # Vérif appareil photo (gphoto2 --auto-detect)
        try:
            # TODO: remplacer par vraie vérif gphoto2
            time.sleep(0.5)
            bar1['value'] = 2
            label_bar1.config(text="2/2")
            win.update()
        except Exception as e:
            var_status.set(f"Erreur connexion appareil photo : {e}")
            label_status.config(fg="#E53935")
            bar1.pack_forget()
            label_bar1.pack_forget()
            return
        time.sleep(0.2)
        bar1.pack_forget()
        label_bar1.pack_forget()
        # 2/4 Acquisition des photos
        var_status.set("2/4 : Acquisition des photos...")
        bar2 = Progressbar(win, maximum=n_photos, length=400)
        bar2.pack(pady=5)
        label_bar2 = Label(win, text=f"0/{n_photos}", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_bar2.pack()
        win.update()
        try:
            start_time = time.time()
            for i in range(1, n_photos+1):
                # TODO: allumage LED via Arduino + attente OK
                # TODO: déclenchement photo via gphoto2
                time.sleep(0.18)
                bar2['value'] = i
                label_bar2.config(text=f"{i}/{n_photos}")
                win.update()
            elapsed = time.time() - start_time
        except Exception as e:
            var_status.set(f"Erreur acquisition photo : {e}")
            label_status.config(fg="#E53935")
            bar2.pack_forget()
            label_bar2.pack_forget()
            return
        bar2.pack_forget()
        label_bar2.pack_forget()
        # 3/4 Copie des photos
        var_status.set("3/4 : Copie des photos...")
        bar3 = Progressbar(win, maximum=100, length=400)
        bar3.pack(pady=5)
        label_bar3 = Label(win, text="0/100", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_bar3.pack()
        win.update()
        try:
            for i in range(1, 101, 10):
                time.sleep(0.05)
                bar3['value'] = i
                label_bar3.config(text=f"{i}/100")
                win.update()
            bar3['value'] = 100
            label_bar3.config(text="100/100")
            win.update()
        except Exception as e:
            var_status.set(f"Erreur copie des photos : {e}")
            label_status.config(fg="#E53935")
            bar3.pack_forget()
            label_bar3.pack_forget()
            return
        time.sleep(0.2)
        bar3.pack_forget()
        label_bar3.pack_forget()
        # 4/4 Vérification intégrité
        var_status.set("4/4 : Vérification de l'intégrité des données...")
        bar4 = Progressbar(win, maximum=100, length=400)
        bar4.pack(pady=5)
        label_bar4 = Label(win, text="0/100", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_bar4.pack()
        win.update()
        try:
            for i in range(1, 101, 20):
                time.sleep(0.05)
                bar4['value'] = i
                label_bar4.config(text=f"{i}/100")
                win.update()
            bar4['value'] = 100
            label_bar4.config(text="100/100")
            win.update()
        except Exception as e:
            var_status.set(f"Erreur vérification intégrité : {e}")
            label_status.config(fg="#E53935")
            bar4.pack_forget()
            label_bar4.pack_forget()
            return
        time.sleep(0.2)
        bar4.pack_forget()
        label_bar4.pack_forget()
        # Succès
        var_status.set("Succès : Acquisition terminée !")
        label_status.config(fg="#33B5E5")
        print(f"[ACQ] Acquisition réelle terminée dans {base_dir}")
        win.after(800, win.destroy)
        self._show_acquisition_summary(base_dir, elapsed, n_photos, windowed=windowed)

    def _retrieve_photos(self, windowed=None):
        import os, time, glob, shutil
        from tkinter import Toplevel, Label, DoubleVar, StringVar
        from tkinter.ttk import Progressbar
        if windowed is None:
            windowed = self.windowed
        n_photos = 24
        # Fenêtre de progression
        win = Toplevel(self)
        win.title("Transfert des photos")
        if windowed:
            win.geometry("800x480")
            win.resizable(False, False)
        else:
            win.geometry("800x480")
            win.attributes('-fullscreen', True)
            win.overrideredirect(True)
            win.resizable(False, False)
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

    def _check_existing_acquisition(self):
        mur = self.mur.get()
        col = self.colonne.get()
        lig = self.ligne.get()
        base_dir = os.path.join(os.path.dirname(__file__), "acquisitions", f"{mur}_C{col}_L{lig}")
        if os.path.exists(base_dir):
            self.label_warning.config(text="Une acquisition existe déjà pour ce mur/colonne/ligne. Un nouveau dossier sera créé (_2, _3, ...)")
        else:
            self.label_warning.config(text="")

    def _mock_acquisition(self, windowed=None):
        import utils, os, time
        from tkinter import Toplevel, Label, DoubleVar, StringVar
        from tkinter.ttk import Progressbar
        if windowed is None:
            windowed = self.windowed
        n_photos = 24
        base_dir = self.acq_dir
        os.makedirs(base_dir, exist_ok=True)
        win = Toplevel(self)
        win.title("Acquisition en cours")
        if windowed:
            win.geometry("800x480")
            win.resizable(False, False)
        else:
            win.geometry("800x480")
            win.attributes('-fullscreen', True)
            win.overrideredirect(True)
            win.resizable(False, False)
        win.configure(bg="#212121")
        label_info = Label(win, text="Acquisition simulée", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16, "bold"))
        label_info.pack(pady=10)
        var_status = StringVar()
        label_status = Label(win, textvariable=var_status, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 14, "bold"))
        label_status.pack(pady=5)
        win.update()
        # 1/4 Vérification connexion
        var_status.set("1/4 : Vérification connexion série et appareil photo...")
        bar1 = Progressbar(win, maximum=2, length=400)
        bar1.pack(pady=5)
        label_bar1 = Label(win, text="2/2", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_bar1.pack()
        win.update()
        print("[MOCK] Vérification connexion série Arduino...")
        time.sleep(0.5)
        bar1['value'] = 1
        label_bar1.config(text="1/2")
        win.update()
        print("[MOCK] Vérification connexion appareil photo...")
        time.sleep(0.5)
        bar1['value'] = 2
        label_bar1.config(text="2/2")
        win.update()
        time.sleep(0.2)
        bar1.pack_forget()
        label_bar1.pack_forget()
        # 2/4 Acquisition des photos
        var_status.set("2/4 : Acquisition des photos...")
        bar2 = Progressbar(win, maximum=n_photos, length=400)
        bar2.pack(pady=5)
        label_bar2 = Label(win, text=f"0/{n_photos}", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_bar2.pack()
        win.update()
        start_time = time.time()
        for i in range(1, n_photos+1):
            photo_path = os.path.join(base_dir, f"photo_{i:02d}.jpg")
            print(f"[MOCK] Allumage LED {i} (commande mock)")
            utils.mock_arduino_command(f"LED {i}")
            print(f"[MOCK] Prise de photo {i} / {n_photos} -> {photo_path}")
            utils.mock_take_photo(photo_path)
            bar2['value'] = i
            label_bar2.config(text=f"{i}/{n_photos}")
            win.update()
            time.sleep(0.12)
        elapsed = time.time() - start_time
        bar2.pack_forget()
        label_bar2.pack_forget()
        # 3/4 Copie des photos (simulation)
        var_status.set("3/4 : Copie des photos...")
        bar3 = Progressbar(win, maximum=100, length=400)
        bar3.pack(pady=5)
        label_bar3 = Label(win, text="0/100", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_bar3.pack()
        win.update()
        for i in range(1, 101, 10):
            bar3['value'] = i
            label_bar3.config(text=f"{i}/100")
            win.update()
            time.sleep(0.08)
        bar3['value'] = 100
        label_bar3.config(text="100/100")
        win.update()
        time.sleep(0.2)
        bar3.pack_forget()
        label_bar3.pack_forget()
        # 4/4 Vérification intégrité
        var_status.set("4/4 : Vérification de l'intégrité des données...")
        bar4 = Progressbar(win, maximum=100, length=400)
        bar4.pack(pady=5)
        label_bar4 = Label(win, text="0/100", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 12))
        label_bar4.pack()
        win.update()
        for i in range(1, 101, 20):
            bar4['value'] = i
            label_bar4.config(text=f"{i}/100")
            win.update()
            time.sleep(0.09)
        bar4['value'] = 100
        label_bar4.config(text="100/100")
        win.update()
        time.sleep(0.2)
        bar4.pack_forget()
        label_bar4.pack_forget()
        # Fin : succès
        var_status.set("Succès : Acquisition simulée terminée !")
        label_status.config(fg="#33B5E5")
        print(f"[MOCK] Acquisition simulée terminée dans {base_dir}")
        win.after(800, win.destroy)
        self._show_acquisition_summary(base_dir, elapsed, n_photos, windowed=windowed)

    def save_params(self):
        import utils
        utils.save_last_acquisition(self.mur.get(), self.colonne.get(), self.ligne.get()) 