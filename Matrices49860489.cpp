// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>
using namespace std;

#include "Matrices49860489.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1)
{
	if ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1) < (size0 + size1) * (size0 + size1))
		return true;
	else
		return false;
}

entity::entity(void)
	: fAlpha(255.f)
{
	ZeroMemory(&matWorld, sizeof(D3DXMATRIX));
	D3DXMatrixIdentity(&matWorld);
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

Hero::Hero()
	: HeroisDie(false),
	fSpriteFrame(0.f),
	fAnimSpeed(0.4f)
{

}

Enemy::Enemy(void) 
	:fSpriteFrame(0.f),
	fAnimSpeed(0.35f)
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

Boss::Boss(void)
	: fSpriteFrame(0.f)
	, fAnimSpeed(0.03f)
{
	HP		=	MAXHP	= 1000;
	x_pos	= SCREEN_WIDTH / 2 + SCREEN_WIDTH / 4;
	y_pos	= SCREEN_HEIGHT / 2 - SCREEN_HEIGHT / 10;
}

Bullet::Bullet(void)
	: fSpriteFrame(0.f)
	, fAnimSpeed(0.3f)
	, fBulletDamage(10.f)
{
}

bool Bullet::check_collision(float x, float y, float _OpponentCollSize)
{
	//충돌 처리 시 
	if (sphere_collision_check(x_pos, y_pos, 32, x, y, _OpponentCollSize) == true)
	{
		bShow = false;

		for(int i = 0; i < EFBOMB_NUM; ++i)
		{
			if(EFBomb[i].bShow)
				continue;

			EFBomb[i].bShow = true;

			EFBomb[i].x_pos = x_pos;
			EFBomb[i].y_pos = y_pos;

			return true;
		}
	}
	else {

		return false;
	}

	return false;
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

EFBomb::EFBomb()
	: bShow(false)
	, fSpriteFrame(0.f)
	, fAnimSpeed(0.5f)
{
}

HpBar::HpBar()
	:bShow(false)
{
	x_pos	= SCREEN_WIDTH / 2;
	y_pos	= SCREEN_HEIGHT * 9 / 10;
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

	while (true)
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

	return (int)msg.wParam;
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
	m_phWnd = &hWnd;
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

		if( D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
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
			&sprite_bullet[i]))// load to sprite
		{
			MessageBox(hWnd, L"Texture Image Load failed", L"System Error", NULL);
		}
	}

	for (int i = 0; i < BOSSANIM_NUM; ++i)
	{
		wsprintf(szBuf3, L"Texture\\Boss\\Boss%d.png", i);

		if( D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
			szBuf3,		// the file name
			150,    // default width
			170,    // default height
			D3DX_DEFAULT,    // no mip mapping
			NULL,    // regular usage
			D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
			D3DPOOL_MANAGED,    // typical memory handling
			D3DX_DEFAULT,    // no filtering
			D3DX_DEFAULT,    // no mip filtering
			D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
			NULL,    // no image info struct
			NULL,    // not using 256 colors
			&sprite_Boss[i]))// load to sprite
		{
			MessageBox(hWnd, L"Texture Image Load failed", L"System Error", NULL);
		}
	}

	for (int i = 0; i < EFBOMBANIM_NUM; ++i)
	{
		wsprintf(szBuf3, L"Texture\\Shoot_\\Shoot_%d.png", i);

		if( D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
			szBuf3,		// the file name
			96,    // default width
			96,    // default height
			D3DX_DEFAULT,    // no mip mapping
			NULL,    // regular usage
			D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
			D3DPOOL_MANAGED,    // typical memory handling
			D3DX_DEFAULT,    // no filtering
			D3DX_DEFAULT,    // no mip filtering
			D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
			NULL,    // no image info struct
			NULL,    // not using 256 colors
			&sprite_EFBomb[i]))// load to sprite
		{
			MessageBox(hWnd, L"Texture Image Load failed", L"System Error", NULL);
		}
	}

	for (int i = 0; i < HPBARANIM_NUM; ++i)
	{
		wsprintf(szBuf3, L"Texture\\HpBar\\HpBar%d.png", i);

		if( D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
			szBuf3,		// the file name
			355,    // default width
			54,    // default height
			D3DX_DEFAULT,    // no mip mapping
			NULL,    // regular usage
			D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
			D3DPOOL_MANAGED,    // typical memory handling
			D3DX_DEFAULT,    // no filtering
			D3DX_DEFAULT,    // no mip filtering
			D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
			NULL,    // no image info struct
			NULL,    // not using 256 colors
			&sprite_HPBar[i]))// load to sprite
		{
			MessageBox(hWnd, L"Texture Image Load failed", L"System Error", NULL);
		}
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

	InitFont();

	return;
}


