#include<SFML\Graphics.hpp>
#include<iostream>
#include<algorithm>
#include<math.h>
#include<sstream>
#include<list>

int offsetX, offsetY;
const double gravity = 1.0 * 1e-8;
const double player_dx = 0.35 * 1e-3;
const double player_dy = 0.2 * 1e-2;

const int screen_width = 1600;
const const int screen_height = 800;

const float half_viewport_x = screen_width / 2;
const float half_viewport_y = screen_height / 2;

const int tile_ground_height = 32;
const int tile_ground_width = 32;

int map_tiles_height = 0;	// 50
int map_tiles_width = 0;	// 100
int map_pixels_width = 0;	// map_tiles_width* tile_ground_width;
int map_pixels_height = 0;	// map_tiles_height* tile_ground_height;
std::vector<sf::Vector2u> empty_tiles_coordinates;

enum Tile
{
	// map
	SOIL = 3111802879,
	GRASS = 582044927,
	EMPTY = 2581195519,

	// bonus
	HEART = 3978044671,
	SPEED = 2097166,
	MINIGUN = 2103822,

	// players
	PLAYER1_SPAWN = 4286523391,
	PLAYER2_SPAWN = 2281707007,
};

const Tile no_collision_tile[] = { Tile::EMPTY, Tile::HEART, Tile::SPEED, Tile::MINIGUN, Tile::PLAYER1_SPAWN, Tile::PLAYER2_SPAWN };

bool is_collision(Tile tile)
{
	for (int i = 0; i < sizeof(no_collision_tile) / sizeof(*no_collision_tile); i++)
	{
		if (tile == no_collision_tile[i])
			return false;
	}

	return true;
}

class TicToc
{
public:
	enum Period { TIC, TOC };
private:
	double period_acc_microseconds;
	double period_microseconds;
	Period _period;
	static std::vector<TicToc*> all_tic_tocs;
	bool should_update;
	void update(float time_microseconds);
public:
	TicToc(double period_sec);
	~TicToc();
	Period get_period();
	void reset();
	static void update_all_tic_tocs(float time);
};
std::vector<TicToc*> TicToc::all_tic_tocs;
void TicToc::update(float time_microseconds)
{
	if (should_update == false)
		return;
	if (_period == TIC)
	{
		period_acc_microseconds += time_microseconds;

		if (period_acc_microseconds >= period_microseconds)
		{
			period_acc_microseconds = period_microseconds;
			_period = TOC;
		}
	}
	else if (_period == TOC)
	{
		period_acc_microseconds -= time_microseconds;

		if (period_acc_microseconds <= 0)
		{
			period_acc_microseconds = 0;
			_period = TIC;
		}
	}
}
TicToc::TicToc(double period_sec)
{
	this->period_acc_microseconds = 0;
	this->period_microseconds = period_sec * 1e6;
	this->_period = Period::TIC;
	this->should_update = false;
	TicToc::all_tic_tocs.push_back(this);
}
TicToc::~TicToc()
{
	auto position = std::find(TicToc::all_tic_tocs.begin(), TicToc::all_tic_tocs.end(), this);
	if (position != TicToc::all_tic_tocs.end())
		TicToc::all_tic_tocs.erase(position);
}
TicToc::Period TicToc::get_period()
{
	should_update = true;

	return _period;
}
void TicToc::reset()
{
	period_acc_microseconds = 0;
	_period = TicToc::Period::TIC;
	should_update = false;
}
void TicToc::update_all_tic_tocs(float time)
{
	for (const auto& tt : TicToc::all_tic_tocs)
	{
		tt->update(time);
	}
}

