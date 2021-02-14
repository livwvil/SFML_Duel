#include<SFML\Graphics.hpp>
#include<iostream>
#include<sstream>
#include<list>

const double gravity = 0.0005;
const int ground_height = 32;
const int ground_width = 32;
const int map_tiles_y = 18;
const int map_tiles_x = 80;
const char noCollision = '-';
int offsetX, offsetY;
const int screen_width = 1500;
const const int screen_height = 700;
const float half_viewport_x = screen_width / 2;
const float half_viewport_y = screen_height / 2;
const float map_width = map_tiles_x * ground_width;
const float map_height = map_tiles_y * ground_height;

const char map[map_tiles_y][map_tiles_x + 1] = {
	"--------------------------------------------------------------------------0-----",
	"1------------------------------------------------------------------------0-----1",
	"1-----------------------------------------------------------------------0------1",
	"1----------------------------------------------------------------------0-------1",
	"1---------------------------------------------------------------------0--------1",
	"1--------------------------------------------------------------------0---------1",
	"1-------------------------------------------------------------------0----------1",
	"1------------------------------------------------------------------0-----------1",
	"1-----------------------------------------------------------------0------------1",
	"1-------------------------h--------------------------------------0-------------1",
	"1---------------------------------00----------------------------0--------------1",
	"1---------------------------------11-------------000000000000000---------------1",
	"1---------------------------------11------------0------------------------------1",
	"1-----------------------------0000110000------00-------------------------------1",
	"1--------00-----------000----0-------------------------------------------------1",
	"1-----------------0------------------------------------------------------------1",
	"1-----------------1------------------------------------------------------------1",
	"10000000000000000010000000000000000000000000000000000000000000000000000000000001",
};
class Spritesheet
{
public:
	sf::Texture* texture;
	sf::IntRect* texture_rect;
	sf::Sprite* sprite;
	int n_running_frames;
	int n_shooting_frames;
	double cur_running_frame;
	double cur_shooting_frame;

	int texture_width;
	int texture_height;
	int texture_character_width;
	int texture_shooting_width;
	int spritesheet_idle_top;
	int spritesheet_run_top;
	int spritesheet_fire_top;
	double running_animation_speed;
	double shooting_animation_speed;
	Spritesheet(sf::Texture* texture, int width, int height, int n_frames)
	{
		this->texture = texture;
		this->texture_rect = new sf::IntRect(0, 0, width, height);
		this->sprite = new sf::Sprite(*(texture));
		this->n_running_frames = n_frames;
		this->n_shooting_frames = 8;
		this->cur_running_frame = 0;
		this->cur_shooting_frame = 0;

		this->texture_width = width;
		this->texture_height = height;

		this->texture_character_width = 42;
		this->texture_shooting_width = 128;

		this->spritesheet_run_top = 0;
		this->spritesheet_fire_top = 87;
		this->spritesheet_idle_top = 146;

		this->running_animation_speed = 0.1;
		this->shooting_animation_speed = 0.007;

	}
	~Spritesheet()
	{
		delete this->texture_rect;
		delete this->sprite;
	}
	void idle(bool sight_left)
	{
		cur_running_frame = 0;
		cur_shooting_frame = 0;
		texture_rect->top = spritesheet_idle_top;

		if (!sight_left)
		{
			texture_rect->left = 0;
			texture_rect->width = texture_width;
		}
		if (sight_left)
		{
			texture_rect->left = texture_width;
			texture_rect->width = -texture_width;
		}
	}
	void run(bool sight_left, double dx, double time)
	{
		cur_shooting_frame = 0;
		texture_rect->top = spritesheet_run_top;

		cur_running_frame += abs(dx) * time * running_animation_speed;
		if (cur_running_frame > n_running_frames)
			cur_running_frame -= n_running_frames;

		if (!sight_left)
		{
			texture_rect->left = int(cur_running_frame) * texture_width;
			texture_rect->width = texture_width;
		}
		if (sight_left)
		{
			texture_rect->left = int(cur_running_frame) * texture_width + texture_width;
			texture_rect->width = -texture_width;
		}
	}
	void shoot(bool sight_left, double time)
	{
		cur_running_frame = 0;
		texture_rect->top = spritesheet_fire_top;

		cur_shooting_frame += time * shooting_animation_speed;
		if (cur_shooting_frame > n_shooting_frames)
			cur_shooting_frame -= n_shooting_frames;

		if (sight_left)
		{
			texture_rect->width = -texture_shooting_width;
			texture_rect->left = int(cur_shooting_frame) * texture_shooting_width + texture_shooting_width;
		}
		if (!sight_left)
		{
			texture_rect->width = texture_shooting_width;
			texture_rect->left = int(cur_shooting_frame) * texture_shooting_width;
		}
	}
};

