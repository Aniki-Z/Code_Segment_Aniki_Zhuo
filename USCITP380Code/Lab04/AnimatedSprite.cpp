#include "AnimatedSprite.h"
#include "Actor.h"
#include "Game.h"

AnimatedSprite::AnimatedSprite(Actor* owner, int drawOrder)
: SpriteComponent(owner, drawOrder)
{
}

void AnimatedSprite::Update(float deltaTime)
{
	if (mAnimName.empty())
	{
		return;
	}

	const std::vector<SDL_Texture*>& frames = mAnims[mAnimName];

	if (!frames.empty())
	{
		if (!mIsPaused)
		{
			mAnimTimer += deltaTime * mAnimFPS;
		}

		int frameIndex = static_cast<int>(mAnimTimer) % frames.size();

		SetTexture(frames[frameIndex]);
	}
}

void AnimatedSprite::AddAnimation(const std::string& name, const std::vector<SDL_Texture*>& images)
{
	mAnims.emplace(name, images);
}
