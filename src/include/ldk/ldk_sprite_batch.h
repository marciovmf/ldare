#ifndef _LDK_SPRITE_BATCH_
#define _LDK_SPRITE_BATCH_

namespace ldk
{
  namespace renderer
  {
    struct SpriteBatch;
    struct Material;

    struct Sprite
    {
      HMaterial material;
      uint32 x;
      uint32 y;
      uint32 width;
      uint32 height;
    };

    LDK_API void makeSprite(Sprite* sprite, HMaterial materialHandle, uint32 x, uint32 y, uint32 width, uint32 height);
    LDK_API SpriteBatch* spriteBatch_create(uint32 maxSprites);
    LDK_API void spriteBatch_begin(SpriteBatch* spriteBatch);
    LDK_API void spriteBatch_draw(SpriteBatch* spriteBatch,const Sprite* sprite, float x, float y, float width, float height, float angle = 0.0f, float pivotX = 0.0f, float pivotY = 0.0f);
    LDK_API Vec2 spriteBatch_drawText(SpriteBatch* spriteBatch, ldk::HMaterial material, ldk::HFont font, Vec3& position, const char* text, float scale, Vec4& color);
    LDK_API void spriteBatch_end(SpriteBatch* spriteBatch);
    LDK_API void spriteBatch_destroy(SpriteBatch* spriteBatch);
  }
}

#endif// _LDK_SPRITE_BATCH_
