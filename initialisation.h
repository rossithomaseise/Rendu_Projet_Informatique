#ifndef __H_INITIALISATION_
#define __H_INITIALISATION_
#include "TicketToRideAPI.h"
/*Structure pour le plateau de jeu : variables constantes 
nombre de villes, nombres de chemins, tableau route??*/
typedef struct{
	int nbCities;
	int nbTracks; 
}s_plateau_jeu; 
/*structure pour le joueur */
typedef struct{
	int wagons_disponible; 
	int nb_carte_main;
	int nb_objectif;
	/*deck du joueur*/
	t_color cards[60]; 
	/*indique le nb de couleur de cartes en main (non dispo pour l'adversaire)*/
	int tab_carte_couleur[9]; 
	int score;
	int tab_objectif_ordre[10];
	int objectif_rempli; /* =0 si aucun rempli, = 1 si un objectif, = 2 si 2 et on redémarre (je ne prends que 2 obj)*/
	int tab_chemin_obj[10];
	/*permet de savoir pour le bot combien de carte de la couleur il utilise en + des locomotives
	je l'utilise pour le décompte des cartes*/
	int carte_utilise;
	/*Pour piocher une carte face visible, on dit la carte que l'on veut pour la fonction*/
	t_color carte_a_pioche[2];
	int prochain_objectif;/*pour indiquer de passer à l'objectif suivant*/
}s_joueur;
/*Structure qui permet de mettre en paramètre pour jouer le coup prise de route ou la prise d'objectifs*/
typedef struct{
	int identifiant_coup;
	/*Données nécessaires pour CLAIM_ROUTE*/
	int city1;
	int city2;
	int color;
	int nbLocomotives;
	/*Variable pour la fonction drawObjectives*/
	t_objective obj[3];
	/*Variable pour la fonction chooseObjectives*/
	int objectiveCards[3];
}s_choixCoup;
/*structure pour les informations de la partie*/
typedef struct{
	/* Variable pour indiquer le 1er tour pour choisir au moins 2 objectifs
	=1 -> premeir tour */
	int premier_tour;
	int numero_tour;
	int numero_joueur_actuel; /*numéro du joueur en train de jouer*/
	int numero_joueur; /*numéro constant du joueur réel*/
	t_color faceUp[5]; /* cartes face visible*/
	int route_dispo[36][36]; /* id city1, id city2, 0 si disponible, 1 si prise */
	int route_distance[36][36];
	int route_couleur[36][36];
	int compteur_ville_relier;/*informe sur le nombre de route prise sur l'objectif*/
}s_partie;

void initialisation_jeu(int tracks[390],s_plateau_jeu* plateauJeu,s_joueur* joueur,s_joueur* adversaire,s_partie* partie,t_color deck[5]);
//void affichage_info_joueur(s_joueur* joueur);
void affichage_info_plateau_jeu(s_plateau_jeu* plateauJeu);
void affichage_info_partie(s_partie* partie);
/*Fonction qui demande à l'utilisateur de choisir un coup avec si nécessaires 
les données pour*/
void choix_coup(s_choixCoup* choixCoup);
/*Fonction qui choisit l'action a joué selon l'id sélectionné par choix_coup*/
t_return_code action_coup(int tracks[390],t_color deck[5],s_choixCoup* choixCoup,s_joueur* joueur,s_partie* partie);
/*Fonction qui compte les cartes selon leur couleur*/
void compteur_carte(t_color card,s_joueur* joueur);
void affiche_objectif(t_objective obj[3]);
void affichage_info_joueur(s_joueur* joueur);
void score_joueur(s_joueur* joueur,s_choixCoup* choixCoup,s_partie* partie);
/*Fonction pour la gestion des informations et mouvements de l'adversaire*/
void affichage_info_adversaire(s_joueur* adversaire);
void score_adversaire(s_joueur* adversaire,t_move* move,s_partie* partie);
void action_adversaire(t_move* move,s_joueur* adversaire,s_partie* partie);
#endif
