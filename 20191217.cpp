// 20191217.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "20191217.h"

#define MAX_LOADSTRING 100

#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define ENEMY_NUM 10
#define BULLET_NUM 100

bool KeyCK = false;

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface


// sprite declarations
LPDIRECT3DTEXTURE9 sprite;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_hero;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_enemy;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_bullet;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_back;    // the pointer to the sprite
LPDIRECT3DTEXTURE9 sprite_clear;    // the pointer to the sprite


LPD3DXFONT dxfont;    // the pointer to the font object
float enemyX = 60.0f, enemyY = 60.0f;    // the enemy position
int health = 150;
int maxhealth = 1000;
LPDIRECT3DTEXTURE9 DisplayTexture;    // the pointer to the texture
int ammo = 10394;    // the player's current ammo

// function prototypes
void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);

void load_display();
void draw_display();
void LoadTexture(LPDIRECT3DTEXTURE9* texture, LPCTSTR filename);
void DrawTexture(LPDIRECT3DTEXTURE9 texture, RECT texcoords, float x, float y, int a);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;


enum { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };


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


};

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




// 적 클래스 
class Enemy :public entity {

	
public:
	void fire();
	void init(float x, float y);
	void move();
	bool show();
	void active();
	void hide();
	bool eShow;

};

void Enemy::init(float x, float y)
{

	x_pos = x;
	y_pos = y;

}


void Enemy::move()
{
	x_pos += 3;

}

bool Enemy::show()
{
	return eShow;
}

void Enemy::active()
{
	eShow = true;

}

void Enemy::hide()
{
	eShow = false;

}




// 총알 클래스 
class Bullet :public entity {

public:
	bool bShow;

	void init(float x, float y);
	void move();
	bool show();
	void hide();
	void active();

};

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
	x_pos -= 8;
}

void Bullet::hide()
{
	bShow = false;

}






//객체 생성 
Hero hero;
Enemy enemy[ENEMY_NUM];
Bullet bullet[BULLET_NUM];



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
		WS_EX_TOPMOST | WS_POPUP, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
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
	d3dpp.Windowed = FALSE;
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

	//load_display();

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"woods.png",    // the file name
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
		&sprite_back);    // load to sprite

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Panel3.png",    // the file name
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
		&sprite);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"attack.png",    // the file name
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
		&sprite_hero);    // load to sprite

	/*D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"attack.png",    // the file name
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
		&sprite_heroAttack);    // load to sprite
		*/

	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"ghost1.png",    // the file name
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
		&sprite_enemy);    // load to sprite


	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"bullet.png",    // the file name
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
		&sprite_bullet);    // load to sprite

	
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"clear.png",    // the file name
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
		&sprite_clear);    // load to sprite
	


	load_display();

	return;
}


void init_game(void)
{
	//객체 초기화 (처음위치)
	hero.init(450,250);

	//적들 초기화 
	for (int i = 0; i < ENEMY_NUM; i++)
	{

		enemy[i].init( rand()%500-400, rand() % 200 + 100);
		enemy[i].active();
		//enemy[i].init((float)(rand() % 500-1000), rand() % 200+50);
	}

	//총알 초기화 
	for (int i = 0; i < BULLET_NUM; i++)
	{
		bullet[i].init(hero.x_pos - 70, hero.y_pos);
	}

}


void do_game_logic(void)
{

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
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].x_pos > 650)
		{
			enemy[i].active();
			enemy[i].init(rand() % 500 - 400, rand() % 200 + 100);
		}
		else
			enemy[i].move();
	}

	for (int i = 0; i < 100; ++i)
	{
		for (int j = 0; j < ENEMY_NUM; ++j)
		{
			// 보이는 총알 중에
			if (bullet[i].show() == TRUE && enemy[j].show()==TRUE)
			{

				// 충돌 되었으면
				if (bullet[i].x_pos < enemy[j].x_pos + 55  //plus enemy x size
					&& enemy[j].x_pos < bullet[i].x_pos + 32
					&&bullet[i].y_pos < enemy[j].y_pos + 50
					&& enemy[j].y_pos < bullet[i].y_pos + 30
					)
				{

					// 충돌한 총알은 안보이고, 게이지 깍기

					bullet[i].hide();
					enemy[j].hide();

					if(health<999)
					health += 30;
					
					
				}

			}
		}

	}


	//총알 처리 
		//if (bullet[i].show() == false)
		//{
	if (KeyCK == false)
	{
		KeyCK = true;
		if (KEY_DOWN(VK_SPACE))
		{
			for (int i = 0; i < 100; ++i)
			{
				if (bullet[i].show() == false)
				{
					bullet[i].active();
					bullet[i].init(hero.x_pos, hero.y_pos);
					break;
				}
			}
		}
	}
	if (KEY_UP(VK_SPACE))
	{
		KeyCK = false;
	}
	

	for (int i = 0; i < 100; ++i)
	{
		if (bullet[i].x_pos < 30)
		{
			bullet[i].hide();
		}

		else
			bullet[i].move();
	}

	
}

// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // // begin sprite drawing with transparency 배경 투명하게

	////UI 창 렌더링 
	//draw_display();

