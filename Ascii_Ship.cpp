#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<list>

using namespace std;

#define UP 72
#define LEFT 75
#define RIGHT 77
#define DOWN 80
#define SHOOT 32

void HideCursor();
void DrawBounds();
void gotoxy(int, int);

class Ship{
	int x, y, hearts, lives;
	
	public:
		Ship(int _x, int _y, int _hearts, int _lives) : x(_x), y(_y), hearts(_hearts), lives(_lives){}
		int GetX() { return x; }
		int GetY() { return y; }
		int GetLives() { return lives; }
		void LooseHeart() { hearts--; }
		void Render();
		void Clean();
		void Move();
		void DrawHearts();
		void Die();
};

class Asteroid{
	int x, y;
	
	public:
		Asteroid(int _x, int _y) : x(_x), y(_y){}
		int GetX() { return x; }
		int GetY() { return y; }
		void Render();
		void Move();
		void Collide(Ship &ship);
};

class Bullet{
	int x, y;
	
	public:
		Bullet(int _x, int _y) : x(_x), y(_y){}
		int GetX() { return x; }
		int GetY() { return y; }
		void Move();
		bool ReachedLimit();
};

int main(){
	
	bool game_over = false;
	int points = 0;
	
	HideCursor();
	DrawBounds();
	
	Ship ship(34, 29, 3, 3);
	
	list<Asteroid*> asts;
	list<Asteroid*>::iterator itA;
	for(int i = 0; i < 5; i++){
		asts.push_back(new Asteroid((rand() % 75) + 3,(rand() % 5) + 4));
	}
	
	list<Bullet*> bullets;
	list<Bullet*>::iterator itB;
	
	ship.Render();
	
	
	while(!game_over){
		
		gotoxy(4, 2); printf("POINTS: %d", points);
		
		if(kbhit()){
			char key = getch();
			if(key == SHOOT){
				bullets.push_back(new Bullet(ship.GetX() + 4, ship.GetY() - 1));
			}
		}
		
		for(itB = bullets.begin(); itB != bullets.end(); itB++){
			(*itB)->Move();
			
			if((*itB)->ReachedLimit()){
				gotoxy((*itB)->GetX(), (*itB)->GetY());
				printf(" ");
				delete(*itB);
				
				itB = bullets.erase(itB);
			}
		}
		
		for(itA = asts.begin(); itA != asts.end(); itA++){
			(*itA)->Move();
			(*itA)->Collide(ship);
		}
		
		for(itA = asts.begin(); itA != asts.end(); itA++){
			for(itB = bullets.begin(); itB != bullets.end(); itB++){
				if((*itA)->GetX() == (*itB)->GetX() && ( (*itA)->GetY()+1 == (*itB)->GetY() || (*itA)->GetY() == (*itB)->GetY() )){
					// Destroy bullet
					gotoxy((*itB)->GetX(), (*itB)->GetY());
					printf(" ");
					delete(*itB);
					itB = bullets.erase(itB);
					
					// Destroy asteroid & spawn another
					asts.push_back(new Asteroid((rand() % 75) + 3, 4));
					gotoxy((*itA)->GetX(), (*itA)->GetY());
					printf(" ");
					delete(*itA);
					itA = asts.erase(itA);
					
					points += 100;
				}
			}	
		}
		
		if(ship.GetLives() <= 0) game_over = true;
		
		ship.DrawHearts();
		ship.Move();
		ship.Die();
		
		Sleep(30);
	}
	
	gotoxy(35, 15); printf("GAME OVER");
	
	getch();
	return 0;
}

void HideCursor(){
	HANDLE hCon;
	CONSOLE_CURSOR_INFO cci;
	
	hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	cci.dwSize = 50;
	cci.bVisible = FALSE;
	
	SetConsoleCursorInfo(hCon, &cci);
}

void DrawBounds(){
	for(int i = 2; i < 78; i++){
		gotoxy(i, 3); printf("%c", 205);
		gotoxy(i, 33); printf("%c", 205);
	}
	
	for(int i = 4; i < 33; i++){
		gotoxy(2, i); printf("%c", 186);
		gotoxy(77, i); printf("%c", 186);
	}
	
	gotoxy(2, 3); printf("%c", 201);
	gotoxy(2, 33); printf("%c", 200);
	gotoxy(77, 3); printf("%c", 187);
	gotoxy(77, 33); printf("%c", 188);
}

void gotoxy(int x, int y){
	HANDLE hCon;
	COORD dwPos;
	
	hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	dwPos.X = x;
	dwPos.Y = y;
	
	SetConsoleCursorPosition(hCon, dwPos);	
}

void Ship::Render(){
	gotoxy(x, y); printf("    %c", 30);
	gotoxy(x, y + 1); printf("   %c%c%c", 91, 207, 93);
	gotoxy(x, y + 2); printf("  %c%c %c%c", 30, 190, 190, 30);
}

void Ship::Clean(){
	gotoxy(x, y);     printf("         ");
	gotoxy(x, y + 1); printf("         ");
	gotoxy(x, y + 2); printf("         ");
}

void Ship::Move(){
	if(kbhit()){
		char key = getch();
		Clean();
		
		if(key == LEFT && x > 3) x--;
		if(key == RIGHT && x + 6 < 75) x++;
		if(key == UP && y > 4) y--;
		if(key == DOWN && y + 3 < 33) y++;
		
		Render();
	}
}

void Ship::DrawHearts(){
	gotoxy(45, 2); printf("LIVES %d", lives);
	gotoxy(60, 2); printf("HEALTH");
	gotoxy(66, 2); printf("        ");
	
	for(int i = 0; i < hearts; i++){
		gotoxy(68 + i, 2); printf("%c", 3);
	}
}

void Ship::Die(){
	if(hearts <= 0){
		Clean();
		gotoxy(x, y);     printf("   **   ");
		gotoxy(x, y + 1); printf("  ****  ");
		gotoxy(x, y + 2); printf("   **   ");
		Sleep(200);
		
		Clean();
		gotoxy(x, y);     printf("*  **  *");
		gotoxy(x, y + 1); printf("  ****  ");
		gotoxy(x, y + 2); printf("*  **  *");
		Sleep(200);
		
		lives--;
		hearts = 3;
		Clean();
		Render();
		DrawHearts();
	}
}

void Asteroid::Render(){
	gotoxy(x, y); printf("%c", 184);
}

void Asteroid::Move(){
	gotoxy(x, y); printf(" ");
	y++;
	
	if(y > 32){
		x = (rand() % 71) + 4;
		y = 4;
	}
	
	Render();
}

void Asteroid::Collide(Ship &ship){
	if(x >= ship.GetX() && x <= (ship.GetX() + 6) && 
	 y >= ship.GetY() && y <= (ship.GetY() + 2)){
		ship.LooseHeart();
		ship.Render();
		ship.DrawHearts();	
		
		x = (rand() % 71) + 4;
		y = 4;
	}
}

void Bullet::Move(){
	gotoxy(x, y); printf(" ");
	y--;
	gotoxy(x, y); printf("*");
}

bool Bullet::ReachedLimit(){
	if(y == 4) return true;
	return false;
}
