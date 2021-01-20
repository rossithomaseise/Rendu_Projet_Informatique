#include "TicketToRideAPI.h"
#include "initialisation.h"
/*Fonction qui permet de trouver le chemin le plus court pour un objectif*/
int distanceMini(int D[36],int Visite[36]);
void affiche_chemin_dest_src(int src, int dest,int Prec[36],s_joueur* joueur);
void algo_dijkstra(int src, int route_distance[36][36],int D[36],int Prec[36],int dest);
/*Fonction du bot 100% autonome*/
t_return_code mon_bot(s_partie* partie,s_choixCoup* choixCoup,s_joueur* joueur,int tracks[390],t_color deck[5]);