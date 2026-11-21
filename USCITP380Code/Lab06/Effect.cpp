#include "Effect.h"

Effect::Effect(Game* game, const Vector2& position, const std::string& animationName,
			   const std::string& soundName)
: Actor(game)
, mLifetime(0.0f)
{
	SetPosition(position);

	mAnimatedSprite = new AnimatedSprite(this, 200);
	mAnimatedSprite->LoadAnimations("Assets/Effects");
	mAnimatedSprite->SetAnimation(animationName);

	mLifetime = mAnimatedSprite->GetAnimDuration(animationName);

	game->GetAudio()->PlaySound(soundName);
}

void Effect::OnUpdate(float deltaTime)
{
	mLifetime -= deltaTime;
	if (mLifetime <= 0.0f)
	{
		SetState(ActorState::Destroy);
	}
}
