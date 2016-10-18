#include<curses.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<errno.h>
#include<error.h>

#define BEGMINES 10
#define BEG_BOARDSIZE 9


typedef enum { FLAGGED, CLICKED, UNCLICKED } SquareState;

//typedef enum { BEGINNER, INTERMEDIATE, EXPERT } Difficulty;

typedef struct square {
    struct square **touching;
    bool alreadychecked;
    SquareState state;
    int sqtouching;
    int surr_mines;
    bool hasmine;
} square;

typedef struct game {
    square *squares;
    int x, y;
    int boardsize;
    int mines;
    bool gameover;
} game;

typedef struct surr_sq_res {
    int *pos;
    int surr_num;
} surr_sq_res;
	

//void setUp(void);

void gameover_msg();

void drawgameboard(game*);

int left(game*);

int right(game*);

int up(game*);

int down(game*);

void input(game*);

void clicked(game*);

void flagged(game*);

void recursivelyClick(square*);

surr_sq_res *getsurrsquares(int, int);

game *newgame();



int main(int argc, char *argv[]) {
  
 initscr();
 noecho();			
 cbreak();		
					 
 keypad(stdscr, TRUE);		
 refresh();

 printf("start game\n");
 game * g;

 g = newgame();
 drawgameboard( g );
 input(g);

 
}


surr_sq_res *getsurrsquares( int pos, int boardsize) {

 surr_sq_res *result;

 result = (surr_sq_res *)malloc(sizeof(surr_sq_res));

 if( pos == 0) {			//top left corner

	result->surr_num = 3;
	result->pos = malloc(sizeof(int)*3);

	result->pos[0] = pos + 1;
	result->pos[1] = pos + boardsize;
	result->pos[2] = pos + boardsize + 1;
 }

 else if( pos == boardsize * boardsize + 1) {		//bottom right corner
	result->surr_num = 3;
	result->pos = malloc(sizeof(int)*3);

	result->pos[0] = pos - 1;
	result->pos[1] = pos - boardsize;
	result->pos[2] = pos - boardsize - 1;
 }

 else if( pos%boardsize == 0) {	// left edge
	
	if(pos + boardsize == boardsize * boardsize) {		//bottom left corner
		result->surr_num = 3;
		result->pos = malloc(sizeof(int)*3);

		result->pos[0] = pos + 1;
		result->pos[1] = pos - boardsize;
		result->pos[2] = pos - boardsize + 1;
	}

	else {		//left side

		result->surr_num = 5;
		result->pos = malloc(sizeof(int)*5);
		
		result->pos[0] = pos + 1;
		result->pos[1] = pos + boardsize;
		result->pos[2] = pos + boardsize + 1;
		result->pos[3] = pos - boardsize;
		result->pos[4] = pos - boardsize + 1;
	}

 }

 else if (pos%boardsize == boardsize - 1 ) { 		//right edge

	if( pos - boardsize == -1) { 		//top right corner
		result->surr_num = 3;
		result->pos = malloc(sizeof(int)*3);

		result->pos[0] = pos - 1;
		result->pos[1] = pos + boardsize;
		result->pos[2] = pos + boardsize - 1;
	}

	else { 		//right side
			
		result->surr_num = 5;
		result->pos = malloc(sizeof(int)*5);
		
		result->pos[0] = pos - 1;
		result->pos[1] = pos + boardsize;
		result->pos[2] = pos + boardsize - 1;
		result->pos[3] = pos - boardsize;
		result->pos[4] = pos - boardsize - 1;
	}
 }

 else if( pos + boardsize > boardsize * boardsize) { // bottom edge

	result->surr_num = 5;
	result->pos = malloc(sizeof(int)*5);
		
	result->pos[0] = pos - 1;
	result->pos[1] = pos + 1;
	result->pos[2] = pos - boardsize;
	result->pos[3] = pos - boardsize - 1;
	result->pos[4] = pos - boardsize + 1;
 }

 else if( pos - boardsize < 0) { 	//top egde

	result->surr_num = 5;
	result->pos = malloc(sizeof(int)*5);
		
	result->pos[0] = pos - 1;
	result->pos[1] = pos + 1;
	result->pos[2] = pos + boardsize;
	result->pos[3] = pos + boardsize - 1;
	result->pos[4] = pos + boardsize + 1;
 }

 else { 		// not an edge square

	result->surr_num = 8;
	result->pos = malloc(sizeof(int)*8);
		
	result->pos[0] = pos + 1;
	result->pos[1] = pos - 1;
	result->pos[2] = pos + boardsize;
	result->pos[3] = pos - boardsize;
	result->pos[4] = pos + boardsize + 1;
	result->pos[5] = pos - boardsize + 1;
	result->pos[6] = pos + boardsize - 1;
	result->pos[7] = pos - boardsize - 1;
 }

 return result;

}
	


