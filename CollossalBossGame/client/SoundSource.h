/*
 * SoundSource.h
 * Creates a class for "sound emitters" that holds a list of all sounds available
 * and the object's position.
 */

#include <vector>
#include "defs.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "Frame.h"

#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H

class SoundSource {
public:
	SoundSource();
	~SoundSource();

	//adds an available sound to our object and returns its id
	uint addSound(char* filename);

	//play sound
	bool playOneShot(uint soundId);

private:
	//all sounds available for our object
	vector<uint> sounds;
};

#endif