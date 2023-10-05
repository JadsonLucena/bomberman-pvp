#include<stdio.h>
#include<stdlib.h>
#include<allegro.h>
#include<time.h>
#include<string.h>

//MACROS

#define TILESIZE 50

//variaveis globais

int screen_state;
int sound_state = TRUE;
volatile int exit_program = FALSE;
volatile int timer;

typedef struct tabela{
	char str1[3],str2[3];
	int bonus;
}tabela;

tabela tab[11][13];


int fecha_programa()
{
	exit_program = TRUE;
}
END_OF_FUNCTION(fecha_programa);

void incrementa_timer()
{
	timer++;
}
END_OF_FUNCTION(incrementa_timer);

void init() //INICIALIZA AS FUNÇÕES BÁSICAS DO ALLEGRO
{
    allegro_init();
    install_timer();
    install_keyboard();
    install_mouse();
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
    set_window_title("Bombermon");
   


    //TIMER
    timer = 0;
    LOCK_FUNCTION(incrementa_timer);
    LOCK_VARIABLE(timer);
    install_int_ex(incrementa_timer, BPS_TO_TIMER(4));

} END_OF_FUNCTION(init);


	
void draw_map(BITMAP* buffer, BITMAP* red, BITMAP* mew2, BITMAP* grama, BITMAP* bomba1, BITMAP* bomba2, BITMAP* bonusF, 
			BITMAP* bonusB, BITMAP* arvore, BITMAP* rocha, 	BITMAP* exp, SAMPLE* kabum, int x, int y)
{
	int i, j;
	

	
	for(i = 0; i<11; i++)
	{
		for(j = 0; j<13;j++)
		{
			draw_sprite(buffer, grama, j*TILESIZE + x, i*TILESIZE + y);
			
			
			
			if(!strcmp(tab[i][j].str2,"B1"))
				draw_sprite(buffer, bomba1, j*TILESIZE + x, i*TILESIZE + y);
			else if(!strcmp(tab[i][j].str2,"B2"))
				draw_sprite(buffer, bomba2, j*TILESIZE + x, i*TILESIZE + y);
			else if(!strcmp(tab[i][j].str2,"B3"))
			{
				draw_sprite(buffer, bomba1, j*TILESIZE + x -5, i*TILESIZE + y);
				draw_sprite(buffer, bomba2, j*TILESIZE + x +5, i*TILESIZE + y);
			}
			else if(!strcmp(tab[i][j].str2,"+B"))
				draw_sprite(buffer, bonusB, j*TILESIZE + x, i*TILESIZE + y);
			else if(!strcmp(tab[i][j].str2,"+F"))
				draw_sprite(buffer, bonusF, j*TILESIZE + x, i*TILESIZE + y);
				
				
			if(!strcmp(tab[i][j].str1, "XX"))
				draw_sprite(buffer, rocha, j*TILESIZE + x, i*TILESIZE + y);
			else if(!strcmp(tab[i][j].str1,"P1"))
				draw_sprite(buffer, red, j*TILESIZE + x, i*TILESIZE + y);
			else if(!strcmp(tab[i][j].str1,"P2"))
				draw_sprite(buffer, mew2, j*TILESIZE + x, i*TILESIZE + y);
			else if(!strcmp(tab[i][j].str1,"P3"))
			{
				draw_sprite(buffer, mew2, j*TILESIZE + x -5, i*TILESIZE + y);
				draw_sprite(buffer, red, j*TILESIZE + x + 5, i*TILESIZE + y);
			}
			else if(!strcmp(tab[i][j].str1,"MM"))
				draw_sprite(buffer, arvore, j*TILESIZE + x, i*TILESIZE + y);
			else if(!strcmp(tab[i][j].str1,"FF"))
			{
				draw_sprite(buffer, exp, j*TILESIZE + x, i*TILESIZE + y);
				//play_sample(kabum, 256, 128, 1000, FALSE);
			}
			
			
			
				
		}
	}
}
END_OF_FUNCTION(draw_map);

