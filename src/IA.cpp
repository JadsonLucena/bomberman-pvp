/*

	Como é feliz o homem que acha a sabedoria, o homem que obtém entendimento. Provérbios 3:13

	A sabedoria se acha entre os idosos? A vida longa traz entendimento? "Deus é que tem sabedoria e poder; a ele pertencem o conselho e o entendimento. Jó 12:12-13

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LINHA 11
#define COLUNA 13


/*
	Default
	----------------------------------------------------------------------------
*/

// recebe os elementos do mapa
typedef struct {
	char celula[5], // recebe a celula completa
		gamer[3], // recebe o lado da celuda que fica os player
		bomba[3]; // recebe o lado da celuda que fica as bombas
} tabuleiro;

// recebe as posicoes de elementos do mapa
typedef struct {
	int linha, coluna;
} vertice;

// informacoes dos players
typedef struct {
	char nome[3], // nome do personagem
	bomba[3]; // nome da bomba
	int range, // distancia que a bomba alcança
	capacidadeBombas; // quantidade de bombas que o player pode soltar
	vertice posicao; // posiçao cartesiana do personagem
} gamer;

gamer player; // eu
gamer inimigo; // adversario

int coloqueiNbombas = 0; // recebe se foi posta uma bomba no mapa pelo player

// cria arquivos
FILE* numeroBombas; // salva o numero de bombas que o player pode soltar consecutivamente
FILE* rangeGamers; // salva o ranger dos gamers para ser utilizado na proxima rodada
FILE* mapaOld; // salva o mapa em binario para ser analizado na proxima rodada 

// mapa a ser lido pala IA
tabuleiro mapa[LINHA][COLUNA];

// direcoes possiveis, sentido horario
int dX[5] = { 0, 0, 1, 0, -1 }; // colunas
int dY[5] = { 0, -1, 0, 1, 0 }; // linhas

int peso(char no[5]) {

	// os valores impostos, sao as quantidades necessarias de passos para seguir
	if (strcmp(no, "MMMM") == 0)
		return 9; // 9 passos pos quando se bota a bomba fica parado, se semovimentasse seria 8

	return 1;
};

/*
	End Default
	----------------------------------------------------------------------------
*/



void identificaPlayers(int id) {
	if (id == 1) {
		strcpy(player.nome,"P1");
		strcpy(player.bomba,"B1");
		
		strcpy(inimigo.nome,"P2");
		strcpy(inimigo.bomba,"B2");
	} else {
		strcpy(inimigo.nome,"P1");
		strcpy(inimigo.bomba,"B1");

		strcpy(player.nome,"P2");
		strcpy(player.bomba,"B2");
	}
}


//funcao que realiza a leitura do mapa.
void geraMapa() {

	char temp[5]; // recebe o elemento de cada posicao do mapa
	
	for(int i = 0; i < LINHA; i++) {
		for(int j = 0; j < COLUNA; j++) {

			scanf("%s", temp); // ler o mapa

			// atualiza para o novo mapa			
			mapa[i][j].gamer[0] = temp[0];
			mapa[i][j].gamer[1] = temp[1];
			mapa[i][j].bomba[0] = temp[2];
			mapa[i][j].bomba[1] = temp[3];
			strcpy(mapa[i][j].celula, temp);			
			
			if (strcmp(mapa[i][j].gamer, player.nome) == 0 || strcmp(mapa[i][j].gamer, "P3") == 0) { // insere a posicao inicial do player
				player.posicao.linha = i;
				player.posicao.coluna = j;
			}
			if (strcmp(mapa[i][j].gamer, inimigo.nome) == 0 || strcmp(mapa[i][j].gamer, "P3") == 0) { // insere a posicao inicial do player
				inimigo.posicao.linha = i;
				inimigo.posicao.coluna = j;
			}
			if (strcmp(mapa[i][j].bomba, player.bomba) == 0 || strcmp(mapa[i][j].bomba, "B3") == 0) { // insere caso tenha sido posto uma bomba no mapa
				coloqueiNbombas++;
			}
			
		}
	}
}


