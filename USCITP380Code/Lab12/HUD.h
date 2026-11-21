#pragma once
#include "UIComponent.h"

class HUD : public UIComponent
{
public:
	HUD(class Actor* owner);
	~HUD();
	void Draw(class Shader* shader) override;

	void ShowSubtitle(const std::string& text);

	void PlayerTakeDamage(float angle);

	void Update(float deltaTime) override;

	const float BASIC_PLACE = 325.0f;
	const float FACTOR = 2.0f;
	const float DRAW_Z = 1.0f;
	const float DAMAGE_DURATION = 1.5f;

private:
	class Font* mFont;
	class Texture* mSubtitleTexture;
	class Texture* mSubtitleShadowTexture;

	Texture* mDamageIndicatorTexture;
	float mDamageIndicatorAngle;
	float mDamageIndicatorTime;
};