//Background image
	RECT back;
	SetRect(&back, 0, 0, 640, 480); //background size
	D3DXVECTOR3 center0(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position0(0.0f, 0.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_back, &back, &center0, &position0, D3DCOLOR_ARGB(255, 255, 255, 255));


	//TEST
	/*
	RECT part0;
	SetRect(&part0, xpos, 0, xpos + 181, 128);
	D3DXVECTOR3 center3(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position3(150.0f, 50.0f, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite, &part0, &center3, &position3, D3DCOLOR_ARGB(127, 255, 255, 255));
	*/

	

	////총알 
	for (int i=0; i < BULLET_NUM; i++)
	{
		if (bullet[i].bShow == true)
		{
			RECT part1;
			SetRect(&part1, 0, 0, 32, 32);  //bullet size=32
			D3DXVECTOR3 center1(0.0f, -50.0f, 0.0f);    // center at the upper-left corner  //x축, y축 순
			D3DXVECTOR3 position1(bullet[i].x_pos, bullet[i].y_pos, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_bullet, &part1, &center1, &position1, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}

	static int frame = 6;    // start the program on the final frame
	if (KEY_UP(VK_SPACE))
		frame = 0;  // when the space key is pressed, start at frame 0

	//else if (KEY_UP(VK_LEFT)) frame = 0;
	if (frame < 6) frame++;     // if we aren't on the last frame, go to the next frame


	// calculate the x-position
	int xpos = frame * 85 + 1; //한 프레임의 너비 +1

	//주인공 
	RECT part;
	SetRect(&part, xpos, 0, xpos+85, 120); //주인공 size
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 position(hero.x_pos, hero.y_pos, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(sprite_hero, &part, &center, &position, D3DCOLOR_ARGB(255, 255, 255, 255));

	


	////적 
	RECT part2;
	SetRect(&part2, 0, 0, 60, 60);  //enemy size
	D3DXVECTOR3 center2(0.0f, 0.0f, 0.0f);    // center at the upper-left corner

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (enemy[i].show() == true)
		{
		
		D3DXVECTOR3 position2(enemy[i].x_pos, enemy[i].y_pos, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_enemy, &part2, &center2, &position2, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}


	draw_display(); //UI

	//Game Clear Scene
	if (health >= 1000)
	{
		RECT clear;
		SetRect(&clear, 0, 0, 640, 480); //background size
		D3DXVECTOR3 center7(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 position7(0.0f, 0.0f, 0.0f);    // position at 0.0 with no depth
		d3dspt->Draw(sprite_clear, &clear, &center7, &position7, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	
	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}

// this loads the display graphics and font
void load_display()
{
	LoadTexture(&DisplayTexture, L"DisplaySprites.png");

	D3DXCreateFont(d3ddev, 20, 0, FW_BOLD, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Arial", &dxfont);

	return;
}


// this draws the display
void draw_display()
{
	RECT Part;


	// DRAW THE HEALTHBAR
	// display the bar
	SetRect(&Part, 1, 1, 505, 12);
	DrawTexture(DisplayTexture, Part, 11, 456, 255);

	// display the health "juice"
	SetRect(&Part, 506, 1, 507, 12);
	for (int index = 0; index < (health * 490 / maxhealth); index++)
	{
		DrawTexture(DisplayTexture, Part, index + 18, 456, 255);
		//DrawTexture(DisplayTexture, Part, index + 118, 456, 255);
		//DrawTexture(DisplayTexture, Part, index + 218, 456, 255);
		//DrawTexture(DisplayTexture, Part, index + 318, 456, 255);

		/*for (int i = 0; i < ENEMY_NUM; i++)
		{
			for (int j = 0; j < ENEMY_NUM; j++)
			{
				if (bullet[i].x_pos < enemy[j].x_pos + 55  //plus enemy x size
					&& enemy[j].x_pos < bullet[i].x_pos + 32
					&& bullet[i].y_pos < enemy[j].y_pos + 50
					&& enemy[j].y_pos < bullet[i].y_pos + 30
					)
				{
					//health +=10;
		
				}
			}
		}*/
		
	}
	///////////////////////////////////////////////////////////

	// DRAW THE AMMO INDICATOR
	// display the backdrop
	SetRect(&Part, 351, 14, 456, 40);
	DrawTexture(DisplayTexture, Part, 530, 449, 127);

	// display the border
	SetRect(&Part, 351, 45, 457, 72);
	DrawTexture(DisplayTexture, Part, 530, 449, 255);

	// display the font
	SetRect(&Part, 535, 453, 630, 470);
	static char strAmmoText[10];
	_itoa_s(ammo, strAmmoText, 10);
	dxfont->DrawTextA(NULL,
		(LPCSTR)&strAmmoText,
		strlen((LPCSTR)&strAmmoText),
		&Part,
		DT_RIGHT,
		D3DCOLOR_ARGB(255, 120, 120, 255));

	return;
}


// this loads a texture from a file
void LoadTexture(LPDIRECT3DTEXTURE9* texture, LPCTSTR filename)
{
	D3DXCreateTextureFromFileEx(d3ddev, filename, D3DX_DEFAULT, D3DX_DEFAULT,
		D3DX_DEFAULT, NULL, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
		D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), NULL, NULL, texture);

	return;
}


// this draws a portion of the specified texture
void DrawTexture(LPDIRECT3DTEXTURE9 texture, RECT texcoords, float x, float y, int a)
{
	D3DXVECTOR3 center(0.0f, 0.0f, 0.0f), position(x, y, 0.0f);
	d3dspt->Draw(texture, &texcoords, &center, &position, D3DCOLOR_ARGB(a, 255, 255, 255));

	return;
}

// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	sprite->Release();
	d3ddev->Release();
	d3d->Release();

	 // 객체 해제 
    	 sprite_hero->Release();
	sprite_enemy->Release();
	sprite_bullet->Release();

	return;
}