void readFile() { // ler o arquivo
	
	numeroBombas = fopen("numeroBombas.txt", "r"); // ler
	rangeGamers = fopen("rangeGamers.txt", "r"); // ler
	mapaOld = fopen("mapaOld.txt", "r"); // ler
	
	if (numeroBombas != NULL) { // caso o arquivo exista
		fscanf(numeroBombas, "%i %i", &player.capacidadeBombas, &inimigo.capacidadeBombas); // adiciona os valores as respectivas variaveis
	} else { // caso nao exista
		numeroBombas = fopen("numeroBombas.txt", "w"); // insere
		fprintf(numeroBombas, "2 2"); // numero de bombas inicial
		player.capacidadeBombas = 2;
		inimigo.capacidadeBombas = 2;
	}
	
	if (rangeGamers != NULL) { // caso o arquivo exista
		fscanf(rangeGamers, "%i %i", &player.range, &inimigo.range); // adiciona os valores as respectivas variaveis
	} else { // caso nao exista
		rangeGamers = fopen("rangeGamers.txt", "w"); // insere
		fprintf(rangeGamers, "3 3"); // range inicial
		player.range = 3;
		inimigo.range = 3;
	}
	
	if (mapaOld != NULL) { // caso o arquivo exista
	
		int bonus;
	
		for (int i = 0; i < LINHA; i++) {
			for (int j = 0; j < COLUNA; j++) {

				fscanf(mapaOld, "%i", &bonus); // passa o valor para a variavel
				
				if (bonus == 1) { // caso exista bonus
				
					if (strcmp(mapa[i][j].gamer, player.nome) == 0) { // se o player etiver sobre o bonus
						player.range++; // incrementa o rangem do player
					} else if (strcmp(mapa[i][j].gamer, inimigo.nome) == 0) { // se o inimigo etiver sobre o bonus
						inimigo.range++; // incrementa o rangem do inimigo
					} else if (strcmp(mapa[i][j].gamer, "P3") == 0) {
						player.range++; // incrementa o rangem do player
						inimigo.range++; // incrementa o rangem do inimigo
					}
					
				} else if (bonus == 2) {

					if (strcmp(mapa[i][j].gamer, player.nome) == 0) { // se o player etiver sobre o bonus
						player.capacidadeBombas++; // incrementa o rangem do player
					} else if (strcmp(mapa[i][j].gamer, inimigo.nome) == 0) { // se o inimigo etiver sobre o bonus
						inimigo.capacidadeBombas++; // incrementa o rangem do inimigo
					} else if (strcmp(mapa[i][j].gamer, "P3") == 0) {
						player.capacidadeBombas++; // incrementa o rangem do player
						inimigo.capacidadeBombas++; // incrementa o rangem do inimigo
					}

				}
			}
		}
		
	} else { // caso nao exista
		mapaOld = fopen("mapaOld.txt", "w"); // insere
		for (int i = 0; i < LINHA * COLUNA; i++)
			fprintf(mapaOld, "0\n"); // inicia tudo com zero (nao existe bonus)
	}
	
	fclose(numeroBombas);
	fclose(rangeGamers);
	fclose(mapaOld);
}

void writeFile() { // escreve no arquivo

	numeroBombas = fopen("numeroBombas.txt", "w"); // insere
	rangeGamers = fopen("rangeGamers.txt", "w"); // insere
	mapaOld = fopen("mapaOld.txt", "w"); // insere
	
	fprintf(numeroBombas, "%i %i", player.capacidadeBombas, inimigo.capacidadeBombas); // salva a quantidade de bombas atualizada no arquivo
	
	fprintf(rangeGamers, "%i %i", player.range, inimigo.range); // salva o range atualizado no arquivo
	
	// atualiza o mapa antigo
	for (int i = 0; i < LINHA; i++) {
		for (int j = 0; j < COLUNA; j++) {
			
			if (strcmp(mapa[i][j].bomba, "+F") == 0) // caso nesta posicao exista um bonus de incremento no range
				fprintf(mapaOld, "1\n");
			else if (strcmp(mapa[i][j].bomba, "+B") == 0) // caso nesta posicao exista um bonus de incremento na quantidade
				fprintf(mapaOld, "2\n");
			else // caso nao exista um bonus
				fprintf(mapaOld, "0\n");
			
		} 
	}
	
	fclose(numeroBombas);
	fclose(rangeGamers);
	fclose(mapaOld);
	
}



