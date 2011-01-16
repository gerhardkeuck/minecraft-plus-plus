#include "Global.h"

bool Game::Active = true;
bool Game::Done = false;
boost::thread UpdateThread(ChunkUpdateThread);
boost::thread GenThread(ChunkGenThread);
boost::thread UnloadThread(ChunkUnloadThread);
double fps = 0;
double tdelta = 0;
double delta = 0;
uint64_t ticks = 0;

bool Game::Init() {
	InitBlocks();
	InitGen();
	Window.Create(sf::VideoMode(800,600,32),"Infinity Cubed",sf::Style::Close|sf::Style::Titlebar,sf::ContextSettings(24,0,0,3,3));
	Window.ShowMouseCursor(false);
	//Window.SetFramerateLimit(30);
	InitGraphics();
	Window.SetActive();
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
		case sf::Event::GainedFocus:
			Active = true;
			break;
		case sf::Event::LostFocus:
			Active = false;
			break;
		}
	}
	player.Step();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,(double)Window.GetWidth()/Window.GetHeight(),0.01,1000);
	gluLookAt(0,0,0,ldx(1,player.rot.d)*ldx(1,player.rot.p),ldy(1,player.rot.d)*ldx(1,player.rot.p),ldy(1,player.rot.p),0,0,-1);
	glTranslated(-player.pos.x,-player.pos.y,-player.pos.z+player.eyeh);
	glMatrixMode(GL_MODELVIEW);
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT);
	//SkyDraw();
	glClear(GL_DEPTH_BUFFER_BIT);
	//Draw everything
#ifdef _DEBUG
	uint8_t range = 7;
#else
	uint8_t range = 10;
#endif
	if(ticks%30==0){//Every so often run through and clean up the chunk list
		Chunks.clear();
		for(uint8_t a=0;a<=range*2;a++){
			for(uint8_t b=0;b<=range*2;b++){
				for(uint8_t c=0;c<=range*2;c++){
					GetChunk(player.pos.cx+a-range,player.pos.cy+b-range,player.pos.cz+c-range,true);
				}
			}
		}
		for(auto x=ChunkPos.begin();x!=ChunkPos.end();x++){
			for(auto y=x->second.begin();y!=x->second.end();y++){
				for(auto z=y->second.begin();z!=y->second.end();z++){
					Chunk* c=z->second;
					if(c!=0){
						double d = pdis(player.pos.cx,player.pos.cy,player.pos.cz,c->x,c->y,c->z);
						if(d<range){
							Chunks.insert(c);
						} else if(d>range*2 && c->updated && c->generated){
							AddChunkUnload(c);
						}
					}
				}
			}
		}
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_3D,TEX);
	for(auto i=Chunks.begin(); i!=Chunks.end(); i++){
		(*i)->Draw();
	}
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//Clear any finished VBOs
	while(!BuffersToUnload.empty() && ChunkUnload.try_lock()){
		auto it = BuffersToUnload.begin();
		glDeleteBuffers(1,&*it);
		BuffersToUnload.erase(it);
		ChunkUnload.unlock();
	}
	//Display the screen
	Window.Display();
	fps = fps*0.8+0.2/Window.GetFrameTime();
	tdelta += Window.GetFrameTime();
	delta = min(max((double)Window.GetFrameTime(),0.0001),0.1);
	ticks++;
	if(ticks%10==0){
		//cout << fps << endl;
	}
	GLCHECK();
	return Window.IsOpened();
}
void Game::Unload() {
	Done = true;
	while(!ChunkThreadDone || !GenThreadDone || !UnloadThreadDone){
		sf::Sleep(0.01);
	}
}