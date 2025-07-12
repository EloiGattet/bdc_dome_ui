Projet de module de controle de dôme RTI.
2 répertoires dans le projet:
Dossier "Interface_RaspberryPi" contient l'interface graphique python affiché sur rapsberry pi sur un écran tactile 7 pouces pour controle du dôme + contrôle de l'appareil photo + prétraitement des données après chaque acquisition et centralisation des photos.
Dossier "Module_controle_dome" contient le code de l'arduino.


PRD Interface_RaspberryPi:
Interface tactile simple, Avec un menu principal, le menu principal permet de soit créer une nouvelle acquisition, soit de voir les acquisitions stockées sur le Raspberry Paris. Quand on va créer une nouvelle acquisition, on va pouvoir choisir les paramètres de prise de vue, c'est à dire le nom de l'acquisition. On va avoir trois choses. La première c'est on va choisir quel est le mur qu'on va scanner, est-ce que c'est mur nord ou mur sud ? Donc il y a un bouton toggle nord ou sud. Le deuxième c'est la colonne qu'on va scanner. Donc là il faut afficher un champ et avoir une flèche pour augmenter ou diminuer la colonne. Et le troisième c'est la ligne, pareil avec une flèche de chaque côté du champ où on va pouvoir augmenter et diminuer la ligne. Il faudrait stocker ces deux valeurs dans un fichier texte pour que ça sauvegarde à chaque fois et qu'on n'est pas à revenir et à tout retaper à chaque fois. Donc stocker, mur nord, enfin les dernières valeurs utilisées en fait. Quand on lance l'acquisition, ça vérifie que l'appareil photo est bien branché. On va utiliser g-photo pour le contrôle de l'appareil photo. Ensuite ça va vérifier que l'art vinault est bien connecté en série. Si tout se passe bien, ça lance l'acquisition, ça prend les 24 photos et ensuite ça centralise les photos et ça les renomme, ça les met dans un dossier qui va s'appeler le nom du mur, le nom de la colonne, le nom de la ligne et dedans il y aura les 24 photos stockées. L'autre page ça va être voir les acquisitions qui ont été faites et donc voir les dossiers avec les photos. Idealement quand on centralise les photos il faudrait avoir un thumbnail qui générait pour pouvoir afficher rapidement dans l'interface les photos. J'aimerais qu'il y ait des outils de vérification de l'intégrité des photos donc vérifier qui est bien les 24 photos, vérifier que toutes les photos ont la lumière, genre si il y a une photo noire ça veut dire que c'est raté ou une photo complètement blanche. Il va falloir des outils un peu d'analyse d'image pour mettre un feedback si il y a un dossier qui n'est pas complet, si il y a une acquisition qui est raté. Dans le menu où on regarde les photos il faudrait avoir un bouton pour transférer vers un média USB branché les photos qui ont été acquises pour pouvoir les sortir rapidement du boîtier Raspberry Pi sur un disque usb externe.



PRD Module_controle_dome:

Objectif, controler 24 LEDs haute puissance, alumage séquentiel des lumières et déclenchement photo, soit via arduino soit via le raspberry qui controle la camera via usb soit via l'arduino + optocoupleur -> délchencheur filaire de l'appareil photo.

Le module de controle a 2 modes: soit via controle série USB depuis un raspberry pi, soit via interface autonome embarquée (donc prévoir de laisser les pins séries libres)

Optocoupleur pour déclenchement appareil photo -> 2 pins (demi pression/pression)
led rgb pour feedback
buzzer
Interface via écran nokia (RST, CE, DC, DIN, CLK, VCC, LIGHT, et GND) -> ajouter PCD8544 et GFX d’Adafruit
Joystick 4 direction + bouton pour navigation dans menus

Bouton supplémentaire, fonction à déterminer (début programme?/test série?)


Si controle série, l'écran affiche un feedback de la prise de vue en cours (led X/24 allumée, temps restant etc), messages de controle via sérial à déterminer et documenter.

bouton reset sur reset arduino (pas besoin de le coder)
Bonus si encore pin disponible : ring 16 neopixels 


Étant donné qu'on a des LED de 30 watts montés sur du plastique, il faut pas qu'elle reste allumée plus de 10 secondes d'affilée. Donc il faut qu'il y ait une espèce de routine qui soit, mais le enable à un pour être sûr que les LED s'éteignent au bout de 10 secondes s'il n'y a eu aucune commande ou s'il n'y a rien eu. Et ça c'est vraiment quelque chose de prioritaire niveau sécurité. Il faut systématiquement qu'il n'y ait jamais une LED qui reste plus de 10 secondes, quoi qu'il arrive allumée. Sinon, sa chauffe, sa fond, c'est pas possible c'est prioritaire niveau sécurité.