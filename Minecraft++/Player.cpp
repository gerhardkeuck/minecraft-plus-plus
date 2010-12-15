#include "Global.h"

Player player;

Player::Player() {
	pos.x = 0;
	pos.y = 0;
	pos.z = 14.5;
	pos.cx = 0;
	pos.cy = 0;
	pos.cz = -1;
	Window.SetCursorPosition(Window.GetWidth()/2,Window.GetHeight()/2);
}

void Player::Step() {
	const sf::Input& input = Window.GetInput();
	int mx = input.GetMouseX()-Window.GetWidth()/2;
	int my = input.GetMouseY()-Window.GetHeight()/2;
	Window.SetCursorPosition(Window.GetWidth()/2,Window.GetHeight()/2);
	rot.d += 0.2*mx;
	rot.p = min((double)89,max((double)-89,rot.p+0.2*my));
	if(input.IsKeyDown(sf::Key::W)){
		pos.x += ldx(0.1,rot.d);
		pos.y += ldy(0.1,rot.d);
	}
	if(input.IsKeyDown(sf::Key::A)){
		pos.x += ldx(0.1,rot.d-90);
		pos.y += ldy(0.1,rot.d-90);
	}
	if(input.IsKeyDown(sf::Key::S)){
		pos.x += ldx(0.1,rot.d+180);
		pos.y += ldy(0.1,rot.d+180);
	}
	if(input.IsKeyDown(sf::Key::D)){
		pos.x += ldx(0.1,rot.d+90);
		pos.y += ldy(0.1,rot.d+90);
	}
}