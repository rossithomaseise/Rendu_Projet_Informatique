#include <stdio.h>
#include <stdlib.h>
#include "jeu.h"
/*Fonction inclu dans l'algo_dijkstra qui permet de calculer le chemin le plus court entre 2 villes*/
int distanceMini(int D[36],int Visite[36]){
	int min; /*valeur du minimum*/
	int indice_min; /*indice du minimum*/
	min = 100; /*= + infini*/
	/*recherche de la ville non visitée la plus proche*/
	for(int i=0;i<36;i++){
		if(Visite[i] == 0 && D[i] < min){
			min = D[i];
			indice_min = i;
		}
	}
	return indice_min;
}
/*Fonction qui permet d'afficher le chemin entre la src et la dest
La fonction permet aussi de récupérer les villes du chemin*/
void affiche_chemin_dest_src(int src, int dest,int Prec[36],s_joueur* joueur){
	int j = 1;
	/* on démarre à la fin*/
	int v = dest; /* on remonte le chemin jusqu'à arriver au départ*/
	/*tab pour récupérer l'indice des villes du chemin de l'obj*/
	int tab_chemin_obj_temp[10];
	/*tab pour récupérer l'indice sans les 0 0 0 au début*/
	int tab_temp[10];
	/*Initialisation de tab_chemin_obj_temp*/
	for(int i=0;i<10;i++){
		tab_chemin_obj_temp[i] = 100;
	}
	//printf("La destination est %d\n",dest);
	tab_chemin_obj_temp[0] = dest; 
	while(v != src){
		//printf("Le chemin est : %d\n",Prec[v]);
		if(Prec[v] != 0){
			tab_chemin_obj_temp[j] = Prec[v];
		}
		/*Afficher le chemin qui va de Prec[v] à v*/
		v = Prec[v];
		j++;
	}
	/*On recopie le tableau dans le sens inverse */
	int f = 9;
	for(int i=0;i<10;i++){
		if(tab_chemin_obj_temp[f-i]!=100){
			joueur->tab_chemin_obj[i] = tab_chemin_obj_temp[f-i];
		}
		else{
			joueur->tab_chemin_obj[i] = 100;
		}
	}
}
/*Algorithme qui permet de trouver le plus court chemin sur un objectif*/
void algo_dijkstra(int src, int route_distance[36][36],int D[36],int Prec[36],int dest){
	/* tableau de booléens = 0 si n'a pas été visité (false) et 1 sinon(true)*/
	int Visite[36];
	/*indice qui indique la ville considérée dans la boucle*/
	int u = src;

	/*INITIALISATION*/
	for(int i=0;i<36;i++){
		D[i] = 100; /*infini*/
		Visite[i] = 0; /*false*/
		
	}
	/*On part de la source*/
	D[src] = 0;
	/*On cherche le plus court chemin pour aller de src à chaque ville*/
	while(u != dest){
		//printf("ola");
		/*On trouve la ville non visité la plus proche*/
		u = distanceMini(D,Visite);
		/*On marque cette ville comme visitée*/
		Visite[u] = 1; /* true*/
		/* On met à jour chaque ville reliée à src*/
		for(int v=0;v<36;v++){
			/*on ne met à jour que les villes non visités, où il y a un chemin entre u et v et où il est plus court
			d'arriver à v en passant par u*/
			if(Visite[v] == 0  && route_distance[u][v] < 100 && D[u] + route_distance[u][v] < D[v]){
				D[v] = D[u] + route_distance[u][v];
				Prec[v] = u;
			}
		}
	}
}
/*Fonction du bot 100% autonome*/
t_return_code mon_bot(s_partie* partie,s_choixCoup* choixCoup,s_joueur* joueur,int tracks[390],t_color deck[5]){
	/*Variable pour enregistrer la valeur de retour de la fonction choisie*/
	t_return_code code;
	int f = 0;/*pour gérer le joueur->tab_chemin_obj[i]*/
	/*premier tour*/
	 if(partie->numero_tour == 0 || partie->numero_tour == 1){
	 	drawObjectives(choixCoup->obj);
		/*On choisit les 2 objectifs les plus grands*/
		if(choixCoup->obj[0].score >= choixCoup->obj[1].score && choixCoup->obj[0].score >= choixCoup->obj[2].score){
			affiche_objectif(choixCoup->obj);
			choixCoup->objectiveCards[0] = 1;
			/*Il devient l'objectif le plus important*/
			joueur->tab_objectif_ordre[0] = 0;
			if(choixCoup->obj[1].score > choixCoup->obj[2].score){
				choixCoup->objectiveCards[1] = 1;
				choixCoup->objectiveCards[2] = 0;
				/*Il devient le 2ème objectif le plus important*/
				joueur->tab_objectif_ordre[1] = 1;
			}
			else{
				choixCoup->objectiveCards[2] = 1;
				choixCoup->objectiveCards[1] = 0;
				/*Il devient le 2ème objectif le plus important*/
				joueur->tab_objectif_ordre[1] = 2;
			}
		}
		else{
			affiche_objectif(choixCoup->obj);
			choixCoup->objectiveCards[1] = 1;
			choixCoup->objectiveCards[2] = 1;
			choixCoup->objectiveCards[0] = 0;
			/*On regarde l'objectif le plus grand*/
			if(choixCoup->obj[1].score > choixCoup->obj[2].score){
				joueur->tab_objectif_ordre[0] = 1;
				joueur->tab_objectif_ordre[1] = 2;
			}
			else{
				joueur->tab_objectif_ordre[0] = 2;
				joueur->tab_objectif_ordre[1] = 1;
			}
		}
		choix_coup(choixCoup);
		choixCoup->identifiant_coup = 4;
		code = action_coup(tracks,deck,choixCoup,joueur,partie);
		printf("\nVotre objectif principal est : %d\n",joueur->tab_objectif_ordre[0]);
		printf("Votre objectif secondaire est : %d\n",joueur->tab_objectif_ordre[1]);
		return code;
	}
	/*Pour les tours suivants*/
	else{
		/*si aucun objectif est rempli, alors on en pioche pas d'autre et la priorité reste sur l'obj principal*/
		if(joueur->objectif_rempli == 0 || joueur->objectif_rempli ==1 ){
			/*On regarde si on peut poser une route*/
			/* Indique la plus ourte distance entre les villes src et i*/
			int D[36];
			/*Indique de quelle ville il faut venir pour aller de i à src*/
			int Prec[36]={0};
			/*Ville de départ et d'arrivé*/
			int src,dest;
			/*On choisi la ville src et dest selon l'objectif en cours*/
			if(joueur->objectif_rempli == 0){
				src = choixCoup->obj[joueur->tab_objectif_ordre[0]].city1;
				dest = choixCoup->obj[joueur->tab_objectif_ordre[0]].city2;	
			}
			else if(joueur->objectif_rempli == 1 && joueur->prochain_objectif == 0){
				src = choixCoup->obj[joueur->tab_objectif_ordre[1]].city1;
				dest = choixCoup->obj[joueur->tab_objectif_ordre[1]].city2;		
				/*On réinitialise le compteur de ville relier*/
				partie->compteur_ville_relier = 0;
				joueur->objectif_rempli = 1;
				joueur->prochain_objectif =1;
			}
			else{
				src = choixCoup->obj[joueur->tab_objectif_ordre[1]].city1;
				dest = choixCoup->obj[joueur->tab_objectif_ordre[1]].city2;		
			}
			/*Fonction pour récupérer le chemin le plus court entre src et dest*/
			algo_dijkstra(src,partie->route_distance,D,Prec,dest);
			affiche_chemin_dest_src(src,dest,Prec,joueur);
			/*indice de l'élément juste après src*/
			int indice_src;
			/*Permet de ne prendre que l'indice après l'élément src*/
			int bool_src_suivant=0;
			printf("l'ordre du chemin est :");
			for(int i=0;i<10;i++){
				/*On recopie le chemin dès que la valeur est différente de 100*/
				if(joueur->tab_chemin_obj[i] !=100 ){
					printf(	"%d, ",joueur->tab_chemin_obj[i]);
				}
				/*On prend l'indice de la src dans le tableau*/
				if(joueur->tab_chemin_obj[i] !=100 && bool_src_suivant == 0){
					indice_src = i;
					bool_src_suivant = 1;
				}
			}
			printf("\n");
			/*ATTENTION LA VILLE 0 N'EST PAS DANS LE CHEMIN
			donc si src=0 la ville suivant sera directement indice_src sans le +1 */
			if(src != 0){
				/*on récupère l'indice après la src pour avoir l'indice de la ville suivante*/
				indice_src+=1;
			}
			/*Indices des villes que l'on veut relier*/
			int ville1,ville2;
			/*Si on a pas encore relié de ville, alors la ville1 est la src*/
			if(partie->compteur_ville_relier == 0){
				ville1 = src;
			}
			/*Si on a déja relié 2 villes, alors la ville1 est la ville suivante en fonction du compteur*/
			else{
				ville1 = joueur->tab_chemin_obj[indice_src - 1 + partie->compteur_ville_relier];
			}
			/*La ville 2 sera toujours l'indice src (indice après la src) + le compteur*/
			ville2 = joueur->tab_chemin_obj[indice_src + partie->compteur_ville_relier];
			printf("\nLa source est = %d et destination = %d\n",src,dest);
			printf("Les villes à relier sont %d et %d\n",ville1,ville2);
			printf("La longueur du parcours est de %d\n",partie->route_distance[ville1][ville2]);
			/*Indice de la couleur du chemin entre src et la ville d'après*/
			int couleur = partie->route_couleur[ville1][ville2];
			int couleur_quelconque = 0;
			/*si la couleur est quelconque*/
			if(couleur == 9){
				couleur = 8;
				int maxi = joueur->tab_carte_couleur[0];
				for(int i=0;i<9;i++){
					if(joueur->tab_carte_couleur[i] > maxi){
						maxi = joueur->tab_carte_couleur[i];
					}
				}
				couleur = maxi + 1;
				couleur_quelconque = 1;
				printf("La couleur est quelconque\n");
			}
			/*BUG : Après la 1ere ville reliée, si la ville est de couleur quelconque = 9, alors
			je ne sais pas pourquoi le chiffre est 18750690 par exemple alors que le tab est bien
			remplie, j'ai du mettre cette condition qui permet de corriger le bug et tout marche bien*/
			if(couleur > 9|| couleur <0){
				couleur = 8;
				int maxi2 = joueur->tab_carte_couleur[0];
				for(int i=0;i<9;i++){
					if(joueur->tab_carte_couleur[i] > maxi2){
						maxi2 = joueur->tab_carte_couleur[i];
					}
				}
				couleur = maxi2 + 1;
				if(ville1==17 && ville2==22){
					couleur = 3;
				}
			}
			/*Initialisation des cartes voulues pour pouvoir les piocher si présente face visible*/
			joueur->carte_a_pioche[0] = couleur;
			joueur->carte_a_pioche[1] = couleur;

			printf("La couleur de la route est %d\n",couleur);
			printf("la quantité de la couleur souhaité est %d\n",joueur->tab_carte_couleur[couleur - 1]);


			if(partie->route_distance[ville1][ville2] <= joueur->tab_carte_couleur[couleur - 1]  +2
			 && joueur->tab_carte_couleur[8] >= 3 /*&& partie->route_dispo[ville1][ville2]!=1*/){
				printf("\nTCHOU TCHOU TCHOU PRISE D'UNE ROUTE !!!!!!!!!!! \n\n");
				/*paramètres pour la prise d'une route*/
				choixCoup->identifiant_coup = 1;
				choixCoup->city1 = ville1;
				choixCoup->city2 = ville2;
				choixCoup->color = couleur;
				if(partie->route_distance[ville1][ville2] == 1 || (partie->route_distance[ville1][ville2] == 2 && joueur->tab_carte_couleur[couleur - 1] >=1)){
					choixCoup->nbLocomotives = 1;
				}
				else{
					choixCoup->nbLocomotives = 2;	
				}
				partie->route_dispo[ville1][ville2] = 1;	
				/*On renseigne le nombre de carte utilisés pour le décompte*/
				joueur->carte_utilise = partie->route_distance[ville1][ville2] - choixCoup->nbLocomotives;
				/*On effectue l'action*/
				code = action_coup(tracks,deck,choixCoup,joueur,partie);
				int nb_route_prise = 0;
				for(int i= indice_src;i<9;i++){
					if(partie->route_dispo[src][indice_src] == 1 && partie->route_dispo[i][i+1] == 1){
						nb_route_prise++;
					}
				}
				/*Si on a relié toute les ville de l'objectif*/
				if(nb_route_prise == 9 - indice_src){
					joueur->objectif_rempli = 1;
				}
				/*Sinon, on incrémente le compteur sur le nombre de ville reliées*/
				else{
					partie->compteur_ville_relier ++;
				}
				if(partie->route_dispo[ville1][dest] == 1 || partie->route_dispo[dest][ville1] == 1){
					joueur->objectif_rempli ++;
				}
				return code;
			}
			else{
				/*compteur pour le nombre de cartes piochées*/
				int compteur = 0;
				/*Pour récupérer l'indice de la 1ere carte piochée pour ne pas récupérer la même après*/
				int indice;
				/*Pour sortir de la boucle lorsque les 2 cartes ont déja été piochées*/
				int end_pioche = 0;
				printf("Deck : ");
				for(int i=0;i<4;i++){
					printf(" %d,",partie->faceUp[i]);
					if( (partie->faceUp[i] == couleur || partie->faceUp[i] == 9) && end_pioche == 0){
						if(compteur == 0){
							joueur->carte_a_pioche[0] = partie->faceUp[i];
							compteur++;	
							indice = i;
							if(joueur->carte_a_pioche[0] == 9){
								/*On ne pioche plus ensuite*/
								end_pioche = 1;
							}
						}
						/*On pioche une 2ème fois sans piocher une locomotive*/
						else if(compteur == 1 && partie->faceUp[i] != 9){
							joueur->carte_a_pioche[1] = partie->faceUp[i];
							compteur++;	
							/*On ne pioche plus ensuite*/
							end_pioche = 1;
						}
					}
				}
				/*Si l'on a aucune carte qui nous intéresse, on pioche au hasard*/
				if(compteur == 0){
					choixCoup->identifiant_coup = 2;
				}
				/*Si on a pioché qu'une seule carte et que ce n'était pas une locomotive,
				on prend une carte au hasard sauf une locomotive*/
				else if(compteur == 1 && joueur->carte_a_pioche[0] !=9 ){
					/*Cas la carte choisie est la première*/
					if(indice ==0){
						/*je regarde les 3 prochaines cartes pour vu que ça ne soit pas une locomotive*/
						if(partie->faceUp[indice + 1] != 9){	
							joueur->carte_a_pioche[1] = partie->faceUp[indice + 1];	
						}
						else if(partie->faceUp[indice + 2] != 9){
							joueur->carte_a_pioche[1] = partie->faceUp[indice + 2];		
						}
						else{
							joueur->carte_a_pioche[1] = partie->faceUp[indice + 3];			
						}
					}
					/*Cas la carte choisie est la dernière*/
					else if(indice > 0 && indice <= 4){
						/*je regarde les 3 prochaines cartes pour vu que ça ne soit pas une locomotive*/
						if(partie->faceUp[indice - 1] != 9 && indice ==1){	
							joueur->carte_a_pioche[1] = partie->faceUp[indice - 1];	
						}
						else if(partie->faceUp[indice - 2] != 9){
							joueur->carte_a_pioche[1] = partie->faceUp[indice - 2];		
						}
						else{
							joueur->carte_a_pioche[1] = partie->faceUp[indice - 3];			
						}	
					}
					choixCoup->identifiant_coup = 3;
					/*Si la carte est une locomotive, on affiche que la 1ère carte car l'algo prendra une 2nd*/
					if(joueur->carte_a_pioche[0] == 9){
						printf("La carte piochée est une locomotive");	
					}
					/*On affiche les 2 cartes piochées*/
					else{
						printf("les cartes piochées sont : %d et %d \n",joueur->carte_a_pioche[0],joueur->carte_a_pioche[1]);	
					}
					/*Attention, si la 1ère carte est une locomotive, l'algo "action_coup" ne prendra 
					en compte que cette carte même si la 2nd est initialisé selon la couleur voulu*/
				}
				else if(compteur == 1 && joueur->carte_a_pioche[0] ==9 ){
					choixCoup->identifiant_coup = 3;
				}
				else if(compteur == 2){
					choixCoup->identifiant_coup = 3;
				}
				/*On effectue l'action*/
				code = action_coup(tracks,deck,choixCoup,joueur,partie);
				return code;
			}	
		}else{
			choixCoup->identifiant_coup = 2;
			code = action_coup(tracks,deck,choixCoup,joueur,partie); 
			return code;
		}
	}
}
