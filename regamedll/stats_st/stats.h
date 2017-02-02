#pragma once

#define STATS_ALL_PLAYERS nullptr

class Stats
{
public:
	Stats();
	~Stats();

	void PlayerDied(CBaseEntity *victim, CBaseEntity *killer,const char *weapon, bool headshot);

private:
	void ClientCmd(const char *command, CBasePlayer *player = STATS_ALL_PLAYERS);
	void PlaySound(const char *snd, CBasePlayer *player = STATS_ALL_PLAYERS);
};

extern Stats *StatsManager;

void InstallStatsManager();
