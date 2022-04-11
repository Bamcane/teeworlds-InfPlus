/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.				*/
#include <game/generated/protocol.h>
#include <game/server/gamecontext.h>
#include <game/server/gamecontroller.h>
#include <base/math.h>
#include "heroaura-pieces.h"
#include "heroaura.h"

CAuraPieces::CAuraPieces(CGameWorld *pGameWorld, vec2 Pos, float StartDegres, int Distance, int Type)
	: CEntity(pGameWorld, CGameWorld::ENTTYPE_PICKUP)
{
	m_Owner = -1;
	m_Pos = Pos;
	m_StartPos = Pos;
	m_Type = Type;
	m_Degres = StartDegres;
	m_Distance = Distance;
	m_StartDistance = Distance;
	m_OwnerCheck = false;

	m_ProximityRadius = 14;
	GameWorld()->InsertEntity(this);
}

void CAuraPieces::Tick()
{

	if(GameServer()->m_pController->IsGameOver())
	{
		GameWorld()->RemoveEntity(this);
		GameWorld()->DestroyEntity(this);
	}
	else
	{
		m_Pos = m_StartPos + (GetDir(m_Degres*pi/180) * m_Distance);
		if ( m_Degres + 5 < 360 )
			m_Degres += 5;
		else
			m_Degres = 0;

		if ( m_Distance > m_StartDistance * 3 )
			m_DistanceCheck = false;
		else if( m_Distance < m_StartDistance )
			m_DistanceCheck = true;

		if(m_OwnerCheck && GameServer()->m_apPlayers[m_Owner])
		{
			m_StartPos = GameServer()->GetPlayerChar(m_Owner)->m_Pos;

			if(!GameServer()->m_apPlayers[m_Owner]->IsHero())
			{
				GameServer()->m_apPlayers[m_Owner]->m_AuraPiecesNum--;
				GameWorld()->RemoveEntity(this);
				GameWorld()->DestroyEntity(this);
			}

			if(GameServer()->m_apPlayers[m_Owner]->m_AuraPiecesDel)
			{
				GameServer()->m_apPlayers[m_Owner]->m_AuraPiecesNum--;
				if(GameServer()->m_apPlayers[m_Owner]->m_AuraPiecesNum == 0)
				{
					GameServer()->m_apPlayers[m_Owner]->m_AuraPiecesDel = false;
					GameServer()->m_apPlayers[m_Owner]->m_AuraCheck[GameServer()->m_apPlayers[m_Owner]->m_AuraNum] = new CAura(&(GameServer()->m_World), m_Owner, 30, 60, POWERUP_ARMOR);
					GameServer()->m_apPlayers[m_Owner]->m_AuraNum += 1;

				}
				GameWorld()->RemoveEntity(this);
				GameWorld()->DestroyEntity(this);
			}
		}else if(m_OwnerCheck && !GameServer()->m_apPlayers[m_Owner])
		{
			GameWorld()->RemoveEntity(this);
			GameWorld()->DestroyEntity(this);
		}else
		{
			HitCharacter();
			if( m_DistanceCheck )
				m_Distance++;
			else
				m_Distance--;

		}
	}
}

void CAuraPieces::Snap(int SnappingClient)
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

void CAuraPieces::HitCharacter()
{
	CCharacter *apCloseCCharacters[MAX_CLIENTS];
	int Num = GameServer()->m_World.FindEntities(m_Pos, 8.0f, (CEntity**)apCloseCCharacters, MAX_CLIENTS, CGameWorld::ENTTYPE_CHARACTER);
	for(int i = 0; i < Num; i++)
	{
		if(apCloseCCharacters[i])
		{
			float Len = distance(apCloseCCharacters[i]->m_Pos, m_Pos);
			if(Len < apCloseCCharacters[i]->m_ProximityRadius+2.0f)
			{
				if(apCloseCCharacters[i]->GetPlayer()->IsHero())
				{
					apCloseCCharacters[i]->GetPlayer()->m_Score++;
					apCloseCCharacters[i]->GetPlayer()->m_AuraPiecesNum++;
					GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE);
					GameServer()->SendBroadcast("Hero found the aura pieces!", -1);
					m_Owner = apCloseCCharacters[i]->GetPlayer()->GetCID();
					m_OwnerCheck = true;
					m_Distance = m_StartDistance;
					if(apCloseCCharacters[i]->GetPlayer()->m_AuraPiecesNum >= 3)
					{
						GameServer()->SendChatTarget(-1, "Hero got the 'Hero Aura'!");
						GameServer()->m_apPlayers[m_Owner]->m_AuraNum++;
						GameServer()->m_apPlayers[m_Owner]->m_AuraPiecesDel = true;
					}
				}
				return;
			}
		}
	}

	
	return;
}