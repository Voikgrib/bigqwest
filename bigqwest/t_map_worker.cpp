
#include<stdio.h>
#include<ncurses.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<panel.h>

//			/home/artyom/bigquest/tyomdimich/inventory/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ START OF DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#define ADRES_INV "inventory/inv_"


#define V_CREATE_CHAR_UP(name)														\
			if(cur_pos == name && Hero.cur_special.name < 10)						\
			{																		\
				score_points--;														\
				Hero.cur_special.name++;											\
				mvwprintw(win, 12 + cur_pos * 2, 26, "%d", Hero.cur_special.name);	\
			}																		\


#define V_CREATE_CHAR_DOWN(name)													\
			if(cur_pos == name && Hero.cur_special.name > 1)						\
			{																		\
				score_points++;														\
				Hero.cur_special.name--;											\
				mvwprintw(win, 12 + cur_pos * 2, 27, "   ");						\
				mvwprintw(win, 12 + cur_pos * 2, 26, "%d", Hero.cur_special.name);	\
			}																		\


#define V_IF_IS_WALL( dir, check_x, check_y)										\
			if(																		\
			 	direction == dir &&													\
				(																	\
				cur_map->map[check_y - Map_y][check_x - Map_x] == '|' ||			\
				cur_map->map[check_y - Map_y][check_x - Map_x] == '=' ||			\
				cur_map->map[check_y - Map_y][check_x - Map_x] == '_' ||			\
				cur_map->map[check_y - Map_y][check_x - Map_x] == '/' ||			\
				cur_map->map[check_y - Map_y][check_x - Map_x] == '\\'				\
				)																	\
			  )																		\
			return 1;


#define V_SKIP( what )																\
			while(cur_dial[cur_pos] != what)										\
				cur_pos++;															\
			cur_pos++;																\

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ END OF DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


struct map_title *loc_init(FILE *cur_map_f, int side, struct npc **cur_npc);
struct map_title *location_changer(int where_m, struct map_title *cur_map, struct npc **cur_npc);

struct dial_tree_br *dial_upload(char *cur_dial, struct dial_tree_br *par, long int cur_pos);

struct npc *npc_finder(struct npc *cur_npc, int find_x, int find_y);
struct npc *npc_upload(char name);

int player_command(int command, struct map_title *cur_map);
int is_wall(int direction, struct map_title *cur_map);

void map_upload(FILE *cur_map_f);
void printer(struct map_title *cur_map, WINDOW *ramka);
void dialog_menu_start(struct npc *cur_npc);
void print_stats(void);
void print_npc(struct npc **cur_npc);
void open_inventory(void);

void hero_create(void);

const int Map_x = 2;
const int Map_y = 6;
const int Map_size_x = 120;
const int Map_size_y = 40;

const int Up = 1;
const int Right = 2;
const int Down = 3;
const int Left = 4;

const int V_max_npc = 5;

struct special
{
	int st;
	int pe;
	int en;
	int ch;
	int in;
	int ag;
	int lu;
};

struct entity
{
	int cur_x;
	int cur_y;
	int cur_hp;
	int max_hp;
	int cur_mp;
	int max_mp;
	struct special cur_special;
	int chsw;
	int* inventory;
	int shekel;
};

struct map_title
{
	char *link_up;				// 1
	char *link_right;			// 2
	char *link_down; 			// 3
	char *link_left;			// 4
	char map[Map_size_y][Map_size_x];
	struct npc *cur_npc;
};

struct dial_tree_br
{
	struct dial_tree_br *parent;
	char *npc_phrase;
	int is_end;
	struct dial_tree_br *br;
	char **phrase;
};

struct npc
{
	int frendly;
	char *name;
	struct dial_tree_br *dial_start;
	struct entity stats;
};

struct entity Hero;

