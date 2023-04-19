#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <chrono>
#include <time.h>
#include <list>
#include <string>


namespace game_project {


const double BaseSpeed = 0.1;
const double BaseBulletSpeed = 0.3;
const float BaseRad = 10;
const double CoolDown = 0.5;
const double AimInterval = 2;
const double AimButtonInterval = 0.5;
const int AimCapacity = 4;
const double ConnectAccuracy = 20;

const int BaseWidth = 800;
const int BaseHeight = 600;

enum eDirection { 
	STOP = 0,
	LEFT,
	RIGHT, 
	UP, 
	DOWN
};

class tank;
class bullet;
class aim;

class game final {
public:

	game(int width, int height) : width_{width}, height_{height} {}
	game() = delete;
	~game() {};

	void Setup();
	void Input(tank &mytank, sf::RenderWindow &window);
	void Logic();

	void menu(sf::RenderWindow & window); 

	void DrawObjects(sf::RenderWindow &window, tank &mytank);

	void AddBullet(bullet &mybullet);
	//void AddAim(object &mybullet);

	bool IsOver() { return GameOver_; }
	
	float getSpeed() { return AimSpeed_; }
private:

	void reset();

	std::chrono::time_point<std::chrono::high_resolution_clock> time_start = 
		std::chrono::high_resolution_clock::now();
	std::chrono::time_point<std::chrono::high_resolution_clock> aim_time = 
		std::chrono::high_resolution_clock::now();

	std::list<bullet> BulletArr_{};
	std::list<aim> AimArr_{};

	bool GameOver_ = 0;
	int width_, height_;
	int score_ = 0;

	int maxAims_ = 1;
	int maxSpeed_ = 1;
	float AimSpeed_ = 0.1;

	std::vector<std::string> speedStr_{"x0.5", "x1", "x2"}; 
};

class object {
public:
	float GetX() { return x_; };
	float GetY() { return y_; };

protected:
	float x_, y_;
};

class tank final : public sf::Sprite {	//, public sf::CircleShape {public object, 
public:

	tank() 	//	: sf::CircleShape{BaseRad} 
	{	
		tex.loadFromFile("img/newtank.png");
		

		sf::Sprite {};
		setTexture(tex);

		//setFillColor(sf::Color::Blue);
		setPosition({200,120});

		sf::Vector2f targetSize1(80.0f, 80.0f); //целевой размер
 
		setScale(
    	targetSize1.x / getLocalBounds().width, 
    	targetSize1.y / getLocalBounds().height);

		//setOrigin((sf::Vector2f)tex.getSize() / 2.f);

		setTextureRect(sf::IntRect(0,0, 93, 97));

	}

	eDirection GetDir() { return dir_; }
	double GetSpeed() { return speed_; }

private:
	friend void game::Input(tank &mytank, sf::RenderWindow &window);

	void setDirMove(eDirection dir);

	sf::Texture tex;


	double speed_ = BaseSpeed;
	eDirection dir_ = UP;
};

class bullet final : public sf::CircleShape { // public object, 
public:
	bullet(float x, float y, eDirection dir) : sf::CircleShape{7}, dir_{dir}
	{
		setFillColor(sf::Color::Green);
		setPosition({x, y});
	}
	//~bullet() {std::cout << "bul end\n";}

	void MoveBullet();
	
private:
	eDirection dir_;
};



class aim final : public sf::Sprite { //, public sf::RectangleShape { public object,
public:
	aim(float x, float y) //: sf::RectangleShape{sf::Vector2f(30, 30)}
	{
		sf::Sprite{};
		tex.loadFromFile("img/enemy.png");
		
		setTexture(tex);

		//setFillColor(sf::Color::Blue);
		//setPosition({200,120});
		setPosition({x, y});
		sf::Vector2f targetSize1(160.0f, 80.0f); //целевой размер
 
		setScale(
    	targetSize1.x / getLocalBounds().width, 
    	targetSize1.y / getLocalBounds().height);

		//setOrigin((sf::Vector2f)tex.getSize() / 2.f);
		//setTextureRect(sf::IntRect(0, 0, 900, 900));
		setTextureRect(sf::IntRect(244, 30, 648 - 244, 494 - 30));
		
		//setFillColor(sf::Color::Red);
		
	}

	aim(const aim &other) 
	{
		//tex = other.tex;
		sf::Sprite{};
		tex.loadFromFile("img/enemy.png");
		
		setTexture(tex);

		sf::Vector2f pos = other.getPosition();

		setPosition({pos.x, pos.y});
		sf::Vector2f targetSize1(160.0f, 80.0f); 
 
		setScale(
    	targetSize1.x / getLocalBounds().width, 
    	targetSize1.y / getLocalBounds().height);

		setTextureRect(sf::IntRect(244, 30, 648 - 244, 494 - 30));
	}
	
