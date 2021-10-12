
/*
  Conversations avec Muybridge
  Code d'origine : Michel Glaize
  Modifications : Rémi Sarrailh (µsini)
  **************************************
  **** 🌉 Gestion du du Moteur      ****
  **************************************

  ⚙ Le moteur est contrôlé par un pilote (drivers) MKS Servo57B
  📝 Manuel et Firmware   - https://github.com/makerbase-mks/MKS-SERVO57B
  📼 Vidéo de Barbatronic - https://www.youtube.com/watch?v=VgVyZDCxjyo&t=2393s

  Le moteur est un Nema23 alimenté en 24v

  La bibliothèque AccelStepper est utilisé, elle permet de gérer
  l'accélération et le contrôle de celui-ci
  📕 Réferentiel AccelStepper : https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.htm
*/

#include <AccelStepper.h>

AccelStepper moteur(AccelStepper::DRIVER, PAS_MOTEUR, DIRECTION_MOTEUR);
bool retour_Train = false; // Le train est-il en train de revenir à sa position zéro ?
bool zero_Connu = false;   // La position zéro est t'elle valide (Le train est arrivé sur la fin de courses)
bool erreur_Zero = false; // La position zéro est incorrecte et doit être corrigé prudemment
float derniere_Position_Moteur = -1.0;

String sequence = ""; // Séquence OSC
bool sequence_Choisi = false; // Active ou désactive les messages envoyé depuis l'IPAD
int sequenceID = 0;
bool reinitialisationSequence = true;
int etape_Precedente = -2;  //  numéro de l'étape précedente activée, initialisé à -1 pour indiquer qu'aucune étape n'est activée
int etape = -2; //  numéro de l'étape activée, initialisé à -1 pour indiquer qu'aucune étape n'est activée
int compteur_Etape = 0; //  Moyenne des étapes detecté
bool nouvelle_Etape = false; // Changement d'étape detecté
int depart_Parcours = 0; // Un utilisateur a été detecté
int absence_Utilisateur = 0;
int tableauEtape[5];
int integrite_Etape;
int etape_Effectue = -2;

/* 🚂 Mise en route du moteur  */

void test_Fin_De_Courses() {
  Serial.print("[🚂] Test Fin de Courses");
  Serial.println(" --->");
  Serial.println(digitalRead(FIN_DE_COURSE));
  delay(1000);
  Serial.print(" --->");
  Serial.println(digitalRead(FIN_DE_COURSE));
  delay(1000);
}

// Active le moteur, et le fin de course
void parametrages_Moteur() {
  Serial.println("[🚂] Activation des moteurs / fin de courses");
  pinMode(FIN_DE_COURSE, INPUT); // Met le fin de course en entrée
  pinMode(MOTEUR_ACTIF, OUTPUT); // Met la broche Moteur Actif en sortie
  digitalWrite(MOTEUR_ACTIF, LOW); // Met la broche Moteur Actif en LOW (actif)

  Serial.print("  ---- Vitesse: ");
  Serial.println(MOTEUR_VITESSE);
  Serial.print("  ---- Accélération: ");
  Serial.println(MOTEUR_ACCELERATION);
  moteur.setMaxSpeed(MOTEUR_VITESSE); // Réglage de la vitesse maximal du moteur
  moteur.setAcceleration(MOTEUR_ACCELERATION); // Réglage de l'accélération
  //test_Fin_De_Courses();
}

/*
************************************
**** ♦ Gestion du fin de courses ***
************************************
*/

// Vérifie si le train est au début du parcours
void verification_Fin_De_Courses() {

  // Le fin de courses à une logique inversé (LOW = Actif)
  if(!digitalRead(FIN_DE_COURSE)) {
    //Serial.println("Train en gare");
    // Si le train se dirige en arrière
    if (moteur.targetPosition() > 0) {
          Serial.println("[🚂] Correction Position Zéro (Fin de courses activé) (mode rapide réactivé)");
          moteur.setCurrentPosition(0);
          moteur.setSpeed(MOTEUR_VITESSE);
          moteur.setAcceleration(MOTEUR_ACCELERATION);
    } else {
        if(retour_Train) {
          Serial.println("[🚂] Position Zéro Correcte - Désactivation du moteur");
          digitalWrite(MOTEUR_ACTIF, HIGH);
          retour_Train = false;
        }
      }
  } else { // Si le fin de courses n'est pas activé
    // Si le train est en train de revenir
    if(retour_Train) {
      //Serial.println(moteur.currentPosition());
      // Mais qu'il ne recule plus
      if(moteur.distanceToGo() == 0){

        // Et que sa position est 0
        if(moteur.currentPosition() == -10) {
          Serial.println("[🚂] Correction Position Zéro nécessaire (mode lent)");
          // Le train se met en mode lent afin d'essayer de rentrer plus doucement
          moteur.setSpeed(500);
          moteur.setAcceleration(500);
          moteur.moveTo(100000000);
        }
      }
    }
  }
}


void reinit_Parcours() {
  Serial.println("[🚂] Parcours réinitialisée");
  sequence = "";
  sequence_Choisi = false; // Active ou désactive les messages envoyé depuis l'IPAD
  sequenceID = 0;
  etape_Precedente = -2;  //  numéro de l'étape précedente activée, initialisé à -1 pour indiquer qu'aucune étape n'est activée
  etape = -2; //  numéro de l'étape activée, initialisé à -1 pour indiquer qu'aucune étape n'est activée
  compteur_Etape = 0; //  Moyenne des étapes detecté
  nouvelle_Etape = false; // Changement d'étape detecté
  depart_Parcours = 0; // Un utilisateur a été detecté
  absence_Utilisateur = 0;
  reinitialisationSequence = false;
  etape_Effectue = -2;
}

void remise_Zero_Train() {
  // Remise à zéro du train
  retour_Train = true; // Prévient le fin de courses que le train est de retour
  Serial.println("[🚂] Retour en cours");
  reinit_Parcours(); // Réinitialisation des variables du parcours
  moteur.moveTo(-10); // Déplace le train vers la position zéro (théorique)
}
