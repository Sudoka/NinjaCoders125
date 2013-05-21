/*
 * AudioEngine.cpp
 * Handles the audio playback
 */
#include "AudioEngine.h"
#include "ConfigurationManager.h"
#include <iostream>

//static members
AudioEngine *AudioEngine::ae;

AudioEngine::AudioEngine() {

	int initResult = startFMOD();
	//handle init success/failure
	switch(initResult) {
	case -1:
		DC::get()->print("[Audio] ERROR: Critical FMOD error. Exiting\n");
		exit(-1); //Hacky solution...probably should change this
		break;
	case 0:
		DC::get()->print("[Audio] WARNING: Init failed - no audio.\n");
		break;
	case 1:
		DC::get()->print("[Audio] Init successful\n");
	}	

	//test audio
	/*char* music = CM::get()->find_config("MUSIC");
	uint testid = addStream(music);
	playOneShot(testid,0.3f);*/

	/*char* link = CM::get()->find_config("LINK");
	uint jumpsound = addSound(link);
	playOneShot(testid);*/
}

AudioEngine::~AudioEngine() {

	for(map<uint, FMOD::Sound *>::iterator it = loadedSounds.begin();
		it != loadedSounds.end();
		++it) {
			it->second->release();
	}
	loadedSounds.clear();
	system->release();
}

/*
 * Creates, and initializes the fmod engine
 * Returns: -1 if encounterd a critical error
 *           0 for unsuccssful init and no audio
 *           1 for init successful
 */
int AudioEngine::startFMOD() {

	fmodErrThrown = false;
	bool computerHasAudio = true;

	DC::get()->print("[Audio] Initializing Audio Engine\n");
	//create our fmod system
	result = FMOD::System_Create(&system);
	FMOD_ERRCHECK(result);
	if(fmodErrThrown)
		return -1;

	result = system->getVersion(&version);
	FMOD_ERRCHECK(result);
	if(fmodErrThrown)
		return -1;
	

	DC::get()->print("[Audio] Using FMOD %d\n", version);

	//ensure the computer's fmod version is correct
	if(version < FMOD_VERSION)
	{
		DC::get()->print("[Audio] ERROR! You are using an old version of FMOD %d. This program requires %d.\n",version,FMOD_VERSION);
		return -1;
	}

	result = system->getNumDrivers(&numDrivers);
	FMOD_ERRCHECK(result);

	bool enableSound = CM::get()->find_config_as_bool("ENABLE_SOUND");
	if(numDrivers == 0 || !enableSound)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		DC::get()->print("[Audio] WARNING: No driver found. Sound not enabled\n.");
		computerHasAudio = false;
		FMOD_ERRCHECK(result);
		if(fmodErrThrown)
			return -1;
	}
	else
	{
		result = system->getDriverCaps(0, &caps, 0, &speakerMode);
		FMOD_ERRCHECK(result);
		if(fmodErrThrown)
			return -1;
	}

	//set the user selected speaker mode
	result = system->setSpeakerMode(speakerMode);
	FMOD_ERRCHECK(result);
	if(fmodErrThrown)
		return 0;

	DC::get()->print("[Audio] Checking for hardware emulation...\n");

	if(caps & FMOD_CAPS_HARDWARE_EMULATED)
	{
		//the user has the hardware acceleration set to off! this is really bad for latency
		result = system->setDSPBufferSize(1024, 10);
		DC::get()->print("[Audio] WARNING: Hardware acceleration off!\n");
		FMOD_ERRCHECK(result);
		if(fmodErrThrown)
			return 0;
	}

	//get the driver info
	result = system->getDriverInfo(0, driverName, 256, 0);
	FMOD_ERRCHECK(result);
	if(fmodErrThrown)
		return 0;

	DC::get()->print("[Audio] Driver: %s\n\n", driverName);

	if(strstr(driverName, "SigmaTel"))
	{
		//SigmaTel devices crackle if format is PCM 16bit
		//set to PCM floating point to fix
		result = system->setSoftwareFormat(48000,FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
		FMOD_ERRCHECK(result)
		if(fmodErrThrown)
			return 0;
	}

	//System::init(Max channels, init flags, extra driver data) only change max channels/flags
	result = system->init(100, FMOD_INIT_NORMAL | FMOD_INIT_VOL0_BECOMES_VIRTUAL, 0);
	if(result == FMOD_ERR_OUTPUT_CREATEBUFFER)
	{
		//selected speaker mode not supported by the soundcard, switch back to stereo and retry init
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		fmodErrThrown = false;
		result = system->init(100, FMOD_INIT_NORMAL, 0);
	}
	FMOD_ERRCHECK(result);
	if(fmodErrThrown)
		return 0;

	if(computerHasAudio)
		return 1;
	else
		return 0;
}