/*
	- Utiliza o sistema do problema da rainha -
	º objeto: insere o que vai ser pesquisado
	º range: recebe o rangem da bomba
	º linha, coluna: a posicao onde o player esta
	º retorna zero se nao encontrou e um se encontrou
*/
bool radar(char objeto[3], int range, int linha, int coluna) { // verifica se ha perigo na margem de seguranca verificando sempre pelo maior range entre os personagens

	vertice direcao;
	bool obstaculo[5] = { false }; // pula o caminho a ser buscado caso tenha encontrado um obstaculo neste caminho 
	
	for (int i = 1; i < range; i++) {

		for (int j = 0; j < 5; j++) {

			if (!obstaculo[j]) { // pula este sentido quando encontrar um obstaculo
			
				direcao.linha = linha + i*dX[j];
				direcao.coluna = coluna + i*dY[j];
				
				if ((direcao.linha >= 0 && direcao.linha < LINHA) && (direcao.coluna >= 0 && direcao.coluna < COLUNA)) { // verifica se o caminho e possivel
						
					tabuleiro elemento = mapa[direcao.linha][direcao.coluna]; // recebe o elemento que sera verificado
					
					if (strcmp(objeto, "B1") == 0 || strcmp(objeto, "B2") == 0) { // retorna a range correta que a bomba alcancara
						if (j == 0 || (j > 0 && strcmp(elemento.celula, "XXXX") != 0 && strcmp(elemento.celula, "MMMM") != 0 && strcmp(elemento.bomba, "+B") != 0 && strcmp(elemento.bomba, "+F") != 0)) { // certifica que nao verifique se houver empecilhos
							if (strcmp(elemento.bomba, objeto) == 0 || strcmp(elemento.bomba, "B3") == 0) { // retorna o range do player
								return true;
							}
						} else {
							obstaculo[j] = true;
						}
					} else if (strcmp(objeto, inimigo.nome) == 0) { // retorna se o inimigo esta dentro da area de ataque
						if (j == 0 || (j > 0 && (strcmp(elemento.celula, "----") == 0 || strcmp(elemento.gamer, objeto) == 0 || strcmp(elemento.gamer, "P3") == 0))) { // certifica que nao verifique se houver empecilhos
							if (strcmp(elemento.gamer, objeto) == 0 || strcmp(elemento.gamer, "P3") == 0) {
								return true;
							}
						} else {
							obstaculo[j] = true;
						}
					}
	
				} else {
					obstaculo[j] = true;
				}
			}
		}
		
	}
	
	return false;
};


