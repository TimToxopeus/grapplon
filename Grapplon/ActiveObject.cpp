#include "ActiveObject.h"
#include "Renderer.h"
#include "Texture.h"

IActiveObject::IActiveObject()
{
	// Automatically add the object to the renderer.
	CRenderer::Instance()->Register(this);

	m_fDepth = 0.0f; // Default depth
}

IActiveObject::~IActiveObject()
{
	// Automatically remove the object from the renderer.
	CRenderer::Instance()->Unregister(this);
}

void IActiveObject::RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle)
{
	// Easy RenderQuad implementation to make further use of RenderQuad easy. <-- Epic sentence.
	CRenderer::Instance()->RenderQuad( target, pTexture, fAngle );
}

void IActiveObject::RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, SDL_Color colour)
{
	CRenderer::Instance()->RenderQuad( target, pTexture, fAngle, colour );
}

void IActiveObject::SetDepth(float fDepth)
{
	m_fDepth = fDepth;
	CRenderer::Instance()->ResortObjects();
}
