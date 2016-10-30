#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <signal.h>
#include <time.h>
#include <curses.h>
#include<ncurses.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<error.h>
#include<errno.h>
#include<math.h>


int beg_mines = 10;
int inter_mines = 40;
int hard_mines = 90;
time_t startt, endt;
 
#define TOP    0
#define MIDDLE 1
#define BOTTOM 2
 
#define MINE 1
 
#define HIDDEN 128
#define FLAG 64
 
typedef struct field_mine {
        size_t column, row;
        size_t mines, squares;
	unsigned char *minefield;
}gameboard;

int gameboard_create(gameboard *, size_t, size_t, size_t);
void square_reveal(gameboard *, size_t, size_t);
void mines_reveal(gameboard *);
void square_flag(gameboard *, size_t, size_t, int);
void gameboard_display(gameboard *);
void wall_display(size_t, int);
void row_display(size_t, gameboard *);
int playgame(unsigned, unsigned, unsigned);
void update_score(int ,int);

int main(int argc, char **argv) {
 
	
    WINDOW *w;
    WINDOW *highscore;
    int high_score1, high_score2, high_score3;
    char rem[5];
    int number1[5], number2[5], number3[5];
    int j, remainder;
    char dig;
    int i;
    char list[100][100] = { "PLAY NEW GAME", "HOW TO PLAY?", "QUIT " };
    int ch, l = 0, ch1;
    size_t x1, y1;
    int ex, u = 0, z = 0, variable = 0;
    int rfd;
    initscr(); 
    getmaxyx(stdscr, y1, x1);
    w = newwin( y1, x1, 0, 0 ); 
    box( w, 0, 0 ); 

	start_color();			
	init_pair( 1, COLOR_BLACK, COLOR_GREEN);
	init_pair( 2, COLOR_BLACK, COLOR_WHITE );
    	init_pair( 3, COLOR_BLACK, COLOR_BLACK );
    	init_pair( 4, COLOR_BLUE,  COLOR_WHITE);
   	init_pair( 5, COLOR_BLACK,  COLOR_RED );
   	init_pair( 6, COLOR_BLACK, COLOR_YELLOW);
	init_pair( 7, COLOR_BLACK, COLOR_CYAN ); 
	
	wbkgd(w, COLOR_PAIR(6));
  
// now print all the menu items and highlight the first one

    for( l=0; l<3; l++ ) {
        if( l == 0 ) 
            wattron( w, A_STANDOUT ); // highlights the first item.
        else
            wattroff( w, A_STANDOUT );
 	wattron( w, A_BOLD );
        mvwprintw( w, y1/2+l, x1/2, "%s", list[l]);
	wattroff( w, A_BOLD );
    }                                                          
 
    wrefresh( w ); // update the terminal screen
 
    l = 0;
    noecho(); 
    keypad( w, TRUE ); 
    curs_set( 0 ); 

	while(1){ 
		ch = wgetch(w);
		if(ch == 10 && l == 0) {  /*the menu for the levels*/
			delwin(w);
			WINDOW *level;
			getmaxyx(stdscr, y1, x1);
			level = newwin( y1, x1, 0, 0 );
			box(level, 0, 0);
			wbkgd(level, COLOR_PAIR(1));
			char levels[100][100] = {  "EASY", "INTERMEDIATE", "DIFFICULT"};
			int bh, l1;
			for( l1 = 0; l1 < 3; l1++ ) {
				if( l1 == 0 ) 
					wattron( level, A_STANDOUT ); 
        			else
					wattroff( level, A_STANDOUT );
 
				mvwprintw(level, y1 / 2 + l1, x1 / 2, "%s", levels[l1]);
				wrefresh(level);
			}
			l1 = 0;
			keypad( level, TRUE );

			while(1) { 
				bh = wgetch(level);
				if(bh == 10 && l1 == 0) {
					delwin(level);
					unsigned i, j, k;
					i = j = 9;
       					k = 10;
					
					ex = playgame(i, j, k);
					if(ex == -1 || ex == 1 )
						break;
					if( ex == 0 )
						goto lable;

					
				}

				mvwprintw( level, y1 / 2 + l1, x1 / 2, "%s", levels[l1]);
				switch( bh ) {
					case KEY_UP:
                        			l1--;
                        			l1 = ( l1 < 0 ) ? 2 : l1;
                        			break;
                			case KEY_DOWN:
                        			l1++;
                        			l1 = ( l1 > 2 ) ? 0 : l1;  
                        			break;
					default :
						break;
            			}


				wattron( level, A_BOLD );
				wattron( level, A_STANDOUT );
				mvwprintw( level, y1 / 2 + l1, x1 / 2, "%s", levels[l1]);
				wattroff( level, A_STANDOUT );
				wattroff( level, A_BOLD );
				
				if(bh == 10 && l1 == 1) {

					delwin(level);
					unsigned i, j, k;
					i = j = 16;
       					k = 40;
					
					ex = playgame(i, j, k);
					if(ex == -1 || ex == 1 )
						break;
					if( ex == 0 )
						goto lable;	
					
					
				}

				if(bh == 10 && l1 == 2) {
				
					delwin(level);
					unsigned i, j, k;
					i = 25;
					j = 16;
       					k = 90;
					
					ex = playgame(i, j, k);
					if(ex == -1 || ex == 1 )
						break;	
					if( ex == 0 )
						goto lable;
				}

	
				} // bh while ends
			} //end of first while if first condition

			

			if(ex == -1) {
				variable = 1;
				while(1) {
					WINDOW *score;
					score = newwin(25,50, y1/4,x1/2);
					box(score, 0, 0);
					wattron( score, A_BOLD );
					wattron( score, COLOR_PAIR(5));
					mvwprintw(score, 1, 25, "YOU LOST!");
					wattroff( score, COLOR_PAIR(5));
					wattroff( score, A_BOLD );
					wrefresh(score);
					wattron( score, A_BOLD );
					mvwprintw(score, 3, 1, "TIME TAKEN : %ld seconds", endt - startt);
					wattroff( score, A_BOLD );
					wrefresh(score);
					wattron( score, A_BOLD );
					mvwprintw(score, 5, 1, "VIEW HIGH SCORES?");
					wattroff( score, A_BOLD );
					wrefresh(score);
					mvwprintw(score, 6, 1, "press y for yes or n to quit");
					wrefresh(score);
					int bh2;
					
				keypad( score, TRUE );
				

				bh2 = wgetch(score);
				if(bh2 == 'y') {
					delwin(score);
					
					rfd = open(argv[1], O_RDONLY);

					if(rfd == -1) {
						perror("cant open file");
						return errno;
					}
					
					highscore = newwin(25,50, y1/4,x1/2);
					box(highscore, 0, 0);
				int v;
				v = 6;
				mvwprintw(highscore, 1, 1, "#1 : ");
				wrefresh(highscore);	
				while(read(rfd, &dig, sizeof(char))) {
					
					if( dig == ' ') 
						break;
					mvwprintw(highscore, 1, v++, "%c", dig);
					wrefresh(highscore);
				}
				mvwprintw(highscore, 1, v, " seconds");
				wrefresh(highscore);

				v = 6;
				mvwprintw(highscore, 2, 1, "#2 : ");
				wrefresh(highscore);	
				while(read(rfd, &dig, sizeof(char))) {
					
					if( dig == ' ') 
						break;
					mvwprintw(highscore, 2, v++, "%c", dig);
					wrefresh(highscore);
				}
				mvwprintw(highscore, 2, v, " seconds");
				wrefresh(highscore);

				v = 6;
				mvwprintw(highscore, 3, 1, "#3 : ");
				wrefresh(highscore);	
				while(read(rfd, &dig, sizeof(char))) {
					
					if( dig == ' ') 
						break;
					mvwprintw(highscore, 3, v++, "%c", dig);
					wrefresh(highscore);
				}
				mvwprintw(highscore, 3, v, " seconds");
				wrefresh(highscore);

				close(rfd);
				
				mvwprintw(highscore, 24, 1, "press q to exit");
				wrefresh(highscore);

				}
				
				else if(bh2 == 'n') {
					z = 1;
					delwin(score);
					break;
				}
				
					
				if( (ch1 = getch()) == 'q') {
					u = 1;
					delwin(highscore);
					break;
				}
				
				}//end of while
				


			}
			else if(ex == 1) {
				
				
				variable = 1;
				rfd = open(argv[1], O_RDONLY);
				if(rfd == -1) {
					perror("cant open file");
					return errno;
				}

				i = 0;
				
				while(read(rfd, &dig, sizeof(char))){
					if( dig == ' ')
						break;
					number1[i++] = dig - '0';
				}
				
				high_score1 = (number1[0] * 100) + (number1[1] * 10) + number1[3]; 
				
				
				i = 0;
				
				while(read(rfd, &dig, sizeof(char))){
					if( dig == ' ')
						break;
					number2[i++] = dig - '0';
				}
				
				high_score2 = (number2[0] * 100) + (number2[1] * 10) + number2[3];
				

				i = 0;
				while(read(rfd, &dig, sizeof(char))){
					if( dig == ' ')
						break;
					number3[i++] = dig - '0';
				}
			
				high_score3 = (number3[0] * 100) + (number3[1] * 10) + number3[3];
				
				
				i = 0;
			
				if((endt-startt) < high_score1) {		
					close(rfd);
					rfd = open(argv[1], O_WRONLY);
					time_t diff;
					diff = endt - startt;
					
					while(diff>0) {
						remainder = diff%10;			//integer
						rem[i++] = remainder + '0';		// coverting int to char
						diff = diff/10;
					}
					for(j = i-1; j>=0; j--)
						write(rfd, &rem[j], sizeof(char)); 
				}

				i = 0;
				if(((endt-startt) > high_score1) && ((endt-startt) < high_score2) ) {
					char dig;
					close(rfd);
					rfd = open(argv[1], O_RDONLY|O_WRONLY);
					while(read(rfd, &dig, sizeof(char))) {
						if(dig == ' ') 
							break;
					}
					time_t diff;
					diff = endt - startt;
					
					while(diff>0) {
						remainder = diff%10;
						rem[i++] = remainder + '0';
						diff = diff/10;
					}
					for(j = i-1 ; j>=0; j--)
						write(rfd, &rem[j], sizeof(char)); 

				}
				i = 0;
				if(((endt-startt) > high_score2) && ((endt-startt) < high_score3) ) {
					char dig;
					close(rfd);
					rfd = open(argv[1], O_RDONLY|O_WRONLY);
					while(read(rfd, &dig, sizeof(char))) {
						if(dig == ' ') 
							break;
					}
					while(read(rfd, &dig, sizeof(char))) {
						if(dig == ' ') 
							break;
					}
					time_t diff;
					diff = endt - startt;

					while(diff>0) {
						remainder = diff%10;
						rem[i++] = remainder + '0';
						diff = diff/10;
					}
					for(j = i-1; j>=0; j--)
						write(rfd, &rem[j], sizeof(char)); 
				}
								
				close(rfd);
				while(1) {
					WINDOW *score;
					score = newwin(25,50, y1/4,x1/2);
					box(score, 0, 0);
					wattron( score, COLOR_PAIR(6));
					wattron( score, A_BOLD );
					mvwprintw(score, 1, 1, "YOU WIN!");
					wattroff( score, A_BOLD );
					wattroff( score, COLOR_PAIR(6));
					wrefresh(score);
					mvwprintw(score, 3, 1, "TIME TAKEN : %ld seconds", endt - startt);
					wrefresh(score);
					wattron( score, A_BOLD );
					mvwprintw(score, 5, 1, "VIEW HIGH SCORES?");
					wattroff( score, A_BOLD );
					wrefresh(score);
					mvwprintw(score, 6, 1, "press y for yes or n to quit");
					wrefresh(score);
					int bh2;
					
				keypad( score, TRUE );
				

				bh2 = wgetch(score);
				if(bh2 == 'y') {
					delwin(score);
					
					rfd = open(argv[1], O_RDONLY);

					if(rfd == -1) {
						perror("cant open file");
						return errno;
					}
					
					highscore = newwin(25,50, y1/4,x1/2);
					box(highscore, 0, 0);
					char dig;
				int v;
				v = 6;
				mvwprintw(highscore, 1, 1, "#1 : ");
				wrefresh(highscore);	
				while(read(rfd, &dig, sizeof(char))) {
					
					if( dig == ' ') 
						break;
					mvwprintw(highscore, 1, v++, "%c", dig);
					wrefresh(highscore);
				}
				mvwprintw(highscore, 1, v, " seconds");
				wrefresh(highscore);

				v = 6;
				mvwprintw(highscore, 2, 1, "#2 : ");
				wrefresh(highscore);	
				while(read(rfd, &dig, sizeof(char))) {
					
					if( dig == ' ') 
						break;
					mvwprintw(highscore, 2, v++, "%c", dig);
					wrefresh(highscore);
				}
				mvwprintw(highscore, 2, v, " seconds");
				wrefresh(highscore);

				v = 6;
				mvwprintw(highscore, 3, 1, "#3 : ");
				wrefresh(highscore);	
				while(read(rfd, &dig, sizeof(char))) {
					
					if( dig == ' ') 
						break;
					mvwprintw(highscore, 3, v++, "%c", dig);
					wrefresh(highscore);
				}
				mvwprintw(highscore, 3, v, " seconds");
				wrefresh(highscore);
				
				close(rfd);
				
				mvwprintw(highscore, 24, 1, "press q to exit");
				wrefresh(highscore);
				
				}
				
				else if(bh2 == 'n') {
					z = 1;
					delwin(score);
					break;
				}
					
					if( (ch1 = getch()) == 'q') {
						u = 1;
						delwin(highscore);
						break;
					}
				
				}
			}
			
			if( variable == 1) {
				if(u == 1) 
					break;
				if(z == 1)
					break;
			}
			mvwprintw( w, y1/2 + l, x1 / 2, "%s", list[l]);
			switch( ch ) {
			case KEY_UP:
                        	l--;
                        	l = ( l<0 ) ? 2 : l;
                        	break;
                	case KEY_DOWN:
                        	l++;
                        	l = ( l>2 ) ? 0 : l;  
                        	break;
			default :
				break;
            		}
            
			wattron( w, A_BOLD );	// now highlight the next item in the list.
			wattron( w, A_STANDOUT );
			mvwprintw( w, y1 / 2 + l, x1 / 2, "%s", list[l]);
			wattroff( w, A_STANDOUT );
			wattroff( w, A_BOLD );


			if( l == 1 && ch == 10){
				int get;
				delwin(w);
				WINDOW *howto;
				getmaxyx(stdscr, y1, x1);
				howto = newwin( y1, x1, 0, 0 );
				box(howto, 0, 0);
				wbkgd(howto, COLOR_PAIR(7));
				keypad(howto, TRUE);

			mvwprintw(howto, 1,1 , "Most people like to just click around the program hoping to not hit a mine, but if you want to master the game, then you have to play with");
 
			mvwprintw( howto, 2, 1, "strategy. The most important skill you can use while playing Minesweeper is a process of elimination.");
			
			wattron( howto, A_BOLD );
			wattron( howto, A_UNDERLINE );
			mvwprintw(howto, 5, 1, "DIRECTIONS TO PLAY : ");
			wattroff( howto, A_UNDERLINE);
			wattroff( howto, A_BOLD );

			mvwprintw(howto, 7, 22, "1. Use the arrow keys to move the cursor around the grid");
			mvwprintw(howto, 8, 22, "2. Press the space bar to reveal the square that you select");
			mvwprintw(howto, 9, 22, "3. Press the f key to flag those sqaures that may be mines");
			mvwprintw(howto, 10, 22, "4. Double pressing the f key makes the returns the square to it's original state");
			mvwprintw(howto, 11, 22, "5. You can also use your mouse to select the required square");
			mvwprintw(howto, 12, 22, "6. Double clicking can be used to flag the square");
			mvwprintw(howto, 13, 22, "7. Press q at any point in your game to quit the game");			
			mvwprintw(howto, y1 - 2, 1, "Press enter to quit");
			get = wgetch(howto);
			if( get == 10)
				break;
			
			
			}

			if(l == 2 && ch == 10) 
				break;
		} //end of first while

  lable:
   endwin();
   return 0;
} //end of main 




