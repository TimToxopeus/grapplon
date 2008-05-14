#include "ActiveObject.h"
#include "Renderer.h"
#include "Texture.h"

IActiveObject::IActiveObject()
{
	// Automatically add the object to the renderer.
	CRenderer::Instance()->Register(this);

	m_fDepth = 0.0f; // Default depth
	m_fAlpha = 1.0f;
	m_eType = UNSET;
}

IActiveObject::~IActiveObject()
{
	// Automatically remove the object from the renderer.
	CRenderer::Instance()->Unregister(this);
}

void IActiveObject::RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, float fAlpha)
{
	// Easy RenderQuad implementation to make further use of RenderQuad easy. <-- Epic sentence.
	if ( fAlpha == -1 )
		CRenderer::Instance()->RenderQuad( target, pTexture, fAngle, m_fAlpha );
	else
		CRenderer::Instance()->RenderQuad( target, pTexture, fAngle, fAlpha );
}

void IActiveObject::RenderQuad(SDL_Rect target, CAnimatedTexture *pTexture, float fAngle, SDL_Color colour, float fAlpha)
{
	if ( fAlpha == -1 )
		CRenderer::Instance()->RenderQuad( target, pTexture, fAngle, colour, m_fAlpha );
	else
		CRenderer::Instance()->RenderQuad( target, pTexture, fAngle, colour, fAlpha );
}

void IActiveObject::SetDepth(float fDepth)
{
	m_fDepth = fDepth;
	CRenderer::Instance()->ResortObjects();
}
