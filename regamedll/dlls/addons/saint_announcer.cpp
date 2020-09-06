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

hudtextparms_t g_hudset;

float g_doubleKillTime;
CBasePlayer* g_doubleKillPlayer;
CBasePlayer* g_lastInTeam;

bool g_firstBlood;

char* g_knifeMsg[4] =
{
	"%s nogpe3AJI %s!!",
	"%s PE3AHyJI %s!!",
	"%s nogxaJI9BuJI %s c HO)I(A!!",
	"%s PA3DEJIAJI %s!!"
};

std::string g_headshotMsg[7] =
{
	"$kn nocmaBuJI $vn LLlMATky!",
	"$kn HABAJIuJI $vn\nB 6ALLlky c $wn",
	"$kn gaJI $vn B DblHl-O c $wn",
	"$vn CJIOBuJI )I(6AHky OT $kn",
	"$vn pacmekaemc9 MO3raMu no cmeHke!",
	"$kn nocmaBuJI JIEBLLly $vn c $wn",
	"$vn noJIy4uJI B 6ALLlHl-O om $kn"
};

#define g_lastInTeamMsgFirst "%s (c %.0f hp) npomuB %d %sOB c %.0f hp:\n"

char* g_lastInTeamMsg[4] =
{
	g_lastInTeamMsgFirst "Mbl BEPuM B TE69!",
	g_lastInTeamMsgFirst "Tbl CDEJIAELLlb ETO!",
	g_lastInTeamMsgFirst "Hy 4TO, ''KILL'' nponuCblBATb 6yDELLlb?",
	g_lastInTeamMsgFirst "ODHoro-mo nogxaJI9BuLLlb?!"
};

char* g_worldspawnMsg[4] =
{
	"%s PA36uJIC9!",
	"%s He PACC4uTAJI nPbl)I(OK...",
	"%s npunaJI K 3EMJIE!",
	"%s Heyga4HO npu3EMJIuJIC9..."
};

char* g_grenadeMsg[4] =
{
	"%s HE 3P9 kynuJI rpaHamy!\n%s emo noH9JI!",
	"%s PA36OM6uJI %s!",
	"%s B3OPBAJI %s!",
	"%s nogxaJI9BuJI %s c rpeHku!"
};

const int g_grenadeTeamMsgCount = 4;
char* g_grenadeTeamMsg[5] =
{
	"%s myno nogopBAJI %s!",
	"rpeHkou %s ye6ALLluJIO %s!",
	"%s kugaem rpeHku OT 6AJIDbl!\n%s emo noH9JI!",
	"%s 3APA6OTAJI ''-1'' HA %s!"
};

char* g_streakKillMsg[7] =
{
	"%s: Multi-Kill!",
	"%s: Ultra-Kill!!",
	"%s IS ON A KILLING SPREE!!!",
	"%s: RAMPAGE!!!",
	"%s IS UNSTOPPABLE!!!",
	"%s IS A MONSTER!",
	"%s IS GODLIKE!!!"
};

char* g_streakKillSounds[7] =
{
	"multikill",
	"ultrakill",
	"killingspree",
	"rampage",
	"unstoppable",
	"monsterkill",
	"godlike"
};

char* g_streakDeathMsg[7] = {
	"%s: Multi-Death...",
	"%s: Ultra-Death...",
	"%s IS ON A DYING SPREE...",
	"%s: LAMAGE...",
	"%s IS VERY STOPPABLE...",
	"%s IS A MOX...",
	"%s IS LOXLIKE..."
};

char* g_streakDeathSounds[7] = {
	"multideath",
	"ultradeath",
	"dyingspree",
	"lamage",
	"verystoppable",
	"mox",
	"loxlike"
};

int g_streakKills[MAX_CLIENTS + 1];
int g_streakDeaths[MAX_CLIENTS + 1];
#define DANGER_HEALTH 25
//bool g_dangerSound[MAX_CLIENTS + 1];

void SaintAnnouncer::ServerActivate()
{
	RestartRound();
}

