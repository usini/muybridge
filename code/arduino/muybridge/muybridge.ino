/*
  Conversation avec Muybridge
  Code d'origine : Michel Glaize
  Modifications : Rémi Sarrailh (µsini)
  *************************************
  **** Gestion de la logique       ****
  *************************************
*/

#include "Arduino.h"
#include "parametres.h" // Les paramètres
#include "moteurs.h" // Fonctions du moteurs et des fin de courses
#include "reseau.h" // Fonctions pour le réseau WiFi et le protocole UDP
#include "osc.h" // Fonctions pour le protocole OSC (par UDP)
#include "capteurs.h" // Fonctions liés au capteur LIDAR Garmin

TaskHandle_t Coeur0;

unsigned long temps_MAJ_position = 0;
unsigned long temps_MAJ_images = 0;
unsigned long temps_MAJ_Moteur = 0;

// Cette fonction vérifie si une personne est présente sur le parcours
void verification_Absence() {
  // Si personne n'est detecté sur le parcours
  if(etape == -1) {
    // Mais que le parcours est en cours d'utilisation
    if(depart_Parcours == 1) {
        Serial.print("[👀] Personne sur le parcours depuis : ");
        Serial.println(millis() - temps_Absence);
        // Et que le delai d'absence (défini dans parametres.h) est dépassé
        if((millis() - temps_Absence) > delai_Absence) {
          // Activation de la réinitialisation de la séquence
          reinitialisationSequence = true;
      }
    }
  }
}

// Cette fonction temporise le changement des images projetées
void changement_Etape() {
  // Si une personne est présent sur le parcours
  if(etape != -1) {
    Serial.println("[🔴] Etape :" + String(etape));
    Serial.println("[🚂] Déplacement vers :" + String(tableau_Position_Moteur[etape]));
    
    moteur.moveTo(-tableau_Position_Moteur[etape]); // Déplacer le train
    absence_Utilisateur = 0; // Remis à zéro du compteur d'absence
    
    // Envoi de l'ordre de changer d'image à Millumin
    String sequence_Etape = sequence + String(etape);
    
    // ⏲ Chrono Millumin
    // Selon la séquence, les mises à jour sont plus ou moins rapide
    // Pour laissez le temps de réactualiser l'animation
    if((millis() - temps_MAJ_images) > delai_MAJ_Millumin[sequenceID]) {

      // Envoi OSC de la séquence à changer
      envoi_Par_OSC(IP_Mac, port_Millumin, adresse_Millumin, sequence_Etape);
      Serial.print("[💻] Millumin --> ");
      Serial.println(sequence_Etape);
      temps_MAJ_images = millis(); // Remise à zéro du chrono
    }

    // Mis à jour de Processing, si l'étape est supérieur à l'étape effectué
    // L'image généré par Processing, ne revient pas en arrière
    if(etape >= etape_Effectue) {
      // Mis à jour de la dernier étape effectué
      etape_Effectue = etape;
      
      // Envoi de la séquence à Processing
      envoi_Par_OSC(IP_Mac, port_Processing, (char *)"", sequence_Etape);
      Serial.print("[💻] Processing --> ");
      Serial.println(sequence_Etape);
    }
  }
}

// Cette fonction envoie à l'IPAD la position du train
void maj_Position_Moteur() {
  // Affiche la position du moteur tout les delai_MAJ_Moteur (défini dans paramètres.h)
  if((millis() - temps_MAJ_Moteur) > delai_MAJ_Moteur) {
    temps_MAJ_Moteur = millis();
    int position_Moteur = -moteur.currentPosition();
    //Serial.println(position_Moteur);
    
    if(position_Moteur < 0) { position_Moteur = 0;} // Si la position du moteur est inférieur à 0 alors la mettre à zéro

    // Transformer la position du moteur en une valeur flotante de 0 à 1
    float position_Moteur_Tablette = mapfloat(position_Moteur, 0, position_Train_Maximal, 0, 1);

    // Si la position du moteur a changé, la mettre à jour
    if(moteur.distanceToGo() != 0) {
      // Envoi du message par OSC sur l'application TouchOSC sur l'IPAD 
      envoi_Par_OSC(IP_iPad, port_Ipad, adresse_Position_Train, position_Moteur_Tablette);
      Serial.print("[📱] Envoi Position Moteur: ");
      Serial.println(position_Moteur);
    }
  }
}

