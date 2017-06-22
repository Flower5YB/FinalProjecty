// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

#include "Matrices49860489.h"

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface

						// sprite declarations
LPDIRECT3DTEXTURE9 sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero[HEROANIM_NUM];    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy[ENEMYANIM_NUM];    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_bullet[BULLETANIM_NUM];    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_background;    // the pointer to the sprite


// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;


bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1)
{
	if ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1) < (size0 + size1) * (size0 + size1))
		return true;
	else
		return false;
}

// 히어로
void Hero::init(float x, float y)
{
	x_pos = x;
	y_pos = y;
}

void Hero::move(int i)
{
	switch (i)
	{
	case MOVE_UP:
		y_pos -= 3;
		break;

	case MOVE_DOWN:
		y_pos += 3;
		break;

	case MOVE_LEFT:
		x_pos -= 3;
		break;

	case MOVE_RIGHT:
		x_pos += 3;
		break;
	}
}

Hero::Hero() : HeroisDie(false)
{

}

void Enemy::init(float x, float y)
{
	x_pos = x;
	y_pos = y;
}

void Enemy::move()
{
	x_pos -= 2;
}

bool Bullet::check_collision(float x, float y)
{
	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 32, x, y, 32) == true)
	{
		bShow = false;
		return true;
	}
	else {

		return false;
	}
}

void Bullet::init(float x, float y)
{
	x_pos = x;
	y_pos = y;

}

bool Bullet::show()
{
	return bShow;

}

void Bullet::active()
{
	bShow = true;

}

void Bullet::move()
{
	x_pos += 8;
}

void Bullet::hide()
{
	bShow = false;

}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"Our Direct3D Program",
		WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);


	//게임 오브젝트 초기화 
	init_game();

	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		DWORD starting_point = GetTickCount();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		do_game_logic();

		render_frame();

		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);


		while ((GetTickCount() - starting_point) < 25);
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object

	wchar_t szBuf[100]   = L"";
	wchar_t szBuf2[100]  = L"";
	wchar_t szBuf3[100]  = L"";

	for(int i = 0; i < 8;++i)
	{
		wsprintf(szBuf, L"Texture\\hero\\hero%d.png", i);

		D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
			szBuf,    // the file name
			80,    // default width
			160,    // default height
			D3DX_DEFAULT,    // no mip mapping
			NULL,    // regular usage
			D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
			D3DPOOL_MANAGED,    // typical memory handling
			D3DX_DEFAULT,    // no filtering
			D3DX_DEFAULT,    // no mip filtering
			D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
			NULL,    // no image info struct
			NULL,    // not using 256 colors
			&sprite_hero[i]);    // load to sprite
	}

	for (int i = 0; i < ENEMYANIM_NUM; ++i)
	{
		wsprintf(szBuf2, L"Texture\\Enemy_Predator\\Run\\EnemyRun%d.png", i);

		D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
			szBuf2,    // the file name
			40,    // default width
			60,    // default height
			D3DX_DEFAULT,    // no mip mapping
			NULL,    // regular usage
			D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
			D3DPOOL_MANAGED,    // typical memory handling
			D3DX_DEFAULT,    // no filtering
			D3DX_DEFAULT,    // no mip filtering
			D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
			NULL,    // no image info struct
			NULL,    // not using 256 colors
			&sprite_enemy[i]);    // load to sprite
	}

	for (int i = 0; i < BULLETANIM_NUM; ++i)
	{
		wsprintf(szBuf3, L"Texture\\Bullet_\\Bullet_%d.png", i);

		D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
			szBuf3,    // the file name
			50,    // default width
			60,    // default height
			D3DX_DEFAULT,    // no mip mapping
			NULL,    // regular usage
			D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
			D3DPOOL_MANAGED,    // typical memory handling
			D3DX_DEFAULT,    // no filtering
			D3DX_DEFAULT,    // no mip filtering
			D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
			NULL,    // no image info struct
			NULL,    // not using 256 colors
			&sprite_bullet[i]);    // load to sprite
	}

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Texture\\Background\\Background.png",    // the file name
		D3DX_DEFAULT,    // default width
		D3DX_DEFAULT,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_background);    // load to sprite

	return;
}


void init_game(void)
{
	//객체 초기화 
	hero.init(0, 150);

	//적들 초기화 
	for (int i = 0; i<ENEMY_NUM; i++)
	{
		enemy[i].init((float)(rand() % 300 + 600), rand() % 300 + 10);
	}

	////총알 초기화
	//for (int i = 0; i < BULLET_NUM; i++)
	//{
	//	bullet[i].init(hero.x_pos, hero.y_pos);
	//}
}

