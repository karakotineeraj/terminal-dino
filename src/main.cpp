#include <cstdlib>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <deque>

struct Position {
	int x;
	int y;
};

struct Player {
	struct Position pos;
	char disPlayer;
};

struct BackgroundObjects {
	struct Position pos;
	char disBackground;
};

struct Cactus {
	struct Position pos;
	uint_fast8_t size;
};

int main(int argc, char *argv[]) {
	srand(time(NULL));
	initscr();

	WINDOW *main_wnd = newwin(24, 80, 0, 0);

	struct Player player = { .pos = { .x =  6, .y =  22 } , .disPlayer =  'O' };

	std::deque<Cactus> obstacles;
	std::deque<BackgroundObjects> background;

	cbreak();
	noecho();
	nodelay(stdscr, true);
	curs_set(0); // Set cursor to Invisible
	
	if(!has_colors()) {
		endwin();
		printf("Your terminal doesn't support colors");
		exit(1);
	}

	start_color();
	refresh();

	box(main_wnd, 0, 0);
	wrefresh(main_wnd);

	// Draw the player
	wattron(main_wnd, A_BOLD);
	mvwaddch(main_wnd, player.pos.y, player.pos.x, 'O');
	wattroff(main_wnd, A_BOLD);

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	wrefresh(main_wnd);

	int ch, move_up = 0;
	int wait_time = 1, frame_passed = 0;
	bool end_program = false; 
	int_fast16_t tick = 1;

	while(true) {
		ch = getch();

		switch (ch) {
			case 'q':
				end_program = true;
				break;
			case 'w':
			case 'i':
			case KEY_UP:
				move_up = 3;
				break;
		}

		int vertical = std::rand() % 18  + 1;
		background.push_back({ .pos = { .x = 78, .y = vertical }, .disBackground = '.' });

		if (background.size() > 0 && background[0].pos.x == 2) {
				mvwaddch(main_wnd, background[0].pos.y, background[0].pos.x, ' ');

			background.pop_front();
		}

		for (int i = 0; i < background.size(); ++i) {
			mvwaddch(main_wnd, background[i].pos.y, background[i].pos.x, ' ');

			wattron(main_wnd, COLOR_PAIR(1));
			mvwaddch(main_wnd, background[i].pos.y, background[i].pos.x - 1, background[i].disBackground);
			wattroff(main_wnd, COLOR_PAIR(1));

			background[i].pos.x--;
		}

		if (tick % 2 == 0) {
			if (obstacles.size() > 0 && obstacles[0].pos.x == 2) {
				for (int j = 0; j < obstacles[0].size; ++j) {
					mvwaddch(main_wnd, obstacles[0].pos.y - j, obstacles[0].pos.x, ' ');
				}

				obstacles.pop_front();
			}

			for (int i = 0; i < obstacles.size(); ++i) {
				for (int j = 0; j < obstacles[i].size; ++j) {
					mvwaddch(main_wnd, obstacles[i].pos.y - j, obstacles[i].pos.x, ' ');
					wattron(main_wnd, A_BOLD);
					wattron(main_wnd, COLOR_PAIR(2));
					mvwaddch(main_wnd, obstacles[i].pos.y - j, obstacles[i].pos.x - 1, '|');
					wattroff(main_wnd, COLOR_PAIR(2));
					wattroff(main_wnd, A_BOLD);
				}

				obstacles[i].pos.x--;
			}

			if (move_up) {
				mvwaddch(main_wnd, player.pos.y, player.pos.x, ' ');
				wattron(main_wnd, A_BOLD);
				mvwaddch(main_wnd, --player.pos.y, player.pos.x, player.disPlayer);
				wattroff(main_wnd, A_BOLD);
				move_up--;
			} else if (player.pos.y < 22) {
				mvwaddch(main_wnd, player.pos.y, player.pos.x, ' ');
				wattron(main_wnd, A_BOLD);
				mvwaddch(main_wnd, ++player.pos.y, player.pos.x, player.disPlayer);
				wattroff(main_wnd, A_BOLD);
			}

			for (int i = 0; i < obstacles.size(); ++i) {
				if (obstacles[i].pos.x == player.pos.x && player.pos.y >= obstacles[i].pos.y - obstacles[i].size + 1) {
					end_program = true;
					break;
				}
			}

			if (wait_time < frame_passed) {
				obstacles.push_back({ .pos = { .x = 78, .y = 22 }, .size = static_cast<uint_fast8_t>(rand() % 3 + 1) });

				for (int j = 0; j < obstacles[obstacles.size() - 1].size; ++j) {
					wattron(main_wnd, A_BOLD);
					mvwaddch(main_wnd, obstacles[obstacles.size() - 1].pos.y - j, obstacles[obstacles.size() - 1].pos.x, '|');
					wattroff(main_wnd, A_BOLD);
				}

				wait_time = rand() % 15 + 10;
				frame_passed = 0;
			}

			frame_passed++;
		}

		if (end_program) 
			break;

		wrefresh(main_wnd); // Refresh the created window

		usleep(100000); // Make the loop time consistent with 1 ms sleep
		tick++;
	}

	endwin();
	return 0;
}