int playgame( unsigned i, unsigned j, unsigned k) {

int flag;
if( k == 10) 
	flag = 0;
if( k == 40)
	flag = 1;
if( k == 90)
	flag = 2;
        				gameboard g;
       					MEVENT event;
        				size_t x, y;
        				int charecter;
       					unsigned x1, y1;
 
       					 srand(time(NULL));
       					 initscr();
					 start_color();
					 init_pair( 1, COLOR_BLACK, COLOR_WHITE);
					 bkgd(COLOR_PAIR(1));
        				 keypad(stdscr, TRUE);
        				 nonl();
       					 cbreak();
       					 noecho();
       					 mousemask(BUTTON1_PRESSED | BUTTON3_PRESSED, NULL);
					 curs_set( TRUE );


  					getmaxyx(stdscr, y1, x1);
       					 if((i * 2 + 1) > y1 || (j * 2 + 1) > x1) {
              					  endwin();
              					  
        				}
 
        				if(gameboard_create(&g, i, j, k) == -1) {
               					 perror(NULL);
                				
        				}
					mvprintw(40, 10, "PRESS q TO QUIT GAME");
		


					y = 0;
					x = 0;
        				move(y , x);
        				gameboard_display(&g);
					if( flag == 0)
						mvprintw(25, 0, "MINES LEFT : 10");
					else if( flag == 1)
						mvprintw(35, 0, "MINES LEFT : 40");
					else
						mvprintw(35, 0, "MINES LEFT : 90");
					
 					move( 1, 1);
					refresh();
					time(&startt);

 
      
  while(g.squares > g.mines && (charecter = getch()) != 'q') {
                switch(charecter) {
                        case KEY_MOUSE:
                                if(getmouse(&event) == OK) {
                                        if(!((size_t)event.x & 1)) 
						event.x++;
                                        if(!((size_t)event.y & 1)) 
						event.y++;
                                        event.x /= 2;
                                        event.y /= 2;
                                        if((size_t)event.x >= g.column || (size_t)event.y >= g.row)
						 break;
                                        if(event.bstate & BUTTON1_PRESSED) {
                                                square_reveal(&g, event.x, event.y);
                                                if(g.minefield[g.column * event.y + event.x] == MINE) {
                                                        mines_reveal(&g);
                                                        move(0, 0);
                                                        gameboard_display(&g);
                                                        addstr("You lost!\n"); 
							time(&endt);
                                                        refresh();
							return -1;
                                     
                                                       
                                                }
                                        }
                                        else if(event.bstate & BUTTON3_PRESSED){
                                                square_flag(&g, event.x, event.y, flag);
						}
                                }
                       		break;
 
                        case KEY_RIGHT:
                                if(x != g.column - 1)  
					x++;
                       		break;
 
                        case KEY_LEFT:
                                if(x != 0) 
					x--;
                        	break;
 
                        case KEY_DOWN:
                                if(y != g.row - 1) 
					y++;
                        	break;
 
                        case KEY_UP:
                                if(y != 0) 
	
				y--;
                        	break;
 
                        case 'f':

                                square_flag(&g, x, y, flag);
                        	break;
 
                        case ' ':
                                square_reveal(&g, x, y);
                                if(g.minefield[g.column * y + x] == MINE) {
                                        mines_reveal(&g);
                                        move(0, 0);
                                        gameboard_display(&g);
					refresh();
                                        addstr("You lost!\n");
					refresh();
					time(&endt);
					return -1;
					break;
                                        
                                }
                        	break;
                	}
                move( 0, 0);
                gameboard_display(&g);
                move(y * 2 + 1 , x * 2 + 1);
                refresh();
        }

        if(g.squares == g.mines) {
                move( 0, 0);
                gameboard_display(&g);
		refresh();
                addstr("You won!\n");
		refresh();
		time(&endt);
		return 1;
        }
	
        free(g.minefield);
	return 0;
	
	
}


