#pragma once

#include "BaseMovableObject.h"

class CPlayerObject;

class CHook : public CBaseMovableObject
{
private:
	CPlayerObject *m_pOwner;
	bool m_bDisconnected;

public:
	CHook( CPlayerObject *pOwner );
	bool IsDisconnected() { return m_bDisconnected; }
	void Disconnect();
	void Reconnect();

	virtual void Update( float fTime );
};
