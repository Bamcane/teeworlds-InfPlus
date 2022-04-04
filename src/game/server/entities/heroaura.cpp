/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.				*/
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/gamecontroller.h>
#include <base/math.h>
#include "heroaura.h"

CAura::CAura(CGameWorld *pGameWorld, int Owner, float StartDegres, int Distance, int Type)
	: CEntity(pGameWorld, CGameWorld::ENTTYPE_PICKUP)
{
	m_Owner = Owner;
	m_Type = Type;
	m_Degres = StartDegres;
	m_Distance = Distance;

	m_ProximityRadius = 14;
	GameWorld()->InsertEntity(this);
	GameServer()->SendBroadcast("You got a 'Hero Aura'", m_Owner);
}

void CAura::Tick()
{

	if(GameServer()->m_pController->IsGameOver())
	{
		GameWorld()->RemoveEntity(this);
		GameWorld()->DestroyEntity(this);
	}else if(GameServer()->m_apPlayers[m_Owner] && !GameServer()->m_apPlayers[m_Owner]->IsHero())
	{
		GameWorld()->RemoveEntity(this);
		GameWorld()->DestroyEntity(this);
	}
	else
	{
		m_Pos = GameServer()->GetPlayerChar(m_Owner)->m_Pos + (GetDir(m_Degres*pi/180) * m_Distance);
		if ( m_Degres + 5 < 360 )
			m_Degres += 5;
		else
			m_Degres = 0;

	}
}

void CAura::Snap(int SnappingClient)
{
	if(NetworkClipped(SnappingClient))
		return;

	CNetObj_Pickup *pP = static_cast<CNetObj_Pickup *>(Server()->SnapNewItem(NETOBJTYPE_PICKUP, m_ID, sizeof(CNetObj_Pickup)));
	if(!pP)
		return;

	pP->m_X = (int)m_Pos.x;
	pP->m_Y = (int)m_Pos.y;

	pP->m_Type = m_Type;
	pP->m_Subtype = 0;
}