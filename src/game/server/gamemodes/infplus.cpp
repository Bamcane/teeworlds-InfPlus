/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>
#include <game/server/entities/heroaura-pieces.h>
#include <game/server/entities/heroaura.h>
#include "infplus.h"

CGameControllerInf::CGameControllerInf(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	// Exchange this to a string that identifies your game mode.
	// DM, TDM and CTF are reserved for teeworlds original modes.
	m_pGameType = "InfPlus ";
	m_StartTick = 0;
	//m_GameFlags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode
}

void CGameControllerInf::Tick()
{
	m_StartTick++;
	// num player
	int Humans = 0, Zombies = 0, Heroes = 0;
	for(int i = 0;i < MAX_CLIENTS;i++)
	{
		CPlayer *pPlayer = GameServer()->m_apPlayers[i];
		if(!pPlayer)
			continue;
		if(pPlayer->GetTeam() == TEAM_SPECTATORS)
			continue;
		if(pPlayer->IsZombie())
		 	Zombies++;
		else 
			Humans++;
		if(pPlayer->IsHero())
			Heroes++;
	}

	// Do win check

	if(m_GameOverTick == -1 && !m_Warmup) {
        if (!Humans) {
            EndRound();
			GameServer()->SendChatTarget(-1,"☢| Zombies infected all humans!");
			GameServer()->SendChatTarget(-1,"!| All Zombies +5 score!");
			GiveGifts(true,5);
			GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE,-1);
			m_StartTick = 0;
            return;
        }else if(!Zombies)
		{
			EndRound();
			GameServer()->SendChatTarget(-1,"⊕| Humans cured all zombies!");
			GameServer()->SendChatTarget(-1,"!| All Humans +6 score!");
			GiveGifts(false,6);
			GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE,-1);
			m_StartTick = 0;
            return;
		}
    }
	DoWincheck();

	// If player < 2
	if(Humans + Zombies < 2)
	{
		DoWarmup(g_Config.m_InfInfectDelay+1);
		m_GameCanStart = true;
		if(m_StartTick % 100 == 0)
		{
			GameServer()->SendBroadcast("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nNeed 2 players to start game",-1);
		}
	}else if(m_GameCanStart)
	{
		StartRound();
		// clear Broadcast
		GameServer()->SendBroadcast("",-1);

		m_GameCanStart = false;
	}else if(!m_Warmup && Heroes == 0 && m_GameOverTick == -1)
	{
		PickHero();
	}else if(!m_Warmup && m_GameOverTick == -1 && Server()->Tick() % 1000 == 0)
	{
		new CAuraPieces(&(GameServer()->m_World), m_aaFlagPoints[1][rand()%m_aNumFlagPoints[1]], 30, 60, POWERUP_HEALTH);
	}
	

	IGameController::Tick();

}

bool CGameControllerInf::OnEntity(const char* pName, vec2 Pivot, vec2 P0, vec2 P1, vec2 P2, vec2 P3, int PosEnv)
{
	vec2 Pos = (P0 + P1 + P2 + P3)/4.0f;
	if(IGameController::OnEntity(pName, Pivot, P0, P1, P2, P3, PosEnv))
		return true;

	int Team = -1;
	if(str_comp(pName, "twFlagStandHero") == 0) Team = TEAM_BLUE;
	if(Team != TEAM_BLUE)
		return false;

	SpawnFlag(Pos);
	m_aaFlagPoints[1][m_aNumFlagPoints[1]++] = Pos;
}

void CGameControllerInf::SpawnFlag(vec2 Pos)
{
	CFlag *F = new CFlag(&GameServer()->m_World, 1, Pos);
	F->m_StandPos = Pos;
	F->m_Pos = Pos;
	GameServer()->m_World.InsertEntity(F);
}