//!----------------------------------------------------------------------
//!
//! Игора
//!
//! смещение на два символа и шесть строк у карты
//!
//! v 0.0.6
//!
//! Vova edition
//!									НЕОБХОДИМО ЗАПИЛИТЬ ПОИСК МОБОВ НА КАРТЕ 
//!----------------------------------------------------------------------
int main()
{
	int cur_sym = 0;
	
	initscr();           
	keypad(stdscr, TRUE);      
	curs_set(0);
	noecho();
	
	printw("%d, %d\n", COLS, LINES);

	hero_create();

	Hero.inventory[0] = 1001;               //тестовое наполнение инвентаря
	Hero.inventory[1] = 2002;
	Hero.inventory[2] = 3002;

	//printw("For exit press 'q' \n");  

	map_upload(fopen("maps/map1_1.txt", "r"));

    endwin();                    
    return 0;
}


//!-----------------------------------------------------------------------
//!
//!  Данная функция вызывается единожды и отвечает за создание персонажа
//!
//!-----------------------------------------------------------------------
void hero_create(void)
{
	int score_points = 33;
	int cur_pos = 0;
	int is_end = 0;
	int sym = 0;
	
	Hero.cur_special.st = 1;
	Hero.cur_special.pe = 1;
	Hero.cur_special.en = 1;
	Hero.cur_special.ch = 1;
	Hero.cur_special.in = 1;
	Hero.cur_special.ag = 1;
	Hero.cur_special.lu = 1;
	

	const int st = 0;
	const int ag = 1;
	const int ch = 2;
	const int in = 3;
	const int pe = 4;
	const int en = 5;
	const int lu = 6;
		
	
	int height_stdscr, width_stdscr;
	getmaxyx(stdscr, height_stdscr, width_stdscr);
	
	WINDOW* win = newwin(height_stdscr, width_stdscr, 0, 0);
	PANEL* pan = new_panel(win);
	keypad(win, TRUE);
	
	box(win, 0, 0);
	mvwprintw(win, 5,5, "___________________________________________________");
	mvwprintw(win, 6,5, "|                                                 |");
	mvwprintw(win, 7,5, "|               > Sozdanie personaza <            |");
	mvwprintw(win, 8,5, "|    Raspredelite 40 ochkov po xaracteristicam    |");
	mvwprintw(win, 9,5, "|                                                 |");
	mvwprintw(win, 10,5, "|_________________________________________________|");
	mvwprintw(win, 12,10, "Sila ---------  1"); /// 26
	mvwprintw(win, 14,10, "Lovcost ------  1");
	mvwprintw(win, 16,10, "Xarisma ------  1");
	mvwprintw(win, 18,10, "Entellect ----  1");
	mvwprintw(win, 20,10, "Vospriatie ---  1");
	mvwprintw(win, 22,10, "Vinoslivost --  1");
	mvwprintw(win, 24,10, "Ydacha -------  1");
	mvwprintw(win, 26,10, "Ostalos' ochkov: %d", score_points); 
	mvwprintw(win, 12, 5, "-->");
	wrefresh(win);

	while(is_end == 0)
	{
		sym = wgetch(win);

		if( ((sym == 'w')||(sym == KEY_UP)) && cur_pos > 0)
		{
			mvwprintw(win, 12 + cur_pos * 2, 5 , "   ");
			cur_pos--;
		}
		else if( ((sym == 's')||(sym == KEY_DOWN)) && cur_pos < 6)	
		{
			mvwprintw(win, 12 + cur_pos * 2, 5 , "   ");
			cur_pos++;
		}

		mvwprintw(win, 12 + cur_pos * 2, 5 , "-->");
		wrefresh(win);

		if( ((sym == 'd')||(sym == KEY_RIGHT)) && score_points > 0)
		{
			V_CREATE_CHAR_UP(st)
			V_CREATE_CHAR_UP(pe)
			V_CREATE_CHAR_UP(en)
			V_CREATE_CHAR_UP(ag)
			V_CREATE_CHAR_UP(ch)
			V_CREATE_CHAR_UP(in)
			V_CREATE_CHAR_UP(lu)
			mvwprintw(win, 26,10, "Ostalos' ochkov: %d                                         ", score_points); 
			wrefresh(win);
		}
		if( ((sym == 'a')||(sym == KEY_LEFT)) && score_points < 33)
		{
			V_CREATE_CHAR_DOWN(st)
			V_CREATE_CHAR_DOWN(pe)
			V_CREATE_CHAR_DOWN(en)
			V_CREATE_CHAR_DOWN(ag)
			V_CREATE_CHAR_DOWN(ch)
			V_CREATE_CHAR_DOWN(in)
			V_CREATE_CHAR_DOWN(lu)
			mvwprintw(win, 26,10, "Ostalos' ochkov: %d                                         ", score_points); 
			wrefresh(win);
		}
		
		if( score_points == 0)
		{
			mvwprintw(win, 26,10, "Ostalos' ochkov: %d   Zavershit' raspredelenie? (press q)", score_points); 
			wrefresh(win);
		}
		
		if(sym == 'q' && score_points == 0)
			is_end = 1;
	}

	Hero.max_hp = Hero.cur_special.st * 10 + Hero.cur_special.en * 5;
	Hero.cur_hp = Hero.max_hp;	
	Hero.max_mp = Hero.cur_special.in * 13 - Hero.cur_special.lu;
	Hero.cur_mp = Hero.max_mp;
	Hero.shekel = Hero.cur_special.lu * 7 + Hero.cur_special.ch * 2;
	
	del_panel(pan);
	delwin(win);
	clear();

	Hero.inventory = (int*) calloc(60, sizeof(int) );
}


