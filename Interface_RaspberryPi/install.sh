#!/bin/bash

# Script d'installation pour l'interface Raspberry Pi
# Installe les dépendances et crée un raccourci sur le bureau

set -e  # Arrêter en cas d'erreur

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}🚀 Installation de l'interface Raspberry Pi...${NC}"
echo ""

# Vérification que nous sommes sur un Raspberry Pi
if ! grep -q "Raspberry Pi" /proc/cpuinfo 2>/dev/null; then
    echo -e "${YELLOW}⚠️  Attention: Ce script est conçu pour Raspberry Pi${NC}"
    read -p "Continuer quand même ? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Configuration de l'encodage
echo -e "${YELLOW}🌐 Configuration de l'encodage...${NC}"
export LANG=fr_FR.UTF-8
export LC_ALL=fr_FR.UTF-8
sudo locale-gen fr_FR.UTF-8 2>/dev/null || true
echo -e "${GREEN}✅ Encodage configuré${NC}"

# Mise à jour du système
echo -e "${YELLOW}📦 Mise à jour du système...${NC}"
sudo apt-get update
echo -e "${GREEN}✅ Système mis à jour${NC}"

# Installation des dépendances système
echo -e "${YELLOW}🔧 Installation des dépendances système...${NC}"
sudo apt-get install -y \
    python3 \
    python3-pip \
    python3-pil \
    python3-pil.imagetk \
    python3-tk \
    git \
    rsync

echo -e "${GREEN}✅ Dépendances système installées${NC}"

# Installation des dépendances Python
echo -e "${YELLOW}🐍 Installation des dépendances Python...${NC}"
# Utilisation de apt pour les packages système
sudo apt-get install -y python3-pillow python3-serial || {
    echo -e "${YELLOW}⚠️  Installation via pip avec --break-system-packages...${NC}"
    pip3 install --break-system-packages pillow pyserial || {
        echo -e "${YELLOW}⚠️  Installation via pip --user...${NC}"
        pip3 install --user pillow pyserial
    }
}

echo -e "${GREEN}✅ Dépendances Python installées${NC}"

# Création du raccourci sur le bureau
echo -e "${YELLOW}📱 Création du raccourci sur le bureau...${NC}"

# Chemin de l'application
APP_PATH="/home/pi/Interface_RaspberryPi"
DESKTOP_PATH="/home/pi/Desktop"
ICON_PATH="/home/pi/Interface_RaspberryPi/icons/logo_mercurio.png"

# Création d'un fichier .desktop complet pour l'icône
cat > "${DESKTOP_PATH}/Interface_Pompei.desktop" << 'EOF'
[Desktop Entry]
Version=1.0
Type=Application
Name=Interface Pompei
Comment=Interface de contrôle du dôme RTI Mercurio
Exec=python3 /home/pi/Interface_RaspberryPi/main.py
Icon=/home/pi/Interface_RaspberryPi/icons/logo_mercurio.png
Terminal=false
Categories=Science;Education;
X-GNOME-UsesNotifications=false
EOF

# Rendre le fichier .desktop exécutable
chmod +x "${DESKTOP_PATH}/Interface_Pompei.desktop"

# Mise à jour du cache des icônes
update-desktop-database "${DESKTOP_PATH}" 2>/dev/null || true

# Suppression des anciens raccourcis pour éviter les doublons
rm -f "${DESKTOP_PATH}/Interface_Pompei.desktop~" 2>/dev/null || true

echo -e "${GREEN}✅ Script et raccourci créés sur le bureau${NC}"

# Création du dossier acquisitions s'il n'existe pas
echo -e "${YELLOW}📁 Création des dossiers nécessaires...${NC}"
mkdir -p "${APP_PATH}/acquisitions"
echo -e "${GREEN}✅ Dossiers créés${NC}"

# Test de l'application
echo -e "${YELLOW}🧪 Test de l'application...${NC}"
cd "${APP_PATH}"
if python3 -c "import main; print('✅ Import OK')" 2>/dev/null; then
    echo -e "${GREEN}✅ Application testée avec succès${NC}"
else
    echo -e "${RED}❌ Erreur lors du test de l'application${NC}"
    echo -e "${YELLOW}   Vérifiez que tous les fichiers sont présents${NC}"
    echo -e "${YELLOW}   Test détaillé des imports...${NC}"
    python3 -c "import tkinter; print('✅ tkinter OK')" || echo "❌ tkinter KO"
    python3 -c "from PIL import Image; print('✅ PIL.Image OK')" || echo "❌ PIL.Image KO"
    python3 -c "from PIL import ImageTk; print('✅ PIL.ImageTk OK')" || echo "❌ PIL.ImageTk KO"
fi

# Configuration des permissions
echo -e "${YELLOW}🔐 Configuration des permissions...${NC}"
chmod +x "${APP_PATH}/main.py"
chmod +x "${APP_PATH}/acquisition.py"
chmod +x "${APP_PATH}/projects.py"
chmod +x "${APP_PATH}/utils.py"

echo -e "${GREEN}✅ Permissions configurées${NC}"

# Message de fin
echo ""
echo -e "${GREEN}🎉 Installation terminée avec succès !${NC}"
echo ""
echo -e "${BLUE}📋 Récapitulatif :${NC}"
echo -e "${BLUE}   ✅ Dépendances installées${NC}"
echo -e "${BLUE}   ✅ Raccourci créé sur le bureau${NC}"
echo -e "${BLUE}   ✅ Dossiers créés${NC}"
echo -e "${BLUE}   ✅ Permissions configurées${NC}"
echo ""
echo -e "${YELLOW}💡 Pour lancer l'application :${NC}"
echo -e "${YELLOW}   - Double-cliquez sur l'icône 'Interface Pompei' sur le bureau${NC}"
echo -e "${YELLOW}   - Ou tapez : python3 ${APP_PATH}/main.py${NC}"
echo ""
echo -e "${YELLOW}🔧 Pour le développement :${NC}"
echo -e "${YELLOW}   - Utilisez le script deploy.sh pour synchroniser les modifications${NC}" 