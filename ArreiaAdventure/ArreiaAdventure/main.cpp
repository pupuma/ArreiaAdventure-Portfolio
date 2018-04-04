#include <Windows.h>
#include <d3dx9.h>
#include <list>
#include "InputSystem.h"
#include "ResourceManager.h"
#include "ComponentSystem.h"
#include "GameSystem.h"
#include "GameTimer.h"
#include "Map.h"
#include "NPC.h"
#include "Monster.h"
#include "Player.h"
#include "PathfinderPlayer.h"
#include "PathfinderMonster.h"
#include "RecoveryItem.h"
#include "Sprite.h"
#include "Font.h"
#include "TileCell.h"
#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	srand(time(NULL));

	int clientWidth = 1280;
	int clientHeight = 720;
	bool isWindow = true;

	// ������ �������α׷�(â) �۵� ��Ÿ�� ����� ���
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;	// â�� ��Ÿ�� ����
	wc.lpfnWndProc = WndProc;	// ������ ���ν���
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;	// �������α׷� �ν��Ͻ� �ڵ� ����
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);	// ������
	wc.hCursor = LoadCursor(0, IDC_ARROW);	// Ŀ��
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// ��� ����
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"GameAppStyle";	// �������α׷��� ��Ÿ�� ��ǥ�̸�
	if (!RegisterClass(&wc))
	{
		return 0;
	}

	// ������ â�� �����ϰ� �ڵ�������
	DWORD style;
	if (isWindow)
	{
		style = WS_OVERLAPPEDWINDOW;
	}
	else
	{
		style = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;
	}
	
	HWND hWnd = CreateWindow(
		L"GameAppStyle",	// �������α׷��� ��Ÿ��
		L"2D Tile Game",	// â�� ����
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,	//x, y
		clientWidth, clientHeight ,	//�ʺ�, ����
		0,	// �θ� â�� ���� ��, �θ�â�� �ڵ�
		0,	// �޴� �ڵ�.
		hInstance,	// ��������������� â�� ����� ���� ���α׷� �ڵ� ����
		0
	);
	if (NULL == hWnd)
	{
		return 0;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// â��� �̸� Ŭ���̾�Ʈ ũ�⸦ ����
	if (true == isWindow)
	{
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		MoveWindow(hWnd,
			0, 0,
			clientWidth + (clientWidth - clientRect.right),
			clientHeight + (clientHeight - clientRect.bottom),
			TRUE
		);
	}


	// DirectX

	HRESULT hr;

	// Direct3D ��ü�� ����
	LPDIRECT3D9 direct3d;
	direct3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (NULL == direct3d)
	{
		return 0;
	}

	// �������� ������ ������ ����
	D3DFORMAT format;
	if(isWindow)
	{
		format = D3DFMT_UNKNOWN;
	}
	else
	{
		format = D3DFMT_X8R8G8B8;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = clientWidth;
	d3dpp.BackBufferHeight = clientHeight;
	d3dpp.BackBufferFormat = format;
	d3dpp.BackBufferCount = 1;	// �� ������ ����
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = isWindow;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// DirectX ����̽��� ����. ����̽��� ���ؼ� �ϵ��� ���� ������ ����
	LPDIRECT3DDEVICE9 device3d;
	hr = direct3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&device3d
	);
	if (FAILED(hr))
	{
		MessageBox(0, L"Failed CreateDevice", L"Error", MB_OK);
		return 0;
	}

	// Sprite COM �������̽� ���
	ID3DXSprite* spriteDX;
	hr = D3DXCreateSprite(device3d, &spriteDX);
	if (FAILED(hr))
	{
		MessageBox(0, L"Failed D3DXCreateSprite", L"Error", MB_OK);
		return 0;
	}

	GameSystem::GetInstance()->SetClientWidth(clientWidth);
	GameSystem::GetInstance()->SetClientHeight(clientHeight);
	GameSystem::GetInstance()->SetDeviceDX(device3d);
	GameSystem::GetInstance()->SetSpriteDX(spriteDX);

	std::list<Component*> stageComponentList;

	Map* map = new Map(L"Map");
	//map->Init(L"MapSprite.png", L"MapData_Layer");
	map->Init(L"MapSprite.png", L"PathMap_Layer");
	stageComponentList.push_back(map);

	// ȸ�� ������ ��ġ
	for (int i = 0; i < 10; i++)
	{
		WCHAR name[256];
		wsprintf(name, L"recovery_item_%d", i);
		RecoveryItem* item = new RecoveryItem(name);
		item->Init(L"item_pack.png", L"recovery_item");
		stageComponentList.push_back(item);
	}

	Character* character = new Player(L"player");
	//Character* character = new PathfinderPlayer(L"player");
	character->Init(L"character_sprite.png", L"player");
	stageComponentList.push_back(character);


	//Monster* monster = new PathfinderMonster(L"monster");
	Character* monster = new Monster(L"monster");
	monster->Init(L"monster_sprite_pack.png", L"npc");
	stageComponentList.push_back(monster);
	

	TilePoint tilePosition;
	tilePosition.x = map->GetWidth() / 2;
	tilePosition.y = map->GetHeight() / 2 + 2;
	TileCell* tileCell = map->GetTileCell(tilePosition);
	Component* midTileObject = tileCell->GetComponentList().front();
	map->SetViewer(character);



	// FPS ���� (60fps)
	float frameTime = 1.0f / 60.0f;
	float frameDuration = 0.0f;

	GameTimer gameTimer;
	gameTimer.Init();

	// �޽��� ���
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	// Ű�ڵ带 ���� �޽��� ��ȭ.
			DispatchMessage(&msg);	// �޽����� â���� ���.
		}
		else
		{
			gameTimer.Update();
			float deltaTime = gameTimer.GetDeltaTime();

			ComponentSystem::GetInstance()->Update(deltaTime);

			for (std::list<Component*>::iterator it = stageComponentList.begin();
				it != stageComponentList.end(); it++)
			{
				(*it)->Update(deltaTime);
			}

			frameDuration += deltaTime;
			if (frameTime <= frameDuration)
			{
				frameDuration = 0;

				device3d->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(124, 124, 124), 0.0f, 0);
				{
					device3d->BeginScene();
					{
						spriteDX->Begin(D3DXSPRITE_ALPHABLEND);
						{
							map->Render();
						}
						spriteDX->End();
					}
					device3d->EndScene();
				}

				// ����̽��� ���ǵǾ�����, �ٽ� ���� ���༭ ��������� �ǵ��� ������Ѵ�.
				{
					// ����̽� ���� Ȯ��
					hr = device3d->TestCooperativeLevel();
					if (FAILED(hr))	// ����̽� ���¿� ���� �߻�
					{
						// ����̽� ���� ���� Ȯ��
						if (D3DERR_DEVICELOST == hr)	// ���� �Ұ��� ����
						{
							Sleep(100);	// ���� ������ ���·� �ٲ� �� ���� ��ٸ���.
						}
						else if (D3DERR_DEVICENOTRESET == hr)	// �ν�Ʈ ����������, ���� ���� ����
						{
							for (std::list<Component*>::iterator it = stageComponentList.begin();
								it != stageComponentList.end(); it++)
							{
								(*it)->Release();
							}

							direct3d = Direct3DCreate9(D3D_SDK_VERSION);
							if (NULL != direct3d)
							{
								hr = direct3d->CreateDevice(
									D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL,
									hWnd,
									D3DCREATE_HARDWARE_VERTEXPROCESSING,
									&d3dpp,
									&device3d
								);
								if (SUCCEEDED(hr))
								{
									hr = D3DXCreateSprite(device3d, &spriteDX);
									if (SUCCEEDED(hr))
									{
										for (std::list<Component*>::iterator it = stageComponentList.begin();
											it != stageComponentList.end(); it++)
										{
											(*it)->Reset();
										}
									}
								}
							}
						}
					}							
				}

				device3d->Present(NULL, NULL, NULL, NULL);
			}
		}
	}

	// ���α׷��� ������ ����, ��� �ߴ��ڿ��������Ѵ�.

	ComponentSystem::GetInstance()->RemoveAllComponents();


	ResourceManager::GetInstance()->RemoveAllTexture();

	if (spriteDX)
	{
		spriteDX->Release();
		spriteDX = NULL;
	}
	if (device3d)
	{
		device3d->Release();
		device3d = NULL;
	}
	if (direct3d)
	{
		direct3d->Release();
		direct3d = NULL;
	}

	return 0;
}


// ������ ���ν����Լ�
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// �޽��� ó��
	int x = 0;
	int y = 0;
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		InputSystem::GetInstance()->MouseDown(x, y);
		return 0;
	case WM_LBUTTONUP:
		InputSystem::GetInstance()->MouseUp();
		return 0;
	case WM_KEYDOWN:
		InputSystem::GetInstance()->KeyDown(wParam);
		return 0;
	case WM_KEYUP:
		if (VK_ESCAPE == wParam)
		{
			DestroyWindow(hWnd);
		}
		InputSystem::GetInstance()->KeyUp(wParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}