void AudioEngine::update() {
	system->update();
}

/*
 * If the sound is not currently in our soundbank, adds it and returns the soundId
 */
uint AudioEngine::addSound(char* filename) {
	uint id = getFileHash(filename);
	map<uint, FMOD::Sound *>::iterator res = loadedSounds.find(id);
	if(res != loadedSounds.end()) //sound already exists
		return id;

	//create a new sound
	bool created = loadSound(filename, id);
	if(created)
		return id;
	else
		return 0;
}

/*
 * If the sound is not currently in our soundbank, adds it and returns the soundId
 */
uint AudioEngine::addStream(char* filename) {
	uint id = getFileHash(filename);
	map<uint, FMOD::Sound *>::iterator res = loadedSounds.find(id);
	if(res != loadedSounds.end()) //sound already exists
		return id;

	//create a new sound
	bool created = loadStream(filename, id);
	if(created)
		return id;
	else
		return 0;
}

/*
 * Opens a given file as a stream and handles any fmod exceptions that occur
 */
bool AudioEngine::loadSound(char* filename, uint soundId) {
	FMOD::Sound *sound;
	result = system->createSound(filename, FMOD_DEFAULT, 0, &sound);
	FMOD_ERRCHECK(result);
	if(fmodErrThrown)
		return false;

	loadedSounds.insert(pair<uint,FMOD::Sound*>(soundId,sound));
	return true;
}

/*
 * Opens a given file as a stream and handles any fmod exceptions that occur
 */
bool AudioEngine::loadStream(char* filename, uint soundId) {
	FMOD::Sound *stream;
	result = system->createStream(filename, FMOD_DEFAULT, 0, &stream);
	FMOD_ERRCHECK(result);
	if(fmodErrThrown)
		return false;

	loadedSounds.insert(pair<uint,FMOD::Sound*>(soundId,stream));
	return true;
}

/*
 * Plays a one shot sample
 */
void AudioEngine::playOneShot(uint soundId) {
	
	map<uint,FMOD::Sound*>::iterator res = loadedSounds.find(soundId);
	if(res != loadedSounds.end())
	{
		FMOD::Channel *chan;
		FMOD::Sound *sound = res->second;
		result = sound->setMode(FMOD_LOOP_OFF); //one shot
		FMOD_ERRCHECK(result);
		result = system->playSound(FMOD_CHANNEL_FREE,sound,false,&chan);
		FMOD_ERRCHECK(result);
	}
}

/*
 * Plays a one shot sample with the option of a starting channel volume
 */
void AudioEngine::playOneShot(uint SoundId, float volume) {

	map<uint,FMOD::Sound*>::iterator res = loadedSounds.find(SoundId);
	if(res != loadedSounds.end())
	{
		FMOD::Channel *chan;
		FMOD::Sound *sound = res->second;
		result = sound->setMode(FMOD_LOOP_OFF); //one shot
		FMOD_ERRCHECK(result);
		result = system->playSound(FMOD_CHANNEL_FREE,sound,true,&chan);
		FMOD_ERRCHECK(result);
		result = chan->setVolume(volume);
		FMOD_ERRCHECK(result);
		result = chan->setPaused(false);
		FMOD_ERRCHECK(result);
	}
}

/*
 * Plays a sample as a loop (mostly for streams)
 */
void AudioEngine::playLoop(uint SoundId) {
	
	map<uint,FMOD::Sound*>::iterator res = loadedSounds.find(SoundId);
	if(res != loadedSounds.end())
	{
		FMOD::Channel *chan;
		FMOD::Sound *sound = res->second;
		result = sound->setMode(FMOD_LOOP_NORMAL); //loop
		FMOD_ERRCHECK(result);
		result = system->playSound(FMOD_CHANNEL_FREE,sound,true,&chan);
		FMOD_ERRCHECK(result);
		result = chan->setPaused(false);
		FMOD_ERRCHECK(result);
	}
}

/*
 * generates a simple hash so a sound can be searched by id as well as filename
 */
uint AudioEngine::getFileHash(char* filename)
{
	uint hash = 0;
	for(int i = 0; i < strlen(filename); i++)
		hash = 65599 * hash + filename[i];
	return hash ^ (hash >> 16);
}