void update_score(int i, int flag) {

 if( flag == 0) {
	if( i == 0)
		mvprintw(25, 0, "MINES LEFT : %d%c", --beg_mines, ' ');
	else
		mvprintw(25, 0, "MINES LEFT : %d%c", ++beg_mines, ' ');
 }

 else if( flag == 1) {
	if( i == 0)
		mvprintw(35, 0, "MINES LEFT : %d%c", --inter_mines, ' ');
	else
		mvprintw(35, 0, "MINES LEFT : %d%c", ++inter_mines, ' ');
 }

 else {
	if( i == 0)
		mvprintw(40, 0, "MINES LEFT : %d%c", --hard_mines, ' ');
	else
		mvprintw(40, 0, "MINES LEFT : %d%c", ++hard_mines, ' ');
 }
}

void square_flag(gameboard *g, size_t x, size_t y, int flag) {

     start_color();	
     init_pair(1, COLOR_BLACK, COLOR_RED); 
     init_pair(2, COLOR_BLACK, COLOR_WHITE); 
        if(x >= g->column || y >= g->row) 
		return;

        if(g->minefield[g->column * y + x] & HIDDEN) {
		attron(COLOR_PAIR(1));
                g->minefield[g->column * y + x] |= FLAG;
		attroff(COLOR_PAIR(1));
                g->minefield[g->column * y + x] &= ~HIDDEN;
		int i = 0;
		update_score(i, flag);
        }
        else if(g->minefield[g->column * y + x] & FLAG) {
		attron(COLOR_PAIR(2));
                g->minefield[g->column * y + x] |= HIDDEN;
		attroff(COLOR_PAIR(2));
                g->minefield[g->column * y + x] &= ~FLAG;
		int i = 1;
		update_score(i, flag);
        }
}
 
