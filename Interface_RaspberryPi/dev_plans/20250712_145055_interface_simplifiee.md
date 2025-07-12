# Plan de développement — Interface Raspberry Pi (vitrine dôme)

**Date : 2025-07-12**

## Synthèse du style et héritage domesUI
- Palette sombre : fond #212121, texte #FFF3AE, secondaire #424035, boutons actifs #33B5E5
- Police : Roboto Mono, tailles variables, gras pour titres/boutons
- Boutons plats, curseur t-cross, icônes PNG systématiques
- Disposition : menu centré, gros boutons, logo en bas à gauche
- Fenêtres plein écran
- Navigation simple, labels clairs
- Réutilisation des icônes (placées dans Interface_RaspberryPi/icons)

## Fonctionnalités attendues (PRD)
- Menu principal :
  - Nouvelle acquisition (icône menu_capture.png)
  - Voir les acquisitions (icône menu_projets.png)
  - Logo en bas à gauche (logo_mercurio.png)
  - (optionnel) Quitter (IconeEteindre.png)
- Nouvelle acquisition :
  - Sélection mur (toggle Nord/Sud)
  - Sélection colonne/ligne (champ + flèches)
  - Validation, sauvegarde auto des derniers choix
- Voir les acquisitions :
  - Liste des dossiers, export USB, suppression, quitter
  - (optionnel) Affichage miniature

## Organisation du code
- main.py : point d’entrée, menu principal, navigation
- acquisition.py : fenêtre nouvelle acquisition
- projects.py : fenêtre visualisation/export/suppression
- utils.py : utilitaires (sauvegarde/restauration, fichiers)
- /icons/ : toutes les images
- 1 classe/fenêtre, héritant de Frame ou Toplevel
- Aucun fichier > 300 lignes

---

# Phases de développement

## Phase 1 — Structure de base & Menu principal ✅
- [x] Créer la structure de fichiers (main.py, acquisition.py, projects.py, utils.py)
- [x] Menu principal :
  - Affichage plein écran, fond sombre
  - Boutons "Nouvelle acquisition" et "Voir les acquisitions" (icônes, police, couleurs)
  - Logo en bas à gauche
  - Navigation (boutons ouvrent les fenêtres correspondantes)
- [x] Pas de logique acquisition/projets, juste navigation

## Phase 2 — Fenêtre "Nouvelle acquisition" ✅
- [x] Interface pour choisir mur (toggle), colonne, ligne (flèches)
- [x] Bouton valider
- [x] Sauvegarde/restauration auto des derniers choix (utils.py)
- [x] Retour menu principal

## Phase 3 — Fenêtre "Voir les acquisitions" (UI) ✅
- [x] Liste des dossiers d’acquisition (Listbox)
- [x] Boutons exporter USB, supprimer, quitter (icônes)
- [x] Navigation et UI conformes domesUI
- [ ] (optionnel) Affichage miniature
- [ ] Logique d'export/suppression à faire
- [x] Retour menu principal

## Phase 4 — Finitions & Refactoring (à venir)
- Vérification DRY, découpage <300 lignes
- Ajustements UI/UX (alignements, tailles, feedback)
- Nettoyage, documentation rapide

---

## Note sur la gestion des boutons/icônes
- Tkinter Button + image : le fond du bouton n’est visible que si le PNG est transparent
- Le texte est bien stylé (fg/bg), mais le fond du texte dépend du PNG
- Pour un rendu parfait, il faut des PNG transparents ou noirs
- Limitation Tkinter : pas de fond de texte custom dans un bouton avec image

---

## Prochaines étapes
- Logique export USB (copie du dossier sélectionné)
- Logique suppression d’acquisition
- (Optionnel) Affichage miniature/thumbnails dans la liste
- Finitions UI (feedback, focus, accessibilité)
- Nettoyage, documentation rapide 