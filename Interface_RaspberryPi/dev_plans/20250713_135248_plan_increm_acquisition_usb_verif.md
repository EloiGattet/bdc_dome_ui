# Dev Plan Incrémental — Interface Raspberry Pi (acquisition, export, vérification)

**Date : 2025-07-13**

## Système de mock (simulation)
- [x] Ajouter un mode "mock" activable par un paramètre (ex: --mock ou MOCK=True)
- [x] Permettre de combiner mock et windowed pour tests sur Mac/PC
- [x] Afficher dans l’UI qu’on est en mode simulation
- [x] Préparer les fonctions mock dans utils.py (connexion Arduino, commande, prise de photo)
- [x] Simuler la connexion Arduino (aucune communication série réelle)
- [x] Simuler la prise de photo (pas de gphoto2, générer des images factices)
- [x] Simuler les retours OK/erreurs
- [x] Intégrer les mocks dans le flux d'acquisition (acquisition.py)
- [x] Ajouter des logs console (print/logging) à chaque étape clé pour le suivi/debug
- [x] Séquence mock multi-étapes avec progress bar :
    - [x] Vérification connexion série (progress bar courte, log)
    - [x] Vérification connexion appareil photo (progress bar courte, log)
    - [x] Prise de vue (progress bar, délai simulé, log)
    - [x] Copie des photos (progress bar, délai simulé, log)
    - [x] Vérification des données (progress bar, délai simulé, log)
    - [x] Affichage du succès dans l’UI (pas de popup, message, fermeture auto)

## UI acquisition & flux photo
- [x] Interface de capture avec progressbar, affichage photo XX/24, temps restant estimé (mock)
- [x] Récupération/copie des photos depuis l'appareil photo après acquisition (squelette, UI, logs, intégration dans le flux réel)
- [x] Mode windowed : toutes les fenêtres 800x480 non redimensionnables
- [x] Gestion du conflit d'acquisition :
    - [x] Affichage d'une bulle d'avertissement si acquisition déjà existante (UI)
    - [x] Jamais bloquant : nouveau dossier suffixé (_2, _3, ...)

## Synthèse des besoins restants (PRD)
- Acquisition complète (dialogue série Arduino + gphoto2, rangement, feedback)
- Export USB et suppression d’acquisition
- Vérification d’intégrité (24 photos, noires/blanches)
- Thumbnails (miniatures)
- Robustesse, feedback utilisateur, finitions UI/UX

---

## Étapes de développement

### 1. Acquisition complète et robuste
- [ ] Vérifier la connexion de l’appareil photo (gphoto2)
- [ ] Vérifier la connexion série Arduino (pyserial)
- [ ] Déclencher séquentiellement les 24 photos :
    - [ ] Allumer LED via Arduino (commande série)
    - [ ] Attendre retour OK Arduino
    - [ ] Déclencher photo via gphoto2
    - [ ] Répéter pour chaque LED
- [ ] Centraliser, renommer et ranger les photos dans un dossier structuré (mur/colonne/ligne)
- [ ] Feedback utilisateur (progression, erreurs, popup si échec)

### 2. Export USB et suppression
- [ ] Détecter la présence d’un média USB monté
- [ ] Copier un dossier d’acquisition sélectionné vers le média USB
- [ ] Feedback utilisateur (popup succès/échec)
- [ ] Suppression sécurisée d’un dossier d’acquisition (confirmation, gestion erreurs)

### 3. Vérification d’intégrité et thumbnails
- [ ] Vérifier la présence des 24 photos dans chaque dossier
- [ ] Détecter photos noires/blanches (analyse simple)
- [ ] Générer une miniature (thumbnail) pour chaque acquisition
- [ ] Afficher l’état (complet/incomplet, OK/KO) et la miniature dans la liste

### 4. Finitions UI/UX et robustesse
- [ ] Feedback utilisateur (popups, messages, icônes d’état)
- [ ] Gestion des erreurs (USB non monté, photo manquante, Arduino non détecté…)
- [ ] Nettoyage, documentation rapide, DRY
- [ ] Accessibilité, focus, feedback visuel

---

## Notes techniques
- Utiliser pyserial pour le dialogue série avec Arduino
- Utiliser subprocess pour piloter gphoto2
- Utiliser PIL pour générer les miniatures
- Centraliser les chemins et la logique de fichiers dans utils.py
- Garder chaque fichier <300 lignes, 1 classe/fenêtre

---

**Priorité :** acquisition robuste et feedback utilisateur, puis export/suppression, puis vérification/thumbnails, puis finitions. 