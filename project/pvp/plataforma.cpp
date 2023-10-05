#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <time.h>
#include <queue>
#include <iostream>


//complementares da probabilidade de aparecerem bonus e arvores

#define P_BONUS 92
#define P_MM 50
#define MAX_ROUND 250
#define db if(0)
#define DELAY 10
#define MULTIP 4

using namespace std;

struct bomba{
	int x,y,fogo;
	bomba(int x=0, int y=0, int fogo=0):x(x),y(y),fogo(fogo){}
};

struct player{
	int x,y,fogo,maximo,soltadas;
	queue<bomba> fila;
	player(int x=0, int y=0, int fogo=0, int maximo =0,int soltadas=0, queue<bomba> fila = queue<bomba>()):
		x(x),y(y),fogo(fogo),maximo(maximo),soltadas(soltadas),fila(fila){};

};

struct jogada{
	int coloca,move,estoura;
	bool podeColoca,podeEstoura;
	jogada(int coloca=0, int move=0, int estoura=0,bool podeColoca = false, bool podeEstoura = false):
		coloca(coloca),move(move),estoura(estoura),podeColoca(podeColoca),podeEstoura(podeEstoura){}
};

struct tabela{
	char str1[3],str2[3];
	int bonus;
	// bonus =0 indica que não tem bonus
	// bonus =1 indica que tem o bonus de +1 bomba
	// bonus= 2 indica que tem o bonus de +1 no alcance da bomba.
}tab[11][13];

/*vetor deslocamento
	0 - fica parado.
	1 - cima
	2 - esquerda
	3 - baixo
	4 - direita
*/

int dx[] = {0,-1,0,1,0};
int dy[] = {0,0,-1,0,1};
int movimento[2], ident, bckp;
bool estavivo1,estavivo2;

//char tab[11][13][3];
player p1,p2;

void Sleep(int atraso){
     float ti1=0, ti2=0;
     ti1 = (float)clock()/(float)CLOCKS_PER_SEC;
     while(ti2-ti1<(atraso)/1000)    ti2 = (float)clock()/(float)CLOCKS_PER_SEC;
}

void imprimiTab(){
	for(int i=0; i<11; i++){
		for(int j=0; j<13; j++)
			printf("%2s%2s ",tab[i][j].str1,tab[i][j].str2);
		puts("");
	}
}


void imprimiTabArq(){
     FILE *out=fopen("mapa.txt","w");
     if(out == NULL)
     {
     	printf("ERRO AO CRIAR O MAPA\n REINICIE A PARTIDA");
     	exit(EXIT_FAILURE);
     	return
     }
     }
     for(int i=0; i<11; i++){
        for(int j=0; j<13; j++)
			fprintf(out,"%2s%2s ",tab[i][j].str1,tab[i][j].str2);
		fprintf(out,"\n");
	 }
	 fclose(out);
}

void printEstado(){
    char nimp[20];
   	sprintf(nimp, "%d", bckp);
    strcat(nimp,".txt");
    FILE *out=fopen(nimp,"a");
    fprintf(out,"%d %d \n",movimento[0],movimento[1]);
	for(int i=0; i<11; i++){
		for(int j=0; j<13; j++)
			fprintf(out,"%2s%2s ",tab[i][j].str1,tab[i][j].str2);
		fprintf(out,"\n");
	}
	if(estavivo1 && estavivo2)  fprintf(out,"0 \n\n\n");
	else if(estavivo1 && !estavivo2) fprintf(out,"1 \n\n\n");
	else if(!estavivo1 && estavivo2) fprintf(out,"2 \n\n\n");
	else fprintf(out,"3 \n\n\n");
	fclose(out);
}

