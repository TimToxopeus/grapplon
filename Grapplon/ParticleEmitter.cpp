#include "ParticleEmitter.h"
#include "Renderer.h"

CParticleEmitter::CParticleEmitter( EmitterType eType, float fTypeParameter, unsigned int iMaxParticles, unsigned int iLifespan, unsigned int iSpawnrate, float fRadius )
{
	m_eType = eType;
	m_fTypeParameter = fTypeParameter;
	m_iMaxParticles = iMaxParticles;
	m_iLifespan = iLifespan;
	m_iSpawnrate = iSpawnrate;
	m_fRadius = fRadius;

	m_iCurParticles = 0;
	m_iAge = 0;
	m_fParticleSpawnTime = 0.0f;

	m_pFirst = NULL;
}

CParticleEmitter::~CParticleEmitter()
{
	while ( m_pFirst )
	{
		CParticle *pParticle = m_pFirst->m_pNext;
		delete m_pFirst;
		m_pFirst = pParticle;
	}

	for ( unsigned int i = 0; i<m_vParticleFactory.size(); i++ )
	{
		delete m_vParticleFactory[i].m_pParticle;
	}
	m_vParticleFactory.clear();
}

void CParticleEmitter::AddToFactory( CParticle *pParticle, unsigned int iChance )
{
	ParticleFactoryEntry e;
	e.m_pParticle = pParticle;
	e.m_iChance = iChance;
	m_vParticleFactory.push_back( e );
}

void CParticleEmitter::SpawnParticle()
{
	if ( m_iCurParticles >= m_iMaxParticles )
		return;

	int random = (int)(rand()%100 + 1);
	for ( unsigned int i = 0; i < m_vParticleFactory.size(); i++ )
	{
		random -= m_vParticleFactory[i].m_iChance;
		if ( random <= 0 )
		{
			CParticle *pNewParticle = m_vParticleFactory[i].m_pParticle->Clone();
			CParticle *pTemp = NULL;
			if ( m_pFirst )
			{
				pTemp = m_pFirst;
				while ( pTemp->m_pNext )
					pTemp = pTemp->m_pNext;
			}
			else
			{
				m_pFirst = pNewParticle;
			}

			if ( m_eType == LINE )
			{
				if ( m_fTypeParameter == 1 )
					pNewParticle->m_vDirection = m_vDirection;
				else
					pNewParticle->m_vDirection = (m_vDirection * -1.0f);
			}
			else
			{
				float angle = 0.0f;
				if ( m_eType == ARC )
					angle = ((float)(rand()%(int)m_fTypeParameter) + 1.0f) - (m_fTypeParameter / 2.0f);
				else
					angle = (float)(rand()%360);
				pNewParticle->m_vDirection = m_vDirection.Rotate( angle );
			}
			pNewParticle->m_vPosition = m_vPosition + (pNewParticle->m_vDirection * m_fRadius);

			if ( pTemp )
			{
				pNewParticle->m_pPrev = pTemp;
				pTemp->m_pNext = pNewParticle;
			}
			m_iCurParticles++;
			return;
		}
	}
}

void CParticleEmitter::Update(float fTime)
{
	m_fParticleSpawnTime += fTime;
	unsigned int millisecPassed = (unsigned int)(m_fParticleSpawnTime * 1000);
	unsigned int mod = millisecPassed % m_iSpawnrate;
	m_fParticleSpawnTime = (float)(millisecPassed - mod) / 1000.0f;

	unsigned int particles = millisecPassed / 10;
	if ( particles > 0 )
	{
		for ( unsigned int i = 0; i<particles; i++ )
			SpawnParticle();
	}

	CParticle *pTemp = m_pFirst;
	while ( pTemp )
	{
		pTemp->Update( fTime );
		if ( pTemp->m_iAge >= pTemp->m_iLifespan ) // Check if this particle has died of age
		{
			if ( pTemp->m_pPrev ) // Check if it has a previous node
			{
				if ( pTemp->m_pNext )
				{
					pTemp->m_pPrev->m_pNext = pTemp->m_pNext;
					pTemp->m_pNext->m_pPrev = pTemp->m_pPrev;
				}
				else
					pTemp->m_pPrev->m_pNext = NULL;
			}
			else // If it has no previous node this is the first node
			{
				if ( pTemp->m_pNext )
				{
					pTemp->m_pNext->m_pPrev = NULL;
					m_pFirst = pTemp->m_pNext;
				}
				else
					m_pFirst = NULL;
			}
			delete pTemp;
			m_iCurParticles--;
		}
		pTemp = pTemp->m_pNext;
	}

	m_iAge += (unsigned int)(fTime * 1000.0f);
}

void CParticleEmitter::Render()
{
	CRenderer *pRenderer = CRenderer::Instance();
	CParticle *pTemp = m_pFirst;
	while ( pTemp )
	{
		float alpha = 1.0f - ((float)pTemp->m_iAge / (float)pTemp->m_iLifespan);
		SDL_Rect target;
		target.w = target.h = 1;
		target.x = (Sint16)pTemp->m_vPosition[0];
		target.y = (Sint16)pTemp->m_vPosition[1];

		pRenderer->RenderQuad( target, pTemp->m_pSprite, 0, pTemp->GetColor(), alpha );

		pTemp = pTemp->m_pNext;
	}
}
