#include "ComponentSystem.h"
#include "Map.h"
#include "TileCell.h"
#include "Sprite.h"

#include "RecoveryItem.h"


RecoveryItem::RecoveryItem(std::wstring name) : Component(name)
{
	_type = eComponentType::CT_ITEM;
}

RecoveryItem::~RecoveryItem()
{
}

void RecoveryItem::Init(std::wstring textureFilename, std::wstring scriptFilename)
{
	// �ʻ� ������ ��ġ�� ����
	{
		Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"Map");
		if (NULL != map)
		{
			TilePoint tilePos;
			tilePos.x = rand() % map->GetWidth();
			tilePos.y = rand() % map->GetHeight();
			while (false == map->GetTileCell(tilePos)->CanMove())
			{
				tilePos.x = rand() % map->GetWidth();
				tilePos.y = rand() % map->GetHeight();
			}
			_tilePosition = tilePos;
			map->SetTileComponent(_tilePosition, this);
		}
	}

	std::wstring script = scriptFilename;
	script = script.append(L".json");
	_sprite = new Sprite();
	_sprite->Init(textureFilename, script);
}

void RecoveryItem::Deinit()
{
}

void RecoveryItem::Update(float deltaTime)
{
	_sprite->SetPosition(_position.x, _position.y);
	_sprite->Update(deltaTime);
}

void RecoveryItem::Render()
{
	_sprite->Render();
}

void RecoveryItem::Release()
{
	_sprite->Release();
}

void RecoveryItem::Reset()
{
	_sprite->Reset();
}

void RecoveryItem::ReceiveMsg(const sMessageParam& param)
{
	if (L"Use" == param.message)
	{
		sMessageParam msg;
		msg.sender = this;
		msg.receiver = param.sender;
		msg.message = L"RecoveryHP";
		msg.recoveryHP = 40;
		ComponentSystem::GetInstance()->SendMsg(msg);

		_isLive = false;
		Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"Map");
		if (NULL != map)
		{
			map->ResetTileComponent(_tilePosition, this);
		}
	}
}