void criaTabuleiro()
{

	int i, j, num1, num2, num3;
	for(i = 0; i<11; i++)
	{
		for(j = 0; j<13; j++)
		{
			if(i%2 && j%2)
			{
				strcpy(tab[i][j].str1,"XX"),strcpy(tab[i][j].str2,"XX");
				tab[i][j].bonus = 0;
			}
			else
			{
				num1 = rand()%100;
				num2 = rand()%100;
				num3 = rand()%100;
				if(num1 > P_MM)
				{
					strcpy(tab[i][j].str1,"MM"), strcpy(tab[i][j].str2,"MM");
					if(num2 > P_BONUS)
						tab[i][j].bonus = 1;
					else if(num3 > P_BONUS)
						tab[i][j].bonus = 2;
				}

				else
					strcpy(tab[i][j].str1,"--"),strcpy(tab[i][j].str2,"--");
			}
		}
	}

	strcpy(tab[0][0].str1,"P1");
	strcpy(tab[10][12].str1,"P2");

	strcpy(tab[0][0].str2,"--");
	strcpy(tab[1][0].str1,"--");
	strcpy(tab[1][0].str2,"--");
	strcpy(tab[0][1].str1,"--");
	strcpy(tab[0][1].str2,"--");

	strcpy(tab[10][12].str2,"--");
	strcpy(tab[9][12].str1,"--");
	strcpy(tab[9][12].str2,"--");
	strcpy(tab[10][11].str1,"--");
	strcpy(tab[10][11].str2,"--");
	p1 = player(0,0,2,2,0,queue<bomba>());
	estavivo1=true;
	p2 = player(10,12,2,2,0,queue<bomba>());
	estavivo2=true;

    imprimiTab();
    imprimiTabArq();
}

void colocaBomba(int x, int y,int id){
	if(id==1){
		if(strcmp(tab[x][y].str2,"--")==0) strcpy(tab[x][y].str2,"B1");
		else if(strcmp(tab[x][y].str2,"B2")==0) strcpy(tab[x][y].str2,"B3");
	}
	else{
		if(strcmp(tab[x][y].str2,"--")==0) strcpy(tab[x][y].str2,"B2");
		else if(strcmp(tab[x][y].str2,"B1")==0) strcpy(tab[x][y].str2,"B3");
	}

}

//faz verificação se o jogador pode se mover.
bool check(int x, int y){
	if(x>=0 && x<11 && y>=0 && y<13 && (strcmp(tab[x][y].str2,"--")==0 || strcmp(tab[x][y].str2,"+F")==0 || strcmp(tab[x][y].str2,"+B")==0)) return true;
	else return false;
}

//faz verificação do alcance do fogo na bomba na hora da explosão.
bool check2(int x, int y){
	if(x>=0 && x<11 && y>=0 && y<13 && strcmp(tab[x][y].str1,"XX")!=0) return true;
	else return false;
}

//movimenta jogador.
void movimenta(int x, int y, int direcao, int id){
	if(id==1){
		if(strcmp(tab[x][y].str1,"P1")==0) strcpy(tab[x][y].str1,"--");
		else strcpy(tab[x][y].str1,"P2");
		int nx = x + dx[direcao];
		int ny = y + dy[direcao];
		if(strcmp(tab[nx][ny].str1,"P2")==0) strcpy(tab[nx][ny].str1,"P3");
		else strcpy(tab[nx][ny].str1,"P1");
	}
	else{
		if(strcmp(tab[x][y].str1,"P2")==0) strcpy(tab[x][y].str1,"--");
		else strcpy(tab[x][y].str1,"P1");
		int nx = x + dx[direcao];
		int ny = y + dy[direcao];
		if(strcmp(tab[nx][ny].str1,"P1")==0) strcpy(tab[nx][ny].str1,"P3");
		else strcpy(tab[nx][ny].str1,"P2");
	}
}


void verificaJogador(int x, int y){
	if(strcmp(tab[x][y].str1,"P1")==0){
		estavivo1=false;
	}
	else if(strcmp(tab[x][y].str1,"P2")==0){
		estavivo2=false;
	}
	else if(strcmp(tab[x][y].str1,"P3")==0){
		estavivo1=false;
		estavivo2=false;
	}
	strcpy(tab[x][y].str1,"FF");
}

