
#include<stdio.h>
#include<ncurses.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ START OF DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ END OF DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


struct map_title *loc_init(FILE *cur_map_f, int side);
struct map_title * location_changer(int where_m, struct map_title *cur_map);

int player_move(int command, struct map_title *cur_map);
int is_wall(int direction, struct map_title *cur_map);

void map_upload(FILE *cur_map_f);
void printer(struct map_title *cur_map);
void hero_create(void);

const int Map_x = 2;
const int Map_y = 6;
const int Map_size_x = 120;
const int Map_size_y = 40;

const int Up = 1;
const int Right = 2;
const int Down = 3;
const int Left = 4;

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
};

struct map_title
{
	char *link_up;				// 1
	char *link_right;			// 2
	char *link_down; 			// 3
	char *link_left;			// 4
	char map[Map_size_y][Map_size_x];
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
//!
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

	printw("For exit press 'q' \n");  

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
	
	clear();
}



//!--------------------------------------------------------------------------
//!
//! Отвечает за перемещение персонажа
//!
//!--------------------------------------------------------------------------
int player_move(int command, struct map_title *cur_map)
{
	if( ((command == 'w')||(command == KEY_UP)) && Hero.cur_y > Map_y && is_wall(Up, cur_map) == 0)
		mvprintw(--Hero.cur_y, Hero.cur_x, "@");
	else if( ((command == 'w')||(command == KEY_UP)) && Hero.cur_y == Map_y && is_wall(Up, cur_map) == 0)
		return 1; // переход на локацию выше
	else if( ((command == 's')||(command == KEY_DOWN)) && Hero.cur_y < Map_y + Map_size_y - 1 && is_wall(Down, cur_map) == 0)
		mvprintw(++Hero.cur_y, Hero.cur_x, "@");
	else if( ((command == 's')||(command == KEY_DOWN)) && Hero.cur_y == Map_y + Map_size_y - 1 && is_wall(Down, cur_map) == 0)
		return 3; // переход на локацию ниже
	else if( ((command == 'd')||(command == KEY_RIGHT)) && Hero.cur_x < Map_x + Map_size_x - 1 && is_wall(Right, cur_map) == 0)
		mvprintw(Hero.cur_y, ++Hero.cur_x, "@");
	else if( ((command == 'd')||(command == KEY_RIGHT)) && Hero.cur_x == Map_x + Map_size_x - 1 && is_wall(Right, cur_map) == 0)
		return 2; // переход на локацию правее
	else if( ((command == 'a')||(command == KEY_LEFT)) && Hero.cur_x > Map_x && is_wall(Left, cur_map) == 0)
		mvprintw(Hero.cur_y, --Hero.cur_x, "@");
	else if( ((command == 'a')||(command == KEY_LEFT)) && Hero.cur_x == Map_x && is_wall(Left, cur_map) == 0)
		return 4; // переход на локацию левее
	else
		mvprintw(Hero.cur_y, Hero.cur_x, "@"); 		// Далее писать новые ифы, так как иначе при отсутствии движения персонаж пропадает с карты

	return 0;
}


//!
//!
//!
//!
//!
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
void printer(struct map_title *cur_map)
{
	int cur_x = 0;
	int max_x = Map_size_x;
	int cur_line = 0;
	int max_line = Map_size_y;

	while(cur_line != max_line)
	{
		cur_x = 0;

		while(cur_x != max_x)
		{
			if(cur_map->map[cur_line][cur_x] != '.')
				mvprintw(cur_line + Map_y, cur_x + Map_x, "%c", cur_map->map[cur_line][cur_x]);
			else
				mvprintw(cur_line + Map_y, cur_x + Map_x, " ");

			cur_x++;
		}
		
		cur_line++;
	}
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
	struct map_title *cur_map = loc_init(cur_map_f, 0);
	int where_m = 0;
	int cur_sym = 0;

	while(cur_sym != 'q')
	{
		while(cur_sym != 'q' && where_m == 0)
		{
			cur_sym = getch();
			printer(cur_map);
			where_m = player_move(cur_sym, cur_map);
    		refresh();                   
		}

		cur_map = location_changer(where_m, cur_map);

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
struct map_title *loc_init(FILE *cur_map_f, int side)
{
	struct map_title *cur_map = (struct map_title*) calloc(1, sizeof(struct map_title));

	int cur_line = 0;
	int max_line = Map_size_y;
	int cur_x = 0;
	int spawn = 0;
	int max_x = Map_size_x;

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
struct map_title *location_changer(int where_m, struct map_title *cur_map)
{
	struct map_title *old_map = cur_map;

	if(where_m == Up && strcmp(cur_map->link_up, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_up,"r"), where_m);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if(where_m == Right && strcmp(cur_map->link_right, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_right,"r"), where_m);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if(where_m == Down && strcmp(cur_map->link_down, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_down,"r"), where_m);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}
	if(where_m == Left && strcmp(cur_map->link_left, "null") != 0)
	{
		cur_map = loc_init(fopen(cur_map->link_left,"r"), where_m);
		free(old_map->link_up);
		free(old_map->link_down);
		free(old_map->link_left);
		free(old_map->link_right);
		free(old_map);
	}

	return cur_map;
}





