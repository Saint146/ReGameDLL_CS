/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#ifndef SOUNDENT_H
#define SOUNDENT_H
#ifdef _WIN32
#pragma once
#endif

// Soundent.h - the entity that spawns when the world
// spawns, and handles the world's active and free sound
// lists.

#define MAX_WORLD_SOUNDS	64		// maximum number of sounds handled by the world at one time.

#define bits_SOUND_NONE		0
#define bits_SOUND_COMBAT	(1 << 0)	// gunshots, explosions
#define bits_SOUND_WORLD	(1 << 1)	// door opening/closing, glass breaking
#define bits_SOUND_PLAYER	(1 << 2)	// all noises generated by player. walking, shooting, falling, splashing
#define bits_SOUND_CARCASS	(1 << 3)	// dead body
#define bits_SOUND_MEAT		(1 << 4)	// gib or pork chop
#define bits_SOUND_DANGER	(1 << 5)	// pending danger. Grenade that is about to explode, explosive barrel that is damaged, falling crate
#define bits_SOUND_GARBAGE	(1 << 6)	// trash cans, banana peels, old fast food bags.

#define bits_ALL_SOUNDS		0xFFFFFFFF

#define SOUNDLIST_EMPTY		-1
#define SOUNDLISTTYPE_FREE	1		// identifiers passed to functions that can operate on either list, to indicate which list to operate on.
#define SOUNDLISTTYPE_ACTIVE	2
#define SOUND_NEVER_EXPIRE	-1		// with this set as a sound's ExpireTime, the sound will never expire.

// CSound - an instance of a sound in the world.
class CSound
{
public:
	void Clear();
	void Reset();

	BOOL FIsSound();
	BOOL FIsScent();

public:
	Vector m_vecOrigin;		// sound's location in space
	int m_iType;			// what type of sound this is
	int m_iVolume;			// how loud the sound is
	float m_flExpireTime;		// when the sound should be purged from the list
	int m_iNext;			// index of next sound in this list ( Active or Free )
	int m_iNextAudible;		// temporary link that monsters use to build a list of audible sounds
};

// CSoundEnt - a single instance of this entity spawns when
// the world spawns. The SoundEnt's job is to update the
// world's Free and Active sound lists.
class CSoundEnt : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual int ObjectCaps() { return FCAP_DONT_SAVE; }
	virtual void Think();

#ifdef HOOK_GAMEDLL

	void Spawn_();
	void Precache_();
	void Think_();

#endif

public:
	void Initialize();

	static void InsertSound(int iType, const Vector &vecOrigin, int iVolume, float flDuration);
	static void FreeSound(int iSound, int iPrevious);

	// return the head of the active list
	static int ActiveList();

	// return the head of the free list
	static int FreeList();

	// return a pointer for this index in the sound list
	static CSound *SoundPointerForIndex(int iIndex);
	static int ClientSoundIndex(edict_t *pClient);

	BOOL IsEmpty() const { return m_iActiveSound == SOUNDLIST_EMPTY; }

	int ISoundsInList(int iListType);
	int IAllocSound();

public:
	int m_iFreeSound;			// index of the first sound in the free sound list
	int m_iActiveSound;			// indes of the first sound in the active sound list
	int m_cLastActiveSounds;		// keeps track of the number of active sounds at the last update. (for diagnostic work)
	BOOL m_fShowReport;			// if true, dump information about free/active sounds.

private:
	CSound m_SoundPool[MAX_WORLD_SOUNDS];
};

extern CSoundEnt *pSoundEnt;

#endif // SOUNDENT_H
