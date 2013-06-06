/*
 * SoundSource.h
 * Creates a class for "sound emitters" that holds a list of all sounds available
 * and the object's position.
 */

#include <vector>
#include "defs.h"
#include "FMOD\inc\fmod.hpp"
#include "FMOD\inc\fmod_errors.h"
#include "Frame.h"

#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H

class SoundSource {
public:
	SoundSource();
	~SoundSource();

	//adds an available sound to our object and returns its id
	uint addSound(char* filename, bool is3D);
	uint addSound(char* filename, bool is3D, float attenDist);

	//play sound
	bool playOneShot(uint soundId);
	bool playOneShot3D(uint soundId, float volume, Vec3f &pos);
	uint playLoop3D(uint soundId, float volume, Vec3f &pos,	uint loopstart, uint loopend); //time in ms

	//loop controls
	bool updateSoundPos(uint channelId, Vec3f &newPos);
	bool stopLoop(uint channelId);


private:
	//all sounds available for our object
	vector<uint> sounds;
};

#endif