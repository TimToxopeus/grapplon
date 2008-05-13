#include "ParticleSystemManager.h"

CParticleSystemManager *CParticleSystemManager::m_pInstance = 0;

CParticleSystemManager::CParticleSystemManager()
{
}

CParticleSystemManager::~CParticleSystemManager()
{
}

void CParticleSystemManager::Update(float fTime)
{
	std::vector<CParticleEmitter *> vDelete;

	for ( unsigned int i = 0; i<m_vEmitters.size(); i++ )
	{
		m_vEmitters[i]->Update( fTime );
		if ( !m_vEmitters[i]->IsAlive() )
		{
			vDelete.push_back( m_vEmitters[i] );
			m_vEmitters.erase( m_vEmitters.begin() + i );
		}
	}

	for ( unsigned int i = 0; i<vDelete.size(); i++ )
	{
		delete vDelete[i];
	}
	vDelete.clear();
}

void CParticleSystemManager::Render()
{
	for ( unsigned int i = 0; i<m_vEmitters.size(); i++ )
	{
		m_vEmitters[i]->Render();
	}
}
