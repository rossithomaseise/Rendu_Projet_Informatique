#include <stdio.h>
#include <stdlib.h>
#include "initialisation.h"
/*Fonction de connexion au serveur, récupération des données et initialisation des paramètres*/
void initialisation_jeu(int tracks[390],s_plateau_jeu* plateauJeu,s_joueur* joueur,s_joueur* adversaire,s_partie* partie,t_color deck[5]){
	/* #### For the function connectToServer #### */
	/*serverName*/
	char* serverName = "li1417-56.members.linode.com";
	/*port number used for the connection*/
	unsigned int port = 5678; //5678 1234
	/*Name of the player*/
	char* name = "Thomas";
	/* #### For the function waitForT2RGame #### */
	/*Name of the game*/
	char *gameName = (char*) malloc(50*sizeof(char));
	/*the number of cities*/
	int nbCities ;
	/*the number of tracks between the cities*/
	int nbTracks ;
	/* #### For the function getMap #### */
	/*array of 4 t_colors with the initial cards in your hand*/
	t_color cards[4];
	/* ######## Connection to the server ######## */
	connectToServer(serverName,port,name);
	/* ######## Initialisation for cards, map and the beginner ######## */
	waitForT2RGame("TRAINING DO_NOTHING timeout=100 start=0 map=USA",gameName,&nbCities,&nbTracks);
	/* ## Initialisation du plateau ## */
	plateauJeu->nbCities = nbCities;
	plateauJeu->nbTracks = nbTracks;
	/*######## Initialisation for the map 
	Get the map, the decks and initial cards and tell who starts ######## */
	/*######## Initialisation des paramètres de la partie : 
	cartes face visible et n° joueur ########*/
	partie->numero_joueur_actuel = getMap(tracks,deck,cards);
	/*Premier tour*/
	partie->premier_tour = 1;
	/*numéro du tour*/
	partie->numero_tour = 0;
	/*Initialisation du compteur sur le nombre de ville reliées*/
	partie->compteur_ville_relier = 0;
	/*numéro du joueur*/
	if(!partie->numero_joueur_actuel){/* =0 => on commence */
		partie->numero_joueur = 0;
	}else{ /* =1 => l'adversaire commence */
		partie->numero_joueur = 1;
	}
	/* cartes face visible*/
	for(int i=0;i<5;i++){
		partie->faceUp[i] = deck[i];
	}
	/*Initialisation à 100 de la distance si route n'existe pas*/
	for(int i=0;i<36;i++){
		for(int j=0;j<36;j++){
			partie->route_distance[i][j] = 100;
		}
	}
	for(int i=0;i<78;i++){
		int indice_city1 = tracks[5*i];
		int indice_city2 = tracks[5*i+1];
		int distance = tracks[5*i+2];
		int couleur = tracks[5*i+3];
		partie->route_distance[indice_city1][indice_city2] = distance;
		partie->route_couleur[indice_city1][indice_city2] = couleur;
	}
	/*Initialisation à 0 => toutes les routes sont disponibles*/
	for(int i=0;i<36;i++){
		for(int j=0;j<36;j++){
			partie->route_dispo[i][j] = 0;
			/*longueur = infini si non comprise entre 0 et 6*/
			if( partie->route_distance[i][j] > 6 || partie->route_distance[i][j] <= 0){
				partie->route_distance[i][j] = 100;
			}
			/*couleur = infini si non comprise entre 0 et 8*/
			if( partie->route_couleur[i][j] > 9 || partie->route_couleur[i][j] <= 0){
				partie->route_couleur[i][j] = 100;
			}
		}
	}
	/*Je recopie pour chaque possibilité cad ville 1 -> ville 2 et ville2 -> ville1*/
	for(int i=0;i<36;i++){
		for(int j=0;j<36;j++){
			partie->route_distance[j][i] = partie->route_distance[i][j];
			partie->route_couleur[j][i] = partie->route_couleur[i][j];
		}
	}
	/* ## Initialisation des paramètres du joueur */
	joueur->wagons_disponible = 45;
	joueur->nb_carte_main = 4;
	joueur->nb_objectif = 0;
	joueur->score = 0;
	joueur->objectif_rempli = 0;
	// joueur->carte_utilise = 0;
	// joueur->prochain_objectif = 0;
	/*initialisation à zéro*/
	for(int i=0;i<9;i++){
		joueur->tab_carte_couleur[i] = 0;
	}
	/*initialisation à 100*/
	for(int i=0;i<10;i++){
		joueur->tab_objectif_ordre[i] = 100;
	}
	// for(int i=0;i<60;i++){
	// 	joueur->cards[i] = 100;
	// }
	// joueur->carte_a_pioche[0] = 0;
	// joueur->carte_a_pioche[1] = 0;
	/*on va incrémenter +1 par carte de couleur */
	for(int i=0;i<4;i++){
		joueur->cards[i] = cards[i];/*on récupère ici les 4 cartes avec leurs couleurs*/
		/*Fonction qui compte les cartes selon leur couleur*/
		compteur_carte(cards[i],joueur);
	}
	/* ## Initialisation des paramètres du joueur */
	adversaire->wagons_disponible = 45;
	adversaire->nb_carte_main = 4;
	adversaire->nb_objectif = 0;
}

