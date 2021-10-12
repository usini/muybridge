/*
  Conversations avec Muybridge
  Code d'origine : Michel Glaize
  Modifications : Rémi Sarrailh (µsini)
  *****************************************
  **** 📶 Gestion du réseau WiFi       ****
  *****************************************
*/

#include <WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;

// Configuration du WiFi et de la connexion entrante UDP d'OSC de l'Arduino (utilisé pour récupérer la séquence)
void parametrages_Reseau() {
  Serial.println(" ");
  Serial.println("... Conversation avec Muybridge par µsini ...");
  Serial.println(" En cas de problème, contactez Rémi Sarrailh");

  WiFi.mode(WIFI_STA);
  WiFi.begin(point_WiFi, mot_De_Passe_WiFi);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("[📶]Connexion au WiFi en cours");
  }
  Serial.print("[📶] Adresse IP : ");
  Serial.println(WiFi.localIP().toString());
  Udp.begin(port_ESP32); // Démarrage de la communication UDP
}
