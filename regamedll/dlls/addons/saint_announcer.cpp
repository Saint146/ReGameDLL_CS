#include "precompiled.h"

#ifndef HOOK_GAMEDLL

SaintAnnouncer* StAnnouncer = NULL;

#endif

SaintAnnouncer::SaintAnnouncer()
{
}

SaintAnnouncer::~SaintAnnouncer()
{
}

void SaintAnnouncer::PlayerDied(CBaseEntity* victimEntity, CBaseEntity* killerEntity, const char* weapon, bool headshot)
{
	CBasePlayer* victim = static_cast<CBasePlayer*>(victimEntity);
	CBasePlayer* killer = static_cast<CBasePlayer*>(killerEntity);

	// headshot sound
	if (headshot)
	{
		if (Q_strcmp(weapon, "awp") == 0)
		{
			PlaySound("saint/ut03/headshot", victim);
			PlaySound("saint/ut03/headshot", killer);
		}
		else if (Q_strcmp(weapon, "scout") == 0)
		{
			PlaySound("saint/headshot_scout");
		}
		else if (Q_strcmp(weapon, "glock18") == 0)
		{
			PlaySound("saint/headshot_glock", victim);
			PlaySound("saint/ut99/headshot", killer);
		}
		else if (Q_strcmp(weapon, "elite") == 0)
		{
			PlaySound("saint/headshot_elite");
		}
		else if (Q_strcmp(weapon, "tmp") == 0)
		{
			PlaySound("saint/wahwah", victim);
			PlaySound("saint/wahwah", killer);
		}
		else if (Q_strcmp(weapon, "mac10") == 0)
		{
			PlaySound("saint/headshot_ingram");
		}
		else
		{
			PlaySound("saint/ut99/headshot", victim);
			PlaySound("saint/ut99/headshot", killer);
		}
	}

	//CLIENT_PRINTF(victim->pev->pContainingEntity, print_console, UTIL_VarArgs("You have been killed by %1s and his %2s\n", STRING(killer->pev->netname), weapon));

	victim = nullptr;
	killer = nullptr;
}

void SaintAnnouncer::ClientCmd(const char* command, CBasePlayer* player)
{
	if (player)
	{
		CLIENT_COMMAND(ENT(player->pev), UTIL_VarArgs("%s\n", command));
	}
	else
	{
		for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
		{
			CBasePlayer* player = UTIL_PlayerByIndex(iIndex);

			if (player == NULL)
				continue;

			if (FNullEnt(player->pev))
				continue;

			if (FStrEq(STRING(player->pev->netname), ""))
				continue;

			if (player->IsBot())
				continue;

			ClientCmd(command, player);
		}
	}
}

void SaintAnnouncer::PlaySound(const char* snd, CBasePlayer* player)
{
	ClientCmd(UTIL_VarArgs("spk %s", snd), player);
}

void InstallSaintAnnouncer()
{
	if (StAnnouncer != NULL)
		delete StAnnouncer;

	StAnnouncer = new SaintAnnouncer();
}
