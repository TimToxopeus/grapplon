#pragma once

#include <ode/ode.h>
#include <vector>

class CODEManager
{
private:
	static CODEManager *m_pInstance;
	CODEManager();
	virtual ~CODEManager();

	dWorldID m_oWorld;
	std::vector<dBodyID> m_vBodies;

public:
	static CODEManager *Instance() { if ( !m_pInstance ) m_pInstance = new CODEManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	dBodyID CreateBody() { dBodyID body = dBodyCreate(m_oWorld); m_vBodies.push_back(body); return body; }
	void Update( float fTime );
};
