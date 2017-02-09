#pragma once

#define STATS_ALL_PLAYERS nullptr

class Stats
{
public:
	Stats();
	~Stats();

	void ServerActivate();
	void RestartRound();
	void PlayerPutInServer(CBasePlayer * player);
	void TeamScores(TeamName winnerTeam);
	void PlayerDamaged(CBasePlayer *victim, CBasePlayer *attacker);
	void PlayerDied(CBasePlayer *victim, CBaseEntity *killerEntity,const char *weapon, bool headshot, bool bombkill);
	void BombPlanted();
	void BombDefused();
	void BombTick(float bombTimeRemaining);

#ifdef _DEBUG
	void Test();
#endif
	
private:
	void ClientCmd(const char *command, CBasePlayer *player = STATS_ALL_PLAYERS);
	void PlaySound(const char *snd, CBasePlayer *player = STATS_ALL_PLAYERS);
	int GetPlayerIndex(CBasePlayer * player);
	void NotifyMessage(char *msg, int msg_dest, CBasePlayer *player = STATS_ALL_PLAYERS);
	void SetHudMessage(byte r, byte g, byte b, float x, float y, int effect, float fxTime, float holdTime, float fadeinTime, float fadeoutTime, int channel);
	void ShowHudMessage(const char *msg);
	void AddStreakKill(CBasePlayer * player);
	void AddStreakDeath(CBasePlayer * player);
	void AnnouncePlayersAlive(CBasePlayer * victim);
	void AnnounceHeadshot(CBasePlayer *victim, CBasePlayer *killer, const char *weapon);
	void AnnounceKnifeKill(CBasePlayer * victim, CBasePlayer * killer);
	void AnnounceWorldspawnKill(CBasePlayer * victim);
	void AnnounceGrenadeKill(CBasePlayer * victim, CBasePlayer * killer);
	void CheckDoubleKill(CBasePlayer *killer);
	const char *ConvertToSaintWeapons(const char *wpn);
};

extern Stats *StatsManager;

void InstallStatsManager();