//! 			struct npc
//!				{
//!				int frendly;
//!				char *name;
//!				struct dial_tree_br *dial_start;
//!				struct entity stats;
//!				};
//!
//!
//!			NT
//!-----------------------------------------------------------------------------------------
//!
//! Данная функция инициализирует нпс (если такой существует)
//!
//!-----------------------------------------------------------------------------------------
struct npc *npc_upload(char name)
{
	char fr = 0;

	char *main_adr = (char*) calloc(100, sizeof(char));
	char *dial_adr = (char*) calloc(100, sizeof(char));
	char *face_adr = (char*) calloc(100, sizeof(char));
	
	sprintf(main_adr, "npc/nonquest/%c/main.txt", name);
	sprintf(dial_adr, "npc/nonquest/%c/dialoge.txt", name);
	sprintf(face_adr, "npc/nonquest/%c/face.txt", name);

	FILE *main_f = fopen(main_adr, "r");

	if(main_f == NULL)
	{
		free(main_adr);
		free(dial_adr);
		free(face_adr);
		return NULL;
	}

	struct npc *cur_npc = (struct npc *) calloc(1, sizeof(struct npc));
	cur_npc->name = (char *) calloc(100, sizeof(char));
	
	fscanf(main_f, "%[^\n]", cur_npc->name);	fgetc(main_f);

	fscanf(main_f, "%c", &fr);					fgetc(main_f);

	if(fr == 'f')
		cur_npc->frendly = 1;
	else
		cur_npc->frendly = 0;
	
	fscanf(main_f, "%d", &cur_npc->stats.cur_special.st);	fgetc(main_f);
	fscanf(main_f, "%d", &cur_npc->stats.cur_special.pe);	fgetc(main_f);
	fscanf(main_f, "%d", &cur_npc->stats.cur_special.en);	fgetc(main_f);
	fscanf(main_f, "%d", &cur_npc->stats.cur_special.ch);	fgetc(main_f);
	fscanf(main_f, "%d", &cur_npc->stats.cur_special.in);	fgetc(main_f);
	fscanf(main_f, "%d", &cur_npc->stats.cur_special.ag);	fgetc(main_f);
	fscanf(main_f, "%d", &cur_npc->stats.cur_special.lu);	fgetc(main_f);

	fclose(main_f);

	FILE *dial_f = fopen(dial_adr, "r");

	fseek(dial_f, 0, SEEK_END);
	long int size = ftell(dial_f);

	char *file_info = (char *) calloc(size, sizeof(char));

	cur_npc->dial_start = dial_upload(file_info, NULL, 0);

	fclose(dial_f);

	//далее подгрузка лица персонажа в окне диалога
	mvprintw(30,31,"$$");///// TEST

	return cur_npc;
}