class TimeCounter
{
private:
	double limit_acc_microseconds;
	double limit_microseconds;
	static std::vector<TimeCounter*> all_timecounters;
	bool should_update;
	void update(float time_microseconds);
public:
	TimeCounter(double limit_sec);
	~TimeCounter();
	double get_progress();
	void reset();
	int get_time();
	int get_time_reversed();
	bool started();
	std::string get_time_as_sring(bool reversed = false);
	static void update_all_timecounters(float time);
};
std::vector<TimeCounter*> TimeCounter::all_timecounters;
void TimeCounter::update(float time_microseconds)
{
	if (should_update == false)
		return;

	if (limit_acc_microseconds == limit_microseconds)
	{
		limit_acc_microseconds = 0;
	}

	limit_acc_microseconds += time_microseconds;

	if (limit_acc_microseconds >= limit_microseconds)
	{
		limit_acc_microseconds = limit_microseconds;
	}
}
TimeCounter::TimeCounter(double limit_sec)
{
	this->limit_acc_microseconds = 0;
	this->limit_microseconds = limit_sec * 1e6;
	this->should_update = false;
	TimeCounter::all_timecounters.push_back(this);
}
TimeCounter::~TimeCounter()
{
	auto position = std::find(TimeCounter::all_timecounters.begin(), TimeCounter::all_timecounters.end(), this);
	if (position != TimeCounter::all_timecounters.end())
		TimeCounter::all_timecounters.erase(position);
}
double TimeCounter::get_progress()
{
	should_update = true;

	return limit_acc_microseconds / limit_microseconds;
}
void TimeCounter::reset()
{
	limit_acc_microseconds = 0;
	should_update = false;
}
int TimeCounter::get_time()
{
	return limit_acc_microseconds * 1e-6;
}
int TimeCounter::get_time_reversed()
{
	return (limit_microseconds - limit_acc_microseconds) * 1e-6;
}
bool TimeCounter::started()
{
	return limit_acc_microseconds != 0.0;
}
std::string TimeCounter::get_time_as_sring(bool reversed)
{
	int time;
	if (reversed)
		time = get_time_reversed();
	else
		time = get_time();

	int seconds = time % 60;
	int minutes = (time - seconds) / 60;

	return std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);

}
void TimeCounter::update_all_timecounters(float time)
{
	for (const auto& tc : TimeCounter::all_timecounters)
	{
		tc->update(time);
	}
}



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
	int texture_character_height;
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

		this->texture_character_height = height; // 55
		this->texture_character_width = 42;
		this->texture_shooting_width = 128;

		//double x_scale = 2.0 * tile_ground_width / texture_character_width;
		//double y_scale = 4.0 * tile_ground_width / texture_character_height;
		//sprite->setScale(x_scale, y_scale);
		//texture_character_width *= x_scale;
		//texture_character_height *= y_scale;

		this->spritesheet_run_top = 0;
		this->spritesheet_fire_top = 87;
		this->spritesheet_idle_top = 146;

		this->running_animation_speed = 0.10;
		this->shooting_animation_speed = 0.000010;

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
		//std::cout << cur_shooting_frame << std::endl;

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
	sf::Texture* background_texture;
	sf::Sprite* background_sprite;

	sf::Texture* ground_texture;
	sf::RectangleShape* ground_tile;

	sf::Texture* heart_texture;
	sf::RectangleShape* heart_tile;

	sf::Texture* minigun_texture;
	sf::RectangleShape* minigun_tile;

	sf::Texture* speed_texture;
	sf::RectangleShape* speed_tile;

	sf::Texture* gun_texture;
	sf::Texture* bullet_texture;
	sf::Texture* player1_spritesheet;
	sf::Texture* player2_spritesheet;
	sf::Font* font;
	sf::Image* map;
	Spritesheet* player1;
	Spritesheet* player2;
	Resources()
	{
		this->background_texture = new sf::Texture();
		this->background_sprite = new sf::Sprite();

		this->ground_texture = new sf::Texture();
		this->ground_tile = new sf::RectangleShape(sf::Vector2f(tile_ground_width, tile_ground_height));

		this->heart_texture = new sf::Texture();
		this->heart_tile = new sf::RectangleShape(sf::Vector2f(tile_ground_width, tile_ground_height));

		this->minigun_texture = new sf::Texture();
		this->minigun_tile = new sf::RectangleShape(sf::Vector2f(tile_ground_width, tile_ground_height));

		this->speed_texture = new sf::Texture();
		this->speed_tile = new sf::RectangleShape(sf::Vector2f(tile_ground_width, tile_ground_height));

		this->gun_texture = new sf::Texture();
		this->bullet_texture = new sf::Texture();
		this->player1_spritesheet = new sf::Texture();
		this->player2_spritesheet = new sf::Texture();
		this->font = new sf::Font();
		this->map = new sf::Image();

		this->background_texture->loadFromFile("./assets/background.png");
		this->ground_texture->loadFromFile("./assets/ground.png");
		this->heart_texture->loadFromFile("./assets/heart.png");
		this->gun_texture->loadFromFile("./assets/gun.png");
		this->minigun_texture->loadFromFile("./assets/minigun.png");
		this->speed_texture->loadFromFile("./assets/speed.png");
		this->bullet_texture->loadFromFile("./assets/bullet.png");
		this->player2_spritesheet->loadFromFile("./assets/player2.png");
		this->player1_spritesheet->loadFromFile("./assets/player1.png");
		this->font->loadFromFile("./assets/arial.ttf");
		this->map->loadFromFile("./assets/map.png");

		auto map_size = map->getSize();
		map_tiles_width = map_size.x;
		map_tiles_height = map_size.y;
		map_pixels_width = map_tiles_width * tile_ground_width;
		map_pixels_height = map_tiles_height * tile_ground_height;
		for (int i = 0; i < map_size.y; i++)
		{
			for (int j = 0; j < map_size.x; j++)
			{
				if (!is_collision((Tile)map->getPixel(j, i).toInteger()))
				{
					empty_tiles_coordinates.push_back(sf::Vector2u(j, i));
				}
			}
		}

		this->background_texture->setRepeated(true);
		this->background_sprite->setTexture(*background_texture);
		this->background_sprite->setTextureRect(sf::IntRect(0, 0, screen_width, screen_height));

		this->player1 = new Spritesheet(player1_spritesheet, 90, 55, 10);
		this->player2 = new Spritesheet(player2_spritesheet, 90, 55, 10);

		this->ground_tile->setTexture(this->ground_texture);
		this->heart_tile->setTexture(this->heart_texture);
		this->minigun_tile->setTexture(this->minigun_texture);
		this->speed_tile->setTexture(this->speed_texture);
	}
	~Resources()
	{
		delete this->background_texture;
		delete this->background_sprite;
		delete this->ground_texture;
		delete this->ground_tile;
		delete this->heart_texture;
		delete this->heart_tile;
		delete this->minigun_tile;
		delete this->speed_tile;
		delete this->gun_texture;
		delete this->minigun_texture;
		delete this->speed_texture;
		delete this->bullet_texture;
		delete this->player1_spritesheet;
		delete this->player2_spritesheet;
		delete this->font;
		delete this->map;
		delete this->player1;
		delete this->player2;
	}
};

