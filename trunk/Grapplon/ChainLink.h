#pragma once

#include "BaseMovableObject.h"
#include <ode/ode.h>

class CPlayerObject;

class CChainLink : public CBaseMovableObject
{
private:
	CPlayerObject *m_pOwner;
	CChainLink* next;
	dJointGroupID chainJoints;

public:
	CChainLink( CPlayerObject *pOwner );
	~CChainLink();

	virtual void Update( float fTime );
};