void SaintAnnouncer::RestartRound()
{
	g_firstBlood = true;
	g_lastInTeam = nullptr;
	//for (int i = 0; i <= MAX_CLIENTS; i++)
	//	g_dangerSound[i] = true;
}

void SaintAnnouncer::PlayerPutInServer(CBasePlayer* player)
{
	int index = GetPlayerIndex(player);

	g_streakKills[index] = 0;
	g_streakDeaths[index] = 0;
}

void SaintAnnouncer::TeamScores(TeamName winnerTeam)
{
	if (g_lastInTeam && g_lastInTeam->m_iTeam == winnerTeam)
	{
		NotifyMessage(UTIL_VarArgs("%s BblTALLLuJI payHg!", STRING(g_lastInTeam->pev->netname)), HUD_PRINTTALK);

		for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
		{
			CBasePlayer* player = UTIL_PlayerByIndex(iIndex);

			if (player == NULL)
				continue;

			if (FNullEnt(player->pev))
				continue;

			if (FStrEq(STRING(player->pev->netname), ""))
				continue;

			if (player->m_iJoiningState != JOINED)
				continue;

			if (player->m_iTeam == winnerTeam)
				PlaySound("saint/cheer", player);
			else
				PlaySound("saint/fooo", player);
		}
	}
}

void SaintAnnouncer::PlayerDamaged(CBasePlayer* victim, CBasePlayer* attacker)
{
	if (victim->pev->health <= DANGER_HEALTH)
		PlaySound("saint/mk/danger", victim);
}

void SaintAnnouncer::PlayerDied(CBasePlayer* victim, CBaseEntity* killerEntity, const char* weapon, bool headshot, bool bombkill)
{
	//NotifyMessage(UTIL_VarArgs("%s killed %s with %s", STRING(killerEntity->pev->netname), STRING(victim->pev->netname), weapon), HUD_PRINTTALK);

	if (g_firstBlood)
	{
		g_firstBlood = false;
		PlaySound("saint/ut99/firstblood");
	}

	if (!bombkill)
	{
		if (Q_stricmp(weapon, "worldspawn") == 0)
			AnnounceWorldspawnKill(victim);

		if (killerEntity->IsPlayer())
		{
			CBasePlayer* killer = dynamic_cast<CBasePlayer*>(killerEntity);

			if (Q_stricmp(weapon, "knife") == 0)
				AnnounceKnifeKill(victim, killer);
			else if (Q_stricmp(weapon, "grenade") == 0)
				AnnounceGrenadeKill(victim, killer);

			if (headshot)
			{
				AnnounceHeadshot(victim, killer, weapon);
			}

			CheckDoubleKill(killer);

			if (killer->m_iTeam != victim->m_iTeam)
			{
				AddStreakKill(killer);
			}
		}

		// don't announce on bombkill because round is already over
		AnnouncePlayersAlive(victim);
	}
	AddStreakDeath(victim);
}

CountdownTimer g_bombTimer;

void SaintAnnouncer::BombPlanted()
{

	//CSGameRules()->m_iC4Timer
}

void SaintAnnouncer::BombDefused()
{

}

void SaintAnnouncer::BombTick(float bombTimeRemaining)
{
	//NotifyMessage(UTIL_VarArgs("%f", bombTimeRemaining), HUD_PRINTTALK);

	// rounding to int
	int bombTime = bombTimeRemaining + 0.5f;
	if ((bombTime == 30 || bombTime == 20 || bombTime <= 10) && bombTime > 0)
	{
		PlaySound(UTIL_VarArgs("saint/count_%d", bombTime));
	}
}

void find_and_replace(std::string& source, const char* find, const char* replace)
{
	int findlen = strlen(find);
	int replen = strlen(replace);

	for (size_t i = 0; (i = source.find(find, i)) != std::string::npos;)
	{
		source.replace(i, findlen, replace);
		i += replen;
	}
}

