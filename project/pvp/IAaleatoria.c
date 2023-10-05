#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

//perdoeem a falta de acentuacao. A configuracao de teclado que eu estou usando nao e a brasileira.

//variavel global usada. O ID e um valor passado pelo programa principal que vai identificar se o jogador e o player 1 ou player 2
int id;
//ID do jogo
int ident;


//esta estrutura guarda as posicoes em i e j num tabuleiro
typedef struct pos
{
	int i;
	int j;
}pos;


//vetores de deslocamento. {parado, sobe, esquerda, desce, direita} 
int dx[] = {0,-1,0,1,0};
int dy[] = {0,0,-1,0,1};


//estrutura do tabuleiro. Como e sabido, os dois primeiros caracteres guardam as informacoes sobre o jogadore e os dois ultimos 
//sobre a presenca de bombas, bonus, etc
//logo, usa-se duas strings

typedef struct tabela
{
	char str1[3],str2[3];
}tabela;

//mapa a ser lido.
tabela tab[11][13];


//funcao que realiza a leitura do mapa. O mapa e passado como entrada padrao pelo programa principal, portanto, nesses casos, usem scanf normalmente.
//usem fscanf para trabalhar com arquivos criados por voces.

void leitura()
{
	int i, j;
	for(i = 0; i<11; i++)
	{
		for(j = 0; j<13; j++)
		{
			char temp[5];
			scanf("%s", temp);
			tab[i][j].str1[0] = temp[0];
			tab[i][j].str1[1] = temp[1];
			tab[i][j].str2[0] = temp[2];
			tab[i][j].str2[1] = temp[3];
		}
	}
}

//funcao retorna a posicao corrente de determinado jogador, cuja string (P1 ou P2) e passada como parametro.
pos cur_pos(char* player)
{
	pos posi;
	int i,j;
	for(i = 0; i < 11; i++)
		for(j = 0; j < 13; j++)
		{
			if(strcmp(tab[i][j].str1, player) == 0 || strcmp(tab[i][j].str1, "B3") == 0 )
			{
				posi.i = i;
				posi.j = j;
				return posi;
			}
		}
}


//funcao que checa se o movimento e valido em determinada posicao
int check(int x, int y)
{
	if(x>=0 && x<11 && y>=0 && y<13 && (strcmp(tab[x][y].str2,"--")==0 || strcmp(tab[x][y].str2,"+F")==0 || strcmp(tab[x][y].str2,"+B")==0))
		return 1;
	else 
		return 0;
}

//retorna o maior elemento de um vetor de 5 posicoes
int MAIOR(int *vec)
{
	int i, maior = 0;
	for(i = 0; i<5; i++)
	{
		if(vec[i]>vec[maior])
			maior = i;
	}
	return maior;
}

int main(int argc, char *argv[])//a assinatura da funcao principal deve ser dessa forma
{
	int i;
	
	//convercao dos identificadores
    id = atoi(argv[1]);	//identificador do Jogador
    ident = atoi(argv[2]); //identificador da partida
 
	pos cur;
	
	
	srand(time(NULL));
	
	leitura();

	int move[5]; 
	
	char s[3];
	if(id==1)strcpy(s,"P1");
	else strcpy(s,"P2");
	
	cur = cur_pos(s); // o parametro a ser passado depende se o jogador atual e 1 o 2
	move[0] = 0;
	for(i = 1; i<5; i++)
		move [i] = check(cur.i + dx[i],cur.j + dy[i])*rand();
	
	//impressao da saida, note que nessa ia a unica coisa que o jogador faz e se mover aleatoriamente.
	printf("%d 0 %d 0\n",ident, MAIOR(move));
	
		
	return 0;
}