class Resources
{
public:
	sf::Texture* ground_texture = new sf::Texture();
	sf::Texture* bullet_texture = new sf::Texture();
	sf::Texture* player1_spritesheet = new sf::Texture();
	sf::Texture* player2_spritesheet = new sf::Texture();
	sf::Font* font;
	Spritesheet* player1;
	Spritesheet* player2;
	Resources()
	{
		this->ground_texture = new sf::Texture();
		this->bullet_texture = new sf::Texture();
		this->player2_spritesheet = new sf::Texture();
		this->player1_spritesheet = new sf::Texture();
		this->font = new sf::Font();

		this->ground_texture->loadFromFile("./assets/ground.png");
		this->bullet_texture->loadFromFile("./assets/bullet.png");
		this->player2_spritesheet->loadFromFile("./assets/player2.png");
		this->player1_spritesheet->loadFromFile("./assets/player1.png");
		this->font->loadFromFile("./assets/arial.ttf");

		this->player1 = new Spritesheet(player1_spritesheet, 90, 55, 10);
		this->player2 = new Spritesheet(player2_spritesheet, 90, 55, 10);
	}
	~Resources()
	{
		delete this->ground_texture;
		delete this->bullet_texture;
		delete this->player1_spritesheet;
		delete this->player2_spritesheet;
		delete this->font;
		delete this->player1;
		delete this->player2;
	}
};

Resources* resources = new Resources();

class Interface
{
private:
	sf::Text* player1_status;
	sf::Text* player2_status;
	float text_offset = 100;
public:
	Interface()
	{
		player1_status = new sf::Text();
		player2_status = new sf::Text();

		player1_status->setFont(*(resources->font));
		player1_status->setCharacterSize(15);
		player1_status->setOutlineColor(sf::Color::Black);
		player1_status->setOutlineThickness(3);
		player1_status->setPosition(text_offset, 0.f);

		player2_status->setFont(*(resources->font));
		player2_status->setCharacterSize(15);
		player2_status->setOutlineColor(sf::Color::Black);
		player2_status->setOutlineThickness(3);
		player2_status->setPosition(1600 - (player2_status->getGlobalBounds().width + text_offset), 0.f);
	}

	~Interface()
	{
		delete this->player1_status;
		delete this->player2_status;
	}

	void set_player1_status(std::string status)
	{
		this->player1_status->setString(status);
	}
	void set_player2_status(std::string status)
	{
		this->player2_status->setString(status);
		this->player2_status->setPosition(1600 - (player2_status->getGlobalBounds().width + text_offset), 0.f);
	}

	sf::Text* get_player1_status()
	{
		return this->player1_status;
	}

	sf::Text* get_player2_status()
	{
		return this->player2_status;
	}
};



class IBullet
{
public:
	virtual void update(double time) = 0;
	virtual double get_damage() = 0;
	virtual void set_damage(double dmg) = 0;
	virtual bool is_alive() = 0;
	virtual sf::Sprite* get_sprite() = 0;
	virtual sf::FloatRect* get_position() = 0;
};

class MachineGunBullet : public IBullet
{
private:
	sf::Sprite* sprite;
	sf::FloatRect* position;
	double b_speed;
	double b_damage;
public:
	void update(double time)
	{
		this->position->left += b_speed * time;
		for (int i = position->top / ground_height; i < (position->top + position->height) / ground_height; i++)
		{
			for (int j = position->left / ground_width; j < (position->left + position->width) / ground_width; j++)
			{
				if ((i >= 0 && i < map_tiles_y) && (j >= 0 && j < map_tiles_x))
					if (map[i][j] != noCollision && map[i][j] != 'h')
					{
						b_damage = 0;
					}
			}
		}
		this->sprite->setPosition(position->left - offsetX, position->top - offsetY);
	}
	double get_damage()
	{
		return b_damage;
	}
	void set_damage(double dmg)
	{
		this->b_damage = dmg;
	}

