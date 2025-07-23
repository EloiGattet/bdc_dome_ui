import tkinter as tk
from PIL import Image, ImageTk
import os

ICONS_PATH = os.path.join(os.path.dirname(__file__), "icons")
ACQ_PATH = os.path.join(os.path.dirname(__file__), "acquisitions")

class ProjectsWindow(tk.Toplevel):
    def __init__(self, master=None, windowed=False, mock=False):
        super().__init__(master)
        self.title("Voir les acquisitions")
        self.configure(bg="#212121")
        self.mock = mock
        if windowed:
            self.geometry("800x480")
            self.resizable(False, False)
        else:
            self.geometry("800x480")
            self.attributes('-fullscreen', True)
            self.overrideredirect(True)
            self.resizable(False, False)
        self._load_icons()
        self._build_ui()

    def _load_icons(self):
        self.icon_retour = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "IconeRetour.png")).resize((60, 60)))
        self.icon_export = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "download.png")).resize((50, 50)))
        self.icon_trash = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "corbeille.png")).resize((50, 50)))
        self.icon_select_all = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "Select_all.png")).resize((40, 40)))
        self.icon_select_none = ImageTk.PhotoImage(Image.open(os.path.join(ICONS_PATH, "Select_none.png")).resize((40, 40)))

    def _build_ui(self):
        # Titre en haut
        label_title = tk.Label(self, text="Acquisitions", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 28, "bold"))
        label_title.place(relx=0.5, y=30, anchor="n")
        if self.mock:
            label_mock = tk.Label(self, text="MODE SIMULATION (MOCK)", bg="#E53935", fg="#FFF", font=("Roboto Mono", 14, "bold"))
            label_mock.place(relx=0.5, y=70, anchor="n")
        # Bouton retour en haut à gauche
        btn_retour = tk.Button(self, image=self.icon_retour, bg="#212121", activebackground="#212121", borderwidth=0, relief="flat", command=self.destroy, cursor="tcross", highlightthickness=0)
        btn_retour.place(x=10, y=10, anchor="nw")
        # Frame central pour la liste
        frame = tk.Frame(self, bg="#212121")
        frame.place(relx=0.5, rely=0.5, anchor="center")
        # Sélection multiple : boutons select all/none avec cadre jaune
        frame_sel_all = tk.Frame(frame, bg="#212121", highlightbackground="#FFF3AE", highlightthickness=3, bd=0)
        frame_sel_all.grid(row=0, column=0, padx=5, pady=(0,5))
        btn_select_all = tk.Button(frame_sel_all, image=self.icon_select_all, bg="#212121", borderwidth=0, relief="flat", command=self._select_all, cursor="tcross", activebackground="#33B5E5")
        btn_select_all.pack()
        frame_sel_none = tk.Frame(frame, bg="#212121", highlightbackground="#FFF3AE", highlightthickness=3, bd=0)
        frame_sel_none.grid(row=0, column=1, padx=5, pady=(0,5))
        btn_select_none = tk.Button(frame_sel_none, image=self.icon_select_none, bg="#212121", borderwidth=0, relief="flat", command=self._select_none, cursor="tcross", activebackground="#33B5E5")
        btn_select_none.pack()
        # Listbox des acquisitions
        if not os.path.exists(ACQ_PATH):
            os.makedirs(ACQ_PATH)
        self.listbox = tk.Listbox(frame, width=32, height=10, bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 16), selectbackground="#33B5E5", selectforeground="#212121", borderwidth=0, highlightthickness=1, relief="flat", selectmode=tk.EXTENDED)
        self.listbox.grid(row=1, column=0, columnspan=2, padx=20, pady=10)
        self._populate_listbox()
        from tkinter import ttk
        style = ttk.Style()
        try:
            style.theme_use('alt')
        except:
            try:
                style.theme_use('clam')
            except:
                pass
        style.configure('Dark.TButton',
                        background='#212121',
                        foreground='#FFF3AE',
                        font=('Roboto Mono', 16, 'bold'),
                        borderwidth=0,
                        focusthickness=3,
                        focuscolor='#33B5E5')
        style.map('Dark.TButton',
                  background=[('active', '#33B5E5')],
                  foreground=[('active', '#212121')])
        # Boutons en bas avec cadre jaune
        frame_export = tk.Frame(self, bg="#212121", highlightbackground="#FFF3AE", highlightthickness=3, bd=0)
        frame_export.place(relx=0.3, rely=0.92, anchor="s")
        btn_export = ttk.Button(frame_export, text="Exporter USB", image=self.icon_export, compound='left', style='Dark.TButton')
        btn_export.pack()
        frame_trash = tk.Frame(self, bg="#212121", highlightbackground="#FFF3AE", highlightthickness=3, bd=0)
        frame_trash.place(relx=0.7, rely=0.92, anchor="s")
        btn_trash = ttk.Button(frame_trash, text="Supprimer", image=self.icon_trash, compound='left', style='Dark.TButton', command=self._delete_selected)
        btn_trash.pack()

    def _populate_listbox(self):
        self.listbox.delete(0, tk.END)
        dossiers = [d for d in os.listdir(ACQ_PATH) if os.path.isdir(os.path.join(ACQ_PATH, d))]
        for d in sorted(dossiers):
            self.listbox.insert(tk.END, d)

    def _select_all(self):
        self.listbox.select_set(0, tk.END)
    def _select_none(self):
        self.listbox.selection_clear(0, tk.END)
    def _delete_selected(self):
        import shutil
        selected = list(self.listbox.curselection())
        if not selected:
            return
        items = [self.listbox.get(i) for i in selected]
        self._show_delete_confirmation(items)

    def _show_delete_confirmation(self, items):
        import tkinter as tk
        import shutil
        win = tk.Toplevel(self)
        win.geometry("800x480")
        win.attributes('-fullscreen', True)
        win.overrideredirect(True)
        win.resizable(False, False)
        win.configure(bg="#212121")
        label = tk.Label(win, text=f"Supprimer définitivement {len(items)} acquisition(s) ?\nCette action est irréversible.", bg="#212121", fg="#FFF3AE", font=("Roboto Mono", 22, "bold"))
        label.pack(pady=80)
        frame_btn = tk.Frame(win, bg="#212121")
        frame_btn.pack(pady=40)
        def confirm():
            for item in items:
                path = os.path.join(ACQ_PATH, item)
                try:
                    shutil.rmtree(path)
                except Exception as e:
                    print(f"Erreur suppression {item} : {e}")
            win.destroy()
            self._populate_listbox()
        def cancel():
            win.destroy()
        btn_ok = tk.Button(frame_btn, text="Confirmer", bg="#E53935", fg="#FFF", font=("Roboto Mono", 18, "bold"), width=14, command=confirm, relief="flat", borderwidth=0, activebackground="#B71C1C")
        btn_cancel = tk.Button(frame_btn, text="Annuler", bg="#33B5E5", fg="#212121", font=("Roboto Mono", 18, "bold"), width=14, command=cancel, relief="flat", borderwidth=0, activebackground="#0097A7")
        btn_ok.pack(side="left", padx=40)
        btn_cancel.pack(side="right", padx=40) 