/*
	- Utiliza o sistema do algoritimo de busca de Dijkstra -
	º posicao: recebe a posicao do player
	º objeto: insere o que vai ser pesquisado
*/
vertice search(vertice posicao, char objeto[3]) { // procura o menor caminho para chegar no inimigo ou fugir da bomba
	
	vertice origem = posicao; // recebe aposição inicial do personagem
	
	vertice heuristica = { posicao.linha - inimigo.posicao.linha, posicao.coluna - inimigo.posicao.coluna }; // serve para comparar a distancia inicial em que estou do inimigo

	int listaAdjacencia[LINHA * COLUNA]; // recebera todos os somatorio do caminhos
	for (int i = 0; i < LINHA * COLUNA; i++) {
		listaAdjacencia[i] = LINHA * COLUNA * 9; // inicia todos os vertices com o tamnaho do maior caminho possivel
	}
	listaAdjacencia[(origem.linha * COLUNA) + origem.coluna] = 0; // inicia a posicao raiz com tamanho zero

	// fila
	vertice fila[LINHA * COLUNA * 2]; // inicia com o tamanho da maior quantidade de elementos que poderia ocorrer nesta fila
	int remove = 0; // remove do inicio da fila
	int inserir = 0; // insere no final da fila
	// end fila

	vertice destino = posicao; // posicao do inimigo
	
	int sentido = rand()%2; // 50% de procurar o inimigo no sentido horario ou anti-horario. Evita o comportamento robotico
	
	while (true) {

		for (int i = (sentido == 0 ? 0 : 4); (sentido == 0 ? i < 5 : i >= 0); (sentido == 0 ? i++ : i--)) {

			int linha = origem.linha + dX[i]; // linha a ser analizada
			int coluna = origem.coluna + dY[i]; // coluna a ser analizada

			if ((linha >= 0 && linha < LINHA) && (coluna >= 0 && coluna < COLUNA)) { // verifica se ainda esta na area do mapa

				if (strcmp(objeto, "B1") == 0 || strcmp(objeto, "B2") == 0) { // fuga da bomba
					if (strcmp(mapa[linha][coluna].celula, "XXXX") != 0 && strcmp(mapa[linha][coluna].celula, "MMMM") != 0 && strcmp(mapa[linha][coluna].bomba, "B1") != 0 && strcmp(mapa[linha][coluna].bomba, "B2") != 0 && strcmp(mapa[linha][coluna].bomba, "B3") != 0) { // verifica se nao e uma parede ou obstaculo

						int somatorio = listaAdjacencia[(origem.linha * COLUNA) + origem.coluna] + 1; // soma acumulada do vertice pai com o filho
	
						if (somatorio < listaAdjacencia[(linha * COLUNA) + coluna]) { // caso a soma do vertice pai com o vertice atual seja a menor possivel
							listaAdjacencia[(linha * COLUNA) + coluna] = somatorio;
						
							// insere o vertice na fila para que seus filho sejam analizados posteriormente
							fila[inserir].linha = linha;
							fila[inserir++].coluna = coluna;
							
							// caso encontre um lugar seguro
							if (!radar(player.bomba, player.range, linha, coluna) && !radar(inimigo.bomba, inimigo.range, linha, coluna)) {							
								if (destino.linha == posicao.linha && destino.coluna == posicao.coluna) {								
									destino = { linha, coluna };

								// escolhe o menor caminho conforme uma heuristica de que o caminho deve ser oposto ao sentido do inimigo
								} else if (somatorio < listaAdjacencia[(destino.linha * COLUNA) + destino.coluna] || (somatorio == listaAdjacencia[(destino.linha * COLUNA) + destino.coluna] && ((heuristica.linha == 0 ? ((linha - inimigo.posicao.linha) <= heuristica.linha || (linha - inimigo.posicao.linha) >= heuristica.linha) : (heuristica.linha > 0 ? (linha - inimigo.posicao.linha) >= heuristica.linha : (linha - inimigo.posicao.linha) <= heuristica.linha)) && (heuristica.coluna == 0 ? ((coluna - inimigo.posicao.coluna) <= heuristica.coluna || (coluna - inimigo.posicao.coluna) >= heuristica.coluna) : (heuristica.coluna > 0 ? (coluna - inimigo.posicao.coluna) >= heuristica.coluna : (coluna - inimigo.posicao.coluna) <= heuristica.coluna))))) {
									destino = { linha, coluna };						
								}
							}
						}
					}
				} else if (strcmp(objeto, inimigo.nome) == 0) { // procura o inimigo
					if (player.posicao.linha == inimigo.posicao.linha && player.posicao.coluna == inimigo.posicao.coluna) { // caso o inimigo esteja na mesma posicao
						
						return player.posicao; // evita a pesquisa desnecessaria e permanece parado
						
					} else if (strcmp(mapa[linha][coluna].celula, "XXXX") != 0) { // verifica se nao e uma parede
		
						int somatorio = listaAdjacencia[(origem.linha * COLUNA) + origem.coluna] + peso(mapa[linha][coluna].celula); // soma acumulada do vertice pai com o filho
	
						if (somatorio < listaAdjacencia[(linha * COLUNA) + coluna]) { // caso a soma do vertice pai com o vertice atual seja a menor possivel
							listaAdjacencia[(linha * COLUNA) + coluna] = somatorio;
						
							// insere o vertice na fila para que seus filho sejam analizados posteriormente
							fila[inserir].linha = linha;
							fila[inserir++].coluna = coluna;
	
							destino = { inimigo.posicao.linha, inimigo.posicao.coluna };
						}
		
					}
				}
			}
		}	

		if (remove < inserir) { // verifica se a fila nao esta esvaziou
			origem = fila[remove++]; // retira o primeiro elemento da fila para que o proximo seja analisado
		} else { // caso esteja vazia

			// caso nao haja fuga, entao fica parado
			if (destino.linha == posicao.linha && destino.coluna == posicao.coluna) {
				   return posicao;
			}

			vertice caminho = destino; // proximo passo			
			int menor = listaAdjacencia[(destino.linha * COLUNA) + destino.coluna]; // vai recebendo o menor vertice ate chegar na origem

			while (true) {

				for (int i = (sentido == 0 ? 1 : 4); (sentido == 0 ? i < 5 : i > 0); (sentido == 0 ? i++ : i--)) {

					int linha = caminho.linha + dX[i]; // linha a ser analizada
					int coluna = caminho.coluna + dY[i]; // coluna a ser analizada
						
					if ((linha >= 0 && linha < LINHA) && (coluna >= 0 && coluna < COLUNA)) { // verifica se ainda esta na area do mapa
						if (strcmp(mapa[linha][coluna].celula, "XXXX") != 0) { // verifica se nao e uma parede

							if (listaAdjacencia[(linha * COLUNA) + coluna] < menor) { // escolhe o menor caminho
								if (listaAdjacencia[(linha * COLUNA) + coluna] != 0) {
								
									menor = listaAdjacencia[(linha * COLUNA) + coluna]; // substitue o menor caminho ate entao encontrado
	
									// insere o caminho no array
									caminho = { linha, coluna };
							
								} else {
									return caminho;
								}
							}

						}
					}
					
				}

			}

		}
	
	}

};