void mines_reveal(gameboard *g) {
        size_t x, y;
 
        for(y = 0; y < g->row; y++)
                for(x = 0; x < g->column ; x++)
                        if((g->minefield[g->column * y + x] & ~(HIDDEN | FLAG)) == MINE)
                                square_reveal(g, x, y);
}
 
void square_reveal(gameboard *g, size_t x, size_t y) {
 
        unsigned char c;
        if(x >= g->column || y >= g->row) 
		return;
        c = g->minefield[g->column * y + x];
        if(c & HIDDEN || c & FLAG) {
                c &= ~(HIDDEN | FLAG);
                g->minefield[g->column * y + x] = c;
                g->squares--;
                if(c == '0') {
                        square_reveal(g, x - 1, y);
                        square_reveal(g, x + 1, y);
                        square_reveal(g, x, y - 1);
                        square_reveal(g, x, y + 1);
                        square_reveal(g, x - 1, y - 1);
                        square_reveal(g, x + 1, y + 1);
                        square_reveal(g, x - 1, y + 1);
                        square_reveal(g, x + 1, y - 1);
                }
        }
}
 
int gameboard_create(gameboard *g, size_t x, size_t y, size_t mines) {
 
        g->column = x;
        g->row = y;
        g->mines = mines;
        g->squares = x * y;
        g->minefield = malloc(g->squares);
        if(g->minefield == NULL) 
		return -1;
        memset(g->minefield, '0', g->squares);              /*initializing 0 to all squares , minefield points to these squarea */
 
        while(mines--) {
                	x = rand() % g->column;
                	y = rand() % g->row;
                	if(g->minefield[g->column * y + x] == MINE) {
                        	mines++;
                        	continue;
                	}
                	g->minefield[g->column * y + x] = MINE;
#define f(x0, y0) if(!((x0) >= g->column || (y0) >= g->row) && g->minefield[g->column * (y0) + (x0)] != MINE) g->minefield[g->column * (y0) + (x0)]++
                        f(x - 1, y  );
                        f(x + 1, y  );
                        f(x , y - 1);
                        f(x , y + 1);
                        f(x - 1, y - 1);
                        f(x + 1, y + 1);
                        f(x - 1, y + 1);
                        f(x + 1, y - 1);
#undef f
        }
        for(x = 0; x < g->squares; x++)
                g->minefield[x] |= HIDDEN; // everything gets the bullet
        return 0;
}
 
 
void gameboard_display(gameboard *g) {
 
        size_t random;
 
        wall_display(g->column, TOP);
        addch('\n');
        for(random = 0; random < g->row; random++) {
                row_display(random, g);
                addch('\n');
                if(random != g->row - 1) {
                        wall_display(g->column, MIDDLE);
                        addch('\n');
                }
        }
        wall_display(g->column, BOTTOM);
        addch('\n');

	
}
 
