#pragma once

#include <SDL.h>

class CTexture; // Forward declaration

class IActiveObject
{
private:
	float m_fDepth;

public:
	IActiveObject();
	virtual ~IActiveObject();

	void RenderQuad(SDL_Rect target, CTexture *pTexture, float fAngle);
	void RenderQuad(SDL_Rect target, CTexture *pTexture, float fAngle, SDL_Color colour);
	virtual void Render() = 0;

	virtual void Update( float fTime ) = 0;
	float GetDepth() const { return m_fDepth; }
	void SetDepth( float fDepth );
};