int verificaBomba(int x, int y){


	bomba aux;
	db printf("%s\n",tab[x][y].str2);
	if(strcmp(tab[x][y].str2,"B1")==0){
		db printf("Entrou B1\n");
		int bombaSoltada = p1.soltadas;
		int retorno;
		p1.soltadas--;

		for(int i=0; i<bombaSoltada; i++){
			aux = p1.fila.front(); p1.fila.pop();
			if(x == aux.x && y==aux.y){
				strcpy(tab[x][y].str2,"FF");
				retorno = aux.fogo;
			}
			else p1.fila.push(aux);
		}
		return retorno;


	}
	db printf("%s\n",tab[x][y].str2);
	if(strcmp(tab[x][y].str2,"B2")==0){
		db printf("Entrou b2\n");
		int bombaSoltada = p2.soltadas;
		int retorno;
		p2.soltadas--;
		for(int i=0; i< bombaSoltada; i++){
			aux = p2.fila.front(); p2.fila.pop();
			if(x == aux.x && y == aux.y){
			    strcpy(tab[x][y].str2,"FF");
				retorno=aux.fogo;
			}
			else p2.fila.push(aux);
		}
		return retorno;
	}
	int bombaSoltada1 = p1.soltadas;
	int bombaSoltada2 = p2.soltadas;
	int alcance1, alcance2;
	p1.soltadas--;
	p2.soltadas--;
	db printf("passou aqui\n");
	for(int i=0; i<bombaSoltada1; i++){
		aux = p1.fila.front(); p1.fila.pop();
		if(x == aux.x && y == aux.y){
			alcance1 = aux.fogo;
		}
		else p1.fila.push(aux);
	}

	for(int i=0; i<bombaSoltada2; i++){
		aux = p2.fila.front(); p2.fila.pop();
		if(x == aux.x && y == aux.y){
			alcance2 = aux.fogo;
		}
		else p2.fila.push(aux);
	}
	strcpy(tab[x][y].str2,"FF");
	return max(alcance1,alcance2);
}

void explosao(int x, int y){
	verificaJogador(x,y);
	int alcance = verificaBomba(x,y);
	for(int i=1; i<5; i++){
		int nx = x;
		int ny = y;
		for(int j=0; j<alcance; j++){
			nx = nx + dx[i];
			ny = ny + dy[i];
			if(check2(nx,ny)){
				verificaJogador(nx,ny);
				if(strcmp(tab[nx][ny].str2,"FF")==0) break;
				else if(strcmp(tab[nx][ny].str2,"+F")==0){
					strcpy(tab[nx][ny].str2,"FF");
					break;
				}
				else if(strcmp(tab[nx][ny].str2,"+B")==0){
					strcpy(tab[nx][ny].str2,"FF");
					break;
				}
				else if(strcmp(tab[nx][ny].str2,"--")==0){
					strcpy(tab[nx][ny].str2,"FF");
				}
				else if(strcmp(tab[nx][ny].str2,"MM")==0){
                    if(tab[nx][ny].bonus==1){
                        strcpy(tab[nx][ny].str2,"+B");
                    }
                    else if(tab[nx][ny].bonus==2){
                        strcpy(tab[nx][ny].str2,"+F");
                    }
                    else{
                        strcpy(tab[nx][ny].str2,"--");
                    }
                    break;
				}
				else{
				    explosao(nx,ny);
                    break;
				}
			}
			else break;
		}
	}

}

