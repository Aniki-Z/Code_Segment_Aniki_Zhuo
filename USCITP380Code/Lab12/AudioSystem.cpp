#include "AudioSystem.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <filesystem>
#include "Math.h"
#include "Actor.h"
#include "Player.h"

SoundHandle SoundHandle::Invalid;

AudioSystem::AudioSystem(Game* game, int numChannels)
: mGame(game)
{
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		return;
	}

	Mix_AllocateChannels(numChannels);

	mChannels.resize(numChannels);

	for (size_t i = 0; i < mChannels.size(); i++)
	{
		mChannels[i].Reset();
	}
}

AudioSystem::~AudioSystem()
{
	for (std::unordered_map<std::string, Mix_Chunk*>::iterator it = mSounds.begin();
		 it != mSounds.end(); ++it)
	{
		Mix_FreeChunk(it->second);
	}

	mSounds.clear();

	Mix_CloseAudio();
}

void AudioSystem::Update(float deltaTime)
{
	for (size_t i = 0; i < mChannels.size(); i++)
	{
		if (mChannels[i].IsValid())
		{
			SoundHandle handle = mChannels[i];

			if (!Mix_Playing(static_cast<int>(i)))
			{
				mChannels[i].Reset();

				auto handleIt = mHandleMap.find(handle);
				if (handleIt != mHandleMap.end())
				{
					Actor* actor = handleIt->second.mActor;
					if (actor)
					{
						auto actorIt = mActorMap.find(actor);
						if (actorIt != mActorMap.end())
						{
							actorIt->second.erase(handle);
							if (actorIt->second.empty())
							{
								mActorMap.erase(actorIt);
							}
						}
					}

					mHandleMap.erase(handleIt);
				}
			}
			else
			{
				auto handleIt = mHandleMap.find(handle);
				if (handleIt != mHandleMap.end())
				{
					Actor* actor = handleIt->second.mActor;
					Player* player = mGame->GetPlayer();
					if (actor && player)
					{
						int volume = CalculateVolume(actor, player);
						Mix_Volume(i, volume);
					}
				}
			}
		}
	}
}

// Plays the sound with the specified name and loops if looping is true
// Returns the SoundHandle which is used to perform any other actions on the
// sound when active
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
SoundHandle AudioSystem::PlaySound(const std::string& soundName, bool looping, Actor* actor,
								   bool stopOnActorRemove, int fadeTimeMS)
{
	Mix_Chunk* chunk = GetSound(soundName);

	if (!chunk)
	{
		SDL_Log("[AudioSystem] PlaySound couldn't find sound for %s", soundName.c_str());
		return SoundHandle::Invalid;
	}

	int availableChannel = -1;
	for (int i = 0; i < mChannels.size(); i++)
	{
		if (!mChannels[i].IsValid())
		{
			availableChannel = i;
			break;
		}
	}

	if (availableChannel == -1)
	{
		for (auto& handleEntry : mHandleMap)
		{
			if (handleEntry.second.mSoundName == soundName)
			{
				availableChannel = handleEntry.second.mChannel;
				SDL_Log("[AudioSystem] PlaySound ran out of channels playing %s! Stopping %s",
						soundName.c_str(), handleEntry.second.mSoundName.c_str());
				StopSound(handleEntry.first);
				break;
			}
		}

		if (availableChannel == -1)
		{
			for (auto& handleEntry : mHandleMap)
			{
				if (!handleEntry.second.mIsLooping)
				{
					availableChannel = handleEntry.second.mChannel;
					SDL_Log("[AudioSystem] PlaySound ran out of channels playing %s! Stopping %s",
							soundName.c_str(), handleEntry.second.mSoundName.c_str());
					StopSound(handleEntry.first);
					break;
				}
			}
		}

		if (availableChannel == -1)
		{
			auto oldestHandle = mHandleMap.begin();
			availableChannel = oldestHandle->second.mChannel;
			SDL_Log("[AudioSystem] PlaySound ran out of channels playing %s! Stopping %s",
					soundName.c_str(), oldestHandle->second.mSoundName.c_str());
			StopSound(oldestHandle->first);
		}
	}

	SoundHandle newHandle = ++mLastHandle;

	HandleInfo newHandleInfo;
	newHandleInfo.mSoundName = soundName;
	newHandleInfo.mIsLooping = looping;
	newHandleInfo.mIsPaused = false;
	newHandleInfo.mChannel = availableChannel;

	newHandleInfo.mActor = actor;
	newHandleInfo.mStopOnActorRemove = stopOnActorRemove;

	mHandleMap[newHandle] = newHandleInfo;

	if (actor)
	{
		mActorMap[actor].insert(newHandle);
	}

	mChannels[availableChannel] = newHandle;

	bool isPlaySuccess = false;

	if (fadeTimeMS > 0)
	{
		isPlaySuccess = Mix_FadeInChannel(availableChannel, chunk, looping ? -1 : 0, fadeTimeMS) !=
						-1;
	}
	else
	{
		isPlaySuccess = Mix_PlayChannel(availableChannel, chunk, looping ? -1 : 0) != -1;
	}

	if (!isPlaySuccess)
	{
		SDL_Log("[AudioSystem] Failed to play sound %s", soundName.c_str());
		return SoundHandle::Invalid;
	}

	Player* player = mGame->GetPlayer();
	if (actor && player)
	{
		int volume = CalculateVolume(actor, player);
		Mix_Volume(availableChannel, volume);
	}

	return newHandle;
}

