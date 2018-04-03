#include "Component.h"
#include "Sprite.h"
#include "TileCell.h"

TileCell::TileCell()
{
	//_sprite = NULL;
	_position.x = _position.y = 0.0f;
}

TileCell::~TileCell()
{
}

void TileCell::Init(int tileX, int tileY)
{
	_tilePostion.x = tileX;
	_tilePostion.y = tileY;
}

void TileCell::Update(float deltaTime)
{
	//_sprite->Update(deltaTime);
}

void TileCell::Render()
{
	for (std::list<Component*>::iterator it = _componentList.begin();
		it != _componentList.end(); it++)
	{
		Component* component = (*it);
		component->SetPosition(_position);
		component->Render();
	}
}

void TileCell::Release()
{
	//_sprite->Release();
}

void TileCell::Reset()
{
	//_sprite->Reset();
}

void TileCell::SetPosition(Point position)
{
	_position = position;
}

void TileCell::AddComponent(Component* component)
{
	_componentList.push_back(component);
}

void TileCell::RemoveComponent(Component* component)
{
	_componentList.remove(component);
}

std::list<Component*> TileCell::GetCollisionList()
{
	std::list<Component*> collisionList;
	for (std::list<Component*>::iterator it = _componentList.begin();
		it != _componentList.end();
		it++)
	{
		if (false == (*it)->CanMove())
		{
			collisionList.push_back((*it));
		}
	}
	return collisionList;
}

bool TileCell::CanMove()
{
	// Ÿ�ϼ��� �������������Ʈ����Ʈ�� �˻��ؼ�
	// �ϳ��� �� �� ����Ÿ���� ������
	// false ����
	for (std::list<Component*>::iterator it = _componentList.begin();
		it != _componentList.end();
		it++)
	{
		if (false == (*it)->CanMove())
		{
			return false;
		}
	}
	return true;
}