//기본 클래스 
class entity {

public:
	float x_pos;
	float y_pos;
	int status;
	int HP;
};

//주인공 클래스 
class Hero :public entity {

public:
	void fire();
	void super_fire();
	void move(int i);
	void init(float x, float y);
	Hero();
	bool HeroisDie;
};

// 적 클래스 
class Enemy :public entity {

public:
	void fire();
	void init(float x, float y);
	void move();

};

// 총알 클래스 
class Bullet :public entity {

public:
	bool bShow;

	void init(float x, float y);
	void move();
	bool show();
	void hide();
	void active();
	bool check_collision(float x, float y);
};

//전역 변수 및 전역 함수 
#define ENEMY_NUM 20
#define BULLET_NUM 20
#define HEROANIM_NUM 8
#define ENEMYANIM_NUM 6
#define BULLETANIM_NUM 4

enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };

Hero hero;
Enemy enemy[ENEMY_NUM];
Bullet bullet[BULLET_NUM];

// function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);
bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1);
