
namespace ldk
{
  namespace renderer
  {
    struct SpriteVertexData
    {
      Vec4 color;
      Vec3 position;
      Vec2 uv;
    };

    static struct SpriteBatch
    {
      HMaterial currentMaterial;
      Renderable renderable;
      VertexBuffer buffer;
      uint32 maxSprites;
      uint32 spriteCount;
      uint32 batchedSpriteCount;
      uint32 *indices;
      bool started;
      SpriteVertexData *vertices;
    }* _spriteBatch;

    void makeSprite(Sprite* sprite, HMaterial materialHandle, uint32 x, uint32 y, uint32 width, uint32 height)
    {
      sprite->material = materialHandle;
      sprite->x = x;
      sprite->y = y;
      sprite->width = width;
      sprite->height = height;
    }

    // gets the global implicit sprite batch context
    static inline SpriteBatch* _getSpriteBatch()
    {
      return _spriteBatch;
    }

    // sets the global implicit sprite batch context
    static inline void _setSpriteBatch(SpriteBatch* spriteBatch)
    {
      _spriteBatch = spriteBatch;
    }

    static void _flushBatch(SpriteBatch* spriteBatch)
    {
      if(spriteBatch->currentMaterial.handle == ldkEngine::handle_invalid())
      {
        LogError("Flushing batch without a material");
        return;
      }

      uint32 totalSpriteCount = spriteBatch->spriteCount;
      uint32 totalVertexCount = totalSpriteCount * 4;
      uint32 totalIndexCount = totalSpriteCount * 6;

      uint32 spriteCount = spriteBatch->batchedSpriteCount;
      uint32 vertexCount = spriteCount * 4;
      uint32 indexCount = spriteCount * 6;
      
      DrawCall drawCall;
      drawCall.renderable = &spriteBatch->renderable;
      drawCall.type = renderer::DrawCall::DRAW_INDEXED;
      drawCall.vertices = spriteBatch->vertices + totalVertexCount - vertexCount;
      drawCall.indexStart = totalIndexCount - indexCount;
      drawCall.indexCount = indexCount;
      drawCall.vertexCount = vertexCount;
      drawCall.material = spriteBatch->currentMaterial;
      
      pushDrawCall(&drawCall);
      spriteBatch->currentMaterial = typedHandle_invalid<HMaterial>();
      spriteBatch->batchedSpriteCount = 0;
    }

    void spriteBatch_initialize(uint32 maxSprites)
    {
      const uint32 numIndices = 6 * maxSprites;
      const uint32 numVertices = maxSprites * 4;
      const size_t indexBufferSize = numIndices * sizeof(uint32);
      const size_t vertexBufferSize = numVertices * sizeof(SpriteVertexData);

      size_t totalSize = (sizeof(SpriteBatch) + indexBufferSize + vertexBufferSize);
      SpriteBatch* spriteBatch =
        (SpriteBatch*) ldkEngine::memory_alloc(totalSize, ldkEngine::Allocation::Tag::SPRITE_BATCH);

      spriteBatch->currentMaterial = typedHandle_invalid<HMaterial>();
      spriteBatch->spriteCount = 0;
      spriteBatch->maxSprites = maxSprites;
      spriteBatch->indices = (uint32*) (spriteBatch+1);
      spriteBatch->started = false;
      spriteBatch->vertices = (SpriteVertexData*)(((char*)spriteBatch->indices) + indexBufferSize);

      renderer::makeVertexBuffer(&spriteBatch->buffer, numVertices);
      
      renderer::addVertexBufferAttribute(&spriteBatch->buffer, "_color", 
          4, renderer::VertexAttributeType::FLOAT, 0);

      renderer::addVertexBufferAttribute(&spriteBatch->buffer, "_pos",
          3, renderer::VertexAttributeType::FLOAT, 4 * sizeof(float));
      
      renderer::addVertexBufferAttribute(&spriteBatch->buffer, "_uv", 
          2, renderer::VertexAttributeType::FLOAT, 7 * sizeof(float));
      

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
      _setSpriteBatch(spriteBatch);
    }

    void spriteBatch_begin()
    {
      SpriteBatch* spriteBatch = _getSpriteBatch();
      if(spriteBatch->started)
      {
        LogError("Ignoring call to spriteBatch_begin. Call spriteBatchEnd() before starting a new batch.");
        return;
      }

      spriteBatch->started = true;
      spriteBatch->spriteCount = 0;
      spriteBatch->batchedSpriteCount = 0;
    }