void SaintAnnouncer::AddStreakKill(CBasePlayer* player)
{
	int index = GetPlayerIndex(player);
	g_streakDeaths[index] = 0;

	int streakKillRate = ++g_streakKills[index] - 3;

	if ((streakKillRate > -1) && (streakKillRate % 2 == 0))
	{
		// announce 3, 5, 7, ... streak
		if ((streakKillRate >>= 1) > 6)
			streakKillRate = 6;

		SetHudMessage(0, 100, 255, 0.05, 0.55, 2, 0.02, 6.0, 0.01, 0.1, 3);
		ShowHudMessage(UTIL_VarArgs(g_streakKillMsg[streakKillRate], STRING(player->pev->netname)));

		PlaySound(UTIL_VarArgs("saint/ut99/%s", g_streakKillSounds[streakKillRate]));
	}
}

void SaintAnnouncer::AddStreakDeath(CBasePlayer* player)
{
	int index = GetPlayerIndex(player);
	g_streakKills[index] = 0;

	int streakDeathRate = ++g_streakDeaths[index] - 3;

	if ((streakDeathRate > -1) && (streakDeathRate % 2 == 0))
	{
		// announce 3, 5, 7, ... streak
		if ((streakDeathRate >>= 1) > 6)
			streakDeathRate = 6;

		SetHudMessage(100, 255, 155, 0.05, 0.5, 2, 0.02, 6.0, 0.01, 0.1, 4);
		ShowHudMessage(UTIL_VarArgs(g_streakDeathMsg[streakDeathRate], STRING(player->pev->netname)));

		PlaySound(UTIL_VarArgs("saint/%s", g_streakDeathSounds[streakDeathRate]));
	}
}

void SaintAnnouncer::AnnouncePlayersAlive(CBasePlayer* victim)
{
	int tCount = 0;
	int ctCount = 0;
	float tHP = 0;
	float ctHP = 0;
	CBasePlayer* ts[32];
	CBasePlayer* cts[32];

	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		CBasePlayer* player = UTIL_PlayerByIndex(iIndex);

		if (player == NULL)
			continue;

		if (FNullEnt(player->pev))
			continue;

		if (FStrEq(STRING(player->pev->netname), ""))
			continue;

		if (player->m_iJoiningState != JOINED)
			continue;

		if (!player->IsAlive())
			continue;

		switch (player->m_iTeam)
		{
		case CT:
			cts[ctCount++] = player;
			ctHP += player->pev->health;
			break;
		case TERRORIST:
			ts[tCount++] = player;
			tHP += player->pev->health;
			break;
		}
	}

	if (tCount == 1 && ctCount == 1)
	{
		// announce 1 on 1 situation
		char msg[200];
		sprintf(msg, "==> %s (c %.0f hp) npomuB %s (c %.0f hp) <==", STRING(cts[0]->pev->netname), cts[0]->pev->health, STRING(ts[0]->pev->netname), ts[0]->pev->health);

		SetHudMessage(0, 255, 255, -1.0, 0.35, 0, 6.0, 6.0, 0.5, 0.15, -1);
		ShowHudMessage(msg);
		PlaySound("saint/versus");
	}
	else if (!g_lastInTeam)
	{
		// announce the lone player in a team
		if (ctCount == 1 && tCount > 1)
			g_lastInTeam = cts[0];
		else if (tCount == 1 && ctCount > 1)
			g_lastInTeam = ts[0];

		if (g_lastInTeam)
		{
			char msg[300];

			if (g_lastInTeam->m_iTeam == CT)
				sprintf(msg, g_lastInTeamMsg[RANDOM_LONG(0, 3)], STRING(g_lastInTeam->pev->netname), g_lastInTeam->pev->health, tCount, "TEPPOP", tHP);
			else
				sprintf(msg, g_lastInTeamMsg[RANDOM_LONG(0, 3)], STRING(g_lastInTeam->pev->netname), g_lastInTeam->pev->health, ctCount, "KOHTP", ctHP);

			SetHudMessage(0, 255, 255, -1.0, 0.35, 0, 6.0, 6.0, 0.5, 0.15, -1);
			ShowHudMessage(msg);

			// announce for the "losing" team
			for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
			{
				CBasePlayer* player = UTIL_PlayerByIndex(iIndex);

				if (player == NULL)
					continue;

				if (FNullEnt(player->pev))
					continue;

				if (FStrEq(STRING(player->pev->netname), ""))
					continue;

				if (player->m_iJoiningState != JOINED)
					continue;

				if (player == g_lastInTeam)
					PlaySound("saint/takvsegda", player);
				else if (player->m_iTeam == g_lastInTeam->m_iTeam)
					PlaySound("saint/nuvotivse", player);
			}
		}
	}
}

