/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_SERVER_GAMEMODES_MOD_H
#define GAME_SERVER_GAMEMODES_MOD_H
#include <game/server/gamecontroller.h>
#include <game/server/entities/flag.h>
#include <game/server/entity.h>
#include <src/base/vmath.h>
#include <game/server/gamecontext.h>

// you can subclass GAMECONTROLLER_CTF, GAMECONTROLLER_TDM etc if you want
// todo a modification with their base as well.
class CGameControllerInf : public IGameController
{
public:
	CGameControllerInf(class CGameContext *pGameServer);
	virtual bool OnEntity(const char* pName, vec2 Pivot, vec2 P0, vec2 P1, vec2 P2, vec2 P3, int PosEnv);
	virtual void Tick();
	void SpawnFlag(vec2 Pos);
	//void DoFlag();
	bool m_GameCanStart = false;
	int m_StartTick;
private:
	CFlag *m_apFlags[2];
	CFlag *m_pFlag;
	CEntity *m_pEnity;
	CPlayer *m_pPlayer;
};
#endif
