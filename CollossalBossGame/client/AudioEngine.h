/*
 * AudioEngine.h
 * Handles the audio playback
 */
#include <map>

#include "defs.h"
#include "DebugConsole.h"
#include "FMOD\inc\fmod.hpp"
#include "FMOD\inc\fmod_errors.h"
#include <time.h>

#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

//error check macro for FMOD init
#define FMOD_ERRCHECK(val_check) \
	if(val_check != FMOD_OK) { \
		DC::get()->print("[Audio] FMOD ERROR! (%d) %s\n", result, FMOD_ErrorString(val_check)); \
		fmodErrThrown = true; \
	}

class AudioEngine {
public:
	static void init() { ae = new AudioEngine(); }
	static AudioEngine *get() { return ae; }
	static void clean() { delete ae; }

	static uint getFileHash(char* filename);

	//add sound to our soundbank
	uint addSound(char* filename, bool is3D);
	uint addSound(char* filename, bool is3D, float attenuationDist); //3d with attenuation
	uint addStream(char* filename);

	void update();
	void setListenerPos(const Vec3f &listenerPos, const Vec3f &listenerForward, const Vec3f &listenerUp);

	//playback
	void playOneShot(uint soundId);
	void playOneShot(uint soundId, float volume);
	void playOneShot3D(uint soundId, float volume, Vec3f &pos);
	uint playLoop(uint soundId);
	uint playLoop3D(uint soundId, float volume, Vec3f &pos, uint loopstart, uint loopend); //returns a channel id

	//channel update methods
	void pauseChannel(uint channelId);
	void playChannel(uint channelId);
	void stopChannel(uint channelId);
	void setChannelVol(uint channelId, float vol);
	void setChannelPos(uint channelId, Vec3f &pos);

	//music and ambiance
	void playAmbiance();
	void stopAmbiance();
	void playMusic();
	void stopMusic();

private:
	//Constructors/destructors are private
	AudioEngine();
	virtual ~AudioEngine();
	static AudioEngine *ae;	//instance

	//FMOD init
	int startFMOD();

	//file loading
	bool loadStream(char* filename, uint soundId);
	bool loadSound(char* filename, uint soundId, bool is3D);
	bool loadSound(char* filename, uint soundId, bool is3D, float attenuationDist);

	//hash table for our sounds and music streams
	map<uint, FMOD::Sound *> loadedSounds;
	map<uint, FMOD::Channel *> channels;
	uint channelCount;

	//FMOD objects
	FMOD::System	 *system;
	FMOD_RESULT		 result;
	uint			 version;
	int				 numDrivers;
	FMOD_SPEAKERMODE speakerMode; //number of channels
	FMOD_CAPS	     caps;
	char			 driverName[256];
	bool			 fmodErrThrown; //used to check success without fatal exits/crashes
	float			 distFactor;

	//listener
	FMOD_VECTOR pos;
	FMOD_VECTOR vel;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;

	Vec3f lastPos;
	clock_t lastTime;
	clock_t newTime;

	//global music and ambiance
	//sort of hacky but I don't have time to implement a music player -Michael
	FMOD::Sound *musicLoop;
	FMOD::Sound *ambianceLoop;
	FMOD::Channel *musicChannel;
	FMOD::Channel *ambianceChannel;

	float musicVol;
	float ambianceVol;

};
typedef AudioEngine AE;
#endif