void SaintAnnouncer::AnnounceHeadshot(CBasePlayer* victim, CBasePlayer* killer, const char* weapon)
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

	// formatting the message
	std::string msg = g_headshotMsg[RANDOM_LONG(0, 6)];
	find_and_replace(msg, "$vn", STRING(victim->pev->netname));
	find_and_replace(msg, "$wn", ConvertToSaintWeapons(weapon));
	find_and_replace(msg, "$kn", STRING(killer->pev->netname));

	SetHudMessage(100, 100, 255, -1.0, 0.29, 0, 6.0, 6.0, 0.5, 0.5, 1);
	ShowHudMessage(msg.c_str());
}

void SaintAnnouncer::AnnounceKnifeKill(CBasePlayer* victim, CBasePlayer* killer)
{
	char* msg = g_knifeMsg[RANDOM_LONG(0, 3)];

	SetHudMessage(255, 100, 100, -1.0, 0.25, 1, 6.0, 6.0, 0.5, 0.15, -1);
	ShowHudMessage(UTIL_VarArgs(msg, STRING(killer->pev->netname), STRING(victim->pev->netname)));

	PlaySound("saint/q3/humiliation");
}

void SaintAnnouncer::AnnounceWorldspawnKill(CBasePlayer* victim)
{
	char* msg = g_worldspawnMsg[RANDOM_LONG(0, 3)];

	SetHudMessage(255, 100, 100, -1.0, 0.25, 0, 6.0, 6.0, 0.5, 0.15, -1);
	ShowHudMessage(UTIL_VarArgs(msg, STRING(victim->pev->netname)));
}

void SaintAnnouncer::AnnounceGrenadeKill(CBasePlayer* victim, CBasePlayer* killer)
{
	char* msg;
	SetHudMessage(255, 100, 100, -1.0, 0.25, 1, 6.0, 6.0, 0.5, 0.15, -1);

	if (victim == killer)
		msg = "%s: SELF-KILL!!";
	else if (killer->m_iTeam != victim->m_iTeam)
		msg = g_grenadeMsg[RANDOM_LONG(0, 3)];
	else
		msg = g_grenadeTeamMsg[RANDOM_LONG(0, 3)];

	ShowHudMessage(UTIL_VarArgs(msg, STRING(killer->pev->netname), STRING(victim->pev->netname)));
}

void SaintAnnouncer::CheckDoubleKill(CBasePlayer* killer)
{
	float nowtime = gpGlobals->time;
	if ((g_doubleKillTime == nowtime) && (g_doubleKillPlayer == killer))
	{
		SetHudMessage(255, 0, 255, -1.0, 0.55, 1, 6.0, 6.0, 0.5, 0.5, 2);
		ShowHudMessage(UTIL_VarArgs("%s COTBOPuJI DOUBLE-KILL!!", STRING(killer->pev->netname)));

		PlaySound("saint/ut03/doublekill");
	}

	g_doubleKillPlayer = killer;
	g_doubleKillTime = nowtime;
}

