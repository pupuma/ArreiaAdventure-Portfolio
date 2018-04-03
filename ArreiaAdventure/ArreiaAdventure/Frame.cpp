#include "GameSystem.h"
#include "Texture.h"
#include "Frame.h"

Frame::Frame()
{
	_frameTime = 0.0f;
}

Frame::~Frame()
{
}

void Frame::Init(Texture* texture, int x, int y, int width, int height, float rotate, float frameTime)
{
	_texture = texture;

	// �̹������� ��¿� ���� ������ �����Ѵ�.
	_textureRect.left = x;
	_textureRect.right = _textureRect.left + width;
	_textureRect.top = y;
	_textureRect.bottom = _textureRect.top + height;

	// �ؽ��Ŀ� ����� �÷�
	_textureColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	_width = width;
	_height = height;

	_rotate = rotate;

	_frameTime = frameTime;
}

void Frame::Render()
{
	// ũ������
	D3DXVECTOR2 scale = D3DXVECTOR2(1.0f, 1.0f);

	// ȸ��
	D3DXVECTOR2 center = D3DXVECTOR2(_width / 2.0f, _height / 2.0f);
	float rotate = _rotate;

	// ��ġ ����
	float x = _x - _width / 2.0f;
	float y = _y - _height / 2.0f;
	D3DXVECTOR2 translate = D3DXVECTOR2(x, y);

	D3DXMATRIX matrix;
	D3DXMatrixTransformation2D(
		&matrix,		// ������
		NULL,			// �������� �߽���
		0.0f,			// Ȯ�� ȸ�� ���.
		&scale,			// Ȯ��. ũ�� ����
		&center,		// ȸ�� �߽�
		rotate,			// ȸ�� ��
		&translate		// ���� �̵� -> ��ġ�� ����
	);

	GameSystem::GetInstance()->GetSpriteDX()->SetTransform(&matrix);
	GameSystem::GetInstance()->GetSpriteDX()->Draw(_texture->GetTextureDX(), &_textureRect, NULL, NULL, _textureColor);
}

void Frame::Relase()
{
}

void Frame::Reset()
{
	//_spriteDX = spriteDX;
}

void Frame::SetPosition(float x, float y)
{
	_x = x;
	_y = y;
}