void do_game_logic(void)
{
	static float count = 0.0f;
	//주인공 처리 
	if (KEY_DOWN(VK_UP))
		hero.move(MOVE_UP);

	if (KEY_DOWN(VK_DOWN))
		hero.move(MOVE_DOWN);

	if (KEY_DOWN(VK_LEFT))
		hero.move(MOVE_LEFT);

	if (KEY_DOWN(VK_RIGHT))
		hero.move(MOVE_RIGHT);


	//적들 처리 
	for (int i = 0; i<ENEMY_NUM; i++)
	{
		if (enemy[i].x_pos < 100)
			enemy[i].init((float)(rand() % 300 + 600), rand() % 300 + 10);
		else
			enemy[i].move();
	}


	//총알 처리 
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].show() == false)
		{
			
			if (KEY_DOWN(VK_SPACE))
			{

				
					bullet[i].active();
					bullet[i].init(hero.x_pos + 5, hero.y_pos - 10);
				
			}
		}


		if (bullet[i].show() == true)
		{
			if (bullet[i].x_pos > 600)
				bullet[i].hide();
			else
				bullet[i].move();
		
			//충돌 처리 
			for (int i = 0; i < ENEMY_NUM; i++)
			{
				for (int j = 0; j < BULLET_NUM; j++)
				{
					if (bullet[j].check_collision(enemy[i].x_pos, enemy[i].y_pos) == true)
					{
						enemy[i].init((float)(rand() % 300), rand() % 200 - 300);
						bullet[j].hide();
					}
				}
			}
		}

	}

}