//!
//!
//!
//!
//!
void dialog_menu_start(struct npc *cur_npc)
{
	

}


//!------------------------------------------------------------------------
//!
//! Данная функция провнряет был ли инициализирован данный нпс, если был то задаёт его координаты
//!
//!------------------------------------------------------------------------
struct npc *npc_finder(char cur_sym, int find_x, int find_y)
{
	struct npc *cur_npc = npc_upload(cur_sym);

	if(cur_npc == NULL)
		return NULL;

	cur_npc->stats.cur_x = find_x;
	cur_npc->stats.cur_y = find_y;

	return cur_npc;
}


//!-------------------------------------------------------------------------------------------
//!
//! Данная функция печатает нпс
//!
//!-------------------------------------------------------------------------------------------
void print_npc(struct npc **cur_npc)
{
	int cur_num = 0;
	//mvprintw(30,30,"!"); //////// TEST

	while(cur_num != V_max_npc && cur_npc[cur_num] != NULL && cur_npc[cur_num]->name != NULL)
	{
		mvprintw(cur_npc[cur_num]->stats.cur_y, cur_npc[cur_num]->stats.cur_x, "%c", cur_npc[cur_num]->name[0]);
		cur_num++;
	}
}

//!-------------------------------------------------------------------------------------------
//!
//! Данная функция инициализирует диалоги
//!
//!-------------------------------------------------------------------------------------------
/*
struct dial_tree_br
{
	struct dial_tree_br *parent;
	char *npc_phrase;
	int is_end;
	struct dial_tree_br *br;
	char **phrase;
};

*/
//! 			T
//!
struct dial_tree_br *dial_upload(char *cur_dial, struct dial_tree_br *par, long int cur_pos)
{
	const int dial_max = 4;
	const int phrase_max = 200;

	int cur_dial_n = 0;
	int cur_phr = 0;
	int i = 0;

	struct dial_tree_br *cur_br = (struct dial_tree_br *)calloc(1, sizeof(struct dial_tree_br));

	cur_br->br = (struct dial_tree_br *)calloc(dial_max, sizeof(struct dial_tree_br));
	cur_br->npc_phrase = (char *)calloc(phrase_max, sizeof(char));

	while(cur_dial_n != dial_max)
	{	
		cur_br->phrase[cur_dial_n] = (char *)calloc(phrase_max, sizeof(char));
		cur_dial++;
	}

	cur_br->parent = par;
	cur_br->is_end = 0;

	cur_dial = 0;

	while(cur_dial[cur_pos] != '#' && cur_dial[cur_pos] != 'B')									
		cur_pos++;												

	if(cur_dial[cur_pos] == 'B')
	{
		while(cur_dial_n != dial_max)
		{
			free(cur_br->phrase[cur_dial_n]);			
			cur_dial_n++;
		}
		free(cur_br->br);
		free(cur_br->npc_phrase);
		cur_br->is_end = 1;
		return cur_br;
	}

	cur_pos++;

	while(cur_dial[cur_pos] != '#')
		cur_br->npc_phrase[i++] = cur_dial[cur_pos++];

	while(cur_dial_n != dial_max && cur_dial[cur_pos] != EOF)
	{
		i = 0;

		V_SKIP('(')
				/// тут будут условия появления фразы
		V_SKIP(')')
 		
		V_SKIP('"')

		while(cur_dial[cur_pos] != '"')
			cur_br->phrase[cur_dial_n][i++] = cur_dial[cur_pos++];
		cur_pos++;

		V_SKIP('{')

		cur_br->br = dial_upload(cur_dial, cur_br, cur_pos);

		V_SKIP('}')

		cur_dial_n++;
	}

	return cur_br;
}