void AudioSystem::StopSound(SoundHandle sound, int fadeTimeMS)
{
	if (!sound.IsValid())
	{
		return;
	}

	std::map<SoundHandle, HandleInfo>::iterator it = mHandleMap.find(sound);
	if (it != mHandleMap.end())
	{
		HandleInfo& info = it->second;
		if (fadeTimeMS > 0)
		{
			Mix_FadeOutChannel(info.mChannel, fadeTimeMS);
		}
		else
		{
			Mix_HaltChannel(info.mChannel);
			mChannels[info.mChannel].Reset();
			mHandleMap.erase(it);
		}
	}
}

void AudioSystem::PauseSound(SoundHandle sound)
{
	if (!sound.IsValid())
	{
		return;
	}

	std::map<SoundHandle, HandleInfo>::iterator it = mHandleMap.find(sound);
	if (it != mHandleMap.end())
	{
		HandleInfo& info = it->second;
		if (!info.mIsPaused)
		{
			Mix_Pause(info.mChannel);
			info.mIsPaused = true;
		}
	}
}

void AudioSystem::ResumeSound(SoundHandle sound)
{
	if (!sound.IsValid())
	{
		return;
	}

	std::map<SoundHandle, HandleInfo>::iterator it = mHandleMap.find(sound);
	if (it != mHandleMap.end())
	{
		HandleInfo& info = it->second;
		if (info.mIsPaused)
		{
			Mix_Resume(info.mChannel);
			info.mIsPaused = false;
		}
	}
}

SoundState AudioSystem::GetSoundState(SoundHandle sound)
{
	if (!sound.IsValid())
	{
		return SoundState::Stopped;
	}

	auto it = mHandleMap.find(sound);
	if (it != mHandleMap.end())
	{
		HandleInfo& info = it->second;

		if (info.mIsPaused)
		{
			return SoundState::Paused;
		}

		return SoundState::Playing;
	}

	return SoundState::Stopped;
}

void AudioSystem::StopAllSounds()
{
	for (size_t i = 0; i < mChannels.size(); i++)
	{
		if (mChannels[i].IsValid())
		{
			StopSound(mChannels[i]);
		}
	}
}

void AudioSystem::CacheAllSounds()
{
#ifndef __clang_analyzer__
	std::error_code ec{};
	for (const auto& rootDirEntry : std::filesystem::directory_iterator{"Assets/Sounds", ec})
	{
		std::string extension = rootDirEntry.path().extension().string();
		if (extension == ".ogg" || extension == ".wav")
		{
			std::string fileName = rootDirEntry.path().stem().string();
			fileName += extension;
			CacheSound(fileName);
		}
	}
#endif
}

// Used to preload the sound data of a sound
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
void AudioSystem::CacheSound(const std::string& soundName)
{
	GetSound(soundName);
}

// If the sound is already loaded, returns Mix_Chunk from the map.
// Otherwise, will attempt to load the file and save it in the map.
// Returns nullptr if sound is not found.
// NOTE: The soundName is without the "Assets/Sounds/" part of the file
//       For example, pass in "ChompLoop.wav" rather than
//       "Assets/Sounds/ChompLoop.wav".
Mix_Chunk* AudioSystem::GetSound(const std::string& soundName)
{
	std::string fileName = "Assets/Sounds/";
	fileName += soundName;

	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("[AudioSystem] Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

// Input for debugging purposes
void AudioSystem::ProcessInput(const Uint8* keyState)
{
	// Debugging code that outputs all active sounds on leading edge of period key
	if (keyState[SDL_SCANCODE_PERIOD] && !mLastDebugKey)
	{
		SDL_Log("[AudioSystem] Active Sounds:");
		for (size_t i = 0; i < mChannels.size(); i++)
		{
			if (mChannels[i].IsValid())
			{
				auto iter = mHandleMap.find(mChannels[i]);
				if (iter != mHandleMap.end())
				{
					HandleInfo& hi = iter->second;
					SDL_Log("Channel %d: %s, %s, looping = %d, paused = %d",
							static_cast<unsigned>(i), mChannels[i].GetDebugStr(),
							hi.mSoundName.c_str(), hi.mIsLooping, hi.mIsPaused);
				}
				else
				{
					SDL_Log("Channel %d: %s INVALID", static_cast<unsigned>(i),
							mChannels[i].GetDebugStr());
				}
			}
		}
	}

	mLastDebugKey = keyState[SDL_SCANCODE_PERIOD];
}

void AudioSystem::RemoveActor(Actor* actor)
{
	auto actorIt = mActorMap.find(actor);
	if (actorIt == mActorMap.end())
	{
		return;
	}

	std::set<SoundHandle> handles = actorIt->second;

	for (SoundHandle handle : handles)
	{
		auto handleIt = mHandleMap.find(handle);
		if (handleIt != mHandleMap.end())
		{
			HandleInfo& info = handleIt->second;
			info.mActor = nullptr;

			if (info.mStopOnActorRemove && Mix_Playing(info.mChannel))
			{
				Mix_HaltChannel(info.mChannel);
				mChannels[info.mChannel] = SoundHandle::Invalid;
				mHandleMap.erase(handleIt);
			}
		}
	}

	mActorMap.erase(actorIt);
}

int AudioSystem::CalculateVolume(Actor* actor, Actor* listener) const
{
	if (!actor || !listener)
	{
		return 128;
	}

	Vector3 actorPos = actor->GetWorldPosition();
	Vector3 listenerPos = listener->GetWorldPosition();

	float distance = (actorPos - listenerPos).Length();

	if (distance >= OUTER_RADIUS)
	{
		return 0;
	}
	else if (distance <= INNER_RADIUS)
	{
		return 128;
	}
	else
	{
		float t = (distance - INNER_RADIUS) / (OUTER_RADIUS - INNER_RADIUS);
		return static_cast<int>((1.0f - t) * 128.0f);
	}
}
