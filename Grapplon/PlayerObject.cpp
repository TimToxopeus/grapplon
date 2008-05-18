#include "PlayerObject.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "Hook.h"
#include "GameSettings.h"

#include "Renderer.h"
#include "LogManager.h"

#include "ODEManager.h"
#include "Vector.h"

#include "ParticleSystemManager.h"

#define SETS CGameSettings::Instance()

CPlayerObject::CPlayerObject( int iPlayer )
{
	m_eType = SHIP;
	m_iPlayer = iPlayer;
	m_pImage = new CAnimatedTexture("media/scripts/Octo.txt");
	m_pImage->SetFramerate( 10 );
	m_pImage->Scale( 0.9f );
	m_pRadius = new CAnimatedTexture("media/scripts/white_radius.txt");
	SetDepth( -1.0f );
	timeSinceNoInput = 5.0f;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, &m_oPhysicsData, 30.0f);
	SetMass( 1000.0f );
	m_oPhysicsData.m_bAffectedByGravity = true;
	m_oPhysicsData.m_fAirDragConst = 3000.0f;

	m_pHook = new CHook( this );
	
	m_bHandleWiiMoteEvents = true;
	y = p = r = 10.0f;

	m_pThrusterLeft = CParticleSystemManager::InstanceNear()->LoadEmitter( "media/scripts/thruster.txt" );
	m_pThrusterRight = CParticleSystemManager::InstanceNear()->LoadEmitter( "media/scripts/thruster.txt" );
}

CPlayerObject::~CPlayerObject()
{
	delete m_pRadius;
}


void CPlayerObject::SetPosition( float fX, float fY ){
	this->SetPosition( Vector(fX, fY, 0.0f) );
}

void CPlayerObject::SetPosition( Vector pos ){
	this->m_pHook->adjustPos( pos - this->GetPosition() );
	CBaseObject::SetPosition(pos);
}

bool CPlayerObject::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if (!m_bHandleWiiMoteEvents) return false;

	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_PLUS))
			wiiuse_motion_sensing(pWiimoteEvent, 1);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_MINUS))
			wiiuse_motion_sensing(pWiimoteEvent, 0);
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) || (SETS->PITCH_ACCEL_OUT != 0.0f && m_fPitchAccel > SETS->PITCH_ACCEL_OUT) )
		{
			if ( m_pHook->m_eHookState == CONNECTED )
			{
				m_pHook->m_eHookState = EJECTING;
			}
		}
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) || (SETS->PITCH_ACCEL_IN != 0.0f && m_fPitchAccel < SETS->PITCH_ACCEL_IN) )
		{
			if ( m_pHook->m_eHookState == HOMING )
				m_pHook->m_eHookState = RETRACTING;
			else if( m_pHook->m_eHookState == SWINGING)
				m_pHook->m_eHookState = THROWING;
		}

		if (WIIUSE_USING_ACC(pWiimoteEvent))
		{
			CalculateAccel( pWiimoteEvent );

			if( abs(m_fXAccel) > SETS->MIN_ACCEL_FOR_PROCESS || abs(m_fZAccel) > SETS->MIN_ACCEL_FOR_PROCESS )
			{
				m_pHook->AddChainForce(m_fXAccel, m_fZAccel);
			} else
			{
				m_pHook->AddChainForce(0, 0);
			}

			y = pWiimoteEvent->orient.yaw;
			p = pWiimoteEvent->orient.pitch;
			r = pWiimoteEvent->orient.roll;

		}

		if (pWiimoteEvent->exp.type == EXP_NUNCHUK)
		{
			if ( pWiimoteEvent->exp.nunchuk.js.mag > 0.05f )
			{
				float difference = -(m_fAngle - pWiimoteEvent->exp.nunchuk.js.ang);
				float direction = difference;
				if ( difference > 180.0f )
					direction = difference - 360.0f;
				else if ( difference < -180.0f )
					direction = difference + 360.0f;

				if ( difference < 0.0f ) difference = -difference;
				if ( difference < 3.0f )
					m_fAngle = pWiimoteEvent->exp.nunchuk.js.ang;
				else
					m_fAngle += (direction / 6.0f);
				m_fVelocityForward = 25000.0f * (sin(1.57f * pWiimoteEvent->exp.nunchuk.js.mag));

				timeSinceNoInput = 0.0f;

				Vector v = GetPosition();
				float angle = (pWiimoteEvent->exp.nunchuk.js.ang - 90.0f)*(3.14f/180.0f);
				v += Vector( cos(angle), sin(angle), 0.0f );
				v -= GetPosition();
				v.Normalize();
				SetForceFront( v * m_fVelocityForward * 100.0f );

				return true;
			}
			else{
				SetForceFront(Vector(0, 0, 0));
				m_fVelocityForward = 0.0f;
			}
		}
	}
	return false;
}