game *newgame() {

 int mines, boardsize, i, j;

 game *g;
 
 g = (game *)malloc(sizeof(game));

 g->x = 0;
 g->y = 0;
 g->gameover = false;

 mines = BEGMINES;
 boardsize = BEG_BOARDSIZE;

 g->mines = mines;
 g->boardsize = boardsize;


 int no_squares = boardsize * boardsize;

 g->squares = malloc(sizeof(square)*no_squares);

 for( i = 0; i < no_squares; i++) {		/* initializing each square */

	g->squares[i].state = UNCLICKED;
	g->squares[i].surr_mines = 0;
	g->squares[i].hasmine = false;
	g->squares[i].alreadychecked = false;
 }


 for( i = 0; i < no_squares; i++ ) {

	surr_sq_res *r = getsurrsquares( i, boardsize);
	g->squares[i].sqtouching = r->surr_num;
	g->squares[i].touching = malloc(sizeof(square *) * r->surr_num);
	for( j = 0; j < r->surr_num; j++) {
		g->squares[i].touching[j] = ( g->squares + r->pos[j]); /* storing surrounding squares position for a particular square */
	}

 }

 int m_added = 0;

 while( m_added < mines ) {
	int position = rand() % no_squares;

	if(!(g->squares[position].hasmine)) {

		g->squares[position].hasmine = true;

		for( i = 0; i < g->squares[position].sqtouching; i++) {

			square *adjascent = g->squares[position].touching[i];
			adjascent->surr_mines ++;
		}
		m_added++;
	}
	
 }

 return g;
}


void drawgameboard( game *g ){

	WINDOW *win;
	int row, column, height, width, starty, startx;

	//height = BEG_BOARDSIZE * 2;
	//width = height * 2;
	
	height = 16;
	width = 32;

	starty = (LINES - height) / 2;	
	startx = (COLS - width) / 2;

	win = newwin(height, width, starty, startx);
	box(win, 0 , 0);		
	wrefresh(win);		

	for( row = 0; row < g->boardsize; row++) {
		mvwaddch( win, row + 1, 0 , '|');


	        for(column = 0; column < g->boardsize; column++){
			char square;
			int pos;

			pos = g->boardsize * row + column;
			switch(g->squares[pos].state) {

				case FLAGGED:
						//square = SQ_FLAGGED_CHAR;
						square = 'f';
						break;

				case CLICKED: 
						 if(g->squares[pos].surr_mines == 0 ) 
							square = ' '; 

						else
							square = (g->squares[pos].surr_mines + '0');
						break;

				case UNCLICKED:
						//square = SQ_UNCLICKED_CHAR;
						square = 'u';
						break;
			}
			mvwaddch(win, row + 1, column + 1, square);

		}

		
		waddch( win, '|');
		waddch(win, '\n');
	}

	wprintw( win, "\nx: %d\ny: %d\n", g->x, g->y);

	wmove( win, g->y + 1, g->x + 1);
 }

int up(game *g){

	if (g->y > 0) {
		g->y -= 1;
        	return 1;
   	} 
   	else {
        	return 0;
   	 }
}

int down(game *g){
	if ((g->y + 1) < g->boardsize) {
        	g->y += 1;
        	return 1;
    	} 
	else {
       	 return 0;
    	}
}

int left(game *g){
	if (g->x > 0) {
        	g->x -= 1;
        	return 1;
    	} 
	else {
        	return 0;
	}
}

int right(game *g){
	if (g->x + 1 < g->boardsize) {
        	g->x += 1;
        	return 1;
    	} 
	else {
        	return 0;
    	}
}

void flagged(game *g)
{
  square *square = g->squares + (g->y * g->boardsize + g->x);   /*pointing to the square that's flagged */

 if (square->state == UNCLICKED)
        square->state = FLAGGED;
 else if (square->state == FLAGGED)
        square->state = UNCLICKED;
   
}

/* void clicked( game *g) {

 square *square = g->squares + (g->y * g->boardsize + g->x);

} */

void gameover_msg(){

 char msg[] = "GAME OVER!";
 clear();
 mvprintw(8, 16 ,"%s", msg);
 endwin();
    
}

void input( game *g ) {

  int charecter = getch();
  switch (charecter) {
    		case KEY_UP:
        			up(g);
       				break;
   		case KEY_DOWN:
        			down(g);
        			break;
    		case KEY_RIGHT:
        			right(g);
        			break;
    		case KEY_LEFT:
        			left(g);
        			break;
    		/*case KEY_ENTER or KEY_RETURN:				
        			clicked(g);
        			break; */
    		case 'f':
        			flagged(g);
        			break;
    		default :
        			gameover_msg();
        			break;
    	}
    flushinp();
}
