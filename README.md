
# reveil_conect-

[![License: CC BY-NC 4.0](https://img.shields.io/badge/License-CC%20BY--NC%204.0-lightgrey)](LICENSE)

Projet "Réveil Connecté" (ESP32)

Description
-
Ce projet implémente un réveil connecté basé sur un ESP32. Il récupère des événements depuis un fichier iCal (URL configurable), synchronise l'heure via NTP et déclenche une alarme sonore et un éclairage progressif pour les événements nommés "reveil".

Principales fonctionnalités
-
- Récupération d'un calendrier iCal via HTTP
- Synchronisation NTP pour l'heure
- Recherche du prochain événement nommé "reveil" et démarrage d'une séquence sonore
- Boutons pour tester l'alarme, l'arrêter, forcer une mise à jour et réinitialiser
- Mode secret (désactivation visuelle/sonore partielle)

Arborescence importante
-
- `code/snapshot_V2.4/reveil/` : version utilisée pour ce README
	- `reveil.ino` : boucle principale et logique d'état
	- `calendar.*` : récupération et parsing iCal + gestion des événements
	- `song_runner.*` : gestion de la séquence sonore (buzzer)
	- `pin.h` : mapping des broches
	- `software_parameters.h` et `local_parameters.h` : paramètres et valeurs locales

Matériel requis
-
- ESP32 (développement compatible Arduino)
- Buzzer actif ou passif connecté à la pin définie
- LED pour indicateur d'erreur / lumière de réveil
- Câblage pour boutons: stop, test, update, soft reset

Pinout (défini dans `pin.h`)
-
- BUZZER_PIN : 13
- ERROR_LED_PIN : 12
- LIGHT_PIN : 25
- STOP_ALARM_PIN : 15
- TEST_ALARM_PIN : 14
- UPDATE_PIN : 27
- SOFT_RESET_PIN : 26

Configuration
-
1. Ouvrir `code/snapshot_V2.4/reveil/local_parameters.h`.
2. Remplir :
	 - `Ical_URL` : URL publique du calendrier iCal à interroger
	 - `WIFI_SSID` et `WIFI_PSW` : crédentials Wi‑Fi
3. Optionnel : ajuster les constantes dans `software_parameters.h` (durées, timing, nombre de boucles, etc.).

Compilation et téléversement
-
- Importer le dossier `code/snapshot_V2.4/reveil/` dans l'IDE Arduino ou utiliser PlatformIO.
- Sélectionner une carte ESP32 compatible et téléverser.

Comportement et commandes
-
- Au démarrage l'appareil tente de récupérer l'iCal et synchroniser l'heure via NTP.
- Il cherche le prochain événement nommé `reveil` et règle l'allumage progressif et la sonnerie.
- Boutons :
	- `TEST_ALARM_PIN` : test d'alarme
	- `STOP_ALARM_PIN` : arrête l'alarme
	- `UPDATE_PIN` : force une mise à jour du calendrier
	- `SOFT_RESET_PIN` : reset logiciel (redémarrage)
- Mode debug série : envoyer `D` sur le port série pour entrer en debug interactif (plusieurs commandes disponibles).

Paramètres importants (extraits de `software_parameters.h`)
-
- `UPDATE_MIN_DELAY_NORMAL` : délai minimal entre mises à jour (s)
- `SCAN_LENGHT` : durée (jours) pour scanner les événements iCal
- `NOTE_TIME`, `MAX_BOUCLE` : paramètres de la mélodie

Débogage
-
Débogage
-
- Ouvrir le moniteur série à 115200 bauds pour logs et commandes debug.
- Envoyer `D` sur le port série pour entrer en mode debug interactif.
- Commandes disponibles en mode debug :
	- `D` : dump du calendrier (appel de `calendar.serial_dump()`)
	- `E` : quitter le mode debug
	- `d` : déconnecter le Wi‑Fi
	- `U` : forcer une mise à jour (téléchargement iCal + NTP)
	- `N` : afficher l'événement suivant et le délai restant
	- `B` : buzzer ON (440 Hz)
	- `b` : buzzer OFF
	- `L` : lumière ON (valeur max)
	- `l` : lumière OFF
	- `M` : LED d'erreur ON
	- `m` : LED d'erreur OFF
	- `R` : reset logiciel (redémarrage)
- Hors du mode debug, envoyer `D` depuis la boucle principale déclenche l'entrée en debug via la série.
- Le code affiche les événements détectés et les étapes de mise à jour dans le moniteur série.

Contribuer
-
- Proposez des PR pour corrections et améliorations. Voir `CONTRIBUTING.md` pour les règles de contribution. Veuillez préciser la version modifiée (dossier `code/last_version/` ou `snapshot_V2.4/`).

Licence
-
Ce projet est distribué sous la licence Creative Commons Attribution-NonCommercial 4.0 International (CC BY-NC 4.0). Voir le fichier `LICENSE` pour les détails. Cette licence interdit l'utilisation commerciale sans autorisation explicite du titulaire du copyright (LEGRIX b, 2025).

Contact
-
Voir l'historique et les versions dans le dépôt pour plus de contexte.

---