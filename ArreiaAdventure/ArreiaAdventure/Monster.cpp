#include "ComponentSystem.h"
#include "Map.h"
#include "TileCell.h"
#include "MoveState.h"
#include "IdleState.h"

#include "Monster.h"

Monster::Monster(std::wstring name) : Character(name)
{
	_type = eComponentType::CT_MONSTER;
}

Monster::~Monster()
{

}

void Monster::UpdateAI(float deltaTime)
{
	if (false == _isLive)
	{
		return;
	}

	if (false == _isMoving)
	{
		Map* map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"Map");
		if (NULL != map)
		{
			// Ž�� - ���� ���� ���� ���� �ִ°�?
			std::vector<eComponentType> findTypeList;
			findTypeList.push_back(eComponentType::CT_NPC);
			findTypeList.push_back(eComponentType::CT_PLAYER);
			Component* findComponent = map->FindComponentInRange(this, 4, findTypeList);
			
			// ���� ������ �� �������� �̵�
			_nextDirection = eDirection::NONE;
			eDirection monsterDirection = eDirection::NONE;
			TilePoint tilePos = GetNextAttackPosition();
			
			if (NULL != findComponent)
			{
				TilePoint targetPosition = findComponent->GetTilePosition();
				if (targetPosition.y < _tilePosition.y)
				{
					_nextDirection = eDirection::UP;
					
				}
				else if (_tilePosition.y < targetPosition.y)
				{
					_nextDirection = eDirection::DOWN;
				}
				else if (targetPosition.x < _tilePosition.x)
				{
					_nextDirection = eDirection::LEFT;
				}
				else if (_tilePosition.x < targetPosition.x)
				{
					_nextDirection = eDirection::RIGHT;
				}
				else
				{
					_nextDirection = (eDirection)(rand() % 4);
				}
				switch (monsterDirection)
				{
				case eDirection::LEFT:
					tilePos.x--;
					break;
				case eDirection::RIGHT:
					tilePos.x++;
					break;
				case eDirection::UP:
					tilePos.y--;
					break;
				case eDirection::DOWN:
					tilePos.y++;
					break;
				}
					
			}	
			else
			{
				_nextDirection = (eDirection)(rand() % 4);
			}

			switch (monsterDirection)
			{
			case eDirection::LEFT:
				tilePos.x--;
				break;
			case eDirection::RIGHT:
				tilePos.x++;
				break;
			case eDirection::UP:
				tilePos.y--;
				break;
			case eDirection::DOWN:
				tilePos.y++;
				break;
			}

			SetNextAttackPosition(tilePos);

			if (eDirection::NONE != _nextDirection)
			{
				_state->ChangeState(eStateType::ST_MOVE);
			}
		}
	}
}

std::vector<Component*> Monster::Collision(std::vector<Component*> collisionList)
{
	std::vector<Component*> filterList;
	for (int i = 0; i < collisionList.size(); i++)
	{
		if (eComponentType::CT_NPC == collisionList[i]->GetType() ||
			eComponentType::CT_PLAYER == collisionList[i]->GetType())
		{
			filterList.push_back(collisionList[i]);
		}
	}
	return filterList;
}