	void MoveAim(float speed);

private:
	eDirection dir_ = UP;
	sf::Vector2f pos_start_;
	sf::Vector2f pos_end_;
	
	int nIter = 0;

	sf::Texture tex{};
};

// ---- methods for game ----


void game::reset()
{
	GameOver_ = 1;
	width_, height_;
	score_ = 0;

	maxAims_ = 1;
	maxSpeed_ = 1;
	AimSpeed_ = 0.1;

	BulletArr_ = std::list<bullet>{};
	AimArr_ = std::list<aim>{};

	time_start = std::chrono::high_resolution_clock::now();
	aim_time = std::chrono::high_resolution_clock::now();
}

void game::Setup()
{
	GameOver_ = 1;
	sf::RenderWindow window(sf::VideoMode({BaseWidth, BaseHeight}), "Test!");
	
	// ---- BackGround ----	

	sf::Texture background;
	background.loadFromFile("img/landscape.jpg");
	sf::Sprite BackGrSprite(background);
	BackGrSprite.setPosition(0, 0);

	// --------------------

	window.clear();
	tank mytank{};


	// ---- Score Show ----

	sf::Font font;
	font.loadFromFile("arial.ttf");
	//font.LoadFromMemory(PointerToFileData, SizeOfFileData);
	sf::Text textScore("Score:", font,  20);
	textScore.setFillColor(sf::Color::Black);
	textScore.setPosition(680, 5);
	textScore.setStyle(sf::Text::Bold);

	// --------------------

	while (window.isOpen()) {

		if (GameOver_)
			menu(window);


		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		Input(mytank, window);

		textScore.setString(std::string("Score:") + std::to_string(score_));

		window.clear(sf::Color::Black);

		window.draw(BackGrSprite);
		window.draw(mytank);
		window.draw(textScore);
		DrawObjects(window, mytank);

		window.display();
	}
}

void game::Input(tank &mytank, sf::RenderWindow &window)
{
	// ---- moving ---- 

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		mytank.setDirMove(LEFT);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		mytank.setDirMove(RIGHT);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		mytank.setDirMove(UP);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		mytank.setDirMove(DOWN);
	}

	// ---- shooting ----

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

		auto time_finish = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration<double, std::milli>(time_finish - time_start); 

		if (elapsed.count() < CoolDown * 1000) 
			return;

		time_start = time_finish;

		sf::Vector2f pos = mytank.getPosition();
		float x = pos.x;
		float y = pos.y;
		BulletArr_.push_back(bullet{x, y, mytank.dir_});
		
	}
}

void game::DrawObjects(sf::RenderWindow &window, tank &mytank)
{	
	// ---- bullets ----

	auto bull = BulletArr_.begin();

	while (bull != BulletArr_.end()) {
		
		sf::Vector2f pos = bull->getPosition();
		//std::cout << "xy = " << pos.x << pos.y << std::endl;
		if (pos.x <= 0 || pos.x >= width_ || pos.y <= 0 || pos.y >= height_) {
			//std::cout <<"here1\n";
			bull = BulletArr_.erase(bull);
			//std::cout <<"here2\n";
			continue;
		}
		//std::cout <<"here3\n";
		window.draw(*bull);
		//std::cout <<"here4\n";
		bull->MoveBullet();
		//std::cout <<"here5\n";
		bull++;
	}

	// ---- aims ----

	auto time_now = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration<double, std::milli>(time_now - aim_time);
	
	if (elapsed.count() > AimInterval * 1000 && AimArr_.size() < maxAims_) {
		
		srand(time(nullptr));

		sf::Vector2f pos;
		sf::Vector2f tank_pos = mytank.getPosition();

		pos.x = rand() % BaseWidth;
		pos.y = rand() % BaseHeight;

		while (std::abs(pos.x - tank_pos.x) < 30)
			pos.x = rand() % BaseWidth;

		while (std::abs(pos.y - tank_pos.y) < 30)
			pos.y = rand() % BaseHeight;

		aim newaim{pos.x, pos.y};
		AimArr_.push_back(newaim);//aim{pos.x, pos.y});

		aim_time = time_now;
	}

	auto aim_it = AimArr_.begin();

	while (aim_it != AimArr_.end()) {

		sf::Vector2f aim_pos = aim_it->getPosition();
		sf::Vector2f tank_pos = mytank.getPosition();
		
		if (std::abs(aim_pos.x - tank_pos.x) < ConnectAccuracy && std::abs(aim_pos.y - tank_pos.y) < ConnectAccuracy) {
			//GameOver_ = 1;
			reset();
			return;
		}

		auto bullet_it = BulletArr_.begin();

		while (bullet_it != BulletArr_.end()) {
			sf::Vector2f bullet_pos = bullet_it->getPosition();
			

			if (std::abs(aim_pos.x - bullet_pos.x) < ConnectAccuracy && std::abs(aim_pos.y - bullet_pos.y) < ConnectAccuracy) {
				aim_it = AimArr_.erase(aim_it);
				bullet_it = BulletArr_.erase(bullet_it);
				score_++;
				//break;
				goto EndLoop;
			}
			else {
				//aim_it++;
				bullet_it++;
				
			}
		}
		
		//window.draw(*aim_it);
		aim_it++;
		
		EndLoop:
		;
	}

	aim_it = AimArr_.begin();
	while (aim_it != AimArr_.end()) {
		
		sf::Vector2f pos = aim_it->getPosition();
		//std::cout << "xy = " << pos.x << pos.y << std::endl;
		if (pos.x <= 0 || pos.x >= width_ || pos.y <= 0 || pos.y >= height_) {
			//std::cout <<"here1\n";
			aim_it = AimArr_.erase(aim_it);
			//std::cout <<"here2\n";
			continue;
		}
		window.draw(*aim_it);
		aim_it->MoveAim(getSpeed());
		aim_it++;
	}


}