	bool is_alive()
	{
		return b_damage != 0;
	}
	sf::Sprite* get_sprite()
	{
		return sprite;
	}
	sf::FloatRect* get_position()
	{
		return position;
	}
	MachineGunBullet(double damage, double speed, float x_pos, float y_pos, bool flight_dir_left)
	{
		this->b_damage = damage;
		this->b_speed = flight_dir_left ? -speed : speed;
		this->sprite = new sf::Sprite(*(resources->bullet_texture));
		if (flight_dir_left)
		{
			sf::IntRect rect = this->sprite->getTextureRect();
			this->sprite->setTextureRect(sf::IntRect(rect.left + rect.width, rect.top, -rect.width, rect.height));
		}
		this->position = new sf::FloatRect(x_pos, y_pos, 4, 4);
	}
	~MachineGunBullet()
	{
		delete this->sprite;
		delete this->position;
	}
};

class BulletGenerator
{
protected:
	double init_b_damage;
	double init_b_speed;
	double rate_of_fire;
	double reloaded_percentage;
	virtual IBullet* create_bullet(float x_pos, float y_pos, bool flight_dir_left) = 0;
	BulletGenerator() {}
public:
	IBullet* get_bullet(float x_pos, float y_pos, bool flight_dir_left)
	{
		if (is_reloaded())
		{
			this->reloaded_percentage = 0;
			IBullet* bullet = create_bullet(x_pos, y_pos, flight_dir_left);
			return bullet;
		}
		else
			return nullptr;
	}
	void update(double time)
	{
		if (!is_reloaded())
		{
			reloaded_percentage += rate_of_fire * time;
		}
	}
	bool is_reloaded()
	{
		return this->reloaded_percentage >= 100;
	}
};

class MGBulletGenerator : public BulletGenerator
{
private:
	IBullet* create_bullet(float x_pos, float y_pos, bool flight_dir_left)
	{
		IBullet* bullet = new MachineGunBullet(init_b_damage, init_b_speed, x_pos, y_pos, flight_dir_left);
		return bullet;
	}
public:
	MGBulletGenerator(double damage, double speed)
	{
		this->init_b_damage = damage;
		this->init_b_speed = speed;
		this->rate_of_fire = 0.5;
		this->reloaded_percentage = 100;
	}
};



class Soldier
{
private:
	Spritesheet* spritesheet;
	BulletGenerator* gun;
	sf::FloatRect* position;
	int hp;
	double dx;
	double dy;
	bool on_ground;
	bool shooting;
	bool sight_left;
	void collision(char dir)
	{
		for (int i = position->top / ground_height; i < (position->top + position->height) / ground_height; i++)
		{
			for (int j = position->left / ground_width; j < (position->left + position->width) / ground_width; j++)
			{
				if ((i >= 0 && i < map_tiles_y) && (j >= 0 && j < map_tiles_x))
				{
					if (map[i][j] == 'h')
						set_hp(10000);

					if (map[i][j] != noCollision && map[i][j] != 'h')
					{
						if (dx > 0 && dir == 'x')
						{
							position->left = j * ground_width - position->width;
						}
						if (dx < 0 && dir == 'x')
						{
							position->left = j * ground_width + ground_width;
						}
						if (dy > 0 && dir == 'y')
						{
							position->top = i * ground_height - position->height;
							dy = 0;
							on_ground = true;
						}
						if (dy < 0 && dir == 'y')
						{
							position->top = i * ground_height + ground_height;
							dy = 0;
						}
					}
				}
			}
		}
	}
public:
	Soldier(Spritesheet* spritesheet, BulletGenerator* gun, double spawnX, double spawnY)
	{
		this->spritesheet = spritesheet;
		this->gun = gun;
		this->position = new sf::FloatRect(spawnX, spawnY, spritesheet->texture_character_width, spritesheet->texture_height);
		this->hp = 10000;
		this->dx = 0;
		this->dy = 0;
		this->on_ground = false;
		this->shooting = false;
		this->sight_left = false;
	}

	~Soldier()
	{
		delete this->position;
	}

	void set_hp(int hp)
	{
		this->hp = hp;
	}

	int get_hp()
	{
		return hp;
	}

	std::pair<Soldier*, IBullet*> shoot()
	{
		IBullet* bullet = nullptr;

		if (sight_left)
		{
			bullet = gun->get_bullet(position->left - (spritesheet->texture_width - spritesheet->texture_character_width), position->top + 0.5 * position->height, sight_left);
		}
		else
		{
			bullet = gun->get_bullet(position->left + position->width + (spritesheet->texture_width - spritesheet->texture_character_width), position->top + 0.5 * position->height, sight_left);
		}

		shooting = true;

		return std::make_pair(this, bullet);
	}

