
namespace ldk
{
  namespace renderer
  {
    struct SpriteVertexData
    {
      Vec3 position;
      Vec2 uv;
    };

    struct SpriteBatch
    {
      Context* context;
      HMaterial currentMaterial;
      Renderable renderable;
      VertexBuffer buffer;
      uint32 maxSprites;
      uint32 spriteCount;
      uint32 state;
      uint32 *indices;
      SpriteVertexData *vertices;
    };

    void makeSprite(Sprite* sprite, HMaterial materialHandle, uint32 x, uint32 y, uint32 width, uint32 height)
    {
      sprite->material = materialHandle;
      sprite->x = x;
      sprite->y = y;
      sprite->width = width;
      sprite->height = height;
    }

    static void _initBatch(SpriteBatch* spriteBatch, Context* context, HMaterial materialHandle)
    {
      spriteBatch->currentMaterial = materialHandle;
      spriteBatch->spriteCount = 0;
      renderable_setMaterial(&spriteBatch->renderable, materialHandle);
    }

    static void _flushBatch(SpriteBatch* spriteBatch)
    {
      if(spriteBatch->currentMaterial.handle == ldkEngine::handle_invalid())
        return;

      // assign the material to the renderable if material changed
      if(spriteBatch->renderable.materialHandle.handle != spriteBatch->currentMaterial.handle)
      {
        renderable_setMaterial(&spriteBatch->renderable, spriteBatch->currentMaterial);
      }
      
      DrawCall drawCall;
      drawCall.renderable = &spriteBatch->renderable;
      drawCall.type = renderer::DrawCall::DRAW_INDEXED;
      drawCall.vertexCount = spriteBatch->spriteCount * 4;
      drawCall.vertices = spriteBatch->vertices;
      drawCall.indexStart = 0;
      drawCall.indexCount = spriteBatch->spriteCount * 6;
      spriteBatch->currentMaterial = typedHandle_invalid<HMaterial>();

      pushDrawCall(spriteBatch->context, &drawCall);
      renderer::flush(spriteBatch->context);
    }

    SpriteBatch* spriteBatch_create(Context* context, uint32 maxSprites)
    {
      const uint32 numIndices = 6 * maxSprites;
      const uint32 indexBufferSize = numIndices * sizeof(uint32);
      const uint32 numVertices = maxSprites * 4;
      const uint32 vertexBufferSize = numVertices * sizeof(SpriteVertexData);

      size_t totalSize = (sizeof(SpriteBatch) + indexBufferSize + vertexBufferSize);
      SpriteBatch* spriteBatch =
        (SpriteBatch*) ldkEngine::memory_alloc(totalSize, ldkEngine::Allocation::Tag::SPRITE_BATCH);

      spriteBatch->context = context;
      spriteBatch->currentMaterial = typedHandle_invalid<HMaterial>();
      spriteBatch->spriteCount = 0;
      spriteBatch->maxSprites = maxSprites;
      spriteBatch->state = 0;
      spriteBatch->indices = (uint32*) (((char*)spriteBatch) + sizeof(SpriteBatch));
      spriteBatch->vertices = (SpriteVertexData*)(((char*)spriteBatch) + indexBufferSize);

      renderer::makeVertexBuffer(&spriteBatch->buffer, maxSprites * 4);
      renderer::addVertexBufferAttribute(&spriteBatch->buffer, "_pos", 3, renderer::VertexAttributeType::FLOAT, 0);
      renderer::addVertexBufferAttribute(&spriteBatch->buffer, "_uuv", 2, renderer::VertexAttributeType::FLOAT,  3 * sizeof(float));

			// Precompute indices for every sprite
			int32 offset = 0;
			for(int32 i=0; i < indexBufferSize; i+=6)
			{

				spriteBatch->indices[i+0] = offset + 0;
				spriteBatch->indices[i+1] = offset + 1;
				spriteBatch->indices[i+2] = offset + 2;
				spriteBatch->indices[i+3] = offset + 0;
				spriteBatch->indices[i+4] = offset + 3;
				spriteBatch->indices[i+5] = offset + 1;
				offset+=4; // 4 offsets per sprite
			}

      makeRenderable(&spriteBatch->renderable, &spriteBatch->buffer, spriteBatch->indices, numIndices, false);
      return spriteBatch;
    }

    void spriteBatch_begin(SpriteBatch* spriteBatch)
    {
      if(spriteBatch->state)
      {
        LogWarning("Multiple calls to spriteBatchBegin(). Call spriteBatchEnd() before starting a new batch.");
        return;
      }

      spriteBatch->state = 0;
      spriteBatch->spriteCount = 0;
    }

