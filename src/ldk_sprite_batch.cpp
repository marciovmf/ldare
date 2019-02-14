
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
      Material* currentMaterial;
      Renderable renderable;
      VertexBuffer buffer;
      uint32 maxSprites;
      uint32 spriteCount;
      uint32 state;
      uint32 *indices;
      SpriteVertexData *vertices;
    };

    void makeSprite(Sprite* sprite, const Material* material, uint32 x, uint32 y, uint32 width, uint32 height)
    {
      sprite->material = (Material*) material;
      sprite->x = x;
      sprite->y = y;
      sprite->width = width;
      sprite->height = height;
    }

    static void _initBatch(SpriteBatch* spriteBatch, Context* context, Material* material)
    {
      spriteBatch->currentMaterial = material;
      spriteBatch->spriteCount = 0;
      setMaterial(&spriteBatch->renderable, spriteBatch->currentMaterial);
    }

    static void _flushBatch(SpriteBatch* spriteBatch)
    {
      // assign the material to the renderable if material changed
      if(spriteBatch->renderable.material != spriteBatch->currentMaterial)
      {
        setMaterial(&spriteBatch->renderable, spriteBatch->currentMaterial);
      }
      
      DrawCall drawCall;
      drawCall.renderable = &spriteBatch->renderable;
      drawCall.type = renderer::DrawCall::DRAW_INDEXED;
      drawCall.vertexCount = spriteBatch->spriteCount * 4;
      drawCall.vertices = spriteBatch->vertices;
      drawCall.indexStart = 0;
      drawCall.indexCount = spriteBatch->spriteCount * 6;
      spriteBatch->currentMaterial = nullptr;

      pushDrawCall(spriteBatch->context, &drawCall);
      renderer::flush(spriteBatch->context);
    }

    SpriteBatch* createSpriteBatch(Context* context, uint32 maxSprites)
    {
      const uint32 numIndices = 6 * maxSprites;
      const uint32 indexBufferSize = numIndices * sizeof(uint32);
      const uint32 numVertices = maxSprites * 4;
      const uint32 vertexBufferSize = numVertices * sizeof(SpriteVertexData);

      SpriteBatch* spriteBatch =
        (SpriteBatch*) ldk::platform::memoryAlloc(sizeof(SpriteBatch) + indexBufferSize + vertexBufferSize);
      spriteBatch->context = context;
      spriteBatch->currentMaterial = nullptr;
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

    void spriteBatchBegin(SpriteBatch* spriteBatch)
    {
      if(spriteBatch->state)
      {
        LogWarning("Multiple calls to spriteBatchBegin(). Call spriteBatchEnd() before starting a new batch.");
        return;
      }

      spriteBatch->state = 0;
      spriteBatch->spriteCount = 0;
    }


    void spriteBatchDraw(
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
      if (spriteBatch->currentMaterial == nullptr)
      {
        spriteBatch->currentMaterial = sprite->material;
      }
      else if (sprite->material != spriteBatch->currentMaterial || spriteBatch->spriteCount > spriteBatch->maxSprites)
      {
        _flushBatch(spriteBatch);
      }

      Material* material = sprite->material;

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

    void spriteBatchEnd(SpriteBatch* spriteBatch)
    {
      spriteBatch->state = 0;
      _flushBatch(spriteBatch);
    }

    void destroySpriteBatch(SpriteBatch* spriteBatch)
    {
      ldk::platform::memoryFree(spriteBatch);
    }
  }

}