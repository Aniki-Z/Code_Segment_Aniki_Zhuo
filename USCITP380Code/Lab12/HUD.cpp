#include "HUD.h"
#include "Shader.h"
#include "Game.h"

HUD::HUD(class Actor* owner)
: UIComponent(owner)
, mSubtitleTexture(nullptr)
, mSubtitleShadowTexture(nullptr)
, mDamageIndicatorAngle(0.0f)
, mDamageIndicatorTime(0.0f)
{
	mFont = new Font();
	mFont->Load("Assets/Inconsolata-Regular.ttf");

	mDamageIndicatorTexture =
		GetGame()->GetRenderer()->GetTexture("Assets/Textures/UI/DamageIndicator.png");
}

HUD::~HUD()
{
	mFont->Unload();
	delete mFont;
}

void HUD::Draw(Shader* shader)
{
	if (mSubtitleTexture && mSubtitleShadowTexture)
	{
		int height = mSubtitleTexture->GetHeight();

		Vector2 basePos(0.0f, -BASIC_PLACE + height / FACTOR);

		DrawTexture(shader, mSubtitleShadowTexture, basePos + Vector2(FACTOR, -FACTOR));

		DrawTexture(shader, mSubtitleTexture, basePos);
	}

	if (mDamageIndicatorTime > 0.0f)
	{
		DrawTexture(shader, mDamageIndicatorTexture, Vector2::Zero, DRAW_Z, mDamageIndicatorAngle);
	}

	if (GetGame()->GetPlayer()->GetPlayerMove()->GetDead())
	{
		DrawTexture(shader,
					GetGame()->GetRenderer()->GetTexture("Assets/Textures/UI/DamageOverlay.png"));
	}
}

void HUD::ShowSubtitle(const std::string& text)
{
	if (mSubtitleTexture)
	{
		mSubtitleTexture->Unload();
		delete mSubtitleTexture;
		mSubtitleTexture = nullptr;

		mSubtitleShadowTexture->Unload();
		delete mSubtitleShadowTexture;
		mSubtitleShadowTexture = nullptr;
	}

	if (!text.empty())
	{
		std::string fullText = "GLaDOS: " + text;
		mSubtitleTexture = mFont->RenderText(fullText, Color::LightGreen);
		mSubtitleShadowTexture = mFont->RenderText(fullText, Color::Black);
	}
}

void HUD::PlayerTakeDamage(float angle)
{
	mDamageIndicatorAngle = angle;
	mDamageIndicatorTime = DAMAGE_DURATION;
}

void HUD::Update(float deltaTime)
{
	if (mDamageIndicatorTime > 0.0f)
	{
		mDamageIndicatorTime -= deltaTime;
	}

	if (mDamageIndicatorTime <= 0.0f)
	{
		mDamageIndicatorTime = 0.0f;
	}
}