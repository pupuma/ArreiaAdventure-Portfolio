#include "ComponentSystem.h"
#include "Map.h"
#include "TileCell.h"
#include "NPC.h"
#include "Player.h"

#include "Character.h"
#include "PathfindingState.h"

PathfindingState::PathfindingState(Character* character) : State(character)
{
}

PathfindingState::~PathfindingState()
{
}

void PathfindingState::Start()
{
	State::Start();

	_map = (Map*)ComponentSystem::GetInstance()->FindComponent(L"Map");
	_map->ResetPahtfinding();

	_targetTileCell = _character->GetTargetTileCell();

	TileCell* startTileCell = _map->GetTileCell(_character->GetTilePosition());

	sPathCommand command;
	command.tileCell = startTileCell;
	command.heuristic = 0.0f;
	_pathfindingQueue.push(command);

	_updateState = eUpdateState::PATHFINDING;
}

void PathfindingState::Stop()
{
	State::Stop();

	while (0 != _pathfindingQueue.size())
	{
		_pathfindingQueue.pop();
	}
}

void PathfindingState::Update(float deltaTime)
{
	State::Update(deltaTime);

	if (eStateType::ST_NONE != _nextState)
	{
		_character->ChangeState(_nextState);
		return;
	}

	switch (_updateState)
	{
		case eUpdateState::PATHFINDING:
		UpdatePathfinding();
		break;
		case eUpdateState::BUILD_PATH:
		UpdateBuildPath();
		break;
	}
}

void PathfindingState::UpdatePathfinding()
{
	// ��ã�� ������Ʈ ����
	if (0 != _pathfindingQueue.size())	// ť�� ���𰡰� ��������� �� �տ� ���Ҹ� ���� �˻�
	{
		sPathCommand command = _pathfindingQueue.top();	// �� �տ� �ִ� ���Ҹ� ����
		_pathfindingQueue.pop();	// ���� �� ť���� ����

		if (false == command.tileCell->IsSearchPathfinding())
		{
			// ���� �˻����� ���� Ÿ�� ���̸�
			command.tileCell->SearchPathfind();	// �˻� �ߴٰ� üũ

												// ��ǥ�� ���� �ߴ°�?
			if (command.tileCell->GetTilePosition().x == _targetTileCell->GetTilePosition().x &&
				command.tileCell->GetTilePosition().y == _targetTileCell->GetTilePosition().y)
			{
				_reverseTileCell = command.tileCell;
				_updateState = eUpdateState::BUILD_PATH;
				return;
			}

			for (int direction = 0; direction < (int)eDirection::NONE; direction++)
			{
				TilePoint currentTilePosition = command.tileCell->GetTilePosition();
				TilePoint searchTilePosision = GetSearchTilePositionByDirection(currentTilePosition, (eDirection)direction);
				TileCell*  searchTileCell = _map->GetTileCell(searchTilePosision);

				if (NULL != searchTileCell &&
					false == searchTileCell->IsSearchPathfinding() &&
					(true == searchTileCell->CanMove() ||
					(searchTileCell->GetTilePosition().x == _targetTileCell->GetTilePosition().x &&
						searchTileCell->GetTilePosition().y == _targetTileCell->GetTilePosition().y)
						)
					)
				{
					float distanceFromStart = command.tileCell->GetDistanceFromStart() + 1;

					float heuristic = 0.0f;

					float distance = 0.0f;
					int distanceW = searchTileCell->GetTilePosition().x - _targetTileCell->GetTilePosition().x;
					int distanceH = searchTileCell->GetTilePosition().y - _targetTileCell->GetTilePosition().y;
					distanceW = distanceW * distanceW;
					distanceH = distanceH * distanceH;
					distance = sqrtf(distanceW + distanceH);

					/*
					if (distance < 10)
					{
						heuristic = CalcAStarHeuristic(distanceFromStart, searchTileCell, _targetTileCell);
					}
					else if (distance < 10)
					{
						heuristic = CalcComplexHeuristic(searchTileCell, _targetTileCell);
					}
					else if (distance < 20)
					{
						heuristic = CalcSimpleHeuristic(command.tileCell, searchTileCell, _targetTileCell);
					}
					else
					{
						heuristic = distanceFromStart;
					}
					*/
					heuristic = CalcAStarHeuristic(distanceFromStart, searchTileCell, _targetTileCell);

					if (NULL == searchTileCell->GetPrevTileCell())
					{
						// ť�� �ִ´�.
						searchTileCell->SetPrevTileCell(command.tileCell);
						searchTileCell->SetDistanceFromStart(distanceFromStart);

						sPathCommand newCommand;
						newCommand.tileCell = searchTileCell;
						newCommand.heuristic = heuristic;
						_pathfindingQueue.push(newCommand);

						//CheckTestMark(searchTileCell);
					}
				}
			}
		}
	}
}