void affichage_info_plateau_jeu(s_plateau_jeu* plateauJeu){
	printf("\n\nIl y a %d villes\n",plateauJeu->nbCities);
}
void affichage_info_partie(s_partie* partie){
	printf("\n\n Le numéro du joueur actuel est %d\n",partie->numero_joueur_actuel);
	printf("Notre numéro de joueur est %d\n",partie->numero_joueur);
	printf("On a comme couleur de cartes face visible :   ");
	for(int i=0;i<5;i++){
		printf("%d   ",partie->faceUp[i]);
	}
}
/*Fonction qui demande à l'utilisateur de choisir un coup avec si nécessaires 
les données pour*/
void choix_coup(s_choixCoup* choixCoup){
	printf("\n\nVeuillez choisir un coup :claim_ROUTE = 1,DRAW_BLIND_CARD = 2,DRAW_CARD = 3,DRAW_OBJECTIVES = 4,CHOOSE_OBJECTIVES = 5 \n");
	/*L'utilisateur choisit le coup par son id*/
	//scanf("%d",&choixCoup->identifiant_coup);
	/*Si le joueur réclame une route, il faut les données nécessaires pour*/
	if(choixCoup->identifiant_coup == 1){
		printf("Entrez : id city1, id city2, color et nb locomotives");
		//scanf("%d %d %d %d",&choixCoup->city1,&choixCoup->city2,&choixCoup->color,&choixCoup->nbLocomotives);
	}
 }