	void set_x_movespeed(float dx)
	{
		if (dx > 0)	sight_left = false;
		if (dx < 0) sight_left = true;
		this->dx = dx;
	}

	void set_sight(bool sightLeft)
	{
		this->sight_left = sightLeft;
	}

	void set_y_movespeed(float dy)
	{
		this->dy = dy;
	}

	void set_position(int x, int y)
	{
		position->left = x;
		position->top = y;
		offsetX = 0;
		offsetY = 0;
	}

	sf::FloatRect* get_position()
	{
		return position;
	}

	Spritesheet* get_spritesheet()
	{
		return spritesheet;
	}

	bool is_on_ground()
	{
		return on_ground;
	}

	void take_damage(int dmg)
	{
		if (this->hp > 0)
			this->hp -= dmg;
		this->hp = hp < 0 ? 0 : hp;
	}

	void update(double time)
	{
		gun->update(time);

		on_ground = false;

		if (shooting) dx = 0;
		position->left += dx * time;
		collision('x');


		dy += gravity * time;
		position->top += dy * time;
		collision('y');

		if (hp == 0)
		{
			spritesheet->sprite->setColor(sf::Color::Red);
		}
		else
		{
			spritesheet->sprite->setColor(sf::Color::White);
		}

		//Animation
		if (dx > 0)
			sight_left = false;
		if (dx < 0)
			sight_left = true;

		//idle
		if (dx == 0 && !shooting)
		{
			spritesheet->idle(sight_left);
		}

		//run
		else if (!shooting)
		{
			spritesheet->run(sight_left, dx, time);
		}

		//fire
		else if (shooting)
		{
			spritesheet->shoot(sight_left, time);
		}

		double sprite_x = position->left - offsetX;
		double sprite_y = position->top - offsetY;

		if (sight_left)
		{
			sprite_x -= spritesheet->texture_character_width;
			if (shooting)
				sprite_x -= spritesheet->texture_shooting_width - spritesheet->texture_width;
		}

		spritesheet->sprite->setTextureRect(*(spritesheet->texture_rect));
		spritesheet->sprite->setPosition(sprite_x, sprite_y);

		dx = 0;
		shooting = false;
	}
};

void drawMap(sf::RenderWindow* mainWindow)
{
	//TODO move to resources
	sf::RectangleShape ground_tile(sf::Vector2f(ground_width, ground_height));
	ground_tile.setTexture(resources->ground_texture);
	sf::RectangleShape heal_tile(sf::Vector2f(ground_width, ground_height));
	heal_tile.setFillColor(sf::Color::Green);
	//
	for (int i = 0; i < map_tiles_y; i++)
	{
		for (int j = 0; j < map_tiles_x; j++)
		{
			float x = j * ground_width - offsetX;
			float y = i * ground_height - offsetY;
			if (map[i][j] == '0')
			{
				ground_tile.setTextureRect(sf::IntRect(515, 400, 256, 137));
				ground_tile.setPosition(x, y);
				mainWindow->draw(ground_tile);
			}
			if (map[i][j] == '1')
			{
				ground_tile.setTextureRect(sf::IntRect(515, 119, 256, 137));
				ground_tile.setPosition(x, y);
				mainWindow->draw(ground_tile);
			}
			if (map[i][j] == 'h')
			{
				heal_tile.setPosition(x, y);
				mainWindow->draw(heal_tile);
			}
		}
	}
}

class KillManager
{
private:
	std::vector<Soldier*> all_soldiers;
	std::map<Soldier*, std::vector<IBullet*>*> shots;
public:
	KillManager() {}

	~KillManager()
	{
		for (const auto& soldier_shots : shots)
		{
			delete soldier_shots.first;
		}
	}

	void subscribe(Soldier* soldier)
	{
		all_soldiers.push_back(soldier);
	}

	void handle_shot(std::pair<Soldier*, IBullet*> shot)
	{
		if (shot.second == nullptr)
			return;
		if (shots.find(shot.first) == shots.end())
			shots.insert(std::make_pair(shot.first, new std::vector<IBullet*>()));
		else
			shots.at(shot.first)->push_back(shot.second);
	}