// ---- methods for tank ----


void tank::setDirMove(eDirection dir)
{
	dir_ = dir;
	
	switch(dir) {
		case LEFT:
			setTextureRect(sf::IntRect(96,0, 96, 97));
			move(-GetSpeed(), 0);
			break;

		case RIGHT:
			setTextureRect(sf::IntRect(192,0, -96, 97));
			move(GetSpeed(), 0);
			break;

		case UP:
			setTextureRect(sf::IntRect(0,0, 93, 97));
			move(0, -GetSpeed());
			break;
		
		case DOWN:
			setTextureRect(sf::IntRect(0,97, 93, -97));
			move(0, GetSpeed());
			break;

		default:
			break;	
	}


}


// ---- methods for aim ----


void aim::MoveAim(float speed)
{
	switch(dir_) {
		case LEFT:
			move(-speed, 0);
			break;
		case RIGHT:
			move(speed, 0);
			break;
		case UP:
			move(0, -speed);
			break;
		case DOWN:
			move(0, speed);
			break;
		default:
			return;
	};

	srand(time(nullptr));

	nIter++;
	if (nIter == 10) {
		dir_ = (eDirection)(1 + rand() % 4);
		nIter &= 0;
	}



}


// ---- methods for bullet ----

void bullet::MoveBullet()
{
	switch(dir_) {
		case LEFT:
			move(-BaseBulletSpeed, 0);
			break;
		case RIGHT:
			move(BaseBulletSpeed, 0);
			break;
		case UP:
			move(0, -BaseBulletSpeed);
			break;
		case DOWN:
			move(0, BaseBulletSpeed);
			break;
		default:
			return;
	};
}

/**
 * 
 * 
 * Menu for game
 *
 *
 */