//!-----------------------------------------------------------------------
//!
//!  Данная функция открывает инвентарь :I
//!
//!-----------------------------------------------------------------------
void open_inventory (void) 
{	
	int i, j;
	int cur_sym = 0;
	
	char* item_name = (char*)malloc(64*sizeof(char));
	char* adres_inv_j = (char*)malloc(192*sizeof(char));
	//инициализация окошек для инвентаря
	
	const int titley = 13;
	const int titlex = 12;
	const int listy = 16;
	const int listx = 12;
	const int descry = 13;
	const int descrx = 57;
	
	const int titleh = 3;
	const int titlew = 45;
	const int listh = 30;
	const int listw = 45;
	const int descrh = 33;
	const int descrw = 50;
	
	WINDOW* win_title = newwin(titleh, titlew, titley, titlex);
	WINDOW* win_list2 = newwin(listh, listw, listy, listx);
	WINDOW* win_list = newwin(listh, listw, listy, listx);
	WINDOW* win_descr = newwin(descrh, descrw, descry, descrx);
	
	PANEL* pan_title = new_panel(win_title);
	PANEL* pan_list2 = new_panel(win_list2);
	PANEL* pan_list = new_panel(win_list);
	PANEL* pan_descr = new_panel(win_descr);
	
	box(win_title, 0, 0);
	box(win_list, 0, 0);
	box(win_descr, 0, 0);
	
	mvwprintw(win_title, titleh/2, 2*titlew/5, "INVENTORY");
	
	wrefresh(win_title);
	wrefresh(win_list);
	wrefresh(win_descr);
	
	//прорисовывание содержимого инвентаря
	
	for (i = 0; Hero.inventory[i] != 0; i++)
	{	
		j = Hero.inventory[i]/100;
		//sprintf(adres_inv_j, "%s%d%s", ADRES_INV, j, ".txt");
		//FILE* inv_fp = fopen(adres_inv_j, "r"); 
		
		//тут надо как-то обрабатывать данные из файлика, чтобы потом выписывать их в окошко инвентаря
		
		if (i < 30)
		{
			mvwprintw(win_list, 1 + i, 5, "%d", Hero.inventory[i]); 	
			wrefresh(win_list);
		}
		else
		{
			mvwprintw(win_list2, 1 + i%30, 5, "%d", Hero.inventory[i]); 
			wrefresh(win_list2);	
		}
		
		//fclose(inv_fp);
	}
	
	//обработка действий пользователя
	
	while(cur_sym != 'q')
	{                
    	cur_sym = getch();
	} 
	
	//завершение работы инвентаря
	
	del_panel(pan_title);
	del_panel(pan_list);
	del_panel(pan_descr);
	
	delwin(win_title);
	delwin(win_list);
	delwin(win_descr);
}



//!--------------------------------------------------------------------------
//!
//! Отвечает за перемещение персонажа
//!
//!--------------------------------------------------------------------------
int player_command(int command, struct map_title *cur_map)
{
	int ret = 0;

	if( ((command == 'w')||(command == KEY_UP)) && Hero.cur_y > Map_y && is_wall(Up, cur_map) == 0)
		mvprintw(--Hero.cur_y, Hero.cur_x, "@");
	else if( ((command == 'w')||(command == KEY_UP)) && Hero.cur_y == Map_y && is_wall(Up, cur_map) == 0)
		ret = 1; // переход на локацию выше
	else if( ((command == 's')||(command == KEY_DOWN)) && Hero.cur_y < Map_y + Map_size_y - 1 && is_wall(Down, cur_map) == 0)
		mvprintw(++Hero.cur_y, Hero.cur_x, "@");
	else if( ((command == 's')||(command == KEY_DOWN)) && Hero.cur_y == Map_y + Map_size_y - 1 && is_wall(Down, cur_map) == 0)
		ret = 3; // переход на локацию ниже
	else if( ((command == 'd')||(command == KEY_RIGHT)) && Hero.cur_x < Map_x + Map_size_x - 1 && is_wall(Right, cur_map) == 0)
		mvprintw(Hero.cur_y, ++Hero.cur_x, "@");
	else if( ((command == 'd')||(command == KEY_RIGHT)) && Hero.cur_x == Map_x + Map_size_x - 1 && is_wall(Right, cur_map) == 0)
		ret = 2; // переход на локацию правее
	else if( ((command == 'a')||(command == KEY_LEFT)) && Hero.cur_x > Map_x && is_wall(Left, cur_map) == 0)
		mvprintw(Hero.cur_y, --Hero.cur_x, "@");
	else if( ((command == 'a')||(command == KEY_LEFT)) && Hero.cur_x == Map_x && is_wall(Left, cur_map) == 0)
		ret = 4; // переход на локацию левее
	else if( command == 'i' )
	{
		open_inventory();
		refresh();
	}
	//else
	mvprintw(Hero.cur_y, Hero.cur_x, "@"); 		// Далее писать новые ифы, так как иначе при отсутствии движения персонаж пропадает с карты

	return ret;
}