Resources* resources = new Resources();

Tile get_tile(int x, int y)
{
	return (Tile)resources->map->getPixel(x, y).toInteger();
}

void set_tile(Tile tile, int x, int y)
{
	resources->map->setPixel(x, y, sf::Color(tile));
}


class Interface
{
private:
	sf::Text* p1_hp;
	sf::Text* p2_hp;
	sf::Text* score;
	sf::Text* time;
	sf::Text* debug;
	sf::Text* round_result;
	float text_offset = 100;
public:
	Interface()
	{
		this->p1_hp = new sf::Text();
		this->p2_hp = new sf::Text();
		this->score = new sf::Text();
		this->time = new sf::Text();
		this->debug = new sf::Text();
		this->round_result = new sf::Text();

		this->p1_hp->setFont(*(resources->font));
		this->p1_hp->setCharacterSize(10);
		this->p1_hp->setOutlineColor(sf::Color::Black);
		this->p1_hp->setOutlineThickness(2);

		this->p2_hp->setFont(*(resources->font));
		this->p2_hp->setCharacterSize(10);
		this->p2_hp->setOutlineColor(sf::Color::Black);
		this->p2_hp->setOutlineThickness(2);

		this->score->setFont(*(resources->font));
		this->score->setCharacterSize(20);
		this->score->setOutlineColor(sf::Color::Black);
		this->score->setOutlineThickness(3);
		this->score->setPosition(half_viewport_x - 100, 0);

		this->time->setFont(*(resources->font));
		this->time->setCharacterSize(30);
		this->time->setOutlineColor(sf::Color::Black);
		this->time->setOutlineThickness(3);
		this->time->setPosition(half_viewport_x - 100, 40);

		this->debug->setFont(*(resources->font));
		this->debug->setCharacterSize(10);
		this->debug->setOutlineColor(sf::Color::Black);
		this->debug->setOutlineThickness(3);
		this->debug->setPosition(0, 0);

		this->round_result->setFont(*(resources->font));
		this->round_result->setCharacterSize(50);
		this->round_result->setOutlineColor(sf::Color::Black);
		this->round_result->setOutlineThickness(3);
		this->round_result->setPosition(half_viewport_x - 300, half_viewport_y);
	}

