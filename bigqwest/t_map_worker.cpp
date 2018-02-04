
#include<stdio.h>
#include<ncurses.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<panel.h>



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ START OF DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



#define V_CREATE_CHAR_UP(name)														\
			if(cur_pos == name && Hero.cur_special.name < 10)						\
			{																		\
				score_points--;														\
				Hero.cur_special.name++;											\
				mvwprintw(win, special_num_y + cur_pos * 2, special_num_x, "%d", Hero.cur_special.name);	\
			}																		\


#define V_CREATE_CHAR_DOWN(name)													\
			if(cur_pos == name && Hero.cur_special.name > 1)						\
			{																		\
				score_points++;														\
				Hero.cur_special.name--;											\
				mvwprintw(win, special_num_y + cur_pos * 2, special_num_x + 1, "   ");						\
				mvwprintw(win, special_num_y + cur_pos * 2, special_num_x, "%d", Hero.cur_special.name);	\
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
			
#define PRINT_INV_DESCR_NAME 														\
			fscanf(inv_fp, "%[^\n]", item_data);		fgetc(inv_fp);				\
			fscanf(inv_fp, "%[^=]", item_data);		fgetc(inv_fp);					\
			fscanf(inv_fp, "%[^\n]", item_data);		fgetc(inv_fp);				\
			mvwprintw(win_descr, name_y, name_x, "name: ");							\
			wprintw(win_descr, item_data);											\
			
#define PRINT_INV_DESCR_CAT															\
			fscanf(inv_fp, "%[^=]", item_data);		fgetc(inv_fp);					\
			fscanf(inv_fp, "%[^\n]", item_data);		fgetc(inv_fp);				\
			mvwprintw(win_descr, cat_y, cat_x, "category: ");						\
			wprintw(win_descr, item_data);											\
			
#define REOPEN_CUR_ITEM_1														\
				sprintf(item_adres, "%s%d%s%d%s", "inventory/inv_", Hero.inventory[cur_pos].cat, "/inv_", Hero.inventory[cur_pos].id, ".txt");	\
				freopen(item_adres, "r", inv_fp); 			
			
#define REOPEN_CUR_ITEM_2													\
				sprintf(item_adres, "%s%d%s%d%s", "inventory/inv_", Hero.inventory[cur_pos+30].cat, "/inv_", Hero.inventory[cur_pos+30].id, ".txt");	\
				freopen(item_adres, "r", inv_fp); 									\

				
#define V_SKIP( what )																\
			while(cur_dial[cur_pos] != what)										\
				cur_pos++;															\
			cur_pos++;																\

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ END OF DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ПРОТОТИПЫ ФУНКЦИЙ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct map_title *loc_init(FILE *cur_map_f, int side, struct npc **cur_npc);
struct map_title * location_changer(int where_m, struct map_title *cur_map, struct npc **cur_npc);

struct dial_tree_br *dial_upload(char *cur_dial, struct dial_tree_br *par, long int cur_pos);

struct npc *npc_finder(struct npc *cur_npc, int find_x, int find_y);
struct npc *npc_upload(char name);

int player_command(int command, struct map_title *cur_map);
int is_wall(int direction, struct map_title *cur_map);

void map_upload(FILE *cur_map_f);
void print_map(struct map_title *cur_map, WINDOW *stats_bar);
void dialog_menu_start(struct npc *cur_npc);
void print_stats(WINDOW *stats_bar);
void open_inventory(void);

void hero_create(void);


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ОПИСАНИЕ КОНСТАНТ ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int Map_x = 2;
const int Map_y = 6;
const int Map_size_x = 120;
const int Map_size_y = 40;

const int Up = 1;
const int Right = 2;
const int Down = 3;
const int Left = 4;

const int V_max_npc = 5;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ОПИСАНИЕ СТРУКТУР ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

struct item
{
	int id;
	int cat;
	int is_equip;
	int equip_num;
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
	struct item* inventory;
	int shekel;
};

struct map_title
{
	char *link_up;				// 1
	char *link_right;			// 2
	char *link_down; 			// 3
	char *link_left;			// 4
	char map[Map_size_y][Map_size_x];
};

struct dial_tree_br //dialog tree branch
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
//!Vova edition
//!
//! v. 0.0.7 да кто вообще смотрит а эти версии? кому они сдались? какой в них смысл? :} 
//! Tyomdimich edition
//! 
//!----------------------------------------------------------------------
int main()
{
	int cur_sym = 0;
	
	initscr();           
	keypad(stdscr, TRUE);      
	curs_set(0);
	noecho();
	
	FILE* log_fp = fopen("log.txt", "w"); //создание/зануление файлов логов
	fclose(log_fp);
	
	hero_create();

	Hero.inventory[0].id = 101;
	Hero.inventory[0].cat = Hero.inventory[0].id/100;
	Hero.inventory[1].id = 302;
	Hero.inventory[1].cat = Hero.inventory[1].id/100;
	Hero.inventory[2].id = 627;
	Hero.inventory[2].cat = Hero.inventory[2].id/100;

	
	

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
	int score_points = 5;
	int cur_pos = 0;
	int is_end = 0;
	int sym = 0;
	
	Hero.cur_special.st = 5;
	Hero.cur_special.pe = 5;
	Hero.cur_special.en = 5;
	Hero.cur_special.ch = 5;
	Hero.cur_special.in = 5;
	Hero.cur_special.ag = 5;
	Hero.cur_special.lu = 5;
	
	const int special_num_y = 12;
	const int special_num_x = 26;
	const int special_point_y = 12;
	const int special_point_x = 5;

	const int st = 0;
	const int ag = 1;
	const int ch = 2;
	const int in = 3;
	const int pe = 4;
	const int en = 5;
	const int lu = 6;
	
	WINDOW* win = newwin(LINES, COLS, 0, 0);
	PANEL* pan = new_panel(win);
	keypad(win, TRUE);
	
	box(win, 0, 0);
	mvwprintw(win, 5,5, "___________________________________________________");
	mvwprintw(win, 6,5, "|                                                 |");
	mvwprintw(win, 7,5, "|               > Sozdanie personaza <            |");
	mvwprintw(win, 8,5, "|    Raspredelite 40 ochkov po xaracteristicam    |");
	mvwprintw(win, 9,5, "|                                                 |");
	mvwprintw(win, 10,5, "|_________________________________________________|");
	mvwprintw(win, 12,10, "Sila ---------  5"); /// 26
	mvwprintw(win, 14,10, "Lovcost ------  5");
	mvwprintw(win, 16,10, "Xarisma ------  5");
	mvwprintw(win, 18,10, "Entellect ----  5");
	mvwprintw(win, 20,10, "Vospriatie ---  5");
	mvwprintw(win, 22,10, "Vinoslivost --  5");
	mvwprintw(win, 24,10, "Ydacha -------  5");
	mvwprintw(win, 26,10, "Ostalos' ochkov: %d", score_points); 
	mvwprintw(win, 12, 5, "-->");
	wrefresh(win);

	while(is_end == 0)
	{
		sym = wgetch(win);

		if( ((sym == 'w')||(sym == KEY_UP)) && cur_pos > 0)
		{
			mvwprintw(win, special_point_y + cur_pos * 2, special_point_x, "   ");
			cur_pos--;
		}
		else if( ((sym == 's')||(sym == KEY_DOWN)) && cur_pos < 6)	
		{
			mvwprintw(win, special_point_y + cur_pos * 2, special_point_x, "   ");
			cur_pos++;
		}

		mvwprintw(win, special_point_y + cur_pos * 2, special_point_x, "-->");
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
		
		if((sym == 'Q')||(sym == 'q' && score_points == 0))
			is_end = 1;
	}

	Hero.max_hp = Hero.cur_special.st * 10 + Hero.cur_special.en * 5;
	Hero.cur_hp = Hero.max_hp;	
	Hero.max_mp = Hero.cur_special.in * 13 - Hero.cur_special.lu;
	Hero.cur_mp = Hero.max_mp;
	Hero.shekel = Hero.cur_special.lu * 7 + Hero.cur_special.ch * 2;
	
	del_panel(pan);
	delwin(win);
	refresh();

	//инициализация инвентаря
	Hero.inventory = (struct item*) calloc(60, sizeof(struct item) );
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
struct npc* npc_upload(char name)
{
	FILE* log_fp = fopen("log.txt", "a");
	fputs("\nstart of npc_upload\n", log_fp);
	fclose(log_fp);
		
	char is_friendly = 0;

	char* main_adr = (char*) calloc(100, sizeof(char));
	char* dial_adr = (char*) calloc(100, sizeof(char));
	char* face_adr = (char*) calloc(100, sizeof(char));
	
	log_fp = fopen("log.txt", "a");
	fputs("get adreses\n", log_fp);
	fclose(log_fp);
	
	sprintf(main_adr, "npc/nonquest/%c/main.txt", name);
	sprintf(dial_adr, "npc/nonquest/%c/dialoge.txt", name);
	sprintf(face_adr, "npc/nonquest/%c/face.txt", name);
	
	log_fp = fopen("log.txt", "a");
	fputs("got adreses\n", log_fp);
	fclose(log_fp);
	
	FILE *main_adr_fp = fopen(main_adr, "r");
	
	log_fp = fopen("log.txt", "a");
	fputs("open main_adr_fp\n", log_fp);
	fclose(log_fp);
	
	if(main_adr_fp == NULL)
	{
		free(main_adr);
		free(dial_adr);
		free(face_adr);
		return NULL;
	}



	struct npc* cur_npc = (struct npc *) calloc(1, sizeof(struct npc));
	cur_npc->name = (char *) calloc(100, sizeof(char));
	
	
	
	fscanf(main_adr_fp, "%[^\n]", cur_npc->name);	fgetc(main_adr_fp);

	fscanf(main_adr_fp, "%c", &is_friendly);		fgetc(main_adr_fp);

	if(is_friendly == 'f')
		cur_npc->frendly = 1;
	else
		cur_npc->frendly = 0;
	
	fscanf(main_adr_fp, "%d", &cur_npc->stats.cur_special.st);	fgetc(main_adr_fp);
	fscanf(main_adr_fp, "%d", &cur_npc->stats.cur_special.pe);	fgetc(main_adr_fp);
	fscanf(main_adr_fp, "%d", &cur_npc->stats.cur_special.en);	fgetc(main_adr_fp);
	fscanf(main_adr_fp, "%d", &cur_npc->stats.cur_special.ch);	fgetc(main_adr_fp);
	fscanf(main_adr_fp, "%d", &cur_npc->stats.cur_special.in);	fgetc(main_adr_fp);
	fscanf(main_adr_fp, "%d", &cur_npc->stats.cur_special.ag);	fgetc(main_adr_fp);
	fscanf(main_adr_fp, "%d", &cur_npc->stats.cur_special.lu);	fgetc(main_adr_fp);

	fclose(main_adr_fp);
	
	
	log_fp = fopen("log.txt", "a");
	fputs("fclose main_adr_fp\n", log_fp);
	fclose(log_fp);

	FILE *dial_adr_fp = fopen(dial_adr, "r");

	log_fp = fopen("log.txt", "a");
	fputs("fopen dial_adres_fp\n", log_fp);
	fclose(log_fp);	
	
	
	fseek(dial_adr_fp, 0, SEEK_END);
	long int size = ftell(dial_adr_fp);

	char* file_info = (char *) calloc(size, sizeof(char));

	cur_npc->dial_start = dial_upload(file_info, NULL, 0);

	fclose(dial_adr_fp);
	
	log_fp = fopen("log.txt", "a");
	fputs("fclose dial_adres_fp\n", log_fp);
	fclose(log_fp); 

	//далее подгрузка лица персонажа в окне диалога
	//mvprintw(30,31,"$$");///// TEST

	log_fp = fopen("log.txt", "a");
	fputs("end of npc_upload\n", log_fp);
	fclose(log_fp);

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
struct npc* npc_finder(char cur_sym, int find_x, int find_y)
{
	FILE* log_fp = fopen("log.txt", "a");	
	fputs("\nstart of npc_finder\n", log_fp);
	fclose(log_fp);
	
	struct npc* cur_npc = npc_upload(cur_sym);

	if(cur_npc == NULL)
		return NULL;

	cur_npc->stats.cur_x = find_x;
	cur_npc->stats.cur_y = find_y;
	
	log_fp = fopen("log.txt", "a");
	fputs("found cur_npc x, y\n", log_fp);
	fputs("end of npc_finder\n", log_fp);
	fclose(log_fp);
	
	return cur_npc;
}


//!-------------------------------------------------------------------------------------------
//!
//! Данная функция печатает нпс
//!
//!-------------------------------------------------------------------------------------------
void print_npc(struct npc** cur_npc)  //void print_npc(struct npc* cur_npc)  ???
{
	FILE* log_fp = fopen("log.txt", "a");
	fputs("\nstart of print_npc\n", log_fp);
	fclose(log_fp);
	
	int cur_num = 0;
	//mvprintw(30,30,"!"); //////// TEST

	while( (cur_num != V_max_npc) && (cur_npc[cur_num] != NULL) && (cur_npc[cur_num]->name != NULL) )
	{
		mvprintw(cur_npc[cur_num]->stats.cur_y, cur_npc[cur_num]->stats.cur_x, "%c", cur_npc[cur_num]->name[0]);
		cur_num++;
	}
	
	log_fp = fopen("log.txt", "a");
	fputs("end of print_npc\n", log_fp);
	fclose(log_fp);
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
//! БАГУЛЯ ХДЕ-ТО ТУТ

struct dial_tree_br* dial_upload(char* cur_dial, struct dial_tree_br* par, long int cur_pos)
{
	FILE* log_fp = fopen("log.txt", "a");	
	fputs("\nstart  of dial_upload\n", log_fp);
	fclose(log_fp);
	
	const int dial_max = 4;
	const int phrase_max = 200;

	int cur_dial_num = 0;
	int cur_phrase = 0;
	int i = 0;

	struct dial_tree_br* cur_br = (struct dial_tree_br *)calloc(1, sizeof(struct dial_tree_br));

	cur_br->br = (struct dial_tree_br *)calloc(dial_max, sizeof(struct dial_tree_br));
	cur_br->npc_phrase = (char *)calloc(phrase_max, sizeof(char));
	cur_br->phrase = (char **)calloc(dial_max, sizeof(char*));
	
	
	log_fp = fopen("log.txt", "a");	
	fputs("\nstart of while1 \n", log_fp);
	fclose(log_fp);
	
	while(cur_dial_num != dial_max)
	{	
		log_fp = fopen("log.txt", "a");	
		fputs("\nwhile1 processing.\n", log_fp);
		fclose(log_fp);
		
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		cur_br->phrase[cur_dial_num] = (char *)calloc(phrase_max, sizeof(char));
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		
		log_fp = fopen("log.txt", "a");	
		fputs("\nwhile1 processing..\n", log_fp);
		fclose(log_fp);
		
		cur_dial_num++;
		
		log_fp = fopen("log.txt", "a");	
		fputs("\nwhile1 processing...\n", log_fp);
		fclose(log_fp);
	}

	cur_br->parent = par;
	cur_br->is_end = 0;

	cur_dial_num = 0;
	
	log_fp = fopen("log.txt", "a");	
	fputs("\nstart of while2 \n", log_fp);
	fclose(log_fp);
	
	while(cur_dial[cur_pos] != '#' && cur_dial[cur_pos] != 'B')									
		cur_pos++;												

	if(cur_dial[cur_pos] == 'B')
	{
		while(cur_dial_num != dial_max)
		{
			free(cur_br->phrase[cur_dial_num]);			
			cur_dial_num++;
		}
		free(cur_br->br);
		free(cur_br->npc_phrase);
		cur_br->is_end = 1;
		return cur_br;
	}

	cur_pos++;
	
	log_fp = fopen("log.txt", "a");	
	fputs("\nstart of while3 \n", log_fp);
	fclose(log_fp);

	while(cur_dial[cur_pos] != '#')
		cur_br->npc_phrase[i++] = cur_dial[cur_pos++];
		
	log_fp = fopen("log.txt", "a");	
	fputs("\nstart of while3 \n", log_fp);
	fclose(log_fp);

	while(cur_dial_num != dial_max && cur_dial[cur_pos] != EOF)
	{
		i = 0;

		V_SKIP('(')
				/// тут будут условия появления фразы
		V_SKIP(')')
 		
		V_SKIP('"')

		while(cur_dial[cur_pos] != '"')
			cur_br->phrase[cur_dial_num][i++] = cur_dial[cur_pos++];
		cur_pos++;

		V_SKIP('{')

		cur_br->br = dial_upload(cur_dial, cur_br, cur_pos);

		V_SKIP('}')

		cur_dial_num++;
	}
	
	log_fp = fopen("log.txt", "a");
	fputs("end of dial_upload\n", log_fp);
	fclose(log_fp);

	return cur_br;
}



//!-----------------------------------------------------------------------
//!
//!  Данная функция открывает инвентарь и позволяет там че-т делать :I
//!
//!-----------------------------------------------------------------------
void open_inventory (void) 
{	
	//описание переменных и констант 
	
	int i;
	int cur_sym = 0;
	int cur_pos = 0;
	int cur_pos_y = 1;
	int cur_pos_x = 1;
	int min_pos = 0;
	int max_pos1 = 0;
	int max_pos2 = 0;
	int count_list = 1;
	
	char* item_data = (char*)malloc(192*sizeof(char));
	char* item_adres = (char*)malloc(192*sizeof(char));
	//char* tmp = (char*)malloc(192*sizeof(char));
	
	const int titley = 9;
	const int titlex = 12;
	const int listy = 12;
	const int listx = 12;
	const int descry = 9;
	const int descrx = 57;
	
	const int titleh = 3;
	const int titlew = 45;
	const int listh = 32;
	const int listw = 45;
	const int descrh = 35;
	const int descrw = 50;
	
	const int name_x = 2;
	const int name_y = 1;
	const int cat_x = 2;
	const int cat_y = 2;
	
	FILE* inv_fp;
	
	//инициализация окошек для инвентаря
	
	WINDOW* win_title = newwin(titleh, titlew, titley, titlex);
	WINDOW* win_list = newwin(listh, listw, listy, listx);
	WINDOW* win_list2 = newwin(listh, listw, listy, listx);
	WINDOW* win_descr = newwin(descrh, descrw, descry, descrx);
	
	PANEL* pan_title = new_panel(win_title);
	PANEL* pan_list2 = new_panel(win_list2);
	PANEL* pan_list = new_panel(win_list);
	PANEL* pan_descr = new_panel(win_descr);
	
	box(win_title, 0, 0);
	box(win_list, 0, 0);
	box(win_list2, 0, 0);
	box(win_descr, 0, 0);
	
	mvwprintw(win_title, titleh/2, 2*titlew/5, "INVENTORY");
	
	wrefresh(win_title);
	wrefresh(win_list);
	wrefresh(win_list2);
	wrefresh(win_descr);
	
	
	//прорисовывание содержимого инвентаря
	
	//инициализация адреса
	sprintf(item_adres, "%s%d%s%d%s", "inventory/inv_", Hero.inventory[0].cat, "/inv_", Hero.inventory[0].id, ".txt");
	inv_fp = fopen(item_adres, "r");  
	
	for (i = 0; Hero.inventory[i].id != 0; i++)
	{	
		//обработка даных из файлов с предметами
		sprintf(item_adres, "%s%d%s%d%s", "inventory/inv_", Hero.inventory[i].cat, "/inv_", Hero.inventory[i].id, ".txt");
		freopen(item_adres, "r", inv_fp); 
		fscanf(inv_fp, "%[^\n]", item_data);		fgetc(inv_fp);
		fscanf(inv_fp, "%[^=]", item_data);		fgetc(inv_fp);
		fscanf(inv_fp, "%[^\n]", item_data);		fgetc(inv_fp);
		
		if (i < 30)
		{
			mvwprintw(win_list, 1 + i, 5, "%s", item_data); 	
			wrefresh(win_list);
		}
		else
		{
			mvwprintw(win_list2, 1 + i%30, 5, "%s", item_data); 
			wrefresh(win_list2);	
		}
	}
	
	i--; //это нужно т.к. i в конце уходит на нулевой предмет
	
	if (i < 30)		//обозначаем максимальные позиции для обоих листов
		max_pos1 = i;
	else
	{
		max_pos1 = 29;
		max_pos2 = i - 30;
	}
	
	//вывод начального содержания на экран, если инвентарь не пустой
	if (Hero.inventory[0].id != 0)
	{
		mvwprintw(win_list, cur_pos_y, cur_pos_x, "-->");
		//вывод инфы на win_descr
		sprintf(item_adres, "%s%d%s%d%s", "inventory/inv_", Hero.inventory[0].cat, "/inv_", Hero.inventory[0].id, ".txt");
		freopen(item_adres, "r", inv_fp); 
		//название
		PRINT_INV_DESCR_NAME
		//категория
		PRINT_INV_DESCR_CAT
							
	}
		
	top_panel(pan_list);
	update_panels();
	doupdate();
	
	
	//надо прописать навигацию в менюшке 
	
	//e(enter) - надеть/снять
	//r - выкинуть	
	
	//обработка действий пользователя
	
	//вывод информации о предмете
	
	
	
	while((cur_sym != 'q')&&(cur_sym != 'i'))
	{                
    	cur_sym = getch();
    	    	
    	if( ((cur_sym == 'w')||(cur_sym == KEY_UP)) && (cur_pos > 0) )
			{
				if (count_list == 1)
				{
					mvwprintw(win_list, cur_pos_y, cur_pos_x, "   ");
					mvwprintw(win_list, --cur_pos_y, cur_pos_x, "-->");
					wrefresh(win_list);
					cur_pos--;
					
					//вывод инфы на win_descr
					REOPEN_CUR_ITEM_1
					//название
					PRINT_INV_DESCR_NAME
					//категория
					PRINT_INV_DESCR_CAT
					
					wrefresh(win_descr);


				}
				if (count_list == 2)
				{
					mvwprintw(win_list2, cur_pos_y, cur_pos_x, "   ");
					mvwprintw(win_list2, --cur_pos_y, cur_pos_x, "-->");
					wrefresh(win_list2);
					cur_pos--;
					
					//вывод инфы на win_descr
					REOPEN_CUR_ITEM_2
					//название
					PRINT_INV_DESCR_NAME
					//категория
					PRINT_INV_DESCR_CAT
					
					wrefresh(win_descr);
				}
			}
		if( ((cur_sym == 's')||(cur_sym == KEY_DOWN)) && (cur_pos < 29) )
			{		
				if ( (count_list == 1)&&(cur_pos < max_pos1) )
				{
					mvwprintw(win_list, cur_pos_y, cur_pos_x, "   ");
					mvwprintw(win_list, ++cur_pos_y, cur_pos_x, "-->");
					wrefresh(win_list);
					cur_pos++;
					
					//вывод инфы на win_descr
					REOPEN_CUR_ITEM_1
					//название
					PRINT_INV_DESCR_NAME
					//категория
					PRINT_INV_DESCR_CAT
					
					wrefresh(win_descr);

				}
				if ( (count_list == 2)&&(cur_pos < max_pos2) )
				{
					mvwprintw(win_list2, cur_pos_y, cur_pos_x, "   ");
					mvwprintw(win_list2, ++cur_pos_y, cur_pos_x, "-->");
					wrefresh(win_list2);
					cur_pos++;
					
					//вывод инфы на win_descr
					REOPEN_CUR_ITEM_2 
					//название
					PRINT_INV_DESCR_NAME
					//категория
					PRINT_INV_DESCR_CAT
					
					wrefresh(win_descr);
				}
			}
		if( ( ((cur_sym == 'd')||(cur_sym == KEY_RIGHT)) ) && (count_list != 1) && (Hero.inventory[0].id!=0) )
			{
				mvwprintw(win_list2, cur_pos_y, cur_pos_x, "   ");
				wrefresh(win_list2);
				
				count_list = 1;
				cur_pos_y = 1;
				cur_pos = 0;
				mvwprintw(win_list, cur_pos_y, cur_pos_x, "-->");
				
				//вывод инфы на win_descr
				REOPEN_CUR_ITEM_1
				//название
				PRINT_INV_DESCR_NAME
				//категория
				PRINT_INV_DESCR_CAT
				
				top_panel(pan_list);
				update_panels();
				doupdate();
			}
		if( ( ((cur_sym == 'a')||(cur_sym == KEY_LEFT)) ) && (count_list != 2) && (Hero.inventory[30].id!=0) )
			{
				mvwprintw(win_list, cur_pos_y, cur_pos_x, "   ");
				wrefresh(win_list);
				
				count_list = 2;
				cur_pos_y = 1;
				cur_pos = 0;
				mvwprintw(win_list2, cur_pos_y, cur_pos_x, "-->");
				
				//вывод инфы на win_descr
				REOPEN_CUR_ITEM_2
				//название
				PRINT_INV_DESCR_NAME
				//категория
				PRINT_INV_DESCR_CAT
				
				top_panel(pan_list2);
				update_panels();
				doupdate();
			}
	} 
	
	//завершение работы инвентаря
	fclose(inv_fp);
	
	free(item_adres);
	free(item_data);
	
	del_panel(pan_title);
	del_panel(pan_list);
	del_panel(pan_descr);
	
	delwin(win_title);
	delwin(win_list);
	delwin(win_descr);
}



//!--------------------------------------------------------------------------
//!
//! Отвечает за перемещение (и не только) персонажа
//!
//!--------------------------------------------------------------------------
int player_command(int command, struct map_title *cur_map)
{
	int border_trigger = 0;

	if( ((command == 'w')||(command == KEY_UP)) && Hero.cur_y > Map_y && is_wall(Up, cur_map) == 0)
		mvprintw(--Hero.cur_y, Hero.cur_x, "@");
	else if( ((command == 'w')||(command == KEY_UP)) && Hero.cur_y == Map_y && is_wall(Up, cur_map) == 0)
		border_trigger = 1; // переход на локацию выше
	else if( ((command == 's')||(command == KEY_DOWN)) && Hero.cur_y < Map_y + Map_size_y - 1 && is_wall(Down, cur_map) == 0)
		mvprintw(++Hero.cur_y, Hero.cur_x, "@");
	else if( ((command == 's')||(command == KEY_DOWN)) && Hero.cur_y == Map_y + Map_size_y - 1 && is_wall(Down, cur_map) == 0)
		border_trigger = 3; // переход на локацию ниже
	else if( ((command == 'd')||(command == KEY_RIGHT)) && Hero.cur_x < Map_x + Map_size_x - 1 && is_wall(Right, cur_map) == 0)
		mvprintw(Hero.cur_y, ++Hero.cur_x, "@");
	else if( ((command == 'd')||(command == KEY_RIGHT)) && Hero.cur_x == Map_x + Map_size_x - 1 && is_wall(Right, cur_map) == 0)
		border_trigger = 2; // переход на локацию правее
	else if( ((command == 'a')||(command == KEY_LEFT)) && Hero.cur_x > Map_x && is_wall(Left, cur_map) == 0)
		mvprintw(Hero.cur_y, --Hero.cur_x, "@");
	else if( ((command == 'a')||(command == KEY_LEFT)) && Hero.cur_x == Map_x && is_wall(Left, cur_map) == 0)
		border_trigger = 4; // переход на локацию левее
	else if( command == 'i' )
	{
		open_inventory();
	}
	//else 
	// Далее писать новые ифы для новых команд
	mvprintw(Hero.cur_y, Hero.cur_x, "@"); 		
	
	return border_trigger;
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
void print_map(struct map_title *cur_map, WINDOW *stats_bar)
{
	int cur_x = 0;
	int cur_y = 0;
	const int max_x = Map_size_x;
	const int max_y = Map_size_y;

	//wrefresh(ramka);

	while(cur_y != max_y)
	{
		cur_x = 0;

		while(cur_x != max_x)
		{
			if(cur_map->map[cur_y][cur_x] != '.')
				mvprintw(cur_y + Map_y, cur_x + Map_x, "%c", cur_map->map[cur_y][cur_x]);
			else
				mvprintw(cur_y + Map_y, cur_x + Map_x, " ");

			cur_x++;
		}
		
		cur_y++;
	}

	print_stats(stats_bar);
}

//!----------------------------------------------------------------------------------------
//!
//! Данная функция отвечает за печать бара со статами
//!
//!----------------------------------------------------------------------------------------
void print_stats(WINDOW *stats_bar)
{
	const int height_stats = 5;
	const int widht_stats = 122;

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
	FILE* log_fp = fopen("log.txt", "a");
	
	fputs("\nstart of map upload\n", log_fp);
	fputs("!!!start of map upload\n", log_fp);
	fputs("!!!start of map upload\n", log_fp);
	
	fclose(log_fp);

	int npc_num = 0;

	const int height_stats = 5;
	const int widht_stats = 122;


	//отчекать введеные функции нпс
	struct npc **cur_loc_npc = (struct npc **) calloc (V_max_npc, sizeof(struct npc*));

	while(npc_num != V_max_npc)
	{
		cur_loc_npc[npc_num] = (struct npc *) calloc (V_max_npc, sizeof(struct npc));
		npc_num++;
	}



	//выводим начальную картинку
	
	struct map_title *cur_map = loc_init(cur_map_f, 0, cur_loc_npc);
	int border_trigger = 0;
	int cur_sym = 0;
		
	WINDOW *stats_bar = newwin(height_stats, widht_stats, 0, 1);
	box(stats_bar, 0, 0);
	//статы обнвляются функцией print_stats в функции print_map
	
	WINDOW *ramka = newwin(Map_size_y + 2, Map_size_x + 2, Map_y - 1, Map_x - 1);
	box(ramka, 0, 0);
	wrefresh(ramka);
	
	print_map(cur_map, stats_bar);
	mvprintw(Hero.cur_y, Hero.cur_x, "@");
	refresh();
	
	//запускаем цикл на обработку действий юзера
	
	while(cur_sym != 'q')
	{
		while(cur_sym != 'q' && border_trigger == 0)
		{
			cur_sym = getch();
			print_map(cur_map, stats_bar);
			
			print_npc(cur_loc_npc); //!чекать работу!
			
			border_trigger = player_command(cur_sym, cur_map);
    		refresh();                   
		}
		
		//отрисовка новой локации при переходе
		cur_map = location_changer(border_trigger, cur_map, cur_loc_npc);
		print_map(cur_map, stats_bar);
		mvprintw(Hero.cur_y, Hero.cur_x, "@"); 
		refresh();

		border_trigger = 0;
	}
	
	log_fp = fopen("log.txt", "a");
	fputs("end of map_upload\n", log_fp);
	
	fclose(log_fp);
}


//!---------------------------------------------------------------------
//!
//! Данная функция инициализирует текущую локацию
//!
//! side - сторона, в которую игрок вышел с предыдущей карты
//!
//!---------------------------------------------------------------------
struct map_title *loc_init(FILE *cur_map_f, int side, struct npc** cur_npc)
{
	FILE* log_fp = fopen("log.txt", "a");
	
	fputs("\nstart of loc_init\n", log_fp);
	fclose(log_fp);
	
	struct map_title *cur_map = (struct map_title*) calloc(1, sizeof(struct map_title));

	
	const int max_y = Map_size_y;
	const int max_x = Map_size_x;	

	int cur_y = 0;
	int cur_x = 0;
	int spawn = 0;
	int npc_num = 0;

	cur_map->link_up = (char*) calloc(100, sizeof(char));
	cur_map->link_right = (char*) calloc(100, sizeof(char));
	cur_map->link_down = (char*) calloc(100, sizeof(char));
	cur_map->link_left  = (char*) calloc(100, sizeof(char));

	fscanf(cur_map_f, "%[^\n]", cur_map->link_up);		fgetc(cur_map_f);
	fscanf(cur_map_f, "%[^\n]", cur_map->link_right);	fgetc(cur_map_f); 
	fscanf(cur_map_f, "%[^\n]", cur_map->link_down);	fgetc(cur_map_f);
	fscanf(cur_map_f, "%[^\n]", cur_map->link_left);	fgetc(cur_map_f);

	while(cur_y != max_y)
	{
		cur_x = 0;

		while(cur_x != max_x)
		{
			cur_map->map[cur_y][cur_x] = fgetc(cur_map_f);
			
			if(cur_map->map[cur_y][cur_x] == 'S')
			{
				Hero.cur_x = cur_x + Map_x;
				Hero.cur_y = cur_y + Map_y;
				spawn = 1;
			}
			
			else if( ('a' <= cur_map->map[cur_y][cur_x]) && (cur_map->map[cur_y][cur_x] <= 'z') )
			{
				cur_npc[npc_num] = npc_finder(cur_map->map[cur_y][cur_x], cur_x, cur_y);

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
		cur_y++;
	}

	fclose(cur_map_f);
	
	log_fp = fopen("log.txt", "a");
	fputs("end of loc_init\n", log_fp);
	
	fclose(log_fp);

	return cur_map;
}


//!------------------------------------------------------------------------------
//!
//! Данная команда отвечает за перемещение персонажа между локациями
//!
//!------------------------------------------------------------------------------
struct map_title *location_changer(int border_trigger, struct map_title *cur_map, struct npc** cur_npc)
{
	FILE* log_fp = fopen("log.txt", "a");
	
	fputs("\nstart of location_changer\n", log_fp);
	
	fclose(log_fp);
	
	struct map_title *old_map = cur_map;
	
	int npc_num = 0;

	while(npc_num != V_max_npc)
		cur_npc[npc_num++] = NULL;
		
		
	if (border_trigger == Up && strcmp(cur_map->link_up, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_up,"r"), border_trigger, cur_npc);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if (border_trigger == Right && strcmp(cur_map->link_right, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_right,"r"), border_trigger, cur_npc);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if (border_trigger == Down && strcmp(cur_map->link_down, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_down,"r"), border_trigger, cur_npc);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if (border_trigger == Left && strcmp(cur_map->link_left, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_left,"r"), border_trigger, cur_npc);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	
	
	log_fp = fopen("log.txt", "a");
	fputs("end of location_changer\n", log_fp);
	
	fclose(log_fp);

	return cur_map;
}