//!-----------------------------------------------------------------------------------------
//!
//! Данная функция отвечает за проверку на наличие стен на пути персонажа
//!
//!-----------------------------------------------------------------------------------------
int is_wall(int direction, struct map_title *cur_map)
{
	V_IF_IS_WALL(Up, Hero.cur_x, Hero.cur_y - 1)
	V_IF_IS_WALL(Right, Hero.cur_x + 1, Hero.cur_y)
	V_IF_IS_WALL(Down, Hero.cur_x, Hero.cur_y + 1)
	V_IF_IS_WALL(Left, Hero.cur_x - 1, Hero.cur_y)

	return 0;
}


//!------------------------------------------------------------------------------------------
//!
//! Данная функция за обновление карты
//!
//!------------------------------------------------------------------------------------------
void printer(struct map_title *cur_map, WINDOW *ramka)
{
	int cur_x = 0;
	int max_x = Map_size_x;
	int cur_line = 0;
	int max_line = Map_size_y;

	box(ramka, 0, 0);
	wrefresh(ramka);

	while(cur_line != max_line)
	{
		cur_x = 0;

		while(cur_x != max_x)
		{
			if(cur_map->map[cur_line][cur_x] != '.' && (cur_map->map[cur_line][cur_x] < 'a' || cur_map->map[cur_line][cur_x] > 'z'))
				mvprintw(cur_line + Map_y, cur_x + Map_x, "%c", cur_map->map[cur_line][cur_x]);
			else
				mvprintw(cur_line + Map_y, cur_x + Map_x, " ");

			cur_x++;
		}
		
		cur_line++;
	}

	print_stats();
}

//!----------------------------------------------------------------------------------------
//!
//! Данная функция отвечает за печать бара со статами
//!
//!----------------------------------------------------------------------------------------
void print_stats(void)
{
	int heg = 5;
	int len = 120;

	WINDOW *stats_bar = newwin(heg, len, 0, 0);

	box(stats_bar, 0, 0);

	mvwprintw(stats_bar, 1, 2,"HP %d   ", Hero.cur_hp);
	mvwprintw(stats_bar, 1, 10,"/ %d   ", Hero.max_hp);
	mvwprintw(stats_bar, 2, 2,"MP %d   ", Hero.cur_mp);
	mvwprintw(stats_bar, 2, 10,"/ %d   ", Hero.max_mp);
	mvwprintw(stats_bar, 3, 2, "shekel = %d         ", Hero.shekel);

	wrefresh(stats_bar);
}	


//!----------------------------------------------------------------------------------------
//! 
//! Данная кманда отвечает за погрузку первого тайла карты, а так же за движение персонажа
//!
//! Порядок описания ссылок
//! вверх - вправо - вниз - влево
//!
//! Размер 40 х 120
//! Сдвиг на (6,2)
//!
//!----------------------------------------------------------------------------------------
void map_upload(FILE *cur_map_f)
{
	int npc_num = 0;
	
	struct npc **cur_loc_npc = (struct npc **) calloc (V_max_npc, sizeof(struct npc*));

	while(npc_num != V_max_npc)
	{
		cur_loc_npc[npc_num] = (struct npc *) calloc (V_max_npc, sizeof(struct npc));
		npc_num++;
	}

	struct map_title *cur_map = loc_init(cur_map_f, 0, cur_loc_npc);
	int where_m = 0;
	int cur_sym = 0;	
	WINDOW *ramka = newwin(Map_size_y + 2, Map_size_x + 2, Map_y - 1, Map_x - 1);

	printer(cur_map, ramka);
	mvprintw(Hero.cur_y, Hero.cur_x, "@"); 
	refresh();	

	while(cur_sym != 'q')
	{
		while(cur_sym != 'q' && where_m == 0)
		{
			cur_sym = getch();
			printer(cur_map, ramka);
			print_npc(cur_loc_npc);
			//refresh();
			where_m = player_command(cur_sym, cur_map);
    		refresh();  
		}

		cur_map = location_changer(where_m, cur_map, cur_loc_npc);
		
		printer(cur_map, ramka);
		mvprintw(Hero.cur_y, Hero.cur_x, "@"); 
		refresh();

		where_m = 0;
	}
}