void wall_display(size_t x, int walltype) {
 
        size_t i;
       
        x = x * 2 - 1;
        if(walltype == TOP) {
                addch(ACS_ULCORNER);
                for(i = 0; i < x; i++) {
			if(i % 2 == 0)
	                        addch(ACS_HLINE);
			else
				addch(ACS_TTEE);
		}
                addch(ACS_URCORNER);
        }
        else if(walltype == MIDDLE) {
                addch(ACS_LTEE);
                for(i = 0; i < x; i++) {
			if(i % 2 == 0)
	                        addch(ACS_HLINE);
			else
				addch(ACS_PLUS);
		}
                addch(ACS_RTEE);
        }
        else {
                addch(ACS_LLCORNER);
                for(i = 0; i < x; i++) {
			if(i % 2 == 0)
	                        addch(ACS_HLINE);
			else
				addch(ACS_BTEE);
		}
                addch(ACS_LRCORNER);
        }
}
 
void row_display(size_t x, gameboard *g) {
 
        size_t i, j, k;
        unsigned char c;
         
        j = g->column * 2 + 1;
        for(i = k =  0; i < j; i++) {
		/* Checks if i is odd, and prints a bullet
		   Or else if i is even, it prints a '|' character
		 */
                if(i & 1) {				
                        c = g->minefield[g->column * x + k];
                        if(c & HIDDEN)
                                addch(ACS_BULLET);
                        else if(c & FLAG)
                                addch(ACS_DIAMOND);
                        else if(c == '0')
                                addch(' ');
                        else if(isdigit(c))
                                addch(c);
                        else if(c == MINE)
                                addch('X');
                        else 
				addch(' ');
                        k++;
                }
                else
                        addch(ACS_VLINE);
        }
}