	~Interface()
	{
		delete this->p1_hp;
		delete this->p2_hp;
		delete this->score;
		delete this->time;
		delete this->debug;
		delete this->round_result;
	}

	void set_round_result(std::string res)
	{
		this->round_result->setString(res);
	}
	void set_score(std::string score)
	{
		this->score->setString(score);
	}
	void set_debug(std::string debug)
	{
		this->debug->setString(debug);
	}
	void set_time(std::string time)
	{
		this->time->setString(time);
	}

	void set_player1_status(std::string status)
	{
		this->p1_hp->setString(status);
	}
	void set_player2_status(std::string status)
	{
		this->p2_hp->setString(status);
		this->p2_hp->setPosition(1600 - (p2_hp->getGlobalBounds().width + text_offset), 0.f);
	}

	void set_player1_status_pos(int x, int y)
	{
		this->p1_hp->setPosition(sf::Vector2f(x, y));
	}

	void set_player2_status_pos(int x, int y)
	{
		this->p2_hp->setPosition(sf::Vector2f(x, y));
	}

	sf::Text* get_player1_status()
	{
		return this->p1_hp;
	}

	sf::Text* get_player2_status()
	{
		return this->p2_hp;
	}

	sf::Text* get_score()
	{
		return this->score;
	}
	sf::Text* get_debug()
	{
		return this->debug;
	}
	sf::Text* get_time()
	{
		return this->time;
	}
	sf::Text* get_round_result()
	{
		return this->round_result;
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
		for (int i = position->top / tile_ground_height; i < (position->top + position->height) / tile_ground_height; i++)
		{
			for (int j = position->left / tile_ground_width; j < (position->left + position->width) / tile_ground_width; j++)
			{
				if ((i >= 0 && i < map_tiles_height) && (j >= 0 && j < map_tiles_width))
					if (is_collision(get_tile(j, i)))
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
		this->rate_of_fire = 0.003;
		this->reloaded_percentage = 100;
	}
};



class Soldier
{
private:
	Spritesheet* spritesheet;
	BulletGenerator* gun;
	sf::FloatRect* position;
	int score;
	int hp;
	double dx;
	double dy;
	bool on_ground;
	bool shooting;
	bool sight_left;
	double spawn_x;
	double spawn_y;
	bool blinking;
	TimeCounter* respawn_time_counter;
	TicToc* blinking_tic_toc;
	void collision(char dir)
	{
		for (int i = position->top / tile_ground_height; i < (position->top + position->height) / tile_ground_height; i++)
		{
			for (int j = position->left / tile_ground_width; j < (position->left + position->width) / tile_ground_width; j++)
			{
				if ((i >= 0 && i < map_tiles_height) && (j >= 0 && j < map_tiles_width))
				{
					Tile tile = get_tile(j, i);
					if (tile == Tile::HEART)
					{
						set_tile(Tile::EMPTY, j, i);
						set_hp(10000);
					}
					else if (tile == Tile::SPEED)
					{
						set_tile(Tile::EMPTY, j, i);
						dx *= 1.5;
					}
					else if (tile == Tile::MINIGUN)
					{
						set_tile(Tile::EMPTY, j, i);

					}

					if (is_collision(get_tile(j, i)))
					{
						if (dx > 0 && dir == 'x')
						{
							position->left = j * tile_ground_width - position->width;
						}
						if (dx < 0 && dir == 'x')
						{
							position->left = j * tile_ground_width + tile_ground_width;
						}
						if (dy > 0 && dir == 'y')
						{
							position->top = i * tile_ground_height - position->height;
							dy = 0;
							on_ground = true;
						}
						if (dy < 0 && dir == 'y')
						{
							position->top = i * tile_ground_height + tile_ground_height;
							dy = 0;
						}
					}
				}
			}
		}
	}
public:
	Soldier(Spritesheet* spritesheet, BulletGenerator* gun, sf::Vector2f spawn)
	{
		this->spritesheet = spritesheet;
		this->gun = gun;
		this->position = new sf::FloatRect(spawn.x, spawn.y, spritesheet->texture_character_width, spritesheet->texture_character_height);
		this->score = 0;
		this->hp = 10000;
		this->dx = 0;
		this->dy = 0;
		this->spawn_x = spawn.x;
		this->spawn_y = spawn.y;
		this->on_ground = false;
		this->shooting = false;
		this->sight_left = false;
		this->blinking = false;
		this->respawn_time_counter = new TimeCounter(3);
		this->blinking_tic_toc = new TicToc(0.3);
	}

	~Soldier()
	{
		delete this->position;
		delete this->respawn_time_counter;
		delete this->blinking_tic_toc;
	}

	void set_hp(int hp)
	{
		this->hp = hp;
	}

	int get_hp()
	{
		return hp;
	}
	void incr_score()
	{
		this->score += 1;
	}
	int get_score()
	{
		return this->score;
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
		//offsetX = 0;
		//offsetY = 0;
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

	bool take_damage(int dmg)
	{
		if (blinking)
			return false;

		if (this->hp > 0)
			this->hp -= dmg;

		if (this->hp <= 0)
		{
			this->hp = 0;
			blinking = true;
			set_position(spawn_x, spawn_y);
			return true;
		}

		return false;
	}

	void update(double time)
	{
		//std::cout << position->left << "\t" << position->top << std::endl;
		gun->update(time);

		on_ground = false;

		if (shooting) dx = 0;
		position->left += dx * time;
		collision('x');


		dy += gravity * time;
		position->top += dy * time;
		collision('y');

		if (blinking)
		{
			if (respawn_time_counter->get_progress() != 1.0)
			{
				TicToc::Period period = blinking_tic_toc->get_period();
				if (period == TicToc::Period::TIC)
				{
					spritesheet->sprite->setColor(sf::Color(0, 0, 0, 0));
				}
				else if (period == TicToc::Period::TOC)
				{
					spritesheet->sprite->setColor(sf::Color::White);
				}
			}
			else
			{
				blinking_tic_toc->reset();
				respawn_time_counter->reset();
				spritesheet->sprite->setColor(sf::Color::White);
				hp = 10000;
				blinking = false;
			}

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

void draw_map(sf::RenderWindow* mainWindow)
{
	for (int i = 0; i < map_tiles_height; i++)
	{
		for (int j = 0; j < map_tiles_width; j++)
		{
			Tile tile = (Tile)resources->map->getPixel(j, i).toInteger();

			float x = j * tile_ground_width - offsetX;
			float y = i * tile_ground_height - offsetY;

			if (tile == Tile::SOIL)
			{
				resources->ground_tile->setTextureRect(sf::IntRect(515, 119, 256, 137));
				resources->ground_tile->setPosition(x, y);
				mainWindow->draw(*(resources->ground_tile));
			}
			else if (tile == Tile::GRASS)
			{
				resources->ground_tile->setTextureRect(sf::IntRect(515, 400, 256, 137));
				resources->ground_tile->setPosition(x, y);
				mainWindow->draw(*(resources->ground_tile));
			}
			else if (tile == Tile::HEART)
			{
				resources->heart_tile->setPosition(x, y);
				mainWindow->draw(*(resources->heart_tile));
			}
			else if (tile == Tile::MINIGUN)
			{
				resources->minigun_tile->setPosition(x, y);
				mainWindow->draw(*(resources->minigun_tile));
			}
			else if (tile == Tile::SPEED)
			{
				resources->speed_tile->setPosition(x, y);
				mainWindow->draw(*(resources->speed_tile));
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
						bool is_enemy_dead = current_soldier->take_damage(bullet->get_damage());
						if (is_enemy_dead)
							soldier->incr_score();
						//bullet->set_damage(bullet->get_damage() / 2 - bullet->get_damage() * 0.1);
						bullet->set_damage(0);
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
		double avg_time = (double)sum_time / (double)pool;
		std::cout << (1 / (avg_time / 1e6)) << std::endl;
		curr_call = 0;
		sum_time = 0;
	}
}


double base_viewport_movespeed = 0.0015;

void move_viewport_to(float x, float y, float time)
{
	if (0 + half_viewport_x > x)
		x = half_viewport_x;
	else if (x > map_pixels_width - half_viewport_x)
		x = map_pixels_width - half_viewport_x;

	if (map_pixels_height - half_viewport_y < y)
		y = map_pixels_height - half_viewport_y;
	else if (y < 0 + half_viewport_y)
		y = 0 + half_viewport_y;
	x -= half_viewport_x;
	y -= half_viewport_y;




	double s_x = abs(offsetX - x);
	double s_y = abs(offsetY - y);
	int dir_x = x > offsetX;
	int dir_y = y > offsetY;

	double rate_v_x = s_x / (s_x + s_y);
	double rate_v_y = 1.0 - rate_v_x;

	double dist = sqrt(s_x * s_x + s_y * s_y);
	double viewport_movespeed = base_viewport_movespeed * atan(0.003 * dist) * 2 / 3.1415926535;

	double viewport_dx = viewport_movespeed * rate_v_x * (dir_x ? 1 : -1);
	double viewport_dy = viewport_movespeed * rate_v_y * (dir_y ? 1 : -1);

	double some_x = viewport_dx * time;
	double some_y = viewport_dy * time;

	x = s_x < some_x ? s_x : some_x;
	y = s_y < some_y ? s_y : some_y;

	if (s_x < 1 && s_y < 1)
		return;

	offsetX += x;
	offsetY += y;
}


sf::Vector2f get_spawn_coordinates(Tile spawn_tile)
{
	for (int i = 0; i < map_tiles_height; i++)
	{
		for (int j = 0; j < map_tiles_width; j++)
		{
			if (get_tile(j, i) == spawn_tile)
				return sf::Vector2f(j * tile_ground_width, i * tile_ground_height);
		}

	}
	return sf::Vector2f(map_tiles_width / 2, 0);
}

sf::Vector2u get_random_map_coordinate()
{
	return empty_tiles_coordinates[rand() % (empty_tiles_coordinates.size() - 1)];
}

sf::Vector2u heart_last_coords = get_random_map_coordinate();
sf::Vector2u minigun_last_coords = get_random_map_coordinate();
sf::Vector2u speed_last_coords = get_random_map_coordinate();

int main()
{
	sf::RenderWindow mainWindow(sf::VideoMode(screen_width, screen_height), "SFML_Duel");
	mainWindow.setFramerateLimit(180);

	BulletGenerator* gun1 = new MGBulletGenerator(100, 0.0005);
	BulletGenerator* gun2 = new MGBulletGenerator(100, 0.005);

	Soldier* player1 = new Soldier(resources->player1, gun1, get_spawn_coordinates(Tile::PLAYER1_SPAWN));
	Soldier* player2 = new Soldier(resources->player2, gun2, get_spawn_coordinates(Tile::PLAYER2_SPAWN));

	offsetX = get_spawn_coordinates(Tile::PLAYER1_SPAWN).x;
	offsetY = get_spawn_coordinates(Tile::PLAYER1_SPAWN).y;

	KillManager* killManager = new KillManager();
	killManager->subscribe(player1);
	killManager->subscribe(player2);

	Interface* interf = new Interface();

	sf::Clock clock;
	std::stringstream ss;

	bool p1_takes_viewport = true;
	bool can_take_viewport = true;

	set_tile(Tile::HEART, heart_last_coords.x, heart_last_coords.y);
	set_tile(Tile::SPEED, speed_last_coords.x, speed_last_coords.y);
	set_tile(Tile::MINIGUN, minigun_last_coords.x, minigun_last_coords.y);
	double t = 5;
	TimeCounter heart_respawn_time_counter(t);
	TimeCounter speed_respawn_time_counter(t);
	TimeCounter minigun_respawn_time_counter(t);
	TimeCounter round_time(15);
	TimeCounter round_result_announce(4);

	TimeCounter viewport_tc(4);
	while (mainWindow.isOpen())
	{

#pragma region timer
		float time = clock.getElapsedTime().asMicroseconds();
		TimeCounter::update_all_timecounters(time);
		TicToc::update_all_tic_tocs(time);


		if (heart_respawn_time_counter.get_progress() == 1.0)
		{
			set_tile(Tile::EMPTY, heart_last_coords.x, heart_last_coords.y);
			heart_last_coords = get_random_map_coordinate();
			set_tile(Tile::HEART, heart_last_coords.x, heart_last_coords.y);
			resources->heart_tile->setFillColor(sf::Color::White);
		}
		else
		{
			resources->heart_tile->setFillColor(sf::Color(255, 255, 255, 255 * (1 - heart_respawn_time_counter.get_progress())));
		}

		if (minigun_respawn_time_counter.get_progress() == 1.0)
		{
			set_tile(Tile::EMPTY, minigun_last_coords.x, minigun_last_coords.y);
			minigun_last_coords = get_random_map_coordinate();
			set_tile(Tile::MINIGUN, minigun_last_coords.x, minigun_last_coords.y);
			resources->minigun_tile->setFillColor(sf::Color::White);
		}
		else
		{
			resources->minigun_tile->setFillColor(sf::Color(255, 255, 255, 255 * (1 - minigun_respawn_time_counter.get_progress())));
		}

		if (speed_respawn_time_counter.get_progress() == 1.0)
		{
			set_tile(Tile::EMPTY, speed_last_coords.x, speed_last_coords.y);
			speed_last_coords = get_random_map_coordinate();
			set_tile(Tile::SPEED, speed_last_coords.x, speed_last_coords.y);
			resources->speed_tile->setFillColor(sf::Color::White);
		}
		else
		{
			resources->speed_tile->setFillColor(sf::Color(255, 255, 255, 255 * (1 - speed_respawn_time_counter.get_progress())));
		}

		dbg_print_avg_fps(time);
		clock.restart();
#pragma endregion

#pragma region events
		sf::Event event;
		while (mainWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				mainWindow.close();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			if (can_take_viewport)
			{
				p1_takes_viewport = !p1_takes_viewport;
			}
			can_take_viewport = false;
		}
		else
		{
			can_take_viewport = true;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			if (player1->is_on_ground())
			{
				player1->set_y_movespeed(-player_dy);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			player1->set_x_movespeed(-player_dx);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			killManager->handle_shot(player1->shoot());
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			player1->set_x_movespeed(player_dx);
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
			player2->set_x_movespeed(-player_dx);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			player2->set_x_movespeed(player_dx);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			if (player2->is_on_ground())
			{
				player2->set_y_movespeed(-player_dy);
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			killManager->handle_shot(player2->shoot());
		}
#pragma endregion

		player1->update(time);
		player2->update(time);
		killManager->update(time);


		float p1_x = player1->get_position()->left;
		float p1_y = player1->get_position()->top;
		float p2_x = player2->get_position()->left;
		float p2_y = player2->get_position()->top;

		ss.str("");
		ss << "HP: " << player1->get_hp();
		interf->set_player1_status(ss.str());
		ss.str("");
		ss << "HP: " << player2->get_hp();
		interf->set_player2_status(ss.str());
		ss.str("");
		ss << "Player 1\t" << player1->get_score() << " : " << player2->get_score() << "\tPlayer 2";
		interf->set_score(ss.str());
		interf->set_time(round_time.get_time_as_sring(true));
		ss.str("");
		ss << "Player 1\n";
		ss << "X: " << (int)p1_x << "\n";
		ss << "Y: " << (int)p1_y << "\n";
		ss << "Player 2\n";
		ss << "X: " << (int)p2_x << "\n";
		ss << "Y: " << (int)p2_y << "\n";
		interf->set_debug(ss.str());



		interf->set_player1_status_pos(p1_x - offsetX, p1_y - offsetY - 10);
		interf->set_player2_status_pos(p2_x - offsetX, p2_y - offsetY - 10);

		if (p1_takes_viewport)
		{
			move_viewport_to(p1_x, p1_y, time);
			if (viewport_tc.get_progress() == 1.0)
			{
				p1_takes_viewport = false;
				viewport_tc.reset();
			}
		}
		else
		{
			move_viewport_to(p2_x, p2_y, time);
			if (viewport_tc.get_progress() == 1.0)
			{
				p1_takes_viewport = true;
				viewport_tc.reset();
			}
		}

		mainWindow.clear(sf::Color::White);
		mainWindow.draw(*(resources->background_sprite));
		draw_map(&mainWindow);
		mainWindow.draw(*(player1->get_spritesheet()->sprite));
		mainWindow.draw(*(player2->get_spritesheet()->sprite));
		mainWindow.draw(*(interf->get_player1_status()));
		mainWindow.draw(*(interf->get_player2_status()));
		mainWindow.draw(*(interf->get_score()));
		mainWindow.draw(*(interf->get_time()));
		mainWindow.draw(*(interf->get_debug()));
		killManager->draw_bullets(&mainWindow);
		if (!round_result_announce.started())
		{
			if (round_time.get_progress() == 1.0)
			{
				int s1 = player1->get_score();
				int s2 = player2->get_score();
				interf->set_round_result(s1 > s2 ? "PLAYER  1 WON!" : s2 > s1 ? "PLAYER 2 WON!" : "DEAD HEAT!");
				round_time.reset();
				round_result_announce.get_progress();
			}
		}
		else
		{
			if (round_result_announce.get_progress() == 1.0)
				round_result_announce.reset();

			mainWindow.draw(*(interf->get_round_result()));
		}
		mainWindow.display();
	}
}