void init_game(void)
{
	//객체 초기화 
	hero.init(64, SCREEN_HEIGHT / 2);

	//적들 초기화 
	for (int i = 0; i<ENEMY_NUM; i++)
	{
		enemy[i].init( (float)(rand() % 300 + 600), (float)(rand() % 300 + 10) );
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
		if(iSCORE >= iGoal)
			break;

		if (enemy[i].x_pos < 100)
			enemy[i].init( (float)(rand() % 300 + 600), (float)(rand() % 300 + 10) );
		else
			enemy[i].move();
	}

	// 타이머 처리
	if(Timer3On)
		++timer3;

	if(timer3 >= 3)
	{
		Timer3On = false;
		timer3 = 0;
	}

	//총알 처리 
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].show() == false && Timer3On == false)
		{
			if (KEY_DOWN(VK_SPACE))
			{
					bullet[i].active();
					bullet[i].init(hero.x_pos + 60, hero.y_pos + 40);
					Timer3On = true;

					return;
			}
		}


		if (bullet[i].show() == true)
		{
			if (bullet[i].x_pos > 600)
				bullet[i].hide();
			else
				bullet[i].move();

			//충돌 처리 
			for (int i = 0; i < BULLET_NUM; i++)
			{
				if(iSCORE < iGoal) // 적 충돌체크
				{
					for (int j = 0; j < ENEMY_NUM; j++)
					{
						if (bullet[i].check_collision(enemy[j].x_pos, enemy[j].y_pos) == true)
						{
							enemy[j].init( (float)(rand() % 300 + SCREEN_WIDTH + 100), (float)(rand() % SCREEN_HEIGHT + 100) );
							bullet[i].hide();
							bullet[i].init(0.f, 0.f);
							++iSCORE;
							break;
						}
					}
				}
				else		// 보스 충돌체크
				{
					if (Boss.HP > 0 && bullet[i].check_collision(Boss.x_pos, Boss.y_pos, 80.f) == true)
					{
						bullet[i].hide();
						bullet[i].init(0.f, 0.f);

						Boss.HP -= (int)bullet[i].fBulletDamage;
						//보스 데미지
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
	
	timer++;
	timer2++;
	
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

	//클래스가 아니라 이렇게 햇지만.. 이렇게 하면안됨
	D3DXMATRIX matWorldTemp;

	ZeroMemory(&matWorldTemp, sizeof(D3DXMATRIX));
	D3DXMatrixIdentity(&matWorldTemp); //단위행렬
	//

	RECT part3;
	SetRect(&part3, 0, 0, 640, 480);
	D3DXVECTOR3 center3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position3(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth

	d3dspt->SetTransform(&matWorldTemp);
	d3dspt->Draw(sprite_background, &part3, &center3, &position3, D3DCOLOR_ARGB(255, 255, 255, 255));

	D3DXMATRIX matScale, matTrans;

	//주인공 
	hero.fSpriteFrame += hero.fAnimSpeed;

	if ((int)hero.fSpriteFrame >= HEROANIM_NUM)
		hero.fSpriteFrame = 0.f;

		RECT HeroPart;
		SetRect(&HeroPart, 0, 0, 118, 142);
		D3DXVECTOR3 HeroCenter(59.0f, 71.0f, 0.0f);    // center at the upper-left corner

		D3DXMatrixTranslation(&matTrans, hero.x_pos, hero.y_pos, 0.f);
		hero.matWorld = matTrans;

		d3dspt->SetTransform(&hero.matWorld);
		d3dspt->Draw(sprite_hero[(int)hero.fSpriteFrame], &HeroPart, &HeroCenter, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));

	////적
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if(iSCORE >= iGoal)
			break;

		enemy[i].fSpriteFrame += enemy[i].fAnimSpeed;

		if ((int)enemy[i].fSpriteFrame >= ENEMYANIM_NUM)
			enemy[i].fSpriteFrame = 0.f;
		
		RECT EnemyPart;
		SetRect(&EnemyPart, 0, 0, 40, 60);
		D3DXVECTOR3 EnemyCenter(20.0f, 30.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 EnemyPosition(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth

		d3dspt->SetTransform(&enemy[i].matWorld);
		d3dspt->Draw(sprite_enemy[(int)enemy[i].fSpriteFrame], &EnemyPart, &EnemyCenter, &EnemyPosition, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	////보스
	D3DXMatrixScaling(&matScale, 2.f, 2.f, 1.f);
	D3DXMatrixTranslation(&matTrans, Boss.x_pos, Boss.y_pos, 0.f);
	Boss.matWorld = matScale * matTrans;

	for (int i = 0; i < BOSSANIM_NUM; i++)
	{
		if(iSCORE < iGoal)
			break;

		if(Boss.HP <= 0)
			Boss.fAlpha -= 1.f;

		if(Boss.fAlpha <= 0)
			Boss.fAlpha = 0.f;

		Boss.fSpriteFrame += Boss.fAnimSpeed;

		if ((int)Boss.fSpriteFrame >= BOSSANIM_NUM)
			Boss.fSpriteFrame = 0.f;
		
		RECT EnemyPart;
		SetRect(&EnemyPart, 0, 0, 150, 170);
		D3DXVECTOR3 EnemyCenter(75.f, 85.f, 0.0f);					// center at the upper-left corner

		d3dspt->SetTransform(&Boss.matWorld);
		d3dspt->Draw(sprite_Boss[(int)Boss.fSpriteFrame], &EnemyPart, &EnemyCenter, NULL, D3DCOLOR_ARGB((int)Boss.fAlpha, 255, 255, 255));
	}

	////총알 
	for (int i = 0; i < BULLET_NUM; i++)
	{
		if (bullet[i].bShow == true)
		{
			bullet[i].fSpriteFrame += bullet[i].fAnimSpeed;

			if((int)bullet[i].fSpriteFrame >= BULLETANIM_NUM)
				bullet[i].fSpriteFrame = 0.f;

			RECT part1;
			SetRect(&part1, 0, 0, 50, 64);
			D3DXVECTOR3 center1(25.0f, 32.0f, 0.0f);    // center at the upper-left corner

			D3DXMatrixTranslation(&matTrans, bullet[i].x_pos, bullet[i].y_pos, 0.f);
			bullet[i].matWorld =  matTrans;

			d3dspt->SetTransform(&bullet[i].matWorld);
			d3dspt->Draw(sprite_bullet[(int)bullet[i].fSpriteFrame], &part1, &center1, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}

	////이팩트
	float fAlphaCount;
	for (int i = 0; i < EFBOMB_NUM; i++)
	{
		if (EFBomb[i].bShow == true)
		{
			fAlphaCount = 120.f / (EFBomb[i].fAnimSpeed * EFBOMBANIM_NUM);

			EFBomb[i].fAlpha -= fAlphaCount;
			if(EFBomb[i].fAlpha <= 0.f)
				EFBomb[i].fAlpha = 0.f;
			
			EFBomb[i].fSpriteFrame += EFBomb[i].fAnimSpeed;

			if((int)EFBomb[i].fSpriteFrame >= EFBOMBANIM_NUM)
			{
				EFBomb[i].fSpriteFrame = 0.f;
				EFBomb[i].bShow = false;
				EFBomb[i].x_pos = 0.f;
				EFBomb[i].y_pos = 0.f;
				EFBomb[i].fAlpha = 255.f;
			}

			RECT part1;
			SetRect(&part1, 0, 0, 96, 96);
			D3DXVECTOR3 center1(48.0f, 48.0f, 0.0f);    // center at the upper-left corner

			D3DXMatrixTranslation(&matTrans, EFBomb[i].x_pos, EFBomb[i].y_pos, 0.f);
			EFBomb[i].matWorld =  matTrans;

			d3dspt->SetTransform(&EFBomb[i].matWorld);
			d3dspt->Draw(sprite_EFBomb[(int)EFBomb[i].fSpriteFrame], &part1, &center1, NULL, D3DCOLOR_ARGB((int)EFBomb[i].fAlpha, 255, 255, 255));
		}
	}

	if(iSCORE < iGoal)
		FontRender(L"Score : %d", &matWorldTemp);
	else
	{
		int iImageX = 355;
		D3DXVECTOR3 center3(177.5f, 27.f, 0.f);		// center at the upper-left corner
	
		for(int i = 0; i < HPBARANIM_NUM; ++i)
		{
			if(i != 1)  // 테두리
				SetRect(&part3, 0, 0, iImageX, 54);
			else		//HP
			{
				iImageX = 355 * Boss.HP / Boss.MAXHP;
				SetRect(&part3, 0, 0, iImageX, 54);
			}

			//알파값
			if(Boss.HP <= 0)
				HpBar[i].fAlpha -= 1.f;

			if(Boss.fAlpha <= 0)
			{
				HpBar[i].fAlpha = 0.f;
				FontRender(L"Congratulate You are A+ !!", &matWorldTemp);
			}
			D3DXMatrixTranslation(&matTrans, HpBar[i].x_pos, HpBar[i].y_pos, 0.f);
			HpBar[i].matWorld = matTrans;

			d3dspt->SetTransform(&HpBar[i].matWorld);
			d3dspt->Draw(sprite_HPBar[i], &part3, &center3, NULL, D3DCOLOR_ARGB((int)HpBar[i].fAlpha, 255, 255, 255));
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
	d3dspt->Release();
	d3ddev->Release();
	d3d->Release();

	//객체 해제 
	sprite_hero[HEROANIM_NUM]->Release();
	sprite_enemy[ENEMYANIM_NUM]->Release();
	sprite_bullet[BULLETANIM_NUM]->Release();

	return;
}

void InitFont(void)
{
	// 폰트
	D3DXFONT_DESC	hFont;
	ZeroMemory(&hFont, sizeof(D3DXFONT_DESC));

	hFont.Width = 10;
	hFont.Height = 20;
	hFont.Weight = FW_HEAVY;
	lstrcpy(hFont.FaceName, L"휴먼둥근헤드라인");
	hFont.CharSet = HANGEUL_CHARSET;

	D3DXCreateFontIndirect(d3ddev, &hFont, &m_pD3DXFont);
}

void FontRender(wchar_t _szBuf[100], D3DXMATRIX* _matWorld)
{
	RECT		rc = {SCREEN_WIDTH / 2 - 50, 20, SCREEN_WIDTH /2 + 50, 120};

	TCHAR			szFps[128];
	wsprintf(szFps, _szBuf, iSCORE);

	d3dspt->SetTransform(_matWorld);
	m_pD3DXFont->DrawTextW(d3dspt, szFps, lstrlen(szFps), &rc, DT_NOCLIP
		, D3DCOLOR_ARGB(255, 255, 0, 0));

}