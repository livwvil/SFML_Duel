#include<SFML\Graphics.hpp>
#include<iostream>
#include<algorithm>
#include<math.h>
#include<sstream>
#include<list>

#pragma region TimeCounter and TicToc

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
	bool repeat;
	void update(float time_microseconds);
public:
	TimeCounter(double limit_sec, bool repeat = true);
	~TimeCounter();
	double get_progress();
	void reset();
	int get_time();
	int get_time_reversed();
	bool started();
	bool achieved();
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
		if (!repeat)
		{
			should_update = false;
			return;
		}
	}

	limit_acc_microseconds += time_microseconds;

	if (limit_acc_microseconds >= limit_microseconds)
	{
		limit_acc_microseconds = limit_microseconds;
	}
}
TimeCounter::TimeCounter(double limit_sec, bool repeat)
{
	this->repeat = repeat;
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
bool TimeCounter::achieved()
{
	return limit_acc_microseconds == limit_microseconds;
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

#pragma endregion

class Resources
{
public:
	sf::Texture* background_texture;
	sf::Texture* ground_texture;
	sf::Texture* heart_texture;
	sf::Texture* minigun_texture;
	sf::Texture* speed_texture;
	sf::Texture* gun_texture;
	sf::Texture* bullet_texture;
	sf::IntRect* bullet_rect;
	sf::Texture* player1_texture;
	sf::Texture* player2_texture;
	sf::Font* font;
	sf::Image* map;

	Resources()
	{
		this->background_texture = new sf::Texture();
		this->ground_texture = new sf::Texture();
		this->heart_texture = new sf::Texture();
		this->minigun_texture = new sf::Texture();
		this->speed_texture = new sf::Texture();
		this->gun_texture = new sf::Texture();
		this->bullet_texture = new sf::Texture();
		this->player1_texture = new sf::Texture();
		this->player2_texture = new sf::Texture();
		this->font = new sf::Font();
		this->map = new sf::Image();


		this->background_texture->loadFromFile("./assets/background.png");
		this->ground_texture->loadFromFile("./assets/ground.png");
		this->heart_texture->loadFromFile("./assets/heart.png");
		this->gun_texture->loadFromFile("./assets/gun.png");
		this->minigun_texture->loadFromFile("./assets/minigun.png");
		this->speed_texture->loadFromFile("./assets/speed.png");
		this->bullet_texture->loadFromFile("./assets/bullet.png");
		this->player2_texture->loadFromFile("./assets/player2.png");
		this->player1_texture->loadFromFile("./assets/player1.png");
		this->font->loadFromFile("./assets/arial.ttf");
		this->map->loadFromFile("./assets/map.png");
		this->background_texture->setRepeated(true);
		this->bullet_rect = new sf::IntRect(0, 0, 8, 8);
	}
	~Resources()
	{
		delete this->background_texture;
		delete this->ground_texture;
		delete this->heart_texture;
		delete this->gun_texture;
		delete this->minigun_texture;
		delete this->speed_texture;
		delete this->bullet_texture;
		delete this->player1_texture;
		delete this->player2_texture;
		delete this->font;
		delete this->map;
		delete this->bullet_rect;
	}
};

class Viewport
{
private:
	double base_viewport_movespeed = 0.0015;
	sf::Vector2i _pos;
	sf::Vector2i _half;
	sf::Vector2i _size;
	sf::Vector2i _map_size;
public:
	Viewport(sf::Vector2i viewport_size, sf::Vector2i map_size)
	{
		this->_pos.x = 0;
		this->_pos.y = 0;
		this->_half.x = viewport_size.x / 2;
		this->_half.y = viewport_size.y / 2;
		this->_map_size = map_size;
	}

	sf::Vector2i pos()
	{
		return _pos;
	}

	void pos(sf::Vector2i pos)
	{
		this->_pos = pos;
	}

	void focus(sf::Vector2i pos)
	{
		this->_pos.x = pos.x - _half.x;
		this->_pos.y = pos.y - _half.y;
	}

	sf::Vector2i half()
	{
		return _half;
	}

	void move_viewport_to(float x, float y, float time)
	{
		if (0 + _half.x > x)
			x = _half.x;
		else if (x > _map_size.x - _half.x)
			x = _map_size.x - _half.x;

		if (_map_size.y - _half.y < y)
			y = _map_size.y - _half.y;
		else if (y < 0 + _half.y)
			y = 0 + _half.y;

		x -= _half.x;
		y -= _half.y;

		double s_x = abs(_pos.x - x);
		double s_y = abs(_pos.y - y);
		int dir_x = x > _pos.x;
		int dir_y = y > _pos.y;

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

		_pos.x += x;
		_pos.y += y;
	}
};

class Map
{
public:
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
	static const int tile_height = 32;
	static const int tile_width = 32;
private:
	sf::Image* map;
	sf::Vector2i size_t;
	sf::Vector2i size_p;
	Viewport* viewport;

	std::vector<sf::Vector2i> empty_tiles_coordinates;

	sf::Vector2i heart_last_coords;
	sf::Vector2i minigun_last_coords;
	sf::Vector2i speed_last_coords;
	TimeCounter* heart_respawn_time_counter;
	TimeCounter* speed_respawn_time_counter;
	TimeCounter* minigun_respawn_time_counter;

	sf::Sprite* background_sprite;
	sf::RectangleShape* ground_tile;
	sf::RectangleShape* heart_tile;
	sf::RectangleShape* minigun_tile;
	sf::RectangleShape* speed_tile;
public:
	Map(sf::Image* map, sf::Vector2i screen_size)
	{
		this->map = map;
		auto map_size = map->getSize();
		this->size_t = sf::Vector2i(map_size.x, map_size.y);
		this->size_p = sf::Vector2i(map_size.x * tile_width, map_size.y * tile_height);

		this->viewport = new Viewport(screen_size, size_p);

		for (int i = 0; i < map_size.y; i++)
		{
			for (int j = 0; j < map_size.x; j++)
			{
				if (!is_tile_collision((Tile)map->getPixel(j, i).toInteger()))
				{
					empty_tiles_coordinates.push_back(sf::Vector2i(j, i));
				}
			}
		}

		this->heart_last_coords = get_random_map_coordinate();
		this->minigun_last_coords = get_random_map_coordinate();
		this->speed_last_coords = get_random_map_coordinate();
		this->heart_respawn_time_counter = new TimeCounter(5);
		this->speed_respawn_time_counter = new TimeCounter(4.5);
		this->minigun_respawn_time_counter = new TimeCounter(4);
		set_tile(Tile::HEART, heart_last_coords.x, heart_last_coords.y);
		set_tile(Tile::SPEED, speed_last_coords.x, speed_last_coords.y);
		set_tile(Tile::MINIGUN, minigun_last_coords.x, minigun_last_coords.y);

		this->background_sprite = new sf::Sprite();
		this->ground_tile = new sf::RectangleShape(sf::Vector2f(tile_width, tile_height));
		this->heart_tile = new sf::RectangleShape(sf::Vector2f(tile_width, tile_height));
		this->minigun_tile = new sf::RectangleShape(sf::Vector2f(tile_width, tile_height));
		this->speed_tile = new sf::RectangleShape(sf::Vector2f(tile_width, tile_height));
	}
	void update()
	{
		if (heart_respawn_time_counter->get_progress() == 1.0)
		{
			set_tile(Tile::EMPTY, heart_last_coords.x, heart_last_coords.y);
			heart_last_coords = get_random_map_coordinate();
			set_tile(Tile::HEART, heart_last_coords.x, heart_last_coords.y);
			heart_tile->setFillColor(sf::Color::White);
		}
		else
		{
			heart_tile->setFillColor(sf::Color(255, 255, 255, 255 * (1 - heart_respawn_time_counter->get_progress())));
		}

		if (minigun_respawn_time_counter->get_progress() == 1.0)
		{
			set_tile(Tile::EMPTY, minigun_last_coords.x, minigun_last_coords.y);
			minigun_last_coords = get_random_map_coordinate();
			set_tile(Tile::MINIGUN, minigun_last_coords.x, minigun_last_coords.y);
			minigun_tile->setFillColor(sf::Color::White);
		}
		else
		{
			minigun_tile->setFillColor(sf::Color(255, 255, 255, 255 * (1 - minigun_respawn_time_counter->get_progress())));
		}

		if (speed_respawn_time_counter->get_progress() == 1.0)
		{
			set_tile(Tile::EMPTY, speed_last_coords.x, speed_last_coords.y);
			speed_last_coords = get_random_map_coordinate();
			set_tile(Tile::SPEED, speed_last_coords.x, speed_last_coords.y);
			speed_tile->setFillColor(sf::Color::White);
		}
		else
		{
			speed_tile->setFillColor(sf::Color(255, 255, 255, 255 * (1 - speed_respawn_time_counter->get_progress())));
		}

	}
	Viewport* get_viewport()
	{
		return viewport;
	}

	void draw_map(sf::RenderWindow* mainWindow)
	{
		mainWindow->draw(*(background_sprite));

		for (int i = 0; i < size_t.y; i++)
		{
			for (int j = 0; j < size_t.x; j++)
			{
				Tile tile = get_tile_by_pos(j, i);

				float x = j * tile_width - viewport->pos().x;
				float y = i * tile_height - viewport->pos().y;

				if (tile == Tile::SOIL)
				{
					ground_tile->setTextureRect(sf::IntRect(515, 119, 256, 137));
					ground_tile->setPosition(x, y);
					mainWindow->draw(*(ground_tile));
				}
				else if (tile == Tile::GRASS)
				{
					ground_tile->setTextureRect(sf::IntRect(515, 400, 256, 137));
					ground_tile->setPosition(x, y);
					mainWindow->draw(*(ground_tile));
				}
				else if (tile == Tile::HEART)
				{
					heart_tile->setPosition(x, y);
					mainWindow->draw(*(heart_tile));
				}
				else if (tile == Tile::MINIGUN)
				{
					minigun_tile->setPosition(x, y);
					mainWindow->draw(*(minigun_tile));
				}
				else if (tile == Tile::SPEED)
				{
					speed_tile->setPosition(x, y);
					mainWindow->draw(*(speed_tile));
				}
			}
		}
	}


	sf::Vector2i get_size_in_tiles()
	{
		return size_t;
	}
	sf::Vector2i get_size_in_pixels()
	{
		return size_p;
	}
	~Map()
	{
		delete this->background_sprite;
		delete this->ground_tile;
		delete this->heart_tile;
		delete this->minigun_tile;
		delete this->speed_tile;
	}
	Map* set_bg_texture(sf::Texture* texture, sf::Vector2i screen_size)
	{
		this->background_sprite->setTextureRect(sf::IntRect(0, 0, screen_size.x, screen_size.y));
		this->background_sprite->setTexture(*texture);
		return this;
	}
	Map* set_heart_texture(sf::Texture* texture)
	{
		this->heart_tile->setTexture(texture);
		return this;
	}
	Map* set_speed_texture(sf::Texture* texture)
	{
		this->speed_tile->setTexture(texture);
		return this;
	}
	Map* set_minigun_texture(sf::Texture* texture)
	{
		this->minigun_tile->setTexture(texture);
		return this;
	}
	Map* set_ground_texture(sf::Texture* texture)
	{
		this->ground_tile->setTexture(texture);
		return this;
	}

	bool is_tile_collision(Tile tile)
	{
		Tile no_collision_tile[] = { Tile::EMPTY, Tile::HEART, Tile::SPEED, Tile::MINIGUN, Tile::PLAYER1_SPAWN, Tile::PLAYER2_SPAWN };

		for (int i = 0; i < sizeof(no_collision_tile) / sizeof(*no_collision_tile); i++)
		{
			if (tile == no_collision_tile[i])
				return false;
		}

		return true;
	}

	Tile get_tile_by_pos(int x, int y)
	{
		return (Tile)(this->map->getPixel(x, y).toInteger());
	}

	void set_tile(Tile tile, int x, int y)
	{
		this->map->setPixel(x, y, sf::Color(tile));
	}

	sf::Vector2f get_pos_by_tile(Tile tile)
	{
		for (int i = 0; i < size_t.y; i++)
		{
			for (int j = 0; j < size_t.x; j++)
			{
				if (get_tile_by_pos(j, i) == tile)
					return sf::Vector2f(j * tile_width, i * tile_height);
			}

		}
		return sf::Vector2f(0, 0);
	}

	sf::Vector2i get_random_map_coordinate()
	{
		return empty_tiles_coordinates[rand() % (empty_tiles_coordinates.size() - 1)];
	}
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

class Interface
{
private:
	sf::Vector2i screen_size;
	sf::Text* p1_hp;
	sf::Text* p2_hp;
	sf::Text* score;
	sf::Text* time;
	sf::Text* debug;
	sf::Text* round_result;
	float text_offset = 100;
public:
	Interface(sf::Font* font, sf::Vector2i screen_size)
	{
		this->screen_size = screen_size;
		this->p1_hp = new sf::Text();
		this->p2_hp = new sf::Text();
		this->score = new sf::Text();
		this->time = new sf::Text();
		this->debug = new sf::Text();
		this->round_result = new sf::Text();

		this->p1_hp->setFont(*(font));
		this->p1_hp->setCharacterSize(10);
		this->p1_hp->setOutlineColor(sf::Color::Black);
		this->p1_hp->setOutlineThickness(2);

		this->p2_hp->setFont(*(font));
		this->p2_hp->setCharacterSize(10);
		this->p2_hp->setOutlineColor(sf::Color::Black);
		this->p2_hp->setOutlineThickness(2);

		this->score->setFont(*(font));
		this->score->setCharacterSize(20);
		this->score->setOutlineColor(sf::Color::Black);
		this->score->setOutlineThickness(3);

		this->time->setFont(*(font));
		this->time->setCharacterSize(30);
		this->time->setOutlineColor(sf::Color::Black);
		this->time->setOutlineThickness(3);

		this->debug->setFont(*(font));
		this->debug->setCharacterSize(12);
		this->debug->setOutlineColor(sf::Color::Black);
		this->debug->setOutlineThickness(2);
		this->debug->setPosition(0, 0);

		this->round_result->setFont(*(font));
		this->round_result->setCharacterSize(50);
		this->round_result->setOutlineColor(sf::Color::Black);
		this->round_result->setOutlineThickness(3);
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
		auto string_size = this->round_result->getGlobalBounds();
		this->round_result->setPosition(screen_size.x / 2 - string_size.width / 2, screen_size.y / 2 - string_size.height / 2);
	}
	void set_score(std::string score)
	{
		this->score->setString(score);
		auto string_size = this->score->getGlobalBounds();
		this->score->setPosition(screen_size.x / 2 - string_size.width / 2, 0);
	}
	void set_debug(std::string debug)
	{
		this->debug->setString(debug);
	}
	void set_time(std::string time)
	{
		this->time->setString(time);
		auto string_size = this->time->getGlobalBounds();
		this->time->setPosition(screen_size.x / 2 - string_size.width / 2, 25);
	}

	void set_p1_hp(std::string status, sf::Vector2f pos)
	{
		this->p1_hp->setString(status);
		this->p1_hp->setPosition(pos);
	}

	void set_p2_hp(std::string status, sf::Vector2f pos)
	{
		this->p2_hp->setString(status);
		//this->p2_hp->setPosition(1600 - (p2_hp->getGlobalBounds().width + text_offset), 0.f);
		this->p2_hp->setPosition(pos);
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
	virtual void update(double time, Map* map) = 0;
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
	MachineGunBullet(double damage, double speed, float x_pos, float y_pos, bool flight_dir_left, sf::Texture* bullet_texture, sf::IntRect* bullet_rect)
	{
		this->b_damage = damage;
		this->b_speed = flight_dir_left ? -speed : speed;
		this->sprite = new sf::Sprite(*(bullet_texture));
		this->sprite->setTextureRect(*bullet_rect);
		if (flight_dir_left)
		{
			this->sprite->setTextureRect(sf::IntRect(bullet_rect->left + bullet_rect->width, bullet_rect->top, -bullet_rect->width, bullet_rect->height));
		}
		else
		{
			this->sprite->setTextureRect(*bullet_rect);
		}
		this->position = new sf::FloatRect(x_pos, y_pos, bullet_rect->width, bullet_rect->height);
	}
	~MachineGunBullet()
	{
		delete this->sprite;
		delete this->position;
	}

	void update(double time, Map* map)
	{
		this->position->left += b_speed * time;

		for (int i = position->top / Map::tile_height; i < (position->top + position->height) / Map::tile_height; i++)
		{
			for (int j = position->left / Map::tile_width; j < (position->left + position->width) / Map::tile_width; j++)
			{
				if ((i >= 0 && i < map->get_size_in_tiles().y) && (j >= 0 && j < map->get_size_in_tiles().x))
					if (map->is_tile_collision(map->get_tile_by_pos(j, i)))
					{
						b_damage = 0;
					}
			}
		}

		this->sprite->setPosition(position->left - map->get_viewport()->pos().x, position->top - map->get_viewport()->pos().y);
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
};

class BulletGenerator
{
protected:
	double init_b_damage;
	double init_b_speed;
	TimeCounter* reload_tc;
	sf::Texture* bullet_texture;
	sf::IntRect* bullet_rect;

	virtual IBullet* create_bullet(float x_pos, float y_pos, bool flight_dir_left) = 0;
	BulletGenerator() {}
public:
	~BulletGenerator() 
	{
		delete this->reload_tc;
	}
	IBullet* get_bullet(float x_pos, float y_pos, bool flight_dir_left)
	{
		if (!reload_tc->started())
		{
			reload_tc->get_progress();
			IBullet* bullet = create_bullet(x_pos, y_pos, flight_dir_left);
			return bullet;
		}
		else
			return nullptr;
	}
	void set_params(double damage, double speed)
	{
		this->init_b_damage = damage;
		this->init_b_speed = speed;
	}
};

class MGBulletGenerator : public BulletGenerator
{
private:
	IBullet* create_bullet(float x_pos, float y_pos, bool flight_dir_left)
	{
		IBullet* bullet = new MachineGunBullet(init_b_damage, init_b_speed, x_pos, y_pos, flight_dir_left, bullet_texture, bullet_rect);
		return bullet;
	}
public:
	MGBulletGenerator(double damage, double speed, sf::Texture* bullet_texture, sf::IntRect* bullet_rect)
	{
		this->init_b_damage = damage;
		this->init_b_speed = speed;
		this->bullet_texture = bullet_texture;
		this->bullet_rect = bullet_rect;
		this->reload_tc = new TimeCounter(0.05, false);
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
	double x_speed;
	bool speed_boosted;
	bool gun_boosted;
	bool blinking;
	TimeCounter* respawn_time_counter;
	TicToc* blinking_tic_toc;
	void collision(char dir, Map* map)
	{
		for (int i = position->top / Map::tile_height; i < (position->top + position->height) / Map::tile_height; i++)
		{
			for (int j = position->left / Map::tile_width; j < (position->left + position->width) / Map::tile_width; j++)
			{
				if ((i >= 0 && i < map->get_size_in_tiles().y) && (j >= 0 && j < map->get_size_in_tiles().x))
				{
					Map::Tile tile = map->get_tile_by_pos(j, i);
					if (tile == Map::Tile::HEART)
					{
						map->set_tile(Map::Tile::EMPTY, j, i);
						set_hp(10000);
					}
					else if (tile == Map::Tile::SPEED)
					{
						map->set_tile(Map::Tile::EMPTY, j, i);
						speed_boosted = !speed_boosted;
					}
					else if (tile == Map::Tile::MINIGUN)
					{
						map->set_tile(Map::Tile::EMPTY, j, i);
						gun_boosted = !gun_boosted;
					}

					if (map->is_tile_collision(map->get_tile_by_pos(j, i)))
					{
						if (dx > 0 && dir == 'x')
						{
							position->left = j * Map::tile_width - position->width;
						}
						if (dx < 0 && dir == 'x')
						{
							position->left = j * Map::tile_width + Map::tile_width;
						}
						if (dy > 0 && dir == 'y')
						{
							position->top = i * Map::tile_height - position->height;
							dy = 0;
							on_ground = true;
						}
						if (dy < 0 && dir == 'y')
						{
							position->top = i * Map::tile_height + Map::tile_height;
							dy = 0;
						}
					}
				}
			}
		}
	}
public:
	Soldier(Spritesheet* spritesheet, sf::Vector2f spawn)
	{
		this->spritesheet = spritesheet;
		this->gun = nullptr;
		this->position = new sf::FloatRect(spawn.x, spawn.y, spritesheet->texture_character_width, spritesheet->texture_character_height);
		this->score = 0;
		this->hp = 10000;
		this->dx = 0;
		this->dy = 0;
		this->x_speed = 0;
		this->speed_boosted = false;
		this->gun_boosted = false;
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

	void respawn()
	{
		this->set_position(spawn_x, spawn_y);
		this->set_hp(10000);
		this->score = 0;
		this->blinking = true;
		this->gun_boosted = false;
		this->speed_boosted = false;
	}

	bool is_speed_boosted()
	{
		return speed_boosted;
	}

	bool is_gun_boosted()
	{
		return gun_boosted;
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
		if (gun == nullptr)
			return std::make_pair(this, nullptr);

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

	void set_gun(BulletGenerator* gun)
	{
		this->gun = gun;
	}

	void set_position(int x, int y)
	{
		position->left = x;
		position->top = y;
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
			respawn();
			return true;
		}

		return false;
	}

	void update(double time, double gravity, Map* map)
	{
		on_ground = false;

		if (shooting) dx = 0;
		position->left += dx * time + (dx > 0 ? x_speed : dx < 0 ? -x_speed : 0);
		collision('x', map);


		dy += gravity * time;
		position->top += dy * time;
		collision('y', map);

		gun_boosted
			? gun->set_params(150, 0.0025)
			: gun->set_params(100, 0.0008);
		speed_boosted
			? x_speed = 2
			: x_speed = 0;

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

		double sprite_x = position->left - map->get_viewport()->pos().x;
		double sprite_y = position->top - map->get_viewport()->pos().y;

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

	void update(float time, Map* map)
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
				bullet->update(time, map);
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

class Game
{
public:
	const double gravity = 1.0 * 1e-8;
	const double player_dx = 0.35 * 1e-3;
	const double player_dy = 0.2 * 1e-2;
private:
	Resources* res;
	Map* map;
	Interface* interf;

	Spritesheet* player1_spritesheet;
	Spritesheet* player2_spritesheet;
	Soldier* player1;
	Soldier* player2;
	KillManager* killManager;
	BulletGenerator* p1_machinegun;
	BulletGenerator* p2_machinegun;
	bool p1_takes_viewport = true;
	bool can_take_viewport = true;
	TimeCounter* round_time;
	TimeCounter* round_result_announce;
	TimeCounter* viewport_tc;
public:
	Game(sf::Vector2i screen_size)
	{
		this->res = new Resources();

		this->map = new Map(res->map, screen_size);
		this->map->
			set_bg_texture(res->background_texture, screen_size)->
			set_heart_texture(res->heart_texture)->
			set_minigun_texture(res->minigun_texture)->
			set_speed_texture(res->speed_texture)->
			set_ground_texture(res->ground_texture);

		this->interf = new Interface(res->font, screen_size);

		this->player1_spritesheet = new Spritesheet(res->player1_texture, 90, 55, 10);
		this->player2_spritesheet = new Spritesheet(res->player2_texture, 90, 55, 10);

		this->p1_machinegun = new MGBulletGenerator(100, 0.0008, res->bullet_texture, res->bullet_rect);
		this->p2_machinegun = new MGBulletGenerator(100, 0.0008, res->bullet_texture, res->bullet_rect);

		this->player1 = new Soldier(player1_spritesheet, map->get_pos_by_tile(Map::Tile::PLAYER1_SPAWN));
		this->player2 = new Soldier(player2_spritesheet, map->get_pos_by_tile(Map::Tile::PLAYER2_SPAWN));
		this->player1->set_gun(p1_machinegun);
		this->player2->set_gun(p2_machinegun);

		this->map->get_viewport()->focus(sf::Vector2i(screen_size.x / 2, screen_size.y / 2));

		this->killManager = new KillManager();
		this->killManager->subscribe(player1);
		this->killManager->subscribe(player2);

		this->round_time = new TimeCounter(120);
		this->round_result_announce = new TimeCounter(8);
		this->viewport_tc = new TimeCounter(4);
	}
	~Game()
	{

	}
	void handle_keyboard_events()
	{
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
	}

	void update(float time)
	{
		handle_keyboard_events();

		map->update();
		player1->update(time, gravity, map);
		player2->update(time, gravity, map);
		killManager->update(time, map);

		float p1_x = player1->get_position()->left;
		float p1_y = player1->get_position()->top;
		float p2_x = player2->get_position()->left;
		float p2_y = player2->get_position()->top;

		std::stringstream ss;

		ss.str("");
		ss << "HP: " << player1->get_hp();
		interf->set_p1_hp(ss.str(), sf::Vector2f(p1_x - map->get_viewport()->pos().x, p1_y - map->get_viewport()->pos().y - 10));
		ss.str("");
		ss << "HP: " << player2->get_hp();
		interf->set_p2_hp(ss.str(), sf::Vector2f(p2_x - map->get_viewport()->pos().x, p2_y - map->get_viewport()->pos().y - 10));
		ss.str("");
		ss << "Player 1\t" << player1->get_score() << " : " << player2->get_score() << "\tPlayer 2";
		interf->set_score(ss.str());
		interf->set_time(round_time->get_time_as_sring(true));
		ss.str("");
		ss << "Player 1\n";
		ss << "HP: " << player1->get_hp() << "\n";
		ss << "X: " << (int)p1_x << "\n";
		ss << "Y: " << (int)p1_y << "\n";
		ss << "Gun: " << (player1->is_gun_boosted() ? "boosted" : "common") << "\n";
		ss << "Speed: " << (player1->is_speed_boosted() ? "boosted" : "common") << "\n";
		ss << "\nPlayer 2\n";
		ss << "HP: " << player2->get_hp() << "\n";
		ss << "X: " << (int)p2_x << "\n";
		ss << "Y: " << (int)p2_y << "\n";
		ss << "Gun: " << (player2->is_gun_boosted() ? "boosted" : "common") << "\n";
		ss << "Speed: " << (player2->is_speed_boosted() ? "boosted" : "common") << "\n";
		interf->set_debug(ss.str());


		if (p1_takes_viewport)
		{
			map->get_viewport()->move_viewport_to(p1_x, p1_y, time);
			if (viewport_tc->get_progress() == 1.0)
			{
				p1_takes_viewport = false;
				viewport_tc->reset();
			}
		}
		else
		{
			map->get_viewport()->move_viewport_to(p2_x, p2_y, time);
			if (viewport_tc->get_progress() == 1.0)
			{
				p1_takes_viewport = true;
				viewport_tc->reset();
			}
		}
	}
	void display_into(sf::RenderWindow* mainWindow)
	{
		map->draw_map(mainWindow);
		mainWindow->draw(*(player1->get_spritesheet()->sprite));
		mainWindow->draw(*(player2->get_spritesheet()->sprite));
		mainWindow->draw(*(interf->get_player1_status()));
		mainWindow->draw(*(interf->get_player2_status()));
		mainWindow->draw(*(interf->get_score()));
		mainWindow->draw(*(interf->get_time()));
		mainWindow->draw(*(interf->get_debug()));
		killManager->draw_bullets(mainWindow);

		if (!round_result_announce->started())
		{
			if (round_time->get_progress() == 1.0)
			{
				int s1 = player1->get_score();
				int s2 = player2->get_score();
				interf->set_round_result(s1 > s2 ? "PLAYER  1 WON!" : s2 > s1 ? "PLAYER 2 WON!" : "DEAD HEAT!");
				round_time->reset();
				round_result_announce->get_progress();
			}
		}
		else
		{
			if (round_result_announce->get_progress() == 1.0)
			{
				round_result_announce->reset();
				player1->respawn();
				player2->respawn();
			}

			mainWindow->draw(*(interf->get_round_result()));
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

int main()
{
	const sf::Vector2i screen_size(1600, 800);

	sf::RenderWindow* mainWindow = new sf::RenderWindow(sf::VideoMode(screen_size.x, screen_size.y), "SFML_Duel");
	mainWindow->setPosition(sf::Vector2i(0, 0));
	mainWindow->setFramerateLimit(180);

	Game* game = new Game(screen_size);

	sf::Clock clock;

	while (mainWindow->isOpen())
	{
		sf::Event event;
		while (mainWindow->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				mainWindow->close();
		}

		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();
		dbg_print_avg_fps(time);
		TimeCounter::update_all_timecounters(time);
		TicToc::update_all_tic_tocs(time);

		game->update(time);

		mainWindow->clear(sf::Color::White);
		game->display_into(mainWindow);
		mainWindow->display();
	}

	delete game;
	delete mainWindow;

	return 0;
}