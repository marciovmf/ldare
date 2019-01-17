
namespace ldk
{

  namespace renderer
  {

    struct SpriteVertexData
    {
      Vec3 position;
      Vec2 uv;
    }

    struct SpriteBatch
    {
      Context& context;
      Material* currentMaterial;
      Renderable renderable;
      VertexBuffer buffer;
      uint32 maxSprites;
      uint32 spriteCount;
      uint32 state;
      uint32 *indices;
      SpriteVertexData *vertices;
    };

    void makeSprite(Sprite& sprite, const Material& material, uint32 x, uint32 y, uint32 width, uint32 height)
    {
      sprite->material = *material;
      sprite->x = x;
      sprite->y = y;
      sprite->width = width;
      sprite->height = height;
    }

    static void _initBatch(Context* context, Material* material)
    {
      spriteBatch->material = material;
      spriteBatch->spriteCount = 0;
      setMaterial(&spriteBatch->renderable, &spriteBatch->material);
    }

    static void _flushBatch(SpriteBatch* spriteBatch)
    {
      // assign the material to the renderable if material changed
      if(spriteBatch->renderable.material != spriteBatch->currentMaterial)
      {
        setMaterial(&spriteBatch->renderable, &spriteBatch->currentMaterial);
      }
      
      DrawCall drawCall;
      drawCall.renderable = dpriteBatch->renderable;
      drawCall.type = renderer::DrawCall::DRAW_INDEXED;
      drawCall.vertexCount = spriteBatch->spriteCount * 4;
      drawCall.vertices = spriteBatch->vertices;
      drawCall.indexStart = 0;
      drawCall.indexCount = spriteBatch->spriteCount * 6;

      pushDrawCall(spriteBatch->context, &drawCall);
    }

    SpriteBatch* createSpriteBatch(Context& context, uint32 maxSprites)
    {
      const uint32 numIndices = 6 * maxSprites;
      const uint32 indexBufferSize = numIndices * sizeof(uint32);
      const uint32 numVertices = maxSprites * 4;
      const uint32 vetexBufferSize = numVertices * sizeof(SpriteVertexData);

      SpriteBatch* spriteBatch =
        (SpriteBatch*) ldk::platform::memoryAlloc(sizeof(SpriteBatch) + indexBufferSize + vertexBufferSize);
      spriteBatch->context = context;
      spriteBatch->material = nullptr;
      spriteBatch->spriteCount = 0;
      spriteBatch->maxSprites = maxSprites;
      spriteBatch->state = 0;
      spriteBatch->indices = (uint32*) (((char*)spriteBatch) + sizeof(SpriteBatch));
      spriteBatch->vertices = (SpriteVertexData*)(((char*)spriteBatch) + indexBufferSize);

      renderer::makeVertexBuffer(&spriteBatch->buffer, maxSprites);
      renderer::addVertexBufferAttribute(&_gameState->buffer, "_pos", 3, renderer::VertexAttributeType::FLOAT, 0);
      renderer::addVertexBufferAttribute(&_gameState->buffer, "_uuv", 2, renderer::VertexAttributeType::FLOAT,  3 * sizeof(float));

			// Precompute indices for every sprite
			int32 offset = 0;
			for(int32 i=0; i < indexBufferSize; i+=6)
			{
				indices[i] 	 = offset;
				indices[i+1] = offset +1;
				indices[i+2] = offset +2;
				indices[i+3] = offset +2;
				indices[i+4] = offset +3;
				indices[i+5] = offset +0;

				offset+=4; // 4 offsets per sprite
			}

      makeRenderable(&_gameState->renderable, &_gameState->buffer, indices, numIndices, false);
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
      spriteBatch->material = nullptr;
      spriteBatch->spriteCount = 0;

    }

    void spriteBatchDraw(
        SpriteBatch* spriteBatch,
        Sprite& sprite,
        float posX,
        float posY,
        float scaleX = 1.0f,
        float scaleY = 1.0f,
        float angle = 0.0f)
    {
      // Flush the batch if material changed or the buffer is full
      if (spriteBatch->currentMaterial == nullptr)
      {
        spriteBatch->currentMaterial = sprite.material;
      }
      else if (sprite.material != spriteBatch->currentMaterial || spriteBatch->spriteCount < spriteBatch->maxSprites)
      {
        _flushBatch(spriteBatch);
      }

      if (material.textureCount == 0)
      {
        LogWarning("Passed material does not have any attached texture.");
        return;
      }

			// sprite vertex order 0,1,2,2,3,0
			// 1 -- 2
			// |    |
			// 0 -- 3

			// map pixel coord to texture space
      Texture texture = material->texture[0];
			Rectangle uvRect;
			uvRect.x = sprite.x / texture.width;
			uvRect.w = sprite.width / texture.width;
			uvRect.y = 1 - (sprite.y / texture.height); // origin is top-left corner
			uvRect.h = sprite.height / texture.height;

			float angle = angle;
			float halfWidth = sprite.width/2;
			float halfHeight = sprite.height/2;
			float s = sin(angle);
			float c = cos(angle);
			float z = 1.0f;

			SpriteVertexData* vertexData = spriteBatch->vertices + spriteBatch->spriteCount;

			// top left
			vertexData->uv = { uvRect.x, uvRect.y};
			float x = -halfWidth;
			float y = halfHeight;
			vertexData->position = 
				Vec3{(x * c - y * s) + sprite.position.x, (x * s + y * c) + sprite.position.y,	z};
			vertexData++;

			// bottom left
			vertexData->uv = { uvRect.x, uvRect.y - uvRect.h};
			x = -halfWidth;
			y = -halfHeight;
			vertexData->position = 
				Vec3{(x * c - y * s) + sprite.position.x, (x * s + y * c) + sprite.position.y, z};
			vertexData++;

			// bottom right
			vertexData->uv = {uvRect.x + uvRect.w, uvRect.y - uvRect.h};
			x = halfWidth;
			y = -halfHeight;
			vertexData->position = 	
				Vec3{(x * c - y * s) + sprite.position.x, (x * s + y * c) + sprite.position.y,	z};
			vertexData++;

			// top right
			vertexData->uv = { uvRect.x + uvRect.w, uvRect.y};
			x = halfWidth;
			y = halfHeight;
			vertexData->position = 
				Vec3{(x * c - y * s) + sprite.position.x, (x * s + y * c) + sprite.position.y,	z};
      
      spriteCount = spriteBatch->spriteCount++;
    }

    void spriteBatchEnd(SpriteBatch* spriteBatch)
    {
      spriteBatch->state = 0;
      _flushBatch(spriteBatch);
    }

    void destroySpriteBatch(SpriteBatch* spriteBatch)
    {
      ldk::platform::memoryFree(SpriteBatch);
    }
  }

}