void fazJogada(){
	jogada j1,j2;
	int a,b,c,tmp1=0,tmp2=0,rinoceronte;
	char aidi[20], jog1[200]="./jogador1 1 ", jog2[200]="./jogador2 2 ";
	bool podeColoca;
    
    sprintf(aidi, "%d", ident);
    strcat(jog1,aidi);
    strcat(jog2,aidi);
    
	FILE *arquivo1, *arquivo2; /*= fopen("saida1.txt","w");
	FILE* arquivo2; = fopen("saida2.txt","w");

	fclose(arquivo1);
	fclose(arquivo2);*/

	// a indica se solta(1) ou não bomba(0)
	// b indica a direção do movimento de acordo com o vetor deslocamento(0,1,2,3,4).
	// c estoua uma bomba(1) ou se não estoura(0).

    strcat(jog1," < mapa.txt > saida1.txt");
	system(jog1);
	//Sleep(80);
	while(tmp1<MULTIP){
        Sleep(DELAY);
        arquivo1=fopen("saida1.txt","r");
        if(arquivo1!=NULL){
            fscanf(arquivo1,"%d ",&rinoceronte);
            if(rinoceronte==ident)               break;
        }
        tmp1++;
    }

	if(tmp1 < MULTIP){
		if(fscanf(arquivo1,"%d %d %d",&a,&b,&c)!=3){
			printf("O jogador 1 demorou para dar a resposta.\n");
			a=b=c=0;
			movimento[0]=0;
		}else{
              movimento[0]=b;
        }
    }
	else{
        printf("Houve algum problema para abrir o arquivo do jogador 1\n");
	    a=b=c=0;
        
	}

	//verifica se o p1 pode colocar a bomba.
	podeColoca = p1.soltadas<p1.maximo;
	podeColoca = podeColoca && strcmp(tab[p1.x][p1.y].str2,"--")==0;
	// p1.soltadas>0 serve para verificar se o p1 tem alguma bomba para estourar visto que ele não pode soltar e estourar bomba no mesmo round.
	j1 = jogada(a,b,c,podeColoca,p1.soltadas>0);
	db cout << j1.coloca << " " << j1.move << " " << j1.estoura << " " << j1.podeColoca << " " << j1.podeEstoura << endl;

	//faz as mesmas checagens para o p2.
	strcat(jog2," < mapa.txt > saida2.txt");
	system(jog2);
	//Sleep(80);
	
    while(tmp2<MULTIP){
        Sleep(DELAY);
        arquivo2=fopen("saida2.txt","r");
        if(arquivo2!=NULL){
            fscanf(arquivo2,"%d ",&rinoceronte);
            if(rinoceronte==ident)               break;
        }
        tmp2++;
    }

	if(tmp2 < MULTIP){
		if(fscanf(arquivo2,"%d %d %d",&a,&b,&c)!=3){

			printf("O jogador 2 demorou para dar a resposta.\n");
			a=b=c=0;
			movimento[1]=0;
		}else{
              movimento[1]=b;
        }
    }
	else{
        printf("Houve algum problema para abrir o arquivo do jogador 2\n");
        a=b=c=0;
	}

	podeColoca = p2.soltadas<p2.maximo;
	podeColoca = podeColoca && strcmp(tab[p2.x][p2.y].str2,"--")==0;
	j2 = jogada(a,b,c,podeColoca,p2.soltadas>0);
	db cout << j2.coloca << " " << j2.move << " " << j2.estoura << " " << j2.podeColoca << " " << j2.podeEstoura << endl;

	fclose(arquivo1);
	fclose(arquivo2);

	// coloca as bombas.
	if(j1.coloca==1 && j1.podeColoca){
		colocaBomba(p1.x,p1.y,1);
		p1.fila.push(bomba(p1.x,p1.y,p1.fogo));
		p1.soltadas++;
	}

	if(j2.coloca==1 && j2.podeColoca){
		colocaBomba(p2.x,p2.y,2);
		p2.fila.push(bomba(p2.x,p2.y,p2.fogo));
		p2.soltadas++;
	}

	db printf("p1 e p2 colocaram a bomba\n\n");

	//faz movimento
	bool achoubonus1,achoubonus2;
	achoubonus1=achoubonus2 = false;
	if(j1.move!=0 && check(p1.x+dx[j1.move],p1.y+dy[j1.move])){
		movimenta(p1.x,p1.y,j1.move,1);
		p1.x = p1.x + dx[j1.move];
		p1.y = p1.y + dy[j1.move];
		if(strcmp(tab[p1.x][p1.y].str2,"+B")==0){
			p1.maximo++;
			achoubonus1=true;
		}
		else if(strcmp(tab[p1.x][p1.y].str2,"+F")==0){
			p1.fogo++;
			achoubonus1=true;
		}
	}

	if(j2.move!=0 && check(p2.x+dx[j2.move],p2.y+dy[j2.move])){
		movimenta(p2.x,p2.y,j2.move,2);
		p2.x = p2.x + dx[j2.move];
		p2.y = p2.y + dy[j2.move];

		if(strcmp(tab[p2.x][p2.y].str2,"+B")==0){
			p2.maximo++;
			achoubonus2=true;
		}
		else if(strcmp(tab[p2.x][p2.y].str2,"+F")==0){
			p2.fogo++;
			achoubonus2=true;
		}
	}

	if(achoubonus1){
		strcpy(tab[p1.x][p1.y].str2,"--");
		tab[p1.x][p1.y].bonus=0;
	}

	if(achoubonus2){
		strcpy(tab[p2.x][p2.y].str2,"--");
		tab[p2.x][p2.y].bonus=0;
	}
    db printf("p1 e p2 fizeram o movimento.\n\n");
	//estoura bomba
	
	bomba aj= p2.fila.front();
	bool limpa = false;
	if(j1.estoura==1 && j1.podeEstoura){
		bomba aux = p1.fila.front();
		explosao(aux.x,aux.y);
		db printf("p1 explodiu a bomba\n");
		limpa = true;
	}

	if(j2.estoura==1 && j2.podeEstoura){
		// lembrar de verificar se a bomba ainda existe na fila.
		if(p2.fila.empty()) return;
		bomba aux = p2.fila.front();
		if(aux.x != aj.x || aux.y != aj.y) return;
		explosao(aux.x,aux.y);
		db printf("p2 explodiu a bomba\n");
		limpa = true;
	}

	if(limpa){
		imprimiTab();
		//imprimiTabArq();
		printEstado();
		puts("");
		puts("");
		for(int i=0; i<11; i++){
			for(int j=0; j<13; j++){
				if(strcmp(tab[i][j].str1,"FF")==0) strcpy(tab[i][j].str1,"--");
				if(strcmp(tab[i][j].str2,"FF")==0) strcpy(tab[i][j].str2,"--");

			}
		}
		imprimiTab();
		imprimiTabArq();
		printEstado();
	}else{
          imprimiTab();
		  imprimiTabArq();
		  printEstado();
    }

    db printf("acabou a funcao\n\n");

}



int main(){

    int rodada=1;
	char str[100];
	char comando[200];
	srand(time(NULL));
	criaTabuleiro();

	//pega o nome do código do primeiro jogador da forma nome.c
	//parte do jogador 1
	printf("Digite o nome do executavel para o jogador 1:\n");
	scanf("%s",str);
	strcpy(comando,"mv ");
	strcat(comando,str);
	strcat(comando, " jogador1");
	system(comando);

	//parte do jogador 2
	printf("Digite o nome do executavel para o jogador 2:\n");
	scanf("%s",str);
	strcpy(comando,"mv ");
	strcat(comando,str);
	strcat(comando, " jogador2");
	system(comando);
    
    ident = bckp = rand();

	while(true){
        
		//imprimiTab();
        movimento[0] = movimento[1] = 0;
		puts("");
		puts("");
		printf("Rodada %d:\n", rodada);
        if(rodada>MAX_ROUND)   break;
		fazJogada();
		rodada++;
        ident++;
        if(!estavivo1 || !estavivo2) break;
	}
	if(estavivo1 && !estavivo2) printf("Jogador 1 ganhou\n");
	else if (estavivo2 && !estavivo1) printf("Jogador 2 ganhou\n");
	else printf("Empate\n");

    return 0;
}

