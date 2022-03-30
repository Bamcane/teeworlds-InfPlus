/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>
#include "infplus.h"

CGameControllerInf::CGameControllerInf(class CGameContext *pGameServer)
: IGameController(pGameServer)
{
	// Exchange this to a string that identifies your game mode.
	// DM, TDM and CTF are reserved for teeworlds original modes.
	m_pGameType = "InfPlus ";

	//m_GameFlags = GAMEFLAG_TEAMS; // GAMEFLAG_TEAMS makes it a two-team gamemode
}

void CGameControllerInf::Tick()
{
	// num player
	int Humans = 0,Zombies = 0;
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
	}

	// Do win check

	if(m_GameOverTick == -1 && !m_Warmup) {
        if (!Humans) {
            EndRound();
			GameServer()->SendChatTarget(-1,"☢| Zombies infected all humans!");
			GameServer()->SendChatTarget(-1,"!| All Zombies +5 score!");
			GiveGifts(true,5);
			GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE,-1);
            return;
        }else if(!Zombies)
		{
			EndRound();
			GameServer()->SendChatTarget(-1,"⊕| Humans cured all zombies!");
			GameServer()->SendChatTarget(-1,"!| All Humans +6 score!");
			GiveGifts(false,6);
			GameServer()->CreateSoundGlobal(SOUND_CTF_CAPTURE,-1);
            return;
		}
    }
	DoWincheck();

	// If player < 2
	if(Humans + Zombies < 2)
	{
		DoWarmup(g_Config.m_InfInfectDelay+1);
		m_GameCanStart = true;
		if(Server()->Tick() % 100 == 0)
		{
			GameServer()->SendBroadcast("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nNeed 2 players to start game",-1);
		}
	}else if(m_GameCanStart)
	{
		StartRound();
		// clear Broadcast
		GameServer()->SendBroadcast("",-1);

		m_GameCanStart = false;
	}
	IGameController::Tick();

	

}