	void update(float time)
	{
		for (const auto& soldier_shots : shots)
		{
			Soldier* soldier = soldier_shots.first;
			std::vector<IBullet*>* bullets = soldier_shots.second;

			for (auto it = (*bullets).begin(); it != (*bullets).end();)
			{
				IBullet* b = *it;
				if (!(b->is_alive()))
				{
					it = (*bullets).erase(it);
					delete b;
				}
				else
				{
					++it;
				}
			}
			for (const auto& bullet : *bullets)
			{
				bullet->update(time);
			}
		}

		for (const auto& soldier_bullets : shots)
		{
			Soldier* soldier = soldier_bullets.first;
			std::vector<IBullet*>* bullets = soldier_bullets.second;

			for (const auto& current_soldier : all_soldiers)
			{
				if (current_soldier == soldier)
					continue;

				for (const auto& bullet : *bullets)
				{
					if (bullet->get_position()->intersects(*(current_soldier->get_position())))
					{
						current_soldier->take_damage(bullet->get_damage());
						bullet->set_damage(bullet->get_damage() / 2 - bullet->get_damage() * 0.1);
					}
				}
			}
		}
	}

	void draw_bullets(sf::RenderWindow* mainWindow)
	{
		for (const auto& soldier_shots : shots)
		{
			std::vector<IBullet*>* bullets = soldier_shots.second;
			for (const auto& bullet : *bullets)
			{
				mainWindow->draw(*(bullet->get_sprite()));
			}
		}
	}
};

const int pool = 500;
int curr_call = 0;
double sum_time = 0;
void dbg_print_avg_fps(float time)
{
	curr_call++;
	if (curr_call <= pool)
	{
		sum_time += time;
	}
	else
	{
		//		std::cout << 1 / (time / 1e6) << std::endl;
		double avg_time = (double)sum_time / (double)pool;
		std::cout << (1 / (avg_time / 1e6)) << std::endl;
		curr_call = 0;
		sum_time = 0;
	}
}


int main()
{
	sf::RenderWindow mainWindow(sf::VideoMode(screen_width, screen_height), "SFML_Duel");
	//mainWindow.setFramerateLimit(60);

	BulletGenerator* gun1 = new MGBulletGenerator(100, 0.1);
	BulletGenerator* gun2 = new MGBulletGenerator(100, 1);

	Soldier* player1 = new Soldier(resources->player1, gun1, 100, 100);
	Soldier* player2 = new Soldier(resources->player2, gun2, 300, 100);

	KillManager* killManager = new KillManager();
	killManager->subscribe(player1);
	killManager->subscribe(player2);

	Interface* interf = new Interface();

	sf::Clock clock;
	std::stringstream ss;
	while (mainWindow.isOpen())
	{

#pragma region timer
		float time = clock.getElapsedTime().asMicroseconds();
		dbg_print_avg_fps(time);
		if (time > 15) time = 15;
		clock.restart();
#pragma endregion

#pragma region events
		sf::Event event;
		while (mainWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				mainWindow.close();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			if (player1->is_on_ground())
			{
				player1->set_y_movespeed(-0.4);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			player1->set_x_movespeed(-0.1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			killManager->handle_shot(player1->shoot());
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			player1->set_x_movespeed(0.1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			player1->set_sight(true);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
		{
			player1->set_sight(false);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			player2->set_x_movespeed(-0.1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			player2->set_x_movespeed(0.1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (player2->is_on_ground())
			{
				player2->set_y_movespeed(-0.4);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			killManager->handle_shot(player2->shoot());
		}
#pragma endregion
		ss.str("");
		ss << "Player 1 HP: " << player1->get_hp();
		interf->set_player1_status(ss.str());
		ss.str("");
		ss << "Player 2 HP: " << player2->get_hp();
		interf->set_player2_status(ss.str());

		killManager->update(time);
		player1->update(time);
		player2->update(time);

		/*Camera*/
		float p1_x = player1->get_position()->left;
		float p1_y = player1->get_position()->top;

		if (0 + half_viewport_x < p1_x && p1_x < map_width - half_viewport_x)
			offsetX = p1_x - half_viewport_x;

		if (map_height - half_viewport_y > p1_y&& p1_y > 0 + half_viewport_y)
			offsetY = p1_y - half_viewport_y;



		mainWindow.clear(sf::Color::White);
		drawMap(&mainWindow);
		mainWindow.draw(*(player1->get_spritesheet()->sprite));
		mainWindow.draw(*(player2->get_spritesheet()->sprite));
		mainWindow.draw(*(interf->get_player1_status()));
		mainWindow.draw(*(interf->get_player2_status()));
		killManager->draw_bullets(&mainWindow);
		mainWindow.display();
	}
}