void PathfindingState::UpdateBuildPath()
{
	// ��ǥ �������� ��� ����
	// �Ųٷ� ���ư��鼭 ���� ����
	if (NULL != _reverseTileCell)
	{
		//CheckBuildTestMark(_reverseTileCell);
		_character->PushPathfindingCell(_reverseTileCell);
		_reverseTileCell = _reverseTileCell->GetPrevTileCell();
	}
	else
	{
		_nextState = eStateType::ST_MOVE;
	}
}

TilePoint PathfindingState::GetSearchTilePositionByDirection(TilePoint tilePosition, eDirection direction)
{
	TilePoint searchTilePosition = tilePosition;
	switch (direction)
	{
	case eDirection::LEFT:
		searchTilePosition.x--;
		break;
	case eDirection::RIGHT:
		searchTilePosition.x++;
		break;
	case eDirection::UP:
		searchTilePosition.y--;
		break;
	case eDirection::DOWN:
		searchTilePosition.y++;
		break;
	}
	return searchTilePosition;
}

float PathfindingState::CalcSimpleHeuristic(TileCell* current, TileCell* search, TileCell* goal)
{
	float heuristic = 0.0f;

	int diffFromCurrent;
	int diffFromSearch;

	// X
	{
		diffFromCurrent = current->GetTilePosition().x - goal->GetTilePosition().x;
		if (diffFromCurrent < 0)
		{
			diffFromCurrent = -diffFromCurrent;
		}

		diffFromSearch = search->GetTilePosition().x - goal->GetTilePosition().x;
		if (diffFromSearch < 0)
		{
			diffFromSearch = -diffFromSearch;
		}
		if (diffFromSearch < diffFromCurrent)
		{
			heuristic -= 1.0f;
		}
		else if (diffFromCurrent < diffFromSearch)
		{
			heuristic += 1.0f;
		}
	}

	// Y
	{
		diffFromCurrent = current->GetTilePosition().y - goal->GetTilePosition().y;
		if (diffFromCurrent < 0)
		{
			diffFromCurrent = -diffFromCurrent;
		}

		diffFromSearch = search->GetTilePosition().y - goal->GetTilePosition().y;
		if (diffFromSearch < 0)
		{
			diffFromSearch = -diffFromSearch;
		}

		if (diffFromSearch < diffFromCurrent)
		{
			heuristic -= 1.0f;
		}
		else if (diffFromCurrent < diffFromSearch)
		{
			heuristic += 1.0f;
		}
	}
	
	return heuristic;
}

float PathfindingState::CalcComplexHeuristic(TileCell* search, TileCell* goal)
{
	int distanceW = search->GetTilePosition().x - goal->GetTilePosition().x;
	int distanceH = search->GetTilePosition().y - goal->GetTilePosition().y;

	distanceW = distanceW * distanceW;
	distanceH = distanceH * distanceH;

	return (float)(distanceW + distanceH);
}

float PathfindingState::CalcAStarHeuristic(float distanceFromStart, TileCell* search, TileCell* goal)
{
	return distanceFromStart + CalcComplexHeuristic(search, goal);
}

void PathfindingState::CheckTestMark(TileCell* tileCell)
{
	NPC* npc = new NPC(L"testmark");
	npc->Init(L"character_sprite_pack.png", L"npc");
	npc->MoveStart(tileCell->GetTilePosition());
	npc->MoveStop();

	// ĳ���Ͱ� �ڱ� ����Ÿ���� �ٶ󺸵��� ���� ����
	TilePoint curPosition = tileCell->GetTilePosition();
	TilePoint prevPosition = tileCell->GetPrevTileCell()->GetTilePosition();
	eDirection direction;
	if (prevPosition.x < curPosition.x)
	{
		direction = eDirection::LEFT;
	}
	if (curPosition.x < prevPosition.x)
	{
		direction = eDirection::RIGHT;
	}
	if (prevPosition.y < curPosition.y)
	{
		direction = eDirection::UP;
	}
	if (curPosition.y < prevPosition.y)
	{
		direction = eDirection::DOWN;
	}
	npc->SetDirection(direction);
}

void PathfindingState::CheckBuildTestMark(TileCell* tileCell)
{
	Player* player = new Player(L"testmark");
	player->Init(L"character_sprite.png", L"player");
	player->MoveStart(tileCell->GetTilePosition());
	player->MoveStop();

	// ĳ���Ͱ� �ڱ� ����Ÿ���� �ٶ󺸵��� ���� ����
	if (NULL != tileCell->GetPrevTileCell())
	{
		TilePoint curPosition = tileCell->GetTilePosition();
		TilePoint prevPosition = tileCell->GetPrevTileCell()->GetTilePosition();
		eDirection direction;
		if (prevPosition.x < curPosition.x)
		{
			direction = eDirection::RIGHT;
		}
		if (curPosition.x < prevPosition.x)
		{
			direction = eDirection::LEFT;
		}
		if (prevPosition.y < curPosition.y)
		{
			direction = eDirection::DOWN;
		}
		if (curPosition.y < prevPosition.y)
		{
			direction = eDirection::UP;
		}
		player->SetDirection(direction);
	}
}

