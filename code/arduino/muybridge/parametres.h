/*
  Conversations avec Muybridge
  Code d'origine : Michel Glaize
  Modifications : Rémi Sarrailh (µsini)
  **************************************
  **** 🛠 Paramètres                ****
  **************************************
*/

/* 🔌 Branchement */

// ⚙ Moteur
const int FIN_DE_COURSE = 35; // Fin de courses (début de parcours)
const int DIRECTION_MOTEUR = 5; // Broche direction du pilote du moteur (avance / recul)
const int PAS_MOTEUR = 4; // Broche Pas du pilote du moteur
const int MOTEUR_ACTIF = 2; // Broche actif du moteur

// Valeurs basées sur les tests effectués sur place
const int MOTEUR_VITESSE = 26500; // Vitesse du moteur
const int MOTEUR_ACCELERATION = 10000; // Accélération du moteur

// 👀 Capteurs de distance
const int ACTIVATION_CAPTEURS_DISTANCES = 32;
const int MONITEUR_CAPTEUR_DISTANCE_1 = 16;
const int MONITEUR_CAPTEUR_DISTANCE_2 = 33;

/* 📶 Réglages Réseau */

/*
  Les paramètres du réseau sont gérés par le routeur WiFi
  🔲 Arduino : 192.168.19.14:8003
  📳 IPAD : 192.168.19.4:8001
  💻 Mac Mini: 192.168.19.6:8005 / 8007
*/

/* Identifiant WiFi */
const char* point_WiFi = "";
const char* mot_De_Passe_WiFi = "";

// 🟧 Port de réception de l'esp32
const unsigned int port_ESP32 = 8003;

// 📳 IPAD
IPAddress IP_iPad(192, 168, 19, 4); // Adresse IP
const unsigned int port_Ipad = 8001; // Port de réception
char* adresse_Sequence = (char *)"/sequence/"; // Adresse d'envoi de la séquence
char* adresse_Position_Train = (char *)"/train"; // Adresse d'envoi de la position du train
char* adresse_Etat_Parcours = (char *)"/parcours"; // Adresse d'envoi de l'état du parcours
char* adresse_Capteur = (char *)"/capteur"; // Adresse de réception du capteur1

// 💻 Mac Mini
IPAddress IP_Mac(192, 168, 19, 6);
const unsigned int port_Processing = 8005;
const unsigned int port_Millumin = 5000;
char* adresse_Millumin = (char *)"/millumin/action/launchColumn"; // Addresse de démarrage de colonne sur Millumin
char* message_Repos = (char *)"Repos";

/* 🚂 Réglages du train */
const int distanceMinimum = 0;  // Tant que le train n'atteint pas cette distance, l'étape reste l'étape 0
const int distance_Maximum = 1000; // Quand le train arrive à cette distance, on suppose qu'il n'y a plus d'utilisateur
const int distance_Intermediaire = 20; // Distance entre chaque étape

const int position_Train_Minimal = 14000;
const int position_Train_Maximal = 699720;

/* V1 (Réglages finaux)
unsigned int tableau_Position_Moteur[] = {
  200, 14280, 28560, 42840, 57120, 71400, 85680, 99960, 114240, 128520, 142800,
  157080, 171360, 185640, 199920, 214200, 228480, 242760, 257040, 271320,
  285600, 299880, 314160, 342720, 357000, 371280, 385560, 399840, 414120,
  428400, 442680, 456960, 471240, 485520, 499800, 514080, 528360, 542640,
  556920, 571200, 585480, 599760, 614040, 628320, 642600, 656880, 671160,
  658440, 699720, 714000
};
*/

/* V2 (ReRéglages avant vernissage)
unsigned int tableau_Position_Moteur[] = {
  14280, 28560, 42840, 57120, 71400, 85680, 99960, 114240, 128520, 142800,
  157080, 171360, 185640, 199920, 214200, 228480, 242760, 257040, 271320,
  285600, 299880, 314160, 328440, 342720, 357000, 371280, 385560, 399840, 414120,
  428400, 442680, 456960, 471240, 485520, 499800, 514080, 528360, 542640,
  556920, 571200, 585480, 599760, 614040, 628320, 642600, 656880, 671160,
  658440, 699720, 714000
};
*/

// V3 (Réglages à partir des valeurs depuis le programme calibrage_etape)
unsigned int tableau_Position_Moteur[] = {
  200, 8740, 27874, 48820, 65415, 83500, 91800, 110133, 122528, 139658, 160411,
  175570, 190748, 209910, 221079, 238640, 253013, 268171, 280158, 300894, 
  312887, 332880, 347642, 363174, 379149, 397504, 413004, 434256, 447006,
  458971, 471758, 490918, 504883, 520830, 533206, 546805, 560359, 576312, 
  597105, 613038, 626641, 648587, 659376, 672131, 689688, 702965, 714000, 
  714000, 714000, 714000
};

// Mise à jour des images affichés par Millumin selon la séquence (0/50/100/150/200/250/300/350/400/450)
unsigned int delai_MAJ_Millumin[] = {
  4000, 1500, 4000, 2100, 4500, 2500, 4000, 4000, 4000, 1500
};

/* 👀 Réglages capteurs */
const int delai_MAJ_Position = 20; // Temps en millisecondes avant vérification de la position
const int delai_Absence = 10 * 1000; // Temps en secondes avant le retour du train en cas d'absence de l'utilisateur
const int delai_MAJ_Moteur = 50; // Temps de Mise à jour de l'état du train sur l'IPAD
