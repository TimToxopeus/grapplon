#pragma once

#include "BaseObject.h"

class CPowerUp : public CBaseObject
{
private:
	int m_iType;

public:
	CPowerUp( int type );
	int GetPowerupType() { return m_iType; }

	virtual void CollideWith( CBaseObject *pOther);
	virtual void Render();
};