void gamescreen()
{
	int i,j,mp1,mp2,win = 0;
	
	FILE* arq = fopen("jogo.txt", "r");
	
	if(!arq) exit(33);
	//sons
	MIDI* game = load_midi("theme.mid");
	SAMPLE *kabum = load_sample("exposao.wav");
	MIDI* vic = load_midi("victory_theme.mid");
	
	
	
	//BITMAPS PLAYERS
	
	BITMAP* redpos[4];
	
	redpos[0] = load_bitmap("ashback.bmp", NULL);
	redpos[1] = load_bitmap("ashleft.bmp", NULL);
	redpos[2] = load_bitmap("ashfront.bmp", NULL);
	redpos[3] = load_bitmap("ashright.bmp", NULL);
	
	BITMAP* mew2pos[4];
	
	mew2pos[0] = load_bitmap("Mew2back.bmp", NULL);
	mew2pos[1] = load_bitmap("Mew2left.bmp", NULL);
	mew2pos[2] = load_bitmap("Mew2front.bmp", NULL);
	mew2pos[3] = load_bitmap("Mew2right.bmp", NULL);
	
	//BITMAPS
	
	BITMAP* buffer = create_bitmap(SCREEN_W, SCREEN_H);
		
	BITMAP* background = load_bitmap("background.bmp", NULL);
	BITMAP* backgroundst = load_bitmap("backgroundelecr.bmp", NULL);
	BITMAP* red = redpos[2];
	BITMAP* mew2 = mew2pos[2];
	BITMAP* grama = load_bitmap("grama_tile.bmp", NULL);
	BITMAP* bomba1 = load_bitmap("electrode.bmp", NULL);
	BITMAP* bomba2 = load_bitmap("electrode2.bmp", NULL);
	BITMAP* bonusF = load_bitmap("pineco.bmp", NULL);
	BITMAP* bonusB = load_bitmap("voltorb.bmp", NULL);
	BITMAP* arvore = load_bitmap("arvore.bmp", NULL);
	BITMAP* rocha = load_bitmap("rock.bmp", NULL);
	BITMAP* exp = load_bitmap("exp.bmp", NULL);
	
	BITMAP* ganhou1 = load_bitmap("ganha1.bmp", NULL);
	BITMAP* ganhou2 = load_bitmap("ganha2.bmp", NULL);
	BITMAP* empate = load_bitmap("empate.bmp", NULL);
	
	
    
    draw_sprite(buffer,backgroundst,0,0);
    draw_sprite(screen,buffer,0,0);
    
    play_midi(game, TRUE);
    set_volume(130, 130);
    
    while(!key[KEY_ENTER]);
	
	clear(buffer);
	stop_midi();
	
	play_midi(game, TRUE);
    set_volume(130, 130);
	while(!exit_program)
	{
		//criaTabuleiro(30,30,70);
		while(timer > 0 && !exit_program)
        {
	
				if(key[KEY_ESC])
					fecha_programa();
				
				
			
				fscanf(arq,"%d %d", &mp1, &mp2);
				if(mp1) red = redpos[mp1 - 1]; //faz a compatibilidade 
				if(mp2) mew2 = mew2pos[mp2 - 1];
			
				for(i = 0; i<11; i++)
					for(j = 0; j<13; j++)
					{
						char temp[5];
						fscanf(arq,"%s", temp);
						tab[i][j].str1[0] = temp[0];
						tab[i][j].str1[1] = temp[1];
						tab[i][j].str2[0] = temp[2];
						tab[i][j].str2[1] = temp[3];
					}
			
				fscanf(arq,"\n%d",&win);
			
				if(win)
					fecha_programa();
			
				
				draw_sprite(buffer,background,0,0);
				draw_map(buffer, red, mew2, grama, bomba1,bomba2, bonusF, bonusB,arvore,rocha,exp,NULL,60,30);//pode estar errado
				
				draw_sprite(screen, buffer, 0, 0);

		        clear(buffer);
		        timer--;
		}
        
	}
	draw_sprite(buffer,background,0,0);
	
	stop_midi();
	
	play_midi(vic, TRUE);
    set_volume(130, 130);
	
	
	
	if(win == 1)
		draw_sprite(buffer, ganhou1, 210, 55); //Desenha tela do vencedor
												
	else if(win == 2)
		draw_sprite(buffer, ganhou2, 210, 55);
	else
		draw_sprite(buffer, empate, 210, 55);
		
	draw_sprite(screen, buffer, 0, 0);
	
	
	while(!key[KEY_ESC]);
	stop_midi();
	
	destroy_bitmap(buffer);
    destroy_bitmap(background);
    destroy_bitmap(grama);
    destroy_bitmap(bomba1);
    destroy_bitmap(bomba2);
    destroy_bitmap(bonusF);
    destroy_bitmap(bonusB);
    destroy_bitmap(arvore);
    destroy_bitmap(rocha);
    destroy_bitmap(exp);
    destroy_bitmap(ganhou1);
    destroy_bitmap(ganhou2);
    destroy_bitmap(empate);
    destroy_bitmap(backgroundst);
    
	for(i = 0; i<4;i++)
	{
		destroy_bitmap(redpos[i]);
		destroy_bitmap(mew2pos[i]);
	}
			
	destroy_sample(kabum);
    destroy_midi(game);
    destroy_midi(vic);
}
END_OF_FUNCTION(gamescreen);

int main()
{
	init();
	gamescreen();
	allegro_exit();
	return 0;
}
END_OF_MAIN();
			
			
			
	
	
