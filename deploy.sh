#!/bin/bash

# Script de déploiement pour l'interface Raspberry Pi
# Synchronise le code local vers pi@mercuriopompei.local

set -e  # Arrêter en cas d'erreur

# Configuration
REMOTE_HOST="pi@mercuriopompei.local"
REMOTE_PATH="/home/pi/Interface_RaspberryPi"
LOCAL_PATH="./Interface_RaspberryPi"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Fonction d'aide
show_help() {
    echo -e "${BLUE}Usage: $0 [OPTIONS]${NC}"
    echo ""
    echo -e "${YELLOW}Options:${NC}"
    echo -e "  ${GREEN}sync${NC}     - Synchronise uniquement les fichiers (défaut)"
    echo -e "  ${GREEN}install${NC}  - Synchronise ET installe les dépendances"
    echo -e "  ${GREEN}full${NC}     - Installation complète (sync + install + test)"
    echo -e "  ${GREEN}help${NC}     - Affiche cette aide"
    echo ""
    echo -e "${YELLOW}Exemples:${NC}"
    echo -e "  ${GREEN}$0${NC}           - Synchronisation rapide"
    echo -e "  ${GREEN}$0 sync${NC}      - Synchronisation rapide"
    echo -e "  ${GREEN}$0 install${NC}   - Synchronisation + installation"
    echo -e "  ${GREEN}$0 full${NC}      - Installation complète"
}

# Vérification des arguments
MODE=${1:-sync}
case $MODE in
    sync|install|full|help)
        ;;
    *)
        echo -e "${RED}❌ Mode invalide: $MODE${NC}"
        show_help
        exit 1
        ;;
esac

if [ "$MODE" = "help" ]; then
    show_help
    exit 0
fi

echo -e "${YELLOW}🚀 Déploiement vers Raspberry Pi...${NC}"
echo -e "${YELLOW}📡 Connexion: ${REMOTE_HOST}${NC}"
echo -e "${YELLOW}📁 Dossier local: ${LOCAL_PATH}${NC}"
echo -e "${YELLOW}📁 Dossier distant: ${REMOTE_PATH}${NC}"
echo -e "${YELLOW}🔧 Mode: ${MODE}${NC}"
echo ""

# Vérification de la connexion SSH
echo -e "${YELLOW}🔍 Test de connexion SSH...${NC}"
if ! ssh -o ConnectTimeout=5 -o BatchMode=yes ${REMOTE_HOST} exit 2>/dev/null; then
    echo -e "${RED}❌ Impossible de se connecter à ${REMOTE_HOST}${NC}"
    echo -e "${RED}   Vérifiez que:${NC}"
    echo -e "${RED}   - Le Raspberry Pi est allumé et connecté au réseau${NC}"
    echo -e "${RED}   - La clé SSH est configurée${NC}"
    echo -e "${RED}   - L'adresse mercuriopompei.local est résolvable${NC}"
    exit 1
fi
echo -e "${GREEN}✅ Connexion SSH OK${NC}"

# Création du dossier distant si nécessaire
echo -e "${YELLOW}📂 Création du dossier distant...${NC}"
ssh ${REMOTE_HOST} "mkdir -p ${REMOTE_PATH}"
echo -e "${GREEN}✅ Dossier distant créé${NC}"

# Synchronisation avec rsync
echo -e "${YELLOW}🔄 Synchronisation des fichiers...${NC}"
rsync -avz --delete \
    --exclude='__pycache__/' \
    --exclude='*.pyc' \
    --exclude='.DS_Store' \
    --exclude='*.log' \
    --exclude='acquisitions/' \
    --exclude='last_acquisition.txt' \
    ${LOCAL_PATH}/ ${REMOTE_HOST}:${REMOTE_PATH}/

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ Synchronisation réussie${NC}"
else
    echo -e "${RED}❌ Erreur lors de la synchronisation${NC}"
    exit 1
fi

# Vérification des dépendances Python
echo -e "${YELLOW}🐍 Vérification des dépendances Python...${NC}"
ssh ${REMOTE_HOST} "cd ${REMOTE_PATH} && python3 -c 'import tkinter, PIL' 2>/dev/null" || {
    echo -e "${YELLOW}⚠️  Installation des dépendances Python...${NC}"
    ssh ${REMOTE_HOST} "sudo apt-get update && sudo apt-get install -y python3-pil python3-pil.imagetk"
}

# Exécution conditionnelle selon le mode
if [ "$MODE" = "install" ] || [ "$MODE" = "full" ]; then
    echo -e "${YELLOW}🔧 Exécution du script d'installation...${NC}"
    ssh ${REMOTE_HOST} "cd ${REMOTE_PATH} && chmod +x install.sh && ./install.sh" || {
        echo -e "${RED}❌ Erreur lors de l'installation${NC}"
        exit 1
    }
fi

# Test optionnel
if [ "$MODE" = "full" ]; then
    echo -e "${YELLOW}🧪 Test complet de l'application...${NC}"
    ssh ${REMOTE_HOST} "cd ${REMOTE_PATH} && python3 -c 'import main; print(\"✅ Application OK\")'" || {
        echo -e "${RED}❌ Erreur lors du test${NC}"
        exit 1
    }
fi

echo ""
echo -e "${GREEN}🎉 Déploiement terminé avec succès !${NC}"
if [ "$MODE" = "sync" ]; then
    echo -e "${GREEN}📱 Fichiers synchronisés sur le Raspberry Pi${NC}"
else
    echo -e "${GREEN}📱 L'application est installée et prête sur le Raspberry Pi${NC}"
fi
echo ""
echo -e "${YELLOW}💡 Pour lancer l'application:${NC}"
echo -e "${YELLOW}   - Double-cliquez sur l'icône 'Interface Pompei' sur le bureau${NC}"
echo -e "${YELLOW}   - Ou connectez-vous en SSH:${NC}"
echo -e "${YELLOW}     ssh ${REMOTE_HOST}${NC}"
echo -e "${YELLOW}     cd ${REMOTE_PATH}${NC}"
echo -e "${YELLOW}     python3 main.py${NC}" 