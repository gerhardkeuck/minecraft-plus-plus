#include "Global.h"



bool Game::Init() {
	InitBlocks();
	Window.Create(sf::VideoMode(800,600,32),"Minecraft++",sf::Style::Close|sf::Style::Titlebar);
	Window.ShowMouseCursor(false);
	Window.SetFramerateLimit(30);
	InitGraphics();
	srand(clock());
	return true;
}
bool Game::Loop() {
	//Event handling
	sf::Event e;
	while(Window.GetEvent(e)){
		switch(e.Type){
		case sf::Event::Closed:
			Window.Close();
			break;
		case sf::Event::KeyPressed:
			switch(e.Key.Code){
			case sf::Key::Escape:
				Window.Close();
				break;
			}
		}
	}
	player.Step();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,(double)Window.GetWidth()/Window.GetHeight(),0.1,1000);
	gluLookAt(0,0,0,ldx(1,player.rot.d)*ldx(1,player.rot.p),ldy(1,player.rot.d)*ldx(1,player.rot.p),ldy(1,player.rot.p),0,0,-1);
	glTranslated(-player.pos.x,-player.pos.y,-player.pos.z);
	glMatrixMode(GL_MODELVIEW);
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	//Draw everything
	int range = 1;
	for(int64_t a=player.pos.cx-range;a<=player.pos.cx+range;a++){
		for(int64_t b=player.pos.cy-range;b<=player.pos.cy+range;b++){
			for(int64_t c=player.pos.cz-range;c<=player.pos.cz+range;c++){
				GetChunk(a,b,c)->Draw();
			}
		}
	}
	//Display the screen
	Window.Display();
	return Window.IsOpened();
}
void Game::Unload() {

}