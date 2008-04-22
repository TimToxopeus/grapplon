#pragma once

#include <vector>

#include <sdl.h>
#include <SDL_opengl.h>

// Forward declaration
class CTexture;
class IActiveObject;

class CRenderer
{
private:
	static CRenderer *m_pInstance;
	CRenderer();
	virtual ~CRenderer();

	std::vector<IActiveObject *> m_vActiveObjects;

	SDL_Surface *m_pScreen;
	bool m_bSorted;

	void SetOrtho();

public:
	static CRenderer *Instance() { if ( !m_pInstance ) m_pInstance = new CRenderer(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	bool Register( IActiveObject *pObject );
	bool Unregister( IActiveObject *pObject );
	void UnregisterAll();

	bool Init();
	void Shutdown();

	void Render();
	void Update( float fTime );
	void ResortObjects();

	void RenderQuad( SDL_Rect target, CTexture *pTexture, float fAngle );
	void RenderQuad( SDL_Rect target, CTexture *pTexture, float fAngle, SDL_Color colour );
};
