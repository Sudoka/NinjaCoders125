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
uint SoundSource::addSound(char* filename, bool is3D) {
	uint soundId = AE::get()->addSound(filename,is3D);
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

/*
 * Plays a sound if the object can play its sound id
 */
bool SoundSource::playOneShot3D(uint soundId, float volume, Vec3f &pos) {
	for(vector<uint>::iterator it = sounds.begin();
		it != sounds.end();
		it++) {
			if(*it == soundId)
			{
				//AE::get()->playOneShot(soundId);
				DC::get()->print("[Audio] Sound playing at: (%f,%f,%f)\n",pos.x,pos.y,pos.z);
				AE::get()->playOneShot3D(soundId,volume,pos);
				return true;
			}
	}
	return false;
}
