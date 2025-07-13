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

# Fonctions mock pour le mode simulation
import random
from PIL import Image

def mock_arduino_connect():
    return True

def mock_arduino_command(cmd):
    # Simule un délai et un retour OK
    import time
    time.sleep(0.1)
    if random.random() < 0.95:
        return "OK"
    else:
        return "ERREUR"

def mock_take_photo(path):
    # Génère une image factice (PNG noir ou blanc aléatoire)
    img = Image.new('RGB', (400, 300), color=(255,255,255) if random.random() > 0.1 else (0,0,0))
    img.save(path)
    return True 