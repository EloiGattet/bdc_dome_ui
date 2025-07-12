import os
import json

LAST_FILE = os.path.join(os.path.dirname(__file__), "last_acquisition.txt")

def save_last_acquisition(mur, colonne, ligne):
    data = {"mur": mur, "colonne": colonne, "ligne": ligne}
    with open(LAST_FILE, "w") as f:
        json.dump(data, f)

def load_last_acquisition():
    if not os.path.exists(LAST_FILE):
        return None
    try:
        with open(LAST_FILE, "r") as f:
            data = json.load(f)
        return data
    except Exception:
        return None 