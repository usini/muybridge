# Conversations avec Muybridge
Voici l'ensemble du code / modèles 3D réalisés pour l'installation Conversation avec Muybridge de Michel Glaize au carrée d'art de nîmes

# Présentation

[![Vidéo de présentation de l'installation Conversation avec Muybridge](doc/lienvimeo_video_conversation_avec_muybridge.png)](https://vimeo.com/628424731)

Vous pouvez voir la présentation de l'installation sur le site de Michel Glaize : https://www.michelglaize.com/teaser/

# 📥 [Télécharger](https://github.com/usini/muybridge/archive/refs/heads/main.zip)

# 📝 Composants
* [2x Lidar Garmin V3](https://www.mouser.fr/ProductDetail/485-4058)
* [DFRobot Emergency Stop Push Button](https://www.mouser.fr/ProductDetail/426-FIT0156)
* [10x Borniers](https://www.mouser.fr/ProductDetail/490-TB002-500-02BE)
* [Olimex ESP32-POE-EA](https://www.mouser.fr/ProductDetail/909-ESP32-POE-EA)
* [Makerbase SERVO57A](https://fr.aliexpress.com/item/1005003211061309.html)
* [Intterrupteur de fin de course](https://www.amazon.fr/course-canique-interrupteurs-imprimante-Makerbot/dp/B08SJDXSJZ)

# Programmation du contrôleur du train
## [Ajouter l'esp32 à l'IDE Arduino](https://iooner.io/programmer-un-esp32-avec-arduino-ide/)
Voici un tutoriel de Iooner qui explique comment ajouter l'esp32 à l'IDE Arduino    
https://iooner.io/programmer-un-esp32-avec-arduino-ide/

## Programmer l'esp32
### Bibliothèques à installer
Il faut installer ces bibliothèques depuis **Croquis** --> **Inclure une bibliothèque** --> **Gérer les bibliothèques**
* OSC (Chercher OSC Open Transport)
* Accelstepper

## Téléversement du programme
Ouvrez le programme muybridge.ino dans **code/arduino/muybridge**
Sélectionner dans **Outils** --> **Type de Carte** --> **ESP32 Arduino** --> **Olimex ESP32-PoE**

## Configurations
Vous pouvez changer les paramètres dans l'onglet **parametres.h**

Notamment:

* point_wifi : nom du routeur wifi
* mot_de_passe_wifi : mot de passe du routeur wifi
* position_Train_Minimal: première position du train en nb de pas
* position_Train_Maximal: dernière position du train en nb de pas
* Tableau_Position_Moteur : tableau du position de chaque étape dans millumin en nb de pas
* delai_MAJ_Millumin: Délai entre le changement d'étape pour chaque séquence dans millumin
* delai_Absence : Temps en secondes avant le retour du train en cas d'absence de l'utilisateur






