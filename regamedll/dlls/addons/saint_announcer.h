#pragma once

#define SAINT_ANNOUNCER_ALL_PLAYERS nullptr

class SaintAnnouncer
{
public:
	SaintAnnouncer();
	~SaintAnnouncer();

	void PlayerDied(CBaseEntity* victim, CBaseEntity* killer, const char* weapon, bool headshot);

private:
	void ClientCmd(const char* command, CBasePlayer* player = SAINT_ANNOUNCER_ALL_PLAYERS);
	void PlaySound(const char* snd, CBasePlayer* player = SAINT_ANNOUNCER_ALL_PLAYERS);
};

extern SaintAnnouncer *StAnnouncer;

void InstallSaintAnnouncer();
