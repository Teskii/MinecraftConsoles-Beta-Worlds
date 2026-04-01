#include "stdafx.h"
#include "ExperienceOrbRenderer.h"
#include "..\Minecraft.World\net.minecraft.world.entity.h"
#include "..\Minecraft.World\net.minecraft.world.level.tile.h"
#include "..\Minecraft.World\net.minecraft.world.item.h"
#include "Tesselator.h"
#include "EntityRenderDispatcher.h"
#include "..\Minecraft.World\Mth.h"
#include "..\Minecraft.World\JavaMath.h"

ResourceLocation ExperienceOrbRenderer::XP_ORB_LOCATION = ResourceLocation(TN_ITEM_EXPERIENCE_ORB);

ExperienceOrbRenderer::ExperienceOrbRenderer()
{
	shadowRadius = 0.0f;
	shadowStrength = 0.0f;
}

void ExperienceOrbRenderer::render(shared_ptr<Entity> _orb, double x, double y, double z, float rot, float a)
{
	// Beta-style console HUD keeps experience orbs functional but hidden.
	(void)_orb;
	(void)x;
	(void)y;
	(void)z;
	(void)rot;
	(void)a;
}

ResourceLocation *ExperienceOrbRenderer::getTextureLocation(shared_ptr<Entity> mob)
{
    return &XP_ORB_LOCATION;
}

void ExperienceOrbRenderer::blit(int x, int y, int sx, int sy, int w, int h)
{
	float blitOffset = 0;
	float us = 1 / 256.0f;
	float vs = 1 / 256.0f;
	Tesselator *t = Tesselator::getInstance();
	t->begin();
	t->vertexUV(x + 0, y + h, blitOffset, (sx + 0) * us, (sy + h) * vs);
	t->vertexUV(x + w, y + h, blitOffset, (sx + w) * us, (sy + h) * vs);
	t->vertexUV(x + w, y + 0, blitOffset, (sx + w) * us, (sy + 0) * vs);
	t->vertexUV(x + 0, y + 0, blitOffset, (sx + 0) * us, (sy + 0) * vs);
	t->end();
}
