#pragma once

#include "BaseMovableObject.h"
#include <ode/ode.h>

class CPlayerObject;
class CChainLink;

class CHook : public CBaseMovableObject
{
private:
	CPlayerObject *m_pOwner;
	bool m_bDisconnected;
	std::vector<CChainLink*> chainLinks;
	dJointGroupID chainJoints;

public:
	CHook( CPlayerObject *pOwner );
	~CHook();

	bool IsDisconnected() { return m_bDisconnected; }
	void Disconnect();
	void Reconnect();
	void AddRope();


	virtual void Update( float fTime );
};