int direcao(vertice lado) { // move conforme as regras da plataforma

	if ((lado.linha - player.posicao.linha) < 0) { // cima
		return 1;
	} else if ((lado.coluna - player.posicao.coluna) < 0) { // esquerce
	   	return 2;
	} else if ((lado.linha - player.posicao.linha) > 0) { // baixo
		return 3;
	} else if ((lado.coluna - player.posicao.coluna) > 0) { // direita
	   	return 4;
	} else { // parado
		return 0;
	}
};


int main(int argc, char *argv[]) { //a assinatura da funcao principal deve ser dessa forma

	srand(time(NULL));

	//convercao dos identificadores
	int id = atoi(argv[1]);	//identificador do Jogador
	int rodada = atoi(argv[2]); // identificador da partida

	identificaPlayers(id);

	geraMapa();
	
	readFile();
	writeFile();
	
	int colocaBomba = 0; // informa se deve colocar ou nao a bomba
	vertice caminho; // procura o menor caminho para o proximo passo que o player deve dar
    int explodeBomba = 0; // informa se deve explodir ou nao a bomba


	// verifica se a propria bomba esta na margem de seguranca
	if (radar(player.bomba, player.range, player.posicao.linha, player.posicao.coluna)) {

		// foge da propria bomba
		caminho = search(player.posicao, player.bomba);
		
	// verifica se ja atingiu o limite de bombas ha solta
	} else if (coloqueiNbombas == player.capacidadeBombas) {
		
		explodeBomba = 1; // explode a ultima bomba
		
		caminho = search(player.posicao, inimigo.nome); // procura o caminho ao inimigo
		
		// verifica se existe uma bomba no proximo passo
		if (radar(player.bomba, player.range, caminho.linha, caminho.coluna) || radar(inimigo.bomba, inimigo.range, caminho.linha, caminho.coluna)) {
			
			// fica parado
			caminho.linha = player.posicao.linha;
			caminho.coluna = player.posicao.coluna;

		}
	
	// verifica se o inimigo esta no range de ataque
	} else if (radar(inimigo.nome, player.range, player.posicao.linha, player.posicao.coluna)) {

		colocaBomba = 1;

		// atualiza o mapa
		strcpy(mapa[player.posicao.linha][player.posicao.coluna].celula, player.nome);
		strcat(mapa[player.posicao.linha][player.posicao.coluna].celula, player.bomba);
		strcpy(mapa[player.posicao.linha][player.posicao.coluna].gamer, player.nome);
		strcpy(mapa[player.posicao.linha][player.posicao.coluna].bomba, player.bomba);

		caminho = search(player.posicao, player.bomba); // foge da propria bomba
		
	// verifica se a bomba do inimigo esta na margem de seguranca
	} else if (radar(inimigo.bomba, inimigo.range, player.posicao.linha, player.posicao.coluna)) {

		// foge da bomba do inimigo 
		caminho = search(player.posicao, inimigo.bomba);

	// verifica se coloquei aluma bomba
	} else if (coloqueiNbombas != 0) {
			
		explodeBomba = 1;
		
		caminho = search(player.posicao, inimigo.nome); // procura o caminho ao inimigo
		
		// verifica se existe uma bomba no proximo passo
		if (radar(player.bomba, player.range, caminho.linha, caminho.coluna) || radar(inimigo.bomba, inimigo.range, caminho.linha, caminho.coluna)) {
			
			// fica parado
			caminho.linha = player.posicao.linha;
			caminho.coluna = player.posicao.coluna;

		}

	} else {
		
		caminho = search(player.posicao, inimigo.nome); // procura o caminho ao inimigo

		// verifica se no proximo passo existe uma arvore
		if (strcmp(mapa[caminho.linha][caminho.coluna].celula, "MMMM") == 0) {

			colocaBomba = 1;
			
			// atualiza o mapa
			strcpy(mapa[player.posicao.linha][player.posicao.coluna].celula, player.nome);
			strcat(mapa[player.posicao.linha][player.posicao.coluna].celula, player.bomba);
			strcpy(mapa[player.posicao.linha][player.posicao.coluna].gamer, player.nome);
			strcpy(mapa[player.posicao.linha][player.posicao.coluna].bomba, player.bomba);
	
			caminho = search(player.posicao, player.bomba); // foge da propria bomba

		// verifica se existe uma bomba no proximo passo
		} else if (radar(player.bomba, player.range, caminho.linha, caminho.coluna) || radar(inimigo.bomba, inimigo.range, caminho.linha, caminho.coluna)) {

			// fica parado
			caminho.linha = player.posicao.linha;
			caminho.coluna = player.posicao.coluna;

		}
				
	}

	printf("%d %d %d %d\n", rodada, colocaBomba, direcao(caminho), explodeBomba); // rodada, bomba, move, explode bomba
		
	return 0;
}