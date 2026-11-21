#pragma once
#include "Actor.h"
#include "CollisionComponent.h"
#include "AudioSystem.h"

class VOTrigger : public Actor
{
public:
	VOTrigger(Game* game, std::string doorName, std::string nextLevel,
			  std::vector<std::string> sounds, std::vector<std::string> subtitles);
	void OnUpdate(float deltaTime) override;
	void OnProcessInput(const Uint8* keyState, Uint32 mouseButtons,
						const Vector2& relativeMouse) override;

private:
	CollisionComponent* mCollisionComponent;
	std::string mDoorName;
	std::string mNextLevel;
	std::vector<std::string> mSounds;
	std::vector<std::string> mSubtitles;

	AudioSystem* mSound;
	size_t mCurrentSoundIndex;
	SoundHandle mCurrentSoundHandle;
	bool mHasActivated;
	bool mPrevFKeyState;

	void PlayNextSound();
};