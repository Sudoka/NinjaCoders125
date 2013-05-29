/*
 * AudioEngine.h
 * Handles the audio playback
 */
#include <map>

#include "defs.h"
#include "DebugConsole.h"
#include "fmod.hpp"
#include "fmod_errors.h"

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
	uint addSound(char* filename);
	uint addStream(char* filename);

	void update();

	//playback
	void playOneShot(uint soundId);
	void playOneShot(uint soundId, float volume);
	void playLoop(uint soundId);

private:
	//Constructors/destructors are private
	AudioEngine();
	virtual ~AudioEngine();
	static AudioEngine *ae;	//instance

	//FMOD init
	int startFMOD();

	//file loading
	bool loadStream(char* filename, uint soundId);
	bool loadSound(char* filename, uint soundId);

	//hash table for our sounds and music streams
	map<uint, FMOD::Sound *> loadedSounds;
	map<uint, FMOD::Channel *> channels;

	//FMOD objects
	FMOD::System	 *system;
	FMOD_RESULT		 result;
	uint			 version;
	int				 numDrivers;
	FMOD_SPEAKERMODE speakerMode; //number of channels
	FMOD_CAPS	     caps;
	char			 driverName[256];
	bool			 fmodErrThrown; //used to check success without fatal exits/crashes
};
typedef AudioEngine AE;
#endif