// this is the function used to render a single frame
void render_frame(void)
{
	static int timer = 0;
	static int timer2 = 0;
	static int timer3 = 0;
	timer++;
	timer2++;
	timer3++;
	
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency

											 //UI 창 렌더링 


											 /*
											 static int frame = 21;    // start the program on the final frame
											 if(KEY_DOWN(VK_SPACE)) frame=0;     // when the space key is pressed, start at frame 0
											 if(frame < 21) frame++;     // if we aren't on the last frame, go to the next frame

											 // calculate the x-position
											 int xpos = frame * 182 + 1;

											 RECT part;
											 SetRect(&part, xpos, 0, xpos + 181, 128);
											 D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
											 D3DXVECTOR3 position(150.0f, 50.0f, 0.0f);    // position at 50, 50 with no depth
											 d3dspt->Draw(sprite, &part, &center, &position, D3DCOLOR_ARGB(127, 255, 255, 255));
											 */

	RECT part3;
	SetRect(&part3, 0, 0, 640, 480);
	D3DXVECTOR3 center3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position3(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_background, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));

	//주인공 
	
	if ( timer >= 0 && timer < 2)
	{
		RECT HeroPart;
		SetRect(&HeroPart, -10, -10, 120, 240);
		D3DXVECTOR3 HeroCenter(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 HeroPosition(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero[0], &HeroPart, &HeroCenter, &HeroPosition, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
		

	if (timer >= 2 && timer < 4)
	{

		RECT HeroPart2;
		SetRect(&HeroPart2, -10, -10, 120, 240);
		D3DXVECTOR3 HeroCenter2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 HeroPosition2(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero[1], &HeroPart2, &HeroCenter2, &HeroPosition2, D3DCOLOR_ARGB(255, 255, 255, 255));

	}
	if (timer >= 4 && timer < 6)
	{
		RECT HeroPart3;
		SetRect(&HeroPart3, -10, -10, 120, 240);
		D3DXVECTOR3 HeroCenter3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 HeroPosition3(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero[2], &HeroPart3, &HeroCenter3, &HeroPosition3, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	
	if (timer >= 6 && timer < 8)
	{
		RECT HeroPart4;
		SetRect(&HeroPart4, -10, -10, 120, 240);
		D3DXVECTOR3 HeroCenter4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 HeroPosition4(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero[3], &HeroPart4, &HeroCenter4, &HeroPosition4, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	if (timer >= 8 && timer < 10)
	{
		RECT HeroPart5;
		SetRect(&HeroPart5, -10, -10, 120, 240);
		D3DXVECTOR3 HeroCenter5(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 HeroPosition5(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero[4], &HeroPart5, &HeroCenter5, &HeroPosition5, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	if (timer >= 10 && timer < 12)
	{
		RECT HeroPart6;
		SetRect(&HeroPart6, -10, -10, 120, 240);
		D3DXVECTOR3 HeroCenter6(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 HeroPosition6(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero[5], &HeroPart6, &HeroCenter6, &HeroPosition6, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	if (timer >= 12 && timer < 14)
	{
		RECT HeroPart7;
		SetRect(&HeroPart7, -10, -10, 120, 240);
		D3DXVECTOR3 HeroCenter7(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 HeroPosition7(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero[6], &HeroPart7, &HeroCenter7, &HeroPosition7, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	if (timer >= 14 && timer < 16)
	{
		RECT HeroPart8;
		SetRect(&HeroPart8, -10, -10, 120, 240);
		D3DXVECTOR3 HeroCenter8(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 HeroPosition8(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_hero[7], &HeroPart8, &HeroCenter8, &HeroPosition8, D3DCOLOR_ARGB(255, 255, 255, 255));
		timer = 0;
	}
	
		

	
	////총알 
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].bShow == true)
		{
			if (timer3 >= 0 && timer3 < 4)
			{
				RECT part1;
				SetRect(&part1, 0, 0, 50, 64);
				D3DXVECTOR3 center1(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
				D3DXVECTOR3 position1(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
				d3dspt->Draw(sprite_bullet[0], &part1, &center1, &position1, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if (timer3 >= 4 && timer3 < 8)
			{
				RECT Bulletpart2;
				SetRect(&Bulletpart2, 0, 0, 50, 64);
				D3DXVECTOR3 Bulletcenter2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
				D3DXVECTOR3 Bulletposition2(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
				d3dspt->Draw(sprite_bullet[1], &Bulletpart2, &Bulletcenter2, &Bulletposition2, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if (timer3 >= 8 && timer3 < 12)
			{
				RECT Bulletpart3;
				SetRect(&Bulletpart3, 0, 0, 50, 64);
				D3DXVECTOR3 Bulletcenter3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
				D3DXVECTOR3 Bulletposition3(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
				d3dspt->Draw(sprite_bullet[2], &Bulletpart3, &Bulletcenter3, &Bulletposition3, D3DCOLOR_ARGB(255, 255, 255, 255));
			}

			if (timer3 >= 12 && timer3 < 16) 
			{
				RECT Bulletpart3;
				SetRect(&Bulletpart3, 0, 0, 50, 64);
				D3DXVECTOR3 Bulletcenter3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
				D3DXVECTOR3 Bulletposition3(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
				d3dspt->Draw(sprite_bullet[1], &Bulletpart3, &Bulletcenter3, &Bulletposition3, D3DCOLOR_ARGB(255, 255, 255, 255));
				timer3 = 0;
			}
		}
	}


	//에네미 
	RECT part2;
	SetRect(&part2, 0, 0, 40, 60);
	D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

	for (int i = 0; i<ENEMY_NUM; i++)
	{

		if (timer2 >= 0 && timer2 < 3)
		{

			RECT EnemyPart;
			SetRect(&EnemyPart, 0, 0, 40, 60);
			D3DXVECTOR3 EnemyCenter(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 EnemyPosition(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy[0], &EnemyPart, &EnemyCenter, &EnemyPosition, D3DCOLOR_ARGB(255, 255, 255, 255));
		
		}

		if (timer2 >= 3 && timer2 < 6)
		{

			RECT EnemyPart2;
			SetRect(&EnemyPart2, 0, 0, 40, 60);
			D3DXVECTOR3 EnemyCenter2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 EnemyPosition2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy[1], &EnemyPart2, &EnemyCenter2, &EnemyPosition2, D3DCOLOR_ARGB(255, 255, 255, 255));

		}

		if (timer2 >= 6 && timer2 < 9)
		{

			RECT EnemyPart3;
			SetRect(&EnemyPart3, 0, 0, 40, 60);
			D3DXVECTOR3 EnemyCenter3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 EnemyPosition3(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy[2], &EnemyPart3, &EnemyCenter3, &EnemyPosition3, D3DCOLOR_ARGB(255, 255, 255, 255));

		}

		if (timer2 >= 9 && timer2 < 12)
		{

			RECT EnemyPart4;
			SetRect(&EnemyPart4, 0, 0, 40, 60);
			D3DXVECTOR3 EnemyCenter4(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 EnemyPosition4(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy[3], &EnemyPart4, &EnemyCenter4, &EnemyPosition4, D3DCOLOR_ARGB(255, 255, 255, 255));

		}

		if (timer2 >= 12 && timer2 <15)
		{
			RECT EnemyPart5;
			SetRect(&EnemyPart5, 0, 0, 40, 60);
			D3DXVECTOR3 EnemyCenter5(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 EnemyPosition5(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy[4], &EnemyPart5, &EnemyCenter5, &EnemyPosition5, D3DCOLOR_ARGB(255, 255, 255, 255));

		}

		if (timer2 >= 15 && timer2 < 18)
		{

			RECT EnemyPart6;
			SetRect(&EnemyPart6, 0, 0, 40, 60);
			D3DXVECTOR3 EnemyCenter6(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 EnemyPosition6(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_enemy[5], &EnemyPart6, &EnemyCenter6, &EnemyPosition6, D3DCOLOR_ARGB(255, 255, 255, 255));
			timer2 = 0;
		}

	}



	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	sprite->Release();
	d3ddev->Release();
	d3d->Release();

	//객체 해제 
	sprite_hero[HEROANIM_NUM]->Release();
	sprite_enemy[ENEMYANIM_NUM]->Release();
	sprite_bullet[BULLETANIM_NUM]->Release();

	return;
}
