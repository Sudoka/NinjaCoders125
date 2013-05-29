/*
 * SoundSource.h
 * Creates a class for "sound emitters" that holds a list of all sounds available
 * and the object's position.
 */

#include "SoundSource.h"
#include "AudioEngine.h"


SoundSource::SoundSource() {

}

SoundSource::~SoundSource() {
	sounds.clear();
}

/*
 * Adds a sound available to the object
 */
uint SoundSource::addSound(char* filename) {
	uint soundId = AE::get()->addSound(filename);
	sounds.push_back(soundId);
	return soundId;
}

/*
 * Plays a sound if the object can play its sound id
 */
bool SoundSource::playOneShot(uint soundId) {
	for(vector<uint>::iterator it = sounds.begin();
		it != sounds.end();
		it++) {
			if(*it == soundId)
			{
				AE::get()->playOneShot(soundId);
				return true;
			}
	}
	return false;
}