void game::menu(sf::RenderWindow & window) 
{
	sf::Texture menuTexture1, menuTexture2, menuTexture3, plusTexute, minusTexture, menuBackground;
	menuTexture1.loadFromFile("img/1.png");
	menuTexture2.loadFromFile("img/2.png");
	menuTexture3.loadFromFile("img/3.png");
	menuBackground.loadFromFile("img/Penguins.jpg");
	//menuBackground.loadFromFile("img/me.jpeg");
	plusTexute.loadFromFile("img/plus.png");
	//plusTexute.setSize(Vector2f(30, 30));
	minusTexture.loadFromFile("img/minus.png");
	
	sf::Sprite menu1(menuTexture1), menu2(menuTexture2), menu3(menuTexture3), menuMinus(minusTexture), menuPlus(plusTexute), 
		menuSpeedPlus(plusTexute), menuSpeedMinus(minusTexture), menuBg(menuBackground);

	bool isMenu = 1;
	int menuNum = 0;
	menu1.setPosition(100, 30);
	menu2.setPosition(100, 100);
	menu3.setPosition(100, 170);
	menuBg.setPosition(345, 0);
	
	// ---- settings rendering ----

	menuPlus.setPosition(154, 200);
	menuMinus.setPosition(98, 200);

	menuSpeedPlus.setPosition(174, 250);
	menuSpeedMinus.setPosition(98, 250);

	sf::Vector2f targetSize1(20.0f, 20.0f); //целевой размер
 
	menuPlus.setScale(
    	targetSize1.x / menuPlus.getLocalBounds().width, 
    	targetSize1.y / menuPlus.getLocalBounds().height);
	
	sf::Vector2f targetSize2(20.0f, 20.0f);
	menuMinus.setScale(
    	targetSize2.x / menuMinus.getLocalBounds().width, 
    	targetSize2.y / menuMinus.getLocalBounds().height);

	menuPlus.setColor(sf::Color::White);
	menuMinus.setColor(sf::Color::White);


	menuSpeedPlus.setScale(
    	targetSize1.x / menuSpeedPlus.getLocalBounds().width, 
    	targetSize1.y / menuSpeedPlus.getLocalBounds().height);
	
	menuSpeedMinus.setScale(
    	targetSize2.x / menuSpeedMinus.getLocalBounds().width, 
    	targetSize2.y / menuSpeedMinus.getLocalBounds().height);

	menuSpeedPlus.setColor(sf::Color::White);
	menuSpeedMinus.setColor(sf::Color::White);
	

	// ---------------------------



	// ---- Aim Number ----


	sf::Font font;
	font.loadFromFile("arial.ttf");
	//font.LoadFromMemory(PointerToFileData, SizeOfFileData);
	sf::Text text("1", font,  20);
	text.setFillColor(sf::Color::Black);
	text.setPosition(127, 201);
	text.setStyle(sf::Text::Bold);
	
	// --------------------------


	// ---- Speed of enemies ----
	

	sf::Text speedText("x1", font,  20);
	speedText.setFillColor(sf::Color::Black);
	speedText.setPosition(127, 251);
	speedText.setStyle(sf::Text::Bold);

	auto time_aim_button_start = std::chrono::high_resolution_clock::now();
	auto time_speed_button_start = std::chrono::high_resolution_clock::now();
 
	// --------------------------

	while (isMenu && window.isOpen())
	{

		menuNum = 0;
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		menu1.setColor(sf::Color::White);
		menu2.setColor(sf::Color::White);
		menuNum = 0;
		window.clear(sf::Color(129, 181, 221));
 
		if (sf::IntRect(100, 30, 300, 30).contains(sf::Mouse::getPosition(window)))  { menu1.setColor(sf::Color::Blue); menuNum = 1; }
		if (sf::IntRect(100, 100, 300, 30).contains(sf::Mouse::getPosition(window))) { menu2.setColor(sf::Color::Blue); menuNum = 2; }
		if (sf::IntRect(98, 200, 20, 20).contains(sf::Mouse::getPosition(window)))   { menuNum = 3; }
		if (sf::IntRect(154, 200, 20, 20).contains(sf::Mouse::getPosition(window)))  { menuNum = 4; }

		if (sf::IntRect(98, 250, 20, 20).contains(sf::Mouse::getPosition(window)))  { menuNum = 5; }
		if (sf::IntRect(174, 250, 20, 20).contains(sf::Mouse::getPosition(window)))  { menuNum = 6; }
 
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			if (menuNum == 1) { isMenu = false; GameOver_ = 0;}
			if (menuNum == 2) { window.close(); isMenu = false; }
			if (menuNum == 3) { 
				auto time_now = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration<double, std::milli>(time_now - time_aim_button_start);
				
				if (elapsed.count() > AimButtonInterval * 1000) {
					time_aim_button_start = time_now;
					if (maxAims_ > 1) 
						maxAims_--; 
				}
			}
			if (menuNum == 4) { 
				auto time_now = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration<double, std::milli>(time_now - time_aim_button_start);
				
				if (elapsed.count() > AimButtonInterval * 1000) {
					time_aim_button_start = time_now;
					if (maxAims_ < 9) 
						maxAims_++; 
				}
			}
			if (menuNum == 5) { 
				auto time_now = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration<double, std::milli>(time_now - time_speed_button_start);
				
				if (elapsed.count() > AimButtonInterval * 1000) {
					time_speed_button_start = time_now;
					if (maxSpeed_ > 0)
						maxSpeed_--; 
				}
			}
			if (menuNum == 6) { 
				auto time_now = std::chrono::high_resolution_clock::now();
				auto elapsed = std::chrono::duration<double, std::milli>(time_now - time_speed_button_start);
				
				if (elapsed.count() > AimButtonInterval * 1000) {
					time_speed_button_start = time_now;
					if (maxSpeed_ < 2) 
						maxSpeed_++; 
				}
			}
		}

		text.setString(std::to_string(maxAims_));
		speedText.setString(speedStr_[maxSpeed_]);

		switch (maxSpeed_) {
			case 0:
				AimSpeed_ = BaseSpeed / 2;
				break;
			case 1:
				AimSpeed_ = BaseSpeed;
				break;
			case 2:
				AimSpeed_ = BaseSpeed * 2;
				break;
			default:
				break;
		}


 
		window.draw(menuBg);
		window.draw(menu1);
		window.draw(menu2);
		window.draw(menu3);
		window.draw(menuMinus);
		window.draw(menuPlus);	
		//window.draw(aimBox);
		window.draw(text);
		window.draw(speedText);
		window.draw(menuSpeedMinus);
		window.draw(menuSpeedPlus);

		
		window.display();
	}
	////////////////////////////////////////////////////
}


}  //  namespace game_project