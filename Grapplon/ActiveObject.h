#pragma once

#include <SDL.h>

class CAnimatedTexture;

enum ObjectType { UNSET = 0, STATE, PARTICLESYSTEM, SHIP, HOOK, PLANET, ASTEROID, CHAINLINK };

class IActiveObject
{
private:
	float m_fDepth;

protected:
	ObjectType m_eType;
	float m_fAlpha;
	bool m_bDeleteMe;

public:
	IActiveObject();
	virtual ~IActiveObject();

	void RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, float fAlpha = -1.0f );
	void RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, SDL_Color colour, float fAlpha = -1.0f );
	virtual void Render() = 0;

	virtual void Update( float fTime ) = 0;
	float GetDepth() const { return m_fDepth; }
	void SetDepth( float fDepth );

	float GetAlpha() const { return m_fAlpha; }
	void SetAlpha( float fAlpha ) { m_fAlpha = fAlpha; }

	ObjectType getType() { return m_eType; }

	bool ShouldBeDeleted() { return m_bDeleteMe; }
};
