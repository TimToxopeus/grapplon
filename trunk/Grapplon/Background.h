#pragma once

#include "BaseObject.h"

class CAnimatedTexture;

class CBackground : public CBaseObject
{
private:
	CAnimatedTexture *m_pSpace;
	CAnimatedTexture *m_pNebula;
	float m_fNebulaOffset;

public:
	CBackground();
	~CBackground();

	virtual void Update( float fTime );
	virtual void Render();
};
