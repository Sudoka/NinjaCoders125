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

uint SoundSource::addSound(char* filename, bool is3D, float attenDist) {
	uint soundId = AE::get()->addSound(filename,is3D,attenDist);
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
				AE::get()->playOneShot3D(soundId,volume,pos);
				return true;
			}
	}
	return false;
}

/*
 * Plays a looped sound and returns a reference to the channel for future modification
 * loop start and end are in miliseconds
 */
uint SoundSource::playLoop3D(uint soundId, float volume, Vec3f &pos, uint loopstart, uint loopend) {
	uint channelId;

	for(vector<uint>::iterator it = sounds.begin();
		it != sounds.end();
		it++) {
			if(*it == soundId)
			{
				channelId = AE::get()->playLoop3D(soundId,volume,pos,loopstart,loopend);
				return channelId;
			}
	}
	return 0;
}

/*
 * Moves the sound to the given location
 */
bool SoundSource::updateSoundPos(uint channelId, Vec3f &newPos) {
	AE::get()->setChannelPos(channelId, newPos);
	return true;
}

/*
 * Stops the loop and frees the channel to be used again
 */
bool SoundSource::stopLoop(uint channelId) {
	AE::get()->stopChannel(channelId);
	return true;
}