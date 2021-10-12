/*
      Conversations avec Muybridge

  Code d'origine : Michel Glaize
  Modifications : Rémi Sarrailh (µsini)
  *****************************************
  **** ☄ Gestion de la communication  ****
  *****************************************

  Ce fichier gère la communication entre les appareils
  📳 La tablette à l'entrée
  💻 Le logiciel qui gère la projection (Millumin)
  💻 Le logiciel qui assiste à l'assemblage de la projection (Processing)
*/

// Bibliothèque d'envoi des données OSC (CNMAT/OSC)
#include <OSCMessage.h>

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* Fonctions destinées à l'envoi de message */

// Crée un message OSC et l'envoi
void envoi_Par_OSC(IPAddress IP, int port, char* addr, String msg) {
  OSCMessage msg_Envoye(addr); // Création un message OSC avec l'adresse
  msg_Envoye.add(msg.c_str()); // Insertion du message OSC
  Udp.beginPacket(IP, port); // Initie le message réseau
  msg_Envoye.send(Udp); // Envoie le message OSC dans un paquet UDP
  Udp.endPacket(); // Finalise le message OSC
  msg_Envoye.empty(); // Nettoyage du message OSC
}

void envoi_Par_OSC(IPAddress IP, int port, char* addr, float msg) {
  OSCMessage msg_Envoye(addr); // Création un message OSC avec l'adresse
  msg_Envoye.add(msg); // Insertion du message OSC
  Udp.beginPacket(IP, port); // Initie le message réseau
  msg_Envoye.send(Udp); // Envoie le message OSC dans un paquet UDP
  Udp.endPacket(); // Finalise le message OSC
  msg_Envoye.empty(); // Nettoyage du message OSC
}

// 📱 Réception IPAD
// Lorsqu'un message /sequence/ est reçu cette fonction est exécutée.

void recuperation_Sequence(OSCMessage &msg_Recu) {

  //if(!sequence_Choisi) {
    int sequence_int;
    sequence_int = msg_Recu.getInt(0);
    sequence = String(sequence_int);
    sequenceID = sequence_int / 50;
    Serial.print("[📱] Séquence choisie : ");
    Serial.println(sequence_int);
    envoi_Par_OSC(IP_iPad, port_Ipad, adresse_Etat_Parcours, "Séquence sélectionnée");
    // Retour vers la tablette pour lui dire d'allumer le bouton
    //envoi_Sur_Ipad(adresse_Position_Train, (char*)sequence);
    sequence_Choisi = true; // Déactivation de la tablette
  //}
}

/*
//🤖 Simulateur du capteur
void simulateurCapteur(OSCMessage &msg_Recu2){
  if(sequence_Choisi){
    int distance;
    distance = msg_Recu2.getInt(0);
    Serial.print("[👀] Distance Reçu : ");
    Serial.println(distance);
    conversion_distance_vers_etape(distance);
  }
}
*/

/*
  Réception d'un message OSC depuis l'IPAD
*/
void receptionSequence() {
  OSCMessage msg_Recu; // création d'un message OSC
  int size;
  if ((size = Udp.parsePacket()) > 0) { // Lecture du paquet UDP
    while (size--)
      msg_Recu.fill(Udp.read()); // Rempli un message OSC avec ce qui arrive depuis le réseau (UDP)
    if (!msg_Recu.hasError()) {
      char adresse_Reception[128];
      msg_Recu.getAddress(adresse_Reception);
      Serial.print("[📱] Message reçu sur ");
      Serial.println(adresse_Reception);
      //msg_Recu.dispatch(adresse_Capteur, simulateurCapteur);
      msg_Recu.dispatch(adresse_Sequence, recuperation_Sequence); //Route /sequence sur la fonction recuperation_Sequence

    }
    else { // En cas d'échec de la communication: Flush de l'UDP et redémarrage de la communication réseau
      Serial.println("Message UDP corrompu");
      Udp.flush();
      Udp.stop();
      Udp.begin(port_ESP32);
    }
  }
}
