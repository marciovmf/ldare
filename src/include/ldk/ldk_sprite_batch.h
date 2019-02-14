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
      Material* material;
      uint32 x;
      uint32 y;
      uint32 width;
      uint32 height;
    };

    LDK_API void makeSprite(Sprite* sprite, const Material* material, uint32 x, uint32 y, uint32 width, uint32 height);
    LDK_API SpriteBatch* createSpriteBatch(Context* context, uint32 maxSprites);
    LDK_API void spriteBatchBegin(SpriteBatch* spriteBatch);
    LDK_API void spriteBatchDraw(SpriteBatch* spriteBatch,const Sprite* sprite, float x, float y, float width, float height, float angle = 0.0f, float pivotX = 0.0f, float pivotY = 0.0f);
    LDK_API void spriteBatchEnd(SpriteBatch* spriteBatch);
    LDK_API void destroySpriteBatch(SpriteBatch* spriteBatch);
  }

}

#endif// _LDK_SPRITE_BATCH_