    void spriteBatch_draw(
        const Sprite* sprite,
        // Bottom left corner of sprite
        float posX,
        float posY,
        float width,
        float height,
        const Vec4& color,
        float angle,
        float rotX,
        float rotY)
    {
      SpriteBatch* spriteBatch = _getSpriteBatch();
      if (!spriteBatch->started)
      {
        LogError("Ignoring call to spriteBatch_draw before spriteBatch_begin.");
        return;
      }

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
      spriteBatch->currentMaterial = sprite->material;

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

			float z = 0.0f;
     
			SpriteVertexData* vertexData = spriteBatch->vertices + spriteBatch->spriteCount * 4;

      if (angle == 0.0f)
      {
        // bottom left
        vertexData->color = color;
        vertexData->uv = { uvRect.x, uvRect.y - uvRect.h};
        vertexData->position = 
          Vec3{posX, posY, z};
        vertexData++;

        // top right
        vertexData->color = color;
        vertexData->uv = { uvRect.x + uvRect.w, uvRect.y};
        vertexData->position = 
          Vec3{posX + width , posY + height, z};
        vertexData++;

        // top left
        vertexData->color = color;
        vertexData->uv = { uvRect.x, uvRect.y};
        vertexData->position = 
          Vec3{posX, posY + height, z};
        vertexData++;

        // bottom right
        vertexData->color = color;
        vertexData->uv = {uvRect.x + uvRect.w, uvRect.y - uvRect.h};
        vertexData->position = 	
          Vec3{posX + width, posY, z};
      }
      else
      {	
        float s = sin(angle);
        float c = cos(angle);

        // bottom left
        vertexData->color = color;
        vertexData->uv = { uvRect.x, uvRect.y};
        float x1 = posX - rotX;
        float y1 = posY - rotY;
        vertexData->position = 
          Vec3{(x1 * c - y1 * s) + rotX, (x1 * s + y1 * c) + rotY, z};
        vertexData++;

        // top right
        vertexData->color = color;
        vertexData->uv = {uvRect.x + uvRect.w, uvRect.y + uvRect.h};
        x1 = (width) + posX - rotX;
        y1 = (height) + posY - rotY;
        vertexData->position = 
          Vec3{(x1 * c - y1 * s) + rotX, (x1 * s + y1 * c) + rotY, z};
        vertexData++;

        // top left
        vertexData->color = color;
        vertexData->uv = { uvRect.x, uvRect.y + uvRect.h};
        x1 = posX - rotX;
        y1 = (height) + posY - rotY;
        vertexData->position = 
          Vec3{(x1 * c - y1 * s) + rotX, (x1 * s + y1 * c) + rotY, z};
        vertexData++;

        // bottom right
        vertexData->color = color;
        vertexData->uv = { uvRect.x + uvRect.w, uvRect.y};
        x1 = (width) + posX - rotX;
        y1 = posY - rotY;
        vertexData->position = 	
          Vec3{(x1 * c - y1 * s) + rotX, (x1 * s + y1 * c) + rotY, z};
      }

      spriteBatch->spriteCount++;
      spriteBatch->batchedSpriteCount++;
    }

    Vec2 spriteBatch_drawText(
        ldk::HMaterial material,
        ldk::HFont font,
        Vec3& position,
        const char* text,
        float scale,
        Vec4& color)
    {
      SpriteBatch* spriteBatch = _getSpriteBatch();
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

      ldk::Rect* gliphList = fontAsset->gliphs;
      uint16 firstCodePoint = fontAsset->fontData->firstCodePoint;
      uint16 lastCodePoint = fontAsset->fontData->lastCodePoint;
      uint16 defaultCodePoint = fontAsset->fontData->defaultCodePoint;

      if (scale <= 0) scale = 1.0f;

      Vec2 textSize = {};
      uint32 advance = 0;
      uint32 vAdvance = 0;

      Rect* defaultGliph = &(gliphList[defaultCodePoint]);

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

        if (c == '\r')
        {
          continue;
        }

        if (c == '\n')
        {
          float yOffset = defaultGliph->h * scale;
          vAdvance += yOffset;
          textSize.y += yOffset;
          advance = 0;
        }
        else if (c == '\t')
        {
          advance += defaultGliph->w * scale * 4; // tab is 4 characters wide
          textSize.x += advance * scale;
        }

        else 
        {
          advance += gliph->w * scale;
          Sprite sprite;
          makeSprite(&sprite, material, gliph->x, gliph->y, gliph->w, gliph->h);

          spriteBatch_draw(
              &sprite,
              position.x + advance,
              position.y - vAdvance,
              gliph->w * scale,
              gliph->h * scale,
              color,
              0.0f,
              0.0f,
              0.0f);

          textSize.x += advance;
        }
      }
      return textSize;
    }

    void spriteBatch_end()
    {
      SpriteBatch* spriteBatch = _getSpriteBatch();
      if(!spriteBatch->started)
      {
        LogError("Ignoring call to spriteBatch_end before spriteBatch_begin.");
        return;
      }
      _flushBatch(spriteBatch);
      spriteBatch->started = false;
    }

    void spriteBatch_finalize()
    {
      SpriteBatch* spriteBatch = _getSpriteBatch();
      ldkEngine::memory_free(spriteBatch);
    }
  }
}
