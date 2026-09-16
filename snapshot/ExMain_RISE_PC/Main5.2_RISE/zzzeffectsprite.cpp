///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZzzOpenglUtil.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzLodTerrain.h"
#include "ZzzTexture.h"
#include "ZzzAi.h"
#include "ZzzEffect.h"
#include "DSPlaySound.h"
#include "WSClient.h"
#include "NewUISystem.h"
#include "RISE/GrowLancerResources.h"
#include "RISE/GrowLancerSpriteAdapter.h"

OBJECT	Sprites   [MAX_SPRITES];

namespace rise { namespace growlancer {
namespace {
bool gBrecheSpriteMarks[MAX_SPRITES] = {};

int SpriteSlot(const OBJECT* sprite)
{
    if (!sprite) return -1;
    const uintptr_t address = reinterpret_cast<uintptr_t>(sprite);
    const uintptr_t first = reinterpret_cast<uintptr_t>(&Sprites[0]);
    const uintptr_t last = reinterpret_cast<uintptr_t>(&Sprites[MAX_SPRITES - 1]);
    if (address < first || address > last) return -1;
    const uintptr_t stride = sizeof(OBJECT);
    const uintptr_t delta = address - first;
    if (stride == 0 || delta % stride != 0) return -1;
    const uintptr_t slot = delta / stride;
    return slot < MAX_SPRITES ? static_cast<int>(slot) : -1;
}
}

void MarkBrecheSprite(OBJECT* sprite)
{
    const int slot = SpriteSlot(sprite);
    if (slot >= 0) gBrecheSpriteMarks[slot] = true;
}

void ClearBrecheSprite(OBJECT* sprite)
{
    const int slot = SpriteSlot(sprite);
    if (slot >= 0) gBrecheSpriteMarks[slot] = false;
}

bool IsBrecheSprite(const OBJECT* sprite)
{
    const int slot = SpriteSlot(sprite);
    return slot >= 0 && sprite->Live && gBrecheSpriteMarks[slot];
}

int CreateBrecheSprite(int texture, float* position, float scale, float* light,
    OBJECT* owner, float rotation)
{
    // Native 1726C50 returns the selected slot, but 0 is also a valid slot.
    // Capture the free-slot identity before calling the shared allocator so a
    // full pool cannot accidentally mark an unrelated live slot.
    int freeSlot = -1;
    for (int i = 0; i < MAX_SPRITES; ++i)
    {
        if (!Sprites[i].Live) { freeSlot = i; break; }
    }
    const int result = CreateSprite(texture, position, scale, light, owner,
        rotation, 0);
    if (freeSlot >= 0 && result == freeSlot && Sprites[result].Live)
        MarkBrecheSprite(&Sprites[result]);
    return result;
}

} }

int CreateSprite(int Type,vec3_t Position,float Scale,vec3_t Light,OBJECT *Owner,float Rotation,int SubType)
{
    if (!g_pOption->GetRenderAllEffects())
    {
        return false;
    }

	for(int i=0;i<MAX_SPRITES;i++)
	{
		OBJECT *o = &Sprites[i];
		if(!o->Live)
		{
			rise::growlancer::ClearBrecheSprite(o);
			o->Live           = true;
			o->Type           = Type;
			o->SubType        = SubType;
			o->Owner          = Owner;
			o->AnimationFrame = 1.f;
    		o->Scale          = Scale;
			o->Angle[2]       = Rotation;
			VectorCopy(Position,o->Position);
			VectorCopy(Position,o->StartPosition);
			VectorCopy(Light,o->Light);
			return i;
		}
	}
	return false;
}

void RenderSprite(OBJECT *o,OBJECT *Owner)
{
    if (rise::growlancer::IsBrecheSprite(o) ||
        o->Type == rise::growlancer::kCircleShinyBitmap ||
        o->Type == rise::growlancer::kWrathLightmarksBitmap ||
        o->Type == rise::growlancer::kWrathFlare01Bitmap)
    {
        // S21 1727082..1727119: render-step visibility envelope, not
        // effect alpha. Keep this private; legacy SS6 sprites are unchanged.
        o->AnimationFrame += o->Visible ? .1f : -.1f;
        if (o->AnimationFrame > 1.f) o->AnimationFrame = 1.f;
        if (o->AnimationFrame < .2f) o->AnimationFrame = .2f;
    }
    else if(o->Visible)
	{
		o->AnimationFrame += 0.1f * static_cast<float>(FPS_ANIMATION_FACTOR);
		if(o->AnimationFrame > 1.f) 
		{
			o->AnimationFrame = 1.f;
		}
	}
	else
	{
		o->AnimationFrame += 0.1f * static_cast<float>(FPS_ANIMATION_FACTOR);
		if(o->AnimationFrame < 0.2f) 
		{
			o->AnimationFrame = 0.2f;
		}
	}
	float Scale = o->AnimationFrame*o->Scale;
	
	BITMAP_t* pBitmap = Bitmaps.GetTexture(o->Type);
	float Width  = pBitmap->Width * Scale;
	float Height = pBitmap->Height * Scale;

    if ( o->Type==BITMAP_FORMATION_MARK )
    {
		float u = 0.0f, v = 0.0f, uw, vw;
		uw=0.33f; vw=0.33f;
        switch ( o->SubType )
        {
        case 0:
            u=0.f; v=0.f;
            break;

        case 1:
            u=0.33f; v=0.f;
            break;

        case 2:
            u=0.66f; v=0.f;
            break;

        case 3:
            u=0.f; v=0.33f;
            break;

        case 4:
            u=0.33f; v=0.33f;
            break;

        case 5:
            u=0.66f; v=0.33f;
            break;

        case 6:
            u=0.f; v=0.66f;
            break;

        case 7:
            u=0.33f; v=0.66f;
            break;
        }

        RenderSprite( o->Type, o->Position, 64, 64, o->Light, o->Angle[2], u, v, uw, vw );
    }
    else
    {
        RenderSprite(o->Type,o->Position,Width,Height,o->Light,o->Angle[2]);
    }
}

void RenderSprites ( BYTE byRenderOneMore )
{
    if (!g_pOption->GetRenderAllEffects())
    {
        return;
    }

	for(int i=0;i<MAX_SPRITES;i++)
	{
		OBJECT *o = &Sprites[i];
        if( byRenderOneMore == 1 )
        {
            if ( o->Position[2] > 350.f ) 
			{
				continue;
			}
        }
        else if( byRenderOneMore == 2 )
        {
            if( o->Position[2] <= 300.f )
            {
                o->Live = false;
                rise::growlancer::ClearBrecheSprite(o);
                continue;
            }
        }

		if(o->Live)
		{
            if( o->Type == BITMAP_FORMATION_MARK )
            {
                EnableAlphaTest ();
            }
            else if(o->SubType == 0)
			{
          	    EnableAlphaBlend();
			}
			else if( o->SubType==1 )
			{
               	EnableAlphaBlendMinus();
			}
            else if( o->SubType==2 )
			{
                EnableAlphaTest();
			}
            else if( o->SubType==3 )
			{
                EnableAlphaBlend2();
			}
    		RenderSprite(o,o->Owner);

            if( byRenderOneMore == 0 || byRenderOneMore == 2 )
            {
                o->Live = false;
                rise::growlancer::ClearBrecheSprite(o);
            }
		}
	}
}

void CheckSprites()
{
    if (!g_pOption->GetRenderAllEffects())
    {
        return;
    }

	for(int i=0; i<MAX_SPRITES; i++)
	{
		OBJECT *o = &Sprites[i];
		if(o->Live)
		{
         	o->Visible = true;
		}
	}
}