const char* SaintAnnouncer::ConvertToSaintWeapons(const char* wpn)
{
	if (Q_strcmp(wpn, "ak47") == 0) return "KAJIALLlA";
	else if (Q_strcmp(wpn, "m4a1") == 0) return "M-ku";
	else if (Q_strcmp(wpn, "galil") == 0) return "raJIl-OXu";
	else if (Q_strcmp(wpn, "aug") == 0) return "LLlmeuepa";
	else if (Q_strcmp(wpn, "grenade") == 0) return "rpeHku";
	else if (Q_strcmp(wpn, "famas") == 0) return "FaMku";
	else if (Q_strcmp(wpn, "sg550") == 0) return "CKOPOCTPEJIku";
	else if (Q_strcmp(wpn, "usp") == 0) return "usp-xu";
	else if (Q_strcmp(wpn, "glock18") == 0) return "rJIoka";
	else if (Q_strcmp(wpn, "deagle") == 0) return "uroJIku";
	else if (Q_strcmp(wpn, "mp5navy") == 0) return "MP-xu";
	else if (Q_strcmp(wpn, "scout") == 0) return "nykaJIku";
	else if (Q_strcmp(wpn, "m249") == 0) return "MALLluHbl";
	else if (Q_strcmp(wpn, "g3sg1") == 0) return "BFG";
	else if (Q_strcmp(wpn, "p90") == 0) return "nemyxa";
	else if (Q_strcmp(wpn, "sg552") == 0) return "SG";
	else if (Q_strcmp(wpn, "ump45") == 0) return "UMP";
	else if (Q_strcmp(wpn, "p228") == 0) return "DEPbMA";
	else if ((Q_strcmp(wpn, "m3") == 0) || (Q_strcmp(wpn, "xm1014") == 0)) return "DPO6ALLlA";
	else if (Q_strcmp(wpn, "mac10") == 0) return "uHrpeM4uKA";
	else if (Q_strcmp(wpn, "elite") == 0) return "6EPETT";
	else if (Q_strcmp(wpn, "fiveseven") == 0) return "fiveseven'a";
	else if (Q_strcmp(wpn, "tmp") == 0) return "TMP-LLlHuKA";
	else if (Q_strcmp(wpn, "knife") == 0) return "HO)I(A";

	return wpn;
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

void SaintAnnouncer::NotifyMessage(char* msg, int msg_dest, CBasePlayer* player)
{
	msg = UTIL_VarArgs("%s\n", msg);

	char c = msg[190];
	msg[190] = 0;

	if (player)
		MESSAGE_BEGIN(MSG_ONE, gmsgTextMsg, NULL, player->pev);
	else
		MESSAGE_BEGIN(MSG_BROADCAST, gmsgTextMsg);

	WRITE_BYTE(msg_dest);
	WRITE_STRING(msg);
	MESSAGE_END();
	msg[190] = c;
}

void SaintAnnouncer::SetHudMessage(byte r, byte g, byte b, float x, float y, int effect, float fxTime, float holdTime, float fadeinTime, float fadeoutTime, int channel)
{
	g_hudset.a1 = 0;
	g_hudset.a2 = 0;
	g_hudset.r2 = 255;
	g_hudset.g2 = 255;
	g_hudset.b2 = 250;
	g_hudset.r1 = r;
	g_hudset.g1 = g;
	g_hudset.b1 = b;
	g_hudset.x = x;
	g_hudset.y = y;
	g_hudset.effect = effect;
	g_hudset.fxTime = fxTime;
	g_hudset.holdTime = holdTime;
	g_hudset.fadeinTime = fadeinTime;
	g_hudset.fadeoutTime = fadeoutTime;
	g_hudset.channel = channel;
}

void SaintAnnouncer::ShowHudMessage(const char* msg)
{
	UTIL_HudMessageAll(g_hudset, msg);
}

void SaintAnnouncer::PlaySound(const char* snd, CBasePlayer* player)
{
	char cmd[255];
	strcpy(cmd, "spk ");
	strcat(cmd, snd);
	ClientCmd(cmd, player);
}

int SaintAnnouncer::GetPlayerIndex(CBasePlayer* player)
{
	for (int iIndex = 1; iIndex <= gpGlobals->maxClients; ++iIndex)
	{
		if (player == UTIL_PlayerByIndex(iIndex))
			return iIndex;
	}
	return 0;
}



void InstallSaintAnnouncer()
{
	if (StAnnouncer != NULL)
		delete StAnnouncer;

	StAnnouncer = new SaintAnnouncer();
}