// 🟠 Coeur 0 de l'ESP32
// Afin de ne pas entraver le mouvement du moteur, toute la logique est concentré dans le CORE 0 de l'ESP32
// Comme expliquer ici : https://valarsystems.com/blogs/news/how-to-use-accelstepper-with-wifi-on-an-esp32
void Taches_Coeur0( void * pvParameters ) {
  // Boucle Infini
  for (;;) {
      // L'utilisateur choisit la séquence (quand le parcours est réinitialisé ou en cours de réinitialisation)
    if( !sequence_Choisi ) {
      // 📱 Réception du numéro de la séquence depuis la tablette si la réception est activée.
      receptionSequence();
    }
    // Si la séquence n'est pas en cours de réinitialisation
    if ( !reinitialisationSequence ) {
      // 🎞 La séquence est choisi et le train a un zéro connu
      // On récupère la position de l'utilisateur
      if( sequence_Choisi ) {

        // Test avec Simulateur Capteur
        //if(!retour_Train) {
          //receptionSequence();
        //}

        // Temporisation des mise à jour des positions
        if((millis() - temps_MAJ_position) > delai_MAJ_Position) {
          temps_MAJ_position = millis(); // ⏱ Redémarrage du chrono Position
          if(!retour_Train) { // 🚂 Si le train n'est pas en train de rentrer à la gare
            recuperation_Position_Utilisateur(); // 👀 Récupération de la position de l'utilisateur
          }
          if(etape != etape_Precedente) { // Si l'étape est différente de l'étape précédente
            if(nouvelle_Etape) {
                nouvelle_Etape = false;
                changement_Etape(); // Changer d'étape
            }
            etape_Precedente = etape; // Mettre à jour l'étape avec l'étape précédente
          }
          verification_Absence(); // Vérification si l'utilisateur est absent du parcours (après qu'il soit initialisé)
        }
      }
    } else { // Si le parcours est en cours de réinitialisation
      if (!retour_Train) {
          remise_Zero_Train(); // 🚂 Ramener le train à la gare
          envoi_Par_OSC(IP_Mac, port_Millumin, adresse_Millumin, message_Repos); // Envoi du message Repos sur Millumin
          envoi_Par_OSC(IP_Mac, port_Processing, (char *)"", message_Repos); // Envoi du message Repos sur Processing
          envoi_Par_OSC(IP_iPad, port_Ipad, adresse_Etat_Parcours, "À votre Tour !"); // Envoi du message Prêt sur l'IPAD
          Udp.flush();
          Udp.stop();
          Udp.begin(port_ESP32);
      }
    }
    maj_Position_Moteur(); // Mis à jour de la position du moteur
  }
}

// Paramétrages du Système
void setup() {
  Serial.begin(115200); // Mise en route de la communication de débogage série

  // Déactive le WatchDog du Coeur 0 de l'ESP32 comme spécifié ici
  // https://valarsystems.com/blogs/news/how-to-use-accelstepper-with-wifi-on-an-esp32

  parametrages_Reseau(); // Mise en route de la connexion
  parametrages_Moteur(); // Paramètrage du moteur et du fin de course
  parametrages_Capteurs(); // Paramétrage des capteurs LIDAR

  Serial.println("[🔴] Création de la tâche logique sur le coeur 0");
  disableCore0WDT();
  xTaskCreatePinnedToCore(Taches_Coeur0, "Coeur_0", 10000, NULL, 1, &Coeur0, 0);
}


// Dans le loop (coeur 1) on ne s'occupe que du moteur et du fin de courses, afin d'être sûr que accelstepper donne
// bien l'ordre de faire tout les pas.
void loop() {
  // Si le train n'est pas en train de revenir et qu'aucune séquence n'est choisi
  verification_Fin_De_Courses(); // Vérification de l'état du capteur de fin de courses
  moteur.run(); // Déplace le moteur s'il n'est pas à sa position demandé

  //Serial.println(moteur.currentPosition());
}