/*Fonction qui permet de jouer un coup*/ 
t_return_code action_coup(int tracks[390],t_color deck[5],s_choixCoup* choixCoup,s_joueur* joueur,s_partie* partie){
	/*Variable pour enregistrer la valeur de retour de la fonction choisie*/
	t_return_code code;
	/*Variable pour la fonction drawCard*/
	t_color card;
	/*Variable pour la fonction drawBlindCard*/
	t_color* card_pioche = (t_color*)malloc(sizeof(int));
	/*Variable pour la fonction claimRoute*/
	int position_tracks = 0;
	/*On regarde si on a bien src < ville après sinon on inverse car il nous faut city 1 < city 2*/
	int city1 = choixCoup->city1;
	int city2 = choixCoup->city2;
	if(city1 > city2){
		int remplacement = city1;
		city1 = city2;
		city2 = remplacement;
	}
	if(choixCoup->identifiant_coup == 1){
		code = claimRoute(city1,city2,choixCoup->color,choixCoup->nbLocomotives);
		/* On regarde où se situe les informations de la route choisie*/
		for(int i =0; i<78;i++){
			if(tracks[5*i] == /*city1*/ choixCoup->city1 ){
				position_tracks = i*5 + 2;
			}
		}
		/*On effectue alors le décompte des cartes selon leur couleurs*/
		if(choixCoup->color == 1){
			joueur->tab_carte_couleur[0]-=joueur->carte_utilise;
		}
		else if(choixCoup->color == 2){
			joueur->tab_carte_couleur[1]-=joueur->carte_utilise;
		}
		else if(choixCoup->color  == 3){
			joueur->tab_carte_couleur[2]-=joueur->carte_utilise;
		}
		else if(choixCoup->color  == 4){
			joueur->tab_carte_couleur[3]-=joueur->carte_utilise;
		}
		else if(choixCoup->color  == 5){
			joueur->tab_carte_couleur[4]-=joueur->carte_utilise;
		}
		else if(choixCoup->color  == 6){
			joueur->tab_carte_couleur[5]-=joueur->carte_utilise;
		}
		else if(choixCoup->color  == 7){
			joueur->tab_carte_couleur[6]-=joueur->carte_utilise;
		}
		else if(choixCoup->color  == 8){
			joueur->tab_carte_couleur[7]-=joueur->carte_utilise;
		}
		else if (choixCoup->color == 9){
			joueur->tab_carte_couleur[8]-=joueur->carte_utilise;	
		}
		/*On décompte le nombre de locomotives*/
		if(choixCoup->nbLocomotives>0){
			joueur->tab_carte_couleur[8]-=choixCoup->nbLocomotives;
		}
		/*On effectue le décompte des wagons disponibles*/
		joueur->wagons_disponible =joueur->wagons_disponible -joueur->carte_utilise - choixCoup->nbLocomotives;
		joueur->nb_carte_main = joueur->nb_carte_main -joueur->carte_utilise - choixCoup->nbLocomotives;
		/*on rend non disponible la route prise*/
		partie->route_dispo[choixCoup->city1][choixCoup->city2] = 1;
		score_joueur(joueur,choixCoup,partie);
	}
	
	/*Coup = piocher une carte dans la pioche*/
	else if(choixCoup->identifiant_coup == 2){
		for(int i=0;i<2;i++){
			code = drawBlindCard(card_pioche);
			joueur->nb_carte_main ++;
			card = *card_pioche;
			/*Fonction qui compte les cartes selon leur couleur*/
			compteur_carte(card,joueur);
		}
	}
	/*Coup = piocher une carte face visible*/
	else if(choixCoup->identifiant_coup == 3){
		for(int i=0;i<2;i++){
			//printf("Entrez la couleur de carte");
			//scanf("%d",&card);
			code = drawCard(joueur->carte_a_pioche[i],partie->faceUp);
			joueur->nb_carte_main ++;
			/*Fonction qui compte les cartes selon leur couleur*/
			compteur_carte(joueur->carte_a_pioche[i],joueur);
			/*Si la carte est multicolor, on ne re-pioche pas*/
			if(joueur->carte_a_pioche[0] == MULTICOLOR){
				i++;
			}
		}
	}
	/*Coup = piocher et choisir un objectif*/
	else if(choixCoup->identifiant_coup == 4){
		/*On pioche un objectif*/
		/*Le drawObjectives est présent dans la fonction bot afin de sélectionner les obj*/
		//drawObjectives(choixCoup->obj);
		affiche_objectif(choixCoup->obj);
		//printf("Entrez 1 pour prendre le ou les objectifs et 0 sinon");
		//scanf("%d %d %d",&choixCoup->objectiveCards[0],&choixCoup->objectiveCards[1],&choixCoup->objectiveCards[2]);
		/*Condition piocher au moins 2 obj au 1er tour*/
		if(partie->premier_tour){
			while((choixCoup->objectiveCards[0]+choixCoup->objectiveCards[1]+choixCoup->objectiveCards[2])<2){
				printf("Au premier tour, il faut piocher au moins 2 objectifs");
				//scanf("%d %d %d",&choixCoup->objectiveCards[0],&choixCoup->objectiveCards[1],&choixCoup->objectiveCards[2]);
			}
			partie->premier_tour = 0;
		}
		/*Condition piocher au moins 1 obj si pas premier tour*/
		if(!partie->premier_tour){
			while((choixCoup->objectiveCards[0]+choixCoup->objectiveCards[1]+choixCoup->objectiveCards[2])<1){
				printf("Il faut piocher au moins 1 objectif");
				//scanf("%d %d %d",&choixCoup->objectiveCards[0],&choixCoup->objectiveCards[1],&choixCoup->objectiveCards[2]);
			}
		}
		code = chooseObjectives(choixCoup->objectiveCards);
		joueur->nb_objectif += choixCoup->objectiveCards[0]+choixCoup->objectiveCards[1]+choixCoup->objectiveCards[2];
	}
	return code;
}
/*Fonction pour compter les cartes présentes dans action_coup*/
void compteur_carte(t_color card,s_joueur* joueur){
	if(card == PURPLE){
		joueur->tab_carte_couleur[0]+=1;
	}
	else if(card== WHITE){
		joueur->tab_carte_couleur[1]+=1;
	}
	else if(card == BLUE){
		joueur->tab_carte_couleur[2]+=1;
	}
	else if(card == YELLOW){
		joueur->tab_carte_couleur[3]+=1;
	}
	else if(card == ORANGE){
		joueur->tab_carte_couleur[4]+=1;
	}
	else if(card == BLACK){
		joueur->tab_carte_couleur[5]+=1;
	}
	else if(card == RED){
		joueur->tab_carte_couleur[6]+=1;
	}
	else if(card == GREEN){
		joueur->tab_carte_couleur[7]+=1;
	}
	else if(card == MULTICOLOR){
		joueur->tab_carte_couleur[8]+=1;
	}
}
/*Fonction qui permet d'afficher les objectifs lors d'un drawObjectif*/
void affiche_objectif(t_objective obj[3]){
	printf("\nVoici la liste des objectifs : \n");
	for(int i=0;i<3;i++){
		printf("Obj n° %d, city1 = %d ; city2 = %d ; score = %d\n",i,obj[i].city1,obj[i].city2,obj[i].score);
	}
}
/*Fonction qui affiche les informations du joueur*/
void affichage_info_joueur(s_joueur* joueur){
	printf("\n\nIl vous reste %d wagons disponibles\n",joueur->wagons_disponible);
	printf("Vous avez %d objectif\n",joueur->nb_objectif);
	printf("Vous avez %d cartes en main\n",joueur->nb_carte_main);
	printf("Vous avez un score de %d\n",joueur->score);
	printf("Vous avez : ");
	printf("%d purple;  ",joueur->tab_carte_couleur[0]);
	printf("%d white;  ",joueur->tab_carte_couleur[1]);
	printf("%d blue;  ",joueur->tab_carte_couleur[2]);
	printf("%d yellow;  ",joueur->tab_carte_couleur[3]);
	printf("%d orange;  ",joueur->tab_carte_couleur[4]);
	printf("%d black;  ",joueur->tab_carte_couleur[5]);
	printf("%d red;  ",joueur->tab_carte_couleur[6]);
	printf("%d green;  ",joueur->tab_carte_couleur[7]);
	printf("%d multicolor \n\n",joueur->tab_carte_couleur[8]);	
}
/*Fonction qui permet de calculer le score du joueur*/
void score_joueur(s_joueur* joueur,s_choixCoup* choixCoup,s_partie* partie){
	/*On augmente le score lors de la prise d'une route*/
	if(choixCoup->identifiant_coup == 1){
		/*Le score varie selon la longueur de la route prise*/
		if(partie->route_distance[choixCoup->city1][choixCoup->city2] == 1){
			joueur->score += 1;
		}
		if(partie->route_distance[choixCoup->city1][choixCoup->city2] == 2){
			joueur->score += 2;
		}
		if(partie->route_distance[choixCoup->city1][choixCoup->city2] == 3){
			joueur->score += 4;
		}
		if(partie->route_distance[choixCoup->city1][choixCoup->city2] == 4){
			joueur->score += 7;
		}
		if(partie->route_distance[choixCoup->city1][choixCoup->city2] == 5){
			joueur->score += 10;
		}
		if(partie->route_distance[choixCoup->city1][choixCoup->city2] == 6){
			joueur->score += 15;
		}
	}
}
/*Fonction qui affiche les informations de l'adversaire*/
void affichage_info_adversaire(s_joueur* adversaire){
	printf("\n\nL'adversaire a %d wagons disponibles\n",adversaire->wagons_disponible);
	printf("L'adversaire a %d objectif\n",adversaire->nb_objectif);
	printf("L'adversaire a %d cartes en main\n",adversaire->nb_carte_main);
	printf("L'adversaire a un score de %d\n",adversaire->score);
}
/*Fonction qui permet de calculer le score de l'adversaire*/
void score_adversaire(s_joueur* adversaire,t_move* move,s_partie* partie){
	/*On augmente le score lors de la prise d'une route*/
	if(move->type == 1){
		/*Le score varie selon la longueur de la route prise*/
		if(partie->route_distance[move->claimRoute.city1][move->claimRoute.city2] == 1){
			adversaire->score += 1;
		}
		if(partie->route_distance[move->claimRoute.city1][move->claimRoute.city2] == 2){
			adversaire->score += 2;
		}
		if(partie->route_distance[move->claimRoute.city1][move->claimRoute.city2] == 3){
			adversaire->score += 4;
		}
		if(partie->route_distance[move->claimRoute.city1][move->claimRoute.city2] == 4){
			adversaire->score += 7;
		}
		if(partie->route_distance[move->claimRoute.city1][move->claimRoute.city2] == 5){
			adversaire->score += 10;
		}
		if(partie->route_distance[move->claimRoute.city1][move->claimRoute.city2] == 6){
			adversaire->score += 15;
		}
	}
}
/*Fonction qui prend en compte les mouvements de l'adversaire*/
void action_adversaire(t_move* move,s_joueur* adversaire,s_partie* partie){
	if(move->type == 1){
		score_adversaire(adversaire,move,partie);
		/*On ne rend plus disponible la route prise*/
		partie->route_dispo[move->claimRoute.city1][move->claimRoute.city2] = 1;
		/*On effectue le décompte des wagons de l'adversaire*/
		adversaire->wagons_disponible -= partie->route_distance[move->claimRoute.city1][move->claimRoute.city2];
		/*On effectue le décompte des cartes en main de l'adversaire*/
		adversaire->nb_carte_main -= partie->route_distance[move->claimRoute.city1][move->claimRoute.city2]; 
	}
	/*Si l'adversaire pioche une carte dans le deck*/
	if(move->type == 2){
		/*On augmente de +1 le nb de carte de l'adversaire*/
		adversaire->nb_carte_main ++;
	}
	/*Si l'adversaire pioche une carte face visible*/
	if(move->type == 3){
		/*On augmente de +1 le nb de carte de l'adversaire*/
		adversaire->nb_carte_main ++;
		for(int i=0;i<5;i++){
				partie->faceUp[i] = move->drawCard.faceUp[i];
		}
	}
}