void CPlayerObject::Render()
{
	SDL_Rect target, size;
	size = m_pRadius->GetSize();
	target.w = (int)((float)size.w * GetScale());
	target.h = (int)((float)size.h * GetScale());
	target.x = (int)GetX() - (int)((float)target.w / 2.0f);
	target.y = (int)GetY() - (int)((float)target.h / 2.0f);

	SDL_Color colour;
	colour.r = colour.g = colour.b = 0;
	if ( m_iPlayer == 0 ) // red
	{
		colour.r = 255;
	}
	else if ( m_iPlayer == 1 ) // blue
	{
		colour.g = 255;
	}
	else if ( m_iPlayer == 2 ) // purple
	{
		colour.r = 255;
		colour.b = 255;
	}
	else // yellow
	{
		colour.r = 255;
		colour.g = 255;
	}

	RenderQuad( target, m_pRadius, m_fAngle, colour );

	CBaseMovableObject::Render();
}

void CPlayerObject::Update( float fTime )
{
	timeSinceNoInput += fTime;
	if ( timeSinceNoInput > 5.0f )
	{
		m_fAngle += 15.0f * fTime;
		m_fVelocityForward = 50.0f;
	}

	if ( m_fRespawnTime > 0.0f )
	{
		m_fRespawnTime -= fTime;

		if ( m_fRespawnTime < 1.0f )
		{
			if ( m_fRespawnTime + fTime > 1.0f )
			{
				Respawn();
				SetDepth( 1.0f );
			}

			SetScale( 1.0f + 3.0f * m_fRespawnTime );
			m_pHook->SetScale( 1.0f + 3.0f * m_fRespawnTime );
			SetAlpha( 1.0f - (1.0f * m_fRespawnTime) );
			m_pHook->SetAlpha( 1.0f - (1.0f * m_fRespawnTime) );
		}

		if ( m_fRespawnTime <= 0.0f )
		{
			m_bHandleWiiMoteEvents = true;
			SetDepth( -1.0f );
			m_fInvincibleTime = 2.0f;
			m_pHook->SetInvincibleTime( 2.0f );
			m_fAlpha = 1.0f;

			if ( m_pThrusterLeft )
				m_pThrusterLeft->ToggleSpawn();
			if ( m_pThrusterRight )
				m_pThrusterRight->ToggleSpawn();
		}
	}

	Vector backward = GetForwardVector();
	if ( m_pThrusterLeft )
		m_pThrusterLeft->SetDirection( backward );
	if ( m_pThrusterRight )
		m_pThrusterRight->SetDirection( backward );

	Vector right = backward.Rotate( 180.0f );
	if ( m_pThrusterLeft )
		m_pThrusterLeft->SetPosition( GetPosition() + backward * -50.0f + right * -20.0f );
	if ( m_pThrusterRight )
		m_pThrusterRight->SetPosition( GetPosition() + backward * -50.0f + right * 30.0f );

	//m_fAngle = GetPosition().CalculateAngle( GetPosition() + Vector(m_oPhysicsData.body->lvel) );

	CBaseMovableObject::Update( fTime );
}

void CPlayerObject::OnDie( CBaseObject *m_pKiller )
{

	Vector nullVec;
	m_pHook->HandlePlayerDied();
	m_bHandleWiiMoteEvents = false;
	SetForceFront(nullVec);
	SetLinVelocity(nullVec);
	SetAngVelocity(nullVec);

	
	m_fInvincibleTime = 4.0f;
	m_pHook->SetInvincibleTime( 4.0f );
	m_fRespawnTime = 2.0f;
	SetAlpha( 0.0f );
	m_pHook->SetAlpha( 0.0f );

	// Spawn emitter
	Vector direction = m_pKiller->GetPosition() - GetPosition();
	direction.Normalize();

	CParticleEmitter *pExplosion = CParticleSystemManager::InstanceNear()->LoadEmitter( "media/scripts/explosion_ship.txt" );
	if ( pExplosion )
	{
		if ( m_pThrusterLeft )
			m_pThrusterLeft->ToggleSpawn();
		if ( m_pThrusterRight )
			m_pThrusterRight->ToggleSpawn();
		pExplosion->SetPosition( GetPosition() );
		pExplosion->SetDirection( direction );
	}
}

void CPlayerObject::Respawn()
{
	int x, y;
	x = y = 0;

	CRenderer *pRenderer = CRenderer::Instance();
	do
	{
		x = rand()%2000 - 1000;
		y = rand()%1500 - 730;
	} while ( pRenderer->ObjectsInRange( x, y, 40 ) );

	Vector v = Vector( (float)x, (float)y, 0.0f );
	SetPosition( v );
	Vector n;
	m_oPhysicsData.m_pOwner->SetLinVelocity(n);
	SetForce(n);
	m_iHitpoints = 10000;
}