//!---------------------------------------------------------------------
//!
//! Данная функция инициализирует текущую локацию
//!
//! side - сторона, в которую игрок вышел с предыдущей карты
//!
//!---------------------------------------------------------------------
struct map_title *loc_init(FILE *cur_map_f, int side, struct npc **cur_npc)
{
	struct map_title *cur_map = (struct map_title*) calloc(1, sizeof(struct map_title));

	int cur_line = 0;
	int max_line = Map_size_y;
	int cur_x = 0;
	int spawn = 0;
	int max_x = Map_size_x;
	int npc_num = 0;

	cur_map->link_up = (char*) calloc(100, sizeof(char));
	cur_map->link_right = (char*) calloc(100, sizeof(char));
	cur_map->link_down = (char*) calloc(100, sizeof(char));
	cur_map->link_left  = (char*) calloc(100, sizeof(char));

	fscanf(cur_map_f, "%[^\n]", cur_map->link_up);		fgetc(cur_map_f);
	fscanf(cur_map_f, "%[^\n]", cur_map->link_right);	fgetc(cur_map_f); 
	fscanf(cur_map_f, "%[^\n]", cur_map->link_down);	fgetc(cur_map_f);
	fscanf(cur_map_f, "%[^\n]", cur_map->link_left);	fgetc(cur_map_f);

	while(cur_line != max_line)
	{
		cur_x = 0;

		while(cur_x != max_x)
		{
			cur_map->map[cur_line][cur_x] = fgetc(cur_map_f);
			
			if(cur_map->map[cur_line][cur_x] == 'S')
			{
				Hero.cur_x = cur_x + Map_x;
				Hero.cur_y = cur_line + Map_y;
				spawn = 1;
			}
			else if('a' <= cur_map->map[cur_line][cur_x] && cur_map->map[cur_line][cur_x] <= 'z')
			{
				cur_npc[npc_num] = npc_finder(cur_map->map[cur_line][cur_x], cur_x, cur_line);

				if(cur_npc[npc_num] != NULL)
					npc_num++;
			}

			cur_x++;
		}

		if(spawn == 0 && side != 0)
		{
			if(side == Up)
				Hero.cur_y = Map_y + Map_size_y - 1;
			else if(side == Right)
				Hero.cur_x = Map_x;
			else if(side == Down)
				Hero.cur_y = Map_y;
			else if(side == Left)
				Hero.cur_x = Map_x + Map_size_x - 1;
		}
		

		fgetc(cur_map_f);
		cur_line++;
	}

	fclose(cur_map_f);

	return cur_map;
}


//!------------------------------------------------------------------------------
//!
//! Данная команда отвечает за перемещение персонажа между локациями
//!
//!------------------------------------------------------------------------------
struct map_title *location_changer(int where_m, struct map_title *cur_map, struct npc **cur_npc)
{
	struct map_title *old_map = cur_map;

	int npc_num = 0;

	while(npc_num != V_max_npc)
		cur_npc[npc_num++] = NULL;

	if(where_m == Up && strcmp(cur_map->link_up, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_up,"r"), where_m, cur_npc);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if(where_m == Right && strcmp(cur_map->link_right, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_right,"r"), where_m, cur_npc);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if(where_m == Down && strcmp(cur_map->link_down, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_down,"r"), where_m, cur_npc);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if(where_m == Left && strcmp(cur_map->link_left, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_left,"r"), where_m, cur_npc);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}

	return cur_map;
}