    void spriteBatch_draw(
        SpriteBatch* spriteBatch,
        const Sprite* sprite,
        // Bottom left corner of sprite
        float posX,
        float posY,
        float width,
        float height,
        float angle,
        float rotX,
        float rotY)
    {
      // Flush the batch if material changed or the buffer is full
      if (spriteBatch->currentMaterial.handle == ldkEngine::handle_invalid())
      {
        spriteBatch->currentMaterial = sprite->material;
      }
      else if (sprite->material.handle != spriteBatch->currentMaterial.handle 
          || spriteBatch->spriteCount > spriteBatch->maxSprites)
      {
        _flushBatch(spriteBatch);
      }

      Material* material = (Material*) ldkEngine::handle_getData(sprite->material.handle);

      if (material->textureCount == 0)
      {
        LogWarning("Passed material does not have any attached texture.");
        return;
      }
			// sprite vertex order 0,1,2,2,3,0
			// map pixel coord to texture space
      Texture texture = material->texture[0];
			Rect uvRect;
			
      uvRect.x = sprite->x / (float) texture.width;
			uvRect.w = sprite->width / (float) texture.width;
			uvRect.y = 1 - (sprite->y /(float) texture.height); 
			uvRect.h = sprite->height / (float) texture.height;

			float s = sin(angle);
			float c = cos(angle);
			float z = 0.0f;
      
			SpriteVertexData* vertexData = spriteBatch->vertices + spriteBatch->spriteCount * 4;

      if (angle == 0.0f)
      {
        // bottom left
        vertexData->uv = { uvRect.x, uvRect.y - uvRect.h};
        vertexData->position = 
          Vec3{posX, posY, z};
        vertexData++;

        // top right
        vertexData->uv = { uvRect.x + uvRect.w, uvRect.y};
        vertexData->position = 
          Vec3{posX + width , posY + height, z};
        vertexData++;

        // top left
        vertexData->uv = { uvRect.x, uvRect.y};
        vertexData->position = 
          Vec3{posX, posY + height, z};
        vertexData++;

        // bottom right
        vertexData->uv = {uvRect.x + uvRect.w, uvRect.y - uvRect.h};
        vertexData->position = 	
          Vec3{posX + width, posY, z};

      }
      else
      {	
        // bottom left
        vertexData->uv = { uvRect.x, uvRect.y};
        float x1 = posX - rotX;
        float y1 = posY - rotY;
        vertexData->position = 
          Vec3{(x1 * c - y1 * s) + rotX, (x1 * s + y1 * c) + rotY, z};
        vertexData++;

        // top right
        vertexData->uv = {uvRect.x + uvRect.w, uvRect.y + uvRect.h};
        x1 = (width) + posX - rotX;
        y1 = (height) + posY - rotY;
        vertexData->position = 
          Vec3{(x1 * c - y1 * s) + rotX, (x1 * s + y1 * c) + rotY, z};
        vertexData++;

        // top left
        vertexData->uv = { uvRect.x, uvRect.y + uvRect.h};
        x1 = posX - rotX;
        y1 = (height) + posY - rotY;
        vertexData->position = 
          Vec3{(x1 * c - y1 * s) + rotX, (x1 * s + y1 * c) + rotY, z};
        vertexData++;

        // bottom right
        vertexData->uv = { uvRect.x + uvRect.w, uvRect.y};
        x1 = (width) + posX - rotX;
        y1 = posY - rotY;
        vertexData->position = 	
          Vec3{(x1 * c - y1 * s) + rotX, (x1 * s + y1 * c) + rotY, z};
      }
      spriteBatch->spriteCount++;
    }

    Vec2 spriteBatch_drawText(
        SpriteBatch* spriteBatch,
        ldk::HMaterial material,
        ldk::HFont font,
        Vec3& position,
        const char* text,
        float scale,
        Vec4& color)
    {
      ldk::Font* fontAsset = (Font*) ldkEngine::handle_getData(font.handle);

      if (fontAsset == nullptr)
      {
        LogError("Invalid font handle");
        return {-1,-1};
      }

      if (fontAsset->gliphs == nullptr)
        return {-1,-1};

      char c;
      const char* ptrChar = text;
      //sprite.color = color;

      ldk::Rect* gliphList = fontAsset->gliphs;
      uint16 firstCodePoint = fontAsset->fontData->firstCodePoint;
      uint16 lastCodePoint = fontAsset->fontData->lastCodePoint;
      uint16 defaultCodePoint = fontAsset->fontData->defaultCodePoint;

      if (scale <= 0) scale = 1.0f;

      Vec2 textSize = {};
      //submit each character as an individual sprite
      while ((c = *ptrChar) != 0)
      {
        ++ptrChar;
        Rect* gliph;

        // avoid indexing undefined characters
        if (c < firstCodePoint || c > lastCodePoint)
        {
          gliph = &(gliphList[defaultCodePoint]);
        }
        else
        {
          c = c - firstCodePoint;
          gliph = &(gliphList[c]);
        }

        //calculate advance
        uint32 advance = 0;
        advance += gliph->w * scale;

        Sprite sprite;
        makeSprite(&sprite, material, gliph->x, gliph->y, gliph->w, gliph->h);
        
        spriteBatch_draw(spriteBatch,
            &sprite,
            position.x + advance,
            position.y,
            gliph->w * scale,
            gliph->h * scale,
            0.0f,
            0.0f,
            0.0f);

        //TODO: account for multi line text
        textSize.x += sprite.width;
        textSize.y = MAX(textSize.y, sprite.height); 	
        
      }
      return textSize;
    }

    void spriteBatch_end(SpriteBatch* spriteBatch)
    {
      spriteBatch->state = 0;
      _flushBatch(spriteBatch);
    }

    void spriteBatch_destroy(SpriteBatch* spriteBatch)
    {
      ldkEngine::memory_free(spriteBatch);
    }
  }
}
