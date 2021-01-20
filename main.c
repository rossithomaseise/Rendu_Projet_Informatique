#include <stdio.h>
#include <stdlib.h>
#include "TicketToRideAPI.h"
#include "initialisation.h"
#include "jeu.h"
//gcc -g -c main.c TicketToRideAPI.c clientAPI.c initialisation.c jeu.c  && gcc -o main main.o TicketToRideAPI.o clientAPI.o initialisation.o jeu.o && ./main
int main(){
	/*#### définitions des structures #### */
	/*Structure pour le plateau de jeu : variables constantes 
	nombre de villes, nombres de chemins, tableau route??*/
	s_plateau_jeu* plateauJeu = (s_plateau_jeu*)malloc(2*sizeof(int));
	/*structure pour le joueur */
	s_joueur* joueur = (s_joueur*)malloc(9*sizeof(int)+62*sizeof(t_color) + sizeof(t_move));
	/*structure pour l'adversaire */
	s_joueur* adversaire = (s_joueur*)malloc(3*sizeof(int)+sizeof(t_color) +sizeof(t_move));
	/*structure pour les informations de la partie*/
	s_partie* partie = (s_partie*)malloc(4*sizeof(int)+sizeof(t_color));
	s_choixCoup* choixCoup = (s_choixCoup*)malloc(10*sizeof(int)+10*sizeof(t_objective));
	/* #### For the function getMap #### */
	/*array of (5 x number of tracks) integers*/
	int tracks[390];
	/* #### Variable pour les fonctions de notre joueur #### */
	t_color* card = (t_color*)malloc(sizeof(int));
	/*Pioche*/
	t_color deck[5];
	/* #### Variables pour la fonction get the opponent move #### */
	t_move* move = (t_move*)malloc(sizeof(t_typeMove));
	int* replay = (int*)malloc(sizeof(int));
	/*Variable qui contient la valeur de la fonction joué*/
	t_return_code code;
	/*Variable de notre position de départ, prend la valeur de getMap*/
	int start;
	int tab[78];
	initialisation_jeu(tracks,plateauJeu,joueur,adversaire,partie,deck);
	printMap();
	/*Avant de commencer la boucle de jeu, on fait piocher un objectif*/
	printf("\nAvant de commencer la partie, vous devez piocher un objectif\n");	
	while(1){
		/*Position de départ*/
		if(start==1){
			start = 2;
			/*Si c'est à l'adversaire de commencer*/
			goto depart;
		}
		/*Notre tour*/
		code = mon_bot(partie,choixCoup,joueur,tracks,deck);
		affichage_info_joueur(joueur);
		printMap();
		/*Conditions si le coup est gagnant, perdant ou si on continue*/
		if(code==NORMAL_MOVE){
		}else if(code==1){
			printf("Vous avez gagné");
			return code;
		}else{
			affichage_info_joueur(joueur);
			printf("Vous avez perdu");
			return code;
		}
		depart: /*Etiquette lorsque l'adversaire commence le jeu*/
		for(int i=0;i<2;i++){
			code = getMove(move,replay);
			action_adversaire(move,adversaire,partie);
			if(!replay){
				i++;
			}
			if(code==NORMAL_MOVE){
			}else if(code==WINNING_MOVE){
				printf("Vous avez perdu");
				return code;
			}else{
				printf("Vous avez gagné");
				return code;
			}
		}
		partie->numero_tour+=1;
		printMap();
	}
	affichage_info_joueur(joueur);
	closeConnection();
	/*On désalloue les pointeurs dynamiques*/
	free(adversaire);
	free(plateauJeu);
	free(joueur);
	free(partie);
	free(choixCoup);
	return 1;
}
