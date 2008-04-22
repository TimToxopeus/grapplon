#include "ODEManager.h"
#include "LogManager.h"
#pragma warning(disable:4018)

CODEManager *CODEManager::m_pInstance = NULL;

CODEManager::CODEManager()
{
	CLogManager::Instance()->LogMessage("Initializing ODE manager.");
	m_oWorld = dWorldCreate();
}

CODEManager::~CODEManager()
{
	CLogManager::Instance()->LogMessage("Terminating ODE manager.");
	CLogManager::Instance()->LogMessage("Clearing up bodies (Bwa ha ha!).");
	if ( m_vBodies.size() > 0 )
	{
		for ( int i = 0; i<m_vBodies.size(); i++ )
		{
			dBodyDestroy( m_vBodies[i] );
		}
		m_vBodies.clear();
	}
	dWorldDestroy( m_oWorld );
}

void CODEManager::Update( float fTime )
{
}
