#include <SFML/Graphics.hpp>
#include <sstream>
#include <fstream>
//#include <iostream>
//#include <Windows.h>
#define PI 3.14159265

using namespace std;
using namespace sf;

const unsigned int mapW = 50;
const unsigned int mapH = 50;
const unsigned int blockSize = 64;
bool Map[mapH][mapW];

void control(CircleShape *const player, float *const angle, Vector2f *const playerDir, float gTime) {
	float moveSpd = 0.005f * (float)blockSize;
	float rotateSpd = 0.1f;
	if (Keyboard::isKeyPressed(Keyboard::Up)) {
		Vector2f dis = player->getPosition() + *playerDir * gTime * moveSpd;
		if (Map[(int)(dis.y / blockSize)][(int)(dis.x / blockSize)] == false)
			player->setPosition(Vector2f(dis));
	}
	if (Keyboard::isKeyPressed(Keyboard::Down)) {
		Vector2f dis = player->getPosition() + *playerDir * gTime * -moveSpd;
		if (Map[(int)(dis.y / blockSize)][(int)(dis.x / blockSize)] == false)
			player->setPosition(Vector2f(dis));
	}
	if (Keyboard::isKeyPressed(Keyboard::Right)) {
		*angle += gTime * rotateSpd;
		if (*angle >= 360.0) 
			*angle -= 360;
		*playerDir = Vector2f(cos(*angle * (float)PI / 180),  sin(*angle * (float)PI / 180));
	}
	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		*angle -= gTime * rotateSpd;
		if (*angle < 0.0)
			*angle += 360;
		*playerDir = Vector2f(cos(*angle * (float)PI / 180), sin(*angle * (float)PI / 180));
	}
}

inline float getFPS(float gTime) {
	return 1000 / gTime;
}

int main()
{
	const unsigned int WIDTH = 640;
	const unsigned int HEIGHT = 480;

	unsigned int depth = 3200;
	float rayStep = 1.0f;
	const float FOV = 60.0f;
	
	float angle = 0;
	unsigned int playerStartX = 0;
	unsigned int playerStartY = 0;

	ifstream file("map.txt");
	if (file.is_open()) {
		char sym;
		for (int i = 0; i < mapH; i++) {
			for (int j = 0; j < mapW; j++) {
				file.get(sym);
				if (sym == '#')
					Map[i][j] = 1;
				else if (sym == 'p') {
					playerStartX = j;
					playerStartY = i;
					Map[i][j] = 0;
				}
				else
					Map[i][j] = 0;
				//cout << sym;
			}
			file.get();
			//cout << endl;
		}
	}
	file.close();

	RenderWindow result(VideoMode(WIDTH, HEIGHT), "3D Ray Casting");
	CircleShape player(3.f);
	player.setFillColor(Color::Green);
	player.setOrigin(Vector2f(3, 3));
	player.setPosition(Vector2f((float)playerStartX * (float)blockSize + ((float)blockSize / (float)2), (float)playerStartY * (float)blockSize + ((float)blockSize / (float)2)));
	ostringstream fpsstream;
	Font font;
	font.loadFromFile("font.ttf");
	Text fpsText;
	fpsText.setFont(font);
	fpsText.setCharacterSize(18);
	fpsText.setFillColor(Color::Red);
	Clock gClock;
	Vector2f playerDir(1.0, 0.0);

	RectangleShape renderLine;

	while (result.isOpen())
	{
		float gTime = (float)gClock.getElapsedTime().asMicroseconds() / 1000;
		gClock.restart();
		Event event;
		while (result.pollEvent(event))
		{
			if (event.type == Event::Closed)
				result.close();
			if (event.type == Event::KeyPressed) {
				if (Keyboard::isKeyPressed(Keyboard::Subtract))
					depth--;
				else if (Keyboard::isKeyPressed(Keyboard::Add))
					depth++;
				else if (Keyboard::isKeyPressed(Keyboard::Q))
					rayStep -= (float)0.01;
				else if (Keyboard::isKeyPressed(Keyboard::W))
					rayStep += (float)0.01;
			}
		}

		control(&player, &angle, &playerDir, gTime);

		result.clear();
		for (int i = 0; i < WIDTH; i++) {
			float distance = 0;//размер луча
			bool isColl = false;//состояние луча
			float rayAngle = (angle - FOV / (float)2) + ((float)i / (float)WIDTH) * FOV;//угол луча
			Vector2f dir = Vector2f(cosf(rayAngle * (float)PI / 180), sinf(rayAngle * (float)PI / 180));//направление луча
			Vector2i testPoint = Vector2i(player.getPosition());//луч
			while (!isColl) {
				if(distance != 0) distance += rayStep * (distance / blockSize);
				else distance += rayStep;
				testPoint.x = (int)(player.getPosition().x + dir.x * distance);
				testPoint.y = (int)(player.getPosition().y + dir.y * distance);
				if (testPoint.x < 0 || testPoint.y < 0 || testPoint.x >= mapW * blockSize || testPoint.y >= mapH * blockSize) {
					isColl = true;
					distance = (float)depth;
				}
				else if (distance >= depth) {
					isColl = true;
				}
				else if (Map[(int)((float)testPoint.y / blockSize)][(int)((float)testPoint.x / blockSize)] == true){
					isColl = true;
					if(distance / blockSize < 1.0)
					distance = distance * cosf( (angle - rayAngle) * (float)PI / 180);
					renderLine.setSize(Vector2f(1, (float)HEIGHT * blockSize / distance));
					renderLine.setPosition(Vector2f((float)i, (float)HEIGHT / 2 - (float)HEIGHT * (float)blockSize / distance / 2));
					result.draw(renderLine);
				}
			}
		}
		fpsstream << "FPS: " << 1000 / gTime << endl << "Angle: " << angle << endl << "X: " << player.getPosition().x / blockSize << endl << "Y: " << player.getPosition().y / blockSize << endl << "Depth: " << depth << endl << "Step: " << rayStep;
		fpsText.setString(fpsstream.str().c_str());
		fpsstream.str("");
		result.draw(fpsText);
		result.display();
	}

	return 0;
}