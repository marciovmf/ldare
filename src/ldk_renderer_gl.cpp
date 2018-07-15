
#define LDK_EXTERN_GL_FUNCTIONS
#include "ldk_gl.h"
#undef LDK_EXTERN_GL_FUNCTIONS

// Sprite batch data
#define SPRITE_BATCH_MAX_SPRITES 10000
#define SPRITE_BATCH_VERTEX_DATA_SIZE sizeof(ldk::SpriteVertexData)
#define SPRITE_BATCH_SPRITE_SIZE SPRITE_BATCH_VERTEX_DATA_SIZE * 4 // 4 vetices per sprite
#define SPRITE_BATCH_BUFFER_SIZE SPRITE_BATCH_MAX_SPRITES * SPRITE_BATCH_SPRITE_SIZE
#define SPRITE_BATCH_INDICES_SIZE SPRITE_BATCH_MAX_SPRITES * 6        //6 indices per quad

#define SPRITE_ATTRIB_COLOR 0
#define SPRITE_ATTRIB_VERTEX 1
#define SPRITE_ATTRIB_UV 2
#define SPRITE_ATTRIB_ZROTATION 3

#ifdef _LDK_DEBUG_
#define checkGlError() checkNoGlError(__FILE__, __LINE__)
#else
#define checkGlError() 
#endif

// render state config keys
#define LDK_MATERIAL_PARAM_CULL_FACE        "cull-face"
#define LDK_MATERIAL_PARAM_DEPTH_TEST       "depth-test"
#define LDK_MATERIAL_PARAM_STENCIL_TEST     "stencil-test"
#define LDK_MATERIAL_PARAM_BLEND_FUNC_SRC   "blend-func-src"
#define LDK_MATERIAL_PARAM_BLEND_FUNC_DST   "blend-func-dst"
#define LDK_MATERIAL_PARAM_BLEND_FUNC_COLOR "blend-func-color"
#define LDK_MATERIAL_PARAM_VERTEX_SHADER	  "vertex-shader"
#define LDK_MATERIAL_PARAM_FRAGMENT_SHADER  "fragment-shader"
#define LDK_MATERIAL_PARAM_MAIN_TEXTURE     "main-texture"

#define LDK_MATERIAL_PARAM_BLEND_RGB_SRC    "main-textue"
#define LDK_MATERIAL_PARAM_BLEND_RGB_SRC    "main-textue"
#define LDK_MATERIAL_PARAM_BLEND_ALPHA_SRC  "main-textue"
#define LDK_MATERIAL_PARAM_BLEND_ALPHA_DST  "main-textue"

#define LDK_MATERIAL_CULL_FRONT "front"
#define LDK_MATERIAL_CULL_BACK  "back"
#define LDK_MATERIAL_CULL_NONE  "none"

#define LDK_MATERIAL_TEST_LESS           "less"
#define LDK_MATERIAL_TEST_LESS_EQUALS    "less_equals"
#define LDK_MATERIAL_TEST_GREATER        "greater"
#define LDK_MATERIAL_TEST_GREATER_EQUALS "greater_equals"
#define LDK_MATERIAL_TEST_EQUALS         "equals"
#define LDK_MATERIAL_TEST_DIFFERENT      "different"
#define LDK_MATERIAL_TEST_ALWAYS         "always"
#define LDK_MATERIAL_TEST_NEVER          "never"
#define LDK_MATERIAL_TEST_DISABLE        "disable"

#define LDK_MATERIAL_BLEND_ZERO "zero"
#define LDK_MATERIAL_BLEND_ONE "one"
#define LDK_MATERIAL_BLEND_SRC_COLOR "src-color"
#define LDK_MATERIAL_BLEND_ONE_MINUS_SRC_COLOR "one-minus-src-color"
#define LDK_MATERIAL_BLEND_DST_COLOR "dst-color"
#define LDK_MATERIAL_BLEND_ONE_MINUS_DST_COLOR "one-minus-dst-color"
#define LDK_MATERIAL_BLEND_SRC_ALPHA "src-alpha"
#define LDK_MATERIAL_BLEND_ONE_MINUS_SRC_ALPHA "one-minus-src-alpha"
#define LDK_MATERIAL_BLEND_DST_ALPHA "dst-alpha"
#define LDK_MATERIAL_BLEND_ONE_MINUS_DST_ALPHA "one-minus-dst-alpha"
#define LDK_MATERIAL_BLEND_CONST_COLOR "const-color"
#define LDK_MATERIAL_BLEND_ONE_MINUS_CONST_COLOR "one-minus-const-color"
#define LDK_MATERIAL_BLEND_CONST_ALPHA "const-alpha"
#define LDK_MATERIAL_BLEND_ONE_MINUS_CONST_ALPHA "one-minus-const-alpha"

#define LDK_MATERIAL_OPTION_DISABLE				  "disable"

static int32 checkNoGlError(const char* file, uint32 line)
{
    const char* error = "UNKNOWN ERROR CODE";
    GLenum err = glGetError();
    int32 success = 1;
    uchar noerror = 1;
    while(err!=GL_NO_ERROR)
    {
        switch(err)
        {
            case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
            case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
            case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
            case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
        }
        success=0;
        LogError("GL ERROR %s at %s:%d",error, file, line);
        noerror=0;
        err=glGetError();
    }
    return success;
}

static void clearGlError()
{
    GLenum err;
    do
    {
        err = glGetError();
    }while (err != GL_NO_ERROR);
}

namespace ldk 
{
#ifdef _MSC_VER
#pragma pack(push,1)
#endif
    struct SpriteVertexData
    {
        Vec4 color;
        Vec3 position;
        Vec2 uv;
        float zRotation;
    };

#ifdef _MSC_VER
#pragma pack(pop)
#endif

	static struct GlobalShaderData
	{
		Mat4 projectionMatrix;
		Mat4 baseModelMatrix;
		Vec4 time; // (deltaTime, time)
	} globalShaderData;

	static bool updateGlobalShaderData = false;
	static ldk::FontAsset fontAsset; // For text batching
	static struct GL_SpriteBatchData
	{
		ldk::Material material; 					// Current bound material
		GLuint vao;
		render::GpuBuffer vertexBuffer;
		render::GpuBuffer indexBuffer;
		render::GpuBuffer uniformBuffer;
		uint32 spriteCount; 								// number of sprites pushed int the current batch
		ldk::Bitmap fallbackBitmap;
		uint32 fallbackBitmapData;
	} spriteBatchData;
	
	static Rectangle _viewPort;

	static GLboolean checkShaderProgramLink(GLuint program)
	{
		GLint success = 0;
		GLchar msgBuffer[1024] = {};
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, sizeof(msgBuffer), 0, msgBuffer);
			LogError("Error linking shader");
			LogError(msgBuffer);
			return GL_FALSE;
		}

		return GL_TRUE;
	}

	static GLboolean checkShaderCompilation(GLuint shader)
	{
		GLint success = 0;
		GLchar msgBuffer[512] = {};

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLint shaderType;
			const char* shaderTypeName;

			glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
			glGetShaderInfoLog(shader, sizeof(msgBuffer), 0, msgBuffer);

			if ( shaderType == GL_VERTEX_SHADER)
				shaderTypeName = "Error compiling vertex shader";
			else if ( shaderType == GL_FRAGMENT_SHADER)
				shaderTypeName = "Error compiling fragment shader";
			else
				shaderTypeName = "Error compiling unknown shader type";

			LogError(shaderTypeName);
			LogError(msgBuffer);
			return GL_FALSE;
		}

		return GL_TRUE;
	}

	static GLuint compileShader(const char* source, GLenum shaderType)
	{
		LDK_ASSERT(shaderType == GL_VERTEX_SHADER || shaderType == GL_FRAGMENT_SHADER,
				"Invalid shader type");

		// Setup default vertex shader
		GLuint shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, (const GLchar**)&source, 0);
		glCompileShader(shader);
		if (!checkShaderCompilation(shader))
			return GL_FALSE;

		return shader;
	}

	namespace render
	{
    static struct _MaterialOptionHashes
    {
      int32 cullFront;
      int32 cullBack;
      int32 cullNone;
      int32 testLess;    
      int32 testLessEquals;    
      int32 testGreater;    
      int32 testGreaterEquals;    
      int32 testDifferent;    
      int32 testAlways;
      int32 testNever;
      int32 BlendZero;
      int32 BlendOne;
      int32 BlendSrcColor;
      int32 BlendOneMinusSrcColor;
      int32 BlendDstColor;
      int32 BlendOneMinusDstColor;
      int32 BlendSrcAlpha;
      int32 BlendOneMinusSrcAlpha;
      int32 BlendDstAlpha;
      int32 BlendOneMinusDstAlpha;
      int32 BlendConstColor;
      int32 BlendOneMinusConstColor;
      int32 BlendConstAlpha;
      int32 BlendOneMinusConstAlpha;
      
      int32 disable;
    } _materialOptionHashes;

		static GLuint createShaderProgram(const char* vertex, const char* fragment)
		{
			GLuint vertexShader = compileShader((const char*)vertex, GL_VERTEX_SHADER);
			GLuint fragmentShader = compileShader((const char*)fragment, GL_FRAGMENT_SHADER);
			GLuint shaderProgram = glCreateProgram();

			glAttachShader(shaderProgram, vertexShader);	
			glAttachShader(shaderProgram, fragmentShader);

			// Link shader program
			glLinkProgram(shaderProgram);
			if (!checkShaderProgramLink(shaderProgram))
				return GL_FALSE;

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			return shaderProgram;
		}

		Shader loadShader(const char* vertexSource, const char* fragmentSource)
		{
			Shader shader = createShaderProgram(vertexSource, fragmentSource);
			return shader;
		}

		Shader loadShaderFromFile(const char* vertexFile, const char* fragmentFile)
		{
			size_t vertShaderFileSize;
			size_t fragShaderFileSize;
			const char* vertexSource = (const char*)platform::loadFileToBuffer(vertexFile, &vertShaderFileSize);
			const char* fragmentSource = (const char*) platform::loadFileToBuffer(fragmentFile, &fragShaderFileSize);

			Shader shader = loadShader(vertexSource, fragmentSource);
			//TODO: remove this when we have a proper way to reuse file I/O memory
			platform::memoryFree((void*)vertexSource);			
			platform::memoryFree((void*)fragmentSource);			

			return shader;
		}


    GLenum getCullStateFromConfig(ldk::VariantSection& section)
    {
      char* cfgValue;
      GLenum value = GL_BACK;
      if (ldk::config_getString(&section, LDK_MATERIAL_PARAM_CULL_FACE, &cfgValue))
      {
        int32 hash = ldk::stringToHash(cfgValue);
        if(hash == _materialOptionHashes.cullFront )
          value = GL_FRONT;
        else if (hash == _materialOptionHashes.cullNone)
          value = GL_FRONT_AND_BACK;
        else if (hash == _materialOptionHashes.cullBack)
          value = GL_BACK;
        else
          LogWarning("invalid culling mode" );
      }
      return value;
    }

    GLenum getTestFromConfig(ldk::VariantSection& section, const char* key)
    {
      char* cfgValue;
      GLenum value = GL_LESS;
      if (ldk::config_getString(&section, key, &cfgValue))
      {
        int32 hash = ldk::stringToHash(cfgValue);
        if (hash == _materialOptionHashes.testLess)
          value = GL_LESS;
        else if (hash == _materialOptionHashes.testLessEquals)
          value = GL_LEQUAL;
        else if (hash == _materialOptionHashes.testGreater)
          value = GL_GREATER;
        else if (hash == _materialOptionHashes.testGreaterEquals)
          value = GL_GEQUAL;
        else if (hash == _materialOptionHashes.testAlways)
          value = GL_ALWAYS;
        else if (hash == _materialOptionHashes.testNever)
          value = GL_NEVER;
        else if (hash == _materialOptionHashes.testDifferent)
          value = GL_NOTEQUAL;
        else if (hash == _materialOptionHashes.disable)
          value = -1; // disable
        else
          LogWarning("invalid depth/stencil test mode" );
      }
      return value;
    }

    GLenum getBlendFuncFromConfig(ldk::VariantSection& section, const char* key, bool dst)
    {
      char* cfgValue;
      GLenum value = dst ? GL_ONE_MINUS_SRC_ALPHA: GL_SRC_ALPHA;

      if (ldk::config_getString(&section, key, &cfgValue))
      {
        int32 hash = ldk::stringToHash(cfgValue);
        if (hash == _materialOptionHashes.BlendZero)
          value = GL_ZERO;
        else if (hash == _materialOptionHashes.BlendOne)
          value = GL_ONE;
        else if (hash == _materialOptionHashes.BlendSrcColor)
          value = GL_SRC_COLOR;
        else if (hash == _materialOptionHashes.BlendOneMinusSrcColor)
          value = GL_ONE_MINUS_SRC_COLOR;
        else if (hash == _materialOptionHashes.BlendDstColor)
          value = GL_DST_COLOR;
        else if (hash == _materialOptionHashes.BlendOneMinusDstColor)
          value = GL_ONE_MINUS_DST_COLOR;
        else if (hash == _materialOptionHashes.BlendSrcAlpha)
          value = GL_SRC_ALPHA;
        else if (hash == _materialOptionHashes.BlendOneMinusSrcAlpha)
          value = GL_ONE_MINUS_SRC_ALPHA;
        else if (hash == _materialOptionHashes.BlendDstAlpha)
          value = GL_DST_ALPHA;
        else if (hash == _materialOptionHashes.BlendOneMinusDstAlpha)
          value = GL_ONE_MINUS_DST_ALPHA;
        else if (hash == _materialOptionHashes.BlendConstColor)
          value = GL_CONSTANT_COLOR;
        else if (hash == _materialOptionHashes.BlendOneMinusConstColor)
          value = GL_ONE_MINUS_CONSTANT_COLOR;
        else if (hash == _materialOptionHashes.BlendConstAlpha)
          value = GL_CONSTANT_ALPHA;
        else if (hash == _materialOptionHashes.BlendOneMinusConstAlpha)
          value = GL_ONE_MINUS_CONSTANT_ALPHA;
        else
          LogWarning("invalid depth/stencil test mode" );
      }
      return value;
    }
    
		//TODO: make filtering paremetrizable when importing texture
		//TODO: Pass texture import settings as an argument to loadTexture
		ldk::Texture loadTexture(const char* bitmapFile)
		{
			clearGlError();
			ldk::Bitmap bitmap;

			if (!ldk::loadBitmap(bitmapFile, &bitmap))
			{
				LogWarning("Using fallback bitmap");
				// we could not load the bitmap. Lets provide a dummy texture
				bitmap = spriteBatchData.fallbackBitmap;
			}

			GLuint textureId;
			glGenTextures(1, &textureId);
			glBindTexture(GL_TEXTURE_2D, textureId);

			// handle correct pixel data format
			GLenum pixelDataType = (bitmap.bitsPerPixel == 32) ? GL_UNSIGNED_BYTE : 
				GL_UNSIGNED_SHORT_4_4_4_4;

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.width, bitmap.height, 0, 
					GL_RGBA, pixelDataType, bitmap.pixels);

			checkGlError();

			glGenerateMipmap(GL_TEXTURE_2D);
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_CLAMP_TO_EDGE);
			//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glBindTexture(GL_TEXTURE_2D, 0);
			ldk::freeAsset(bitmap.bmpFileMemoryToRelease_, bitmap.bmpMemorySize_);

			ldk::Texture texture = {};
			texture.width = bitmap.width;
			texture.height = bitmap.height;
			texture.id = textureId;
			return texture;
		}

    ldk::Material loadMaterial(const char* materialFile)
    {
      char* fragmentSource = "";
      char* vertexSource = "";
      char* textureFile = "";

      ldk::Material material = {};

      ldk::VariantSectionRoot* root = ldk::config_parseFile((const char*)materialFile);
      if (root)
      {
        ldk::VariantSection* section = ldk::config_getSection(root, (const char*) "material");
        if (section)
        {
          ldk::config_getString(section, LDK_MATERIAL_PARAM_VERTEX_SHADER, &vertexSource);
          ldk::config_getString(section, LDK_MATERIAL_PARAM_FRAGMENT_SHADER, &fragmentSource);
          ldk::config_getString(section, LDK_MATERIAL_PARAM_MAIN_TEXTURE, &textureFile);
          
          float* blendColor;
          int32 blendColorArraySize = ldk::config_getFloatArray(section, LDK_MATERIAL_PARAM_BLEND_FUNC_COLOR, &blendColor);
          if (blendColorArraySize == 4)
          {
            material.blendConstantColor.x = *(blendColor++);
            material.blendConstantColor.y = *(blendColor++);
            material.blendConstantColor.z = *(blendColor++);
            material.blendConstantColor.w = *(blendColor++);
          }
          else if (blendColorArraySize != -1) // not defined in the file
          {
            LogWarning("%s must be a 4 element array, not %d", LDK_MATERIAL_PARAM_BLEND_FUNC_COLOR, blendColorArraySize);
          }

          //material.blendFunc = getBlendFuncFromConfig(*section);
          material.cullMode = getCullStateFromConfig(*section);
          material.depthFunc = getTestFromConfig(*section, LDK_MATERIAL_PARAM_DEPTH_TEST);
          material.blendFuncSrc = getBlendFuncFromConfig(*section, LDK_MATERIAL_PARAM_BLEND_FUNC_SRC, false);
          material.blendFuncDst = getBlendFuncFromConfig(*section, LDK_MATERIAL_PARAM_BLEND_FUNC_DST, true);
          material.stencilFunc = getTestFromConfig(*section, LDK_MATERIAL_PARAM_STENCIL_TEST);
        }

        //TODO: Allocate this properly when we have a memory manager.
        ldk::Bitmap bitmap;
        material.shader = ldk::render::loadShader(vertexSource, fragmentSource);
        material.texture = ldk::render::loadTexture(textureFile);


        // dispose of the parsed material memory
        ldk::config_dispose(root);
      }
      return material;
    }

		void unloadMaterial(Material* material)
		{
			//TODO: We are leaking texuture memory here! Unload this properly when we have a memory manager.
		}

		void updateRenderer(float deltaTime)
		{
			globalShaderData.time.x = deltaTime;
			globalShaderData.time.y += deltaTime;
			updateGlobalShaderData = true;
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void setViewportAspectRatio(uint32 windowWidth, uint32 windowHeight, uint32 virtualWidth, uint32 virtualHeight)
		{
			float targetAspectRatio = virtualWidth / (float) virtualHeight;

			// Try full viewport width with cropped, height if necessary
			int32 viewportWidth = windowWidth;
			int32 viewportHeight = (int)(viewportWidth / targetAspectRatio + 0.5f);

			// if calculated viewport height does not fit the window width,
			// switch to pillar box to preserve the aspect ratio
			if (viewportHeight > windowHeight)
			{
				viewportHeight = windowHeight;
				viewportWidth = (int)(viewportHeight* targetAspectRatio + 0.5f);
			}

			// center viewport
			int32 viewportX = (windowWidth / 2) - (viewportWidth / 2);
			int32 viewportY = (windowHeight / 2) - (viewportHeight / 2);
			setViewport(viewportX, viewportY, viewportWidth, viewportHeight);

			globalShaderData.baseModelMatrix = Mat4();
			globalShaderData.baseModelMatrix.scale(
					viewportWidth / (float)virtualWidth, 
					viewportHeight / (float)virtualHeight, 1.0f);

			// Update global viewport data
			_viewPort.x = viewportX;
			_viewPort.y = viewportY;
			_viewPort.w = viewportWidth;
			_viewPort.h = viewportHeight;
		}

		void setViewport(uint32 x, uint32 y, uint32 width, uint32 height)
		{
			globalShaderData.baseModelMatrix = Mat4();
			globalShaderData.projectionMatrix.orthographic(0, width, 0, height, -100, 100);
			glViewport(x, y, width, height);
			updateGlobalShaderData = true;
			
			// Update global viewport data
			_viewPort.x = x;
			_viewPort.y = y;
			_viewPort.w = width;
			_viewPort.h = height;
		}

		const Rectangle& getViewPort()
		{
			return _viewPort;
		}

		int32 spriteBatchInit()
		{
			clearGlError();
			// initialize fallback bitmap
			ldk::Bitmap fallbackBitmap = {};
			fallbackBitmap.bitsPerPixel = 32;
			fallbackBitmap.width = fallbackBitmap.height = 1;
			fallbackBitmap.bmpMemorySize_ = 4;
			//TODO: is it possible that at some poit someone try to release this memory ?

			spriteBatchData.fallbackBitmapData = 0xFFFFFFFF; // Default 1pixel white! ABRG
			spriteBatchData.fallbackBitmap = fallbackBitmap;
			spriteBatchData.fallbackBitmap.pixels = (uchar*) &spriteBatchData.fallbackBitmapData;

			glGenVertexArrays(1, &spriteBatchData.vao);

			// ARRAY buffer ---------------------------------------------
			glBindVertexArray(spriteBatchData.vao);

			// VERTEX buffer ---------------------------------------------
			render::GpuBufferLayout layout[] = {
				{SPRITE_ATTRIB_COLOR, 																					 // index
					render::GpuBufferLayout::Type::FLOAT32,  											 // type
					render::GpuBufferLayout::Size::X4,       											 // size
					SPRITE_BATCH_VERTEX_DATA_SIZE,          											 // stride
					0},  										                 											 // start

				{SPRITE_ATTRIB_VERTEX, 																					 // index
					render::GpuBufferLayout::Type::FLOAT32,												 // type
					render::GpuBufferLayout::Size::X3, 														 // size
					SPRITE_BATCH_VERTEX_DATA_SIZE, 																 // stride
					4 * sizeof(float)}, 																					 // start

				{SPRITE_ATTRIB_UV, 																							 // index
					render::GpuBufferLayout::Type::FLOAT32,  											 // type
					render::GpuBufferLayout::Size::X2,       											 // size
					SPRITE_BATCH_VERTEX_DATA_SIZE,          											 // stride
					(7 * sizeof(float))}, 																				 // start

				{SPRITE_ATTRIB_ZROTATION, 																			 // index
					render::GpuBufferLayout::Type::FLOAT32,  											 // type
					render::GpuBufferLayout::Size::X1,       											 // size
					SPRITE_BATCH_VERTEX_DATA_SIZE,          											 // stride
					(9 * sizeof(float))}};                  											 // start

			spriteBatchData.vertexBuffer = 
				render::createBuffer(render::GpuBuffer::Type::VERTEX_DYNAMIC, 	 // buffer type
						SPRITE_BATCH_MAX_SPRITES * sizeof(SpriteVertexData), 				 // buffer size
						layout, 																										 // buffer layout
						sizeof(layout)/sizeof(render::GpuBufferLayout));
			checkGlError();

			// INDEX buffer ---------------------------------------------
			// Precompute indices for every sprite
			GLushort indices[SPRITE_BATCH_INDICES_SIZE]={};
			int32 offset = 0;

			for(int32 i=0; i < SPRITE_BATCH_INDICES_SIZE; i+=6)
			{
				indices[i] 	 = offset;
				indices[i+1] = offset +1;
				indices[i+2] = offset +2;
				indices[i+3] = offset +2;
				indices[i+4] = offset +3;
				indices[i+5] = offset +0;

				offset+=4; // 4 offsets per sprite
			}

			spriteBatchData.indexBuffer = 
				render::createBuffer(render::GpuBuffer::Type::INDEX, 					 // buffer type
						SPRITE_BATCH_INDICES_SIZE * sizeof(uint16), 								 // buffer size
						nullptr, 																										 // buffer layout
						0, 																													 // layout count
						(void*)indices); 																						 // buffer data
			checkGlError();
		
			// Uniform buffer
			spriteBatchData.uniformBuffer = 
				render::createBuffer(render::GpuBuffer::Type::UNIFORM,					 // buffer type
						sizeof(globalShaderData), 																	 // buffer size
						nullptr, 																										 // buffer layout
						0, 																													 // layout count
						nullptr); 																						 // buffer data
			checkGlError();
			glBindVertexArray(0);

      //TODO: Marcio, this is a hack for testing stuff in 2D. Move this to material state
      glClearColor(1, 1, 1, 1);
#if _LDK_DEBUG_
      glEnable(GL_DEBUG_OUTPUT);
#endif

      // comput hashes for material parameters
      _materialOptionHashes.cullFront = ldk::stringToHash(LDK_MATERIAL_CULL_FRONT);    
      _materialOptionHashes.cullBack = ldk::stringToHash(LDK_MATERIAL_CULL_BACK);    
      _materialOptionHashes.cullNone = ldk::stringToHash(LDK_MATERIAL_CULL_NONE);    
      _materialOptionHashes.testLess = ldk::stringToHash(LDK_MATERIAL_TEST_LESS);    
      _materialOptionHashes.testLessEquals = ldk::stringToHash(LDK_MATERIAL_TEST_EQUALS);    
      _materialOptionHashes.testGreater = ldk::stringToHash(LDK_MATERIAL_TEST_GREATER);    
      _materialOptionHashes.testGreaterEquals = ldk::stringToHash(LDK_MATERIAL_TEST_GREATER_EQUALS);    
      _materialOptionHashes.testDifferent = ldk::stringToHash(LDK_MATERIAL_TEST_DIFFERENT);    
      _materialOptionHashes.testAlways = ldk::stringToHash(LDK_MATERIAL_TEST_ALWAYS);
      _materialOptionHashes.testNever = ldk::stringToHash(LDK_MATERIAL_TEST_NEVER);

     _materialOptionHashes.BlendZero =ldk::stringToHash(LDK_MATERIAL_BLEND_ZERO);
     _materialOptionHashes.BlendOne =ldk::stringToHash(LDK_MATERIAL_BLEND_ONE);
     _materialOptionHashes.BlendSrcColor =ldk::stringToHash(LDK_MATERIAL_BLEND_SRC_COLOR);
     _materialOptionHashes.BlendOneMinusSrcColor =ldk::stringToHash(LDK_MATERIAL_BLEND_ONE_MINUS_SRC_COLOR);
     _materialOptionHashes.BlendDstColor =ldk::stringToHash(LDK_MATERIAL_BLEND_DST_COLOR);
     _materialOptionHashes.BlendOneMinusDstColor =ldk::stringToHash(LDK_MATERIAL_BLEND_ONE_MINUS_DST_COLOR);
     _materialOptionHashes.BlendSrcAlpha =ldk::stringToHash(LDK_MATERIAL_BLEND_SRC_ALPHA);
     _materialOptionHashes.BlendOneMinusSrcAlpha =ldk::stringToHash(LDK_MATERIAL_BLEND_ONE_MINUS_SRC_ALPHA);
     _materialOptionHashes.BlendDstAlpha =ldk::stringToHash(LDK_MATERIAL_BLEND_DST_ALPHA);
     _materialOptionHashes.BlendOneMinusDstAlpha =ldk::stringToHash(LDK_MATERIAL_BLEND_ONE_MINUS_DST_ALPHA);
     _materialOptionHashes.BlendConstColor =ldk::stringToHash(LDK_MATERIAL_BLEND_CONST_COLOR);
     _materialOptionHashes.BlendOneMinusConstColor =ldk::stringToHash(LDK_MATERIAL_BLEND_ONE_MINUS_CONST_COLOR);
     _materialOptionHashes.BlendConstAlpha =ldk::stringToHash(LDK_MATERIAL_BLEND_CONST_ALPHA);
     _materialOptionHashes.BlendOneMinusConstAlpha =ldk::stringToHash(LDK_MATERIAL_BLEND_ONE_MINUS_CONST_ALPHA);

      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glEnable(GL_BLEND);
      glDepthMask(GL_TRUE);

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //    glLineWidth(1.0);
      //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      //    glPolygonOffset(-1,-1);
      return 1;
    }

    void spriteBatchBegin(const ldk::Material& material)
    { 
      clearGlError();
      spriteBatchData.material = material;
      spriteBatchData.spriteCount = 0;

      if (updateGlobalShaderData)
      {
        // Set global uniform data
        render::bindBuffer(spriteBatchData.uniformBuffer);
        render::setBufferData(spriteBatchData.uniformBuffer, &globalShaderData, sizeof(globalShaderData));

        // Bind the global uniform buffer to the 'ldk' global 
        unsigned int block_index = glGetUniformBlockIndex(material.shader, "ldk");
        const GLuint bindingPointIndex = 0;
        glUniformBlockBinding(material.shader, block_index, bindingPointIndex);
        checkGlError();

        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPointIndex, spriteBatchData.uniformBuffer.id);
        checkGlError();

        updateGlobalShaderData = false;
      }

      // set pipeline sates
      glCullFace(material.cullMode);
      if (material.depthFunc == -1)
      {
        glDisable(GL_DEPTH_TEST);
      } 
      else
      {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(material.depthFunc);
      }

      //TODO:implement blend func parameter
      glBlendFunc(material.blendFuncSrc, material.blendFuncDst);
      glBlendColor( material.blendConstantColor.x,
          material.blendConstantColor.y,
          material.blendConstantColor.z,
          material.blendConstantColor.w);

      //TODO: implement stencil values
      //glStencilFunc(material.stencilFunc,???,???);

      LDK_ASSERT(checkGlError(), "GL ERROR!");
      render::bindBuffer(spriteBatchData.vertexBuffer);
    }

		void spriteBatchSubmit(const Sprite& sprite)
		{
			clearGlError();
			Material& material = spriteBatchData.material;
			// sprite vertex order 0,1,2,2,3,0
			// 1 -- 2
			// |    |
			// 0 -- 3

			// map pixel coord to texture space
			Rectangle uvRect;
			uvRect.x = sprite.srcRect.x / material.texture.width;
			uvRect.w = sprite.srcRect.w / material.texture.width;
			uvRect.y = 1 - (sprite.srcRect.y / material.texture.height); // origin is top-left corner
			uvRect.h = sprite.srcRect.h / material.texture.height;

			float angle = sprite.angle;
			float halfWidth = sprite.width/2;
			float halfHeight = sprite.height/2;
			float s = sin(sprite.angle);
			float c = cos(sprite.angle);
			float z = sprite.position.z;

			SpriteVertexData vertices[4];
			SpriteVertexData* vertexData = vertices;

			// top left
			vertexData->color = sprite.color;
			vertexData->uv = { uvRect.x, uvRect.y};
			vertexData->zRotation = angle;	
			float x = -halfWidth;
			float y = halfHeight;
			vertexData->position = 
				Vec3{(x * c - y * s) + sprite.position.x, (x * s + y * c) + sprite.position.y,	z};
			vertexData++;

			// bottom left
			vertexData->color = sprite.color;
			vertexData->uv = { uvRect.x, uvRect.y - uvRect.h};
			vertexData->zRotation = angle;	
			x = -halfWidth;
			y = -halfHeight;
			vertexData->position = 
				Vec3{(x * c - y * s) + sprite.position.x, (x * s + y * c) + sprite.position.y, z};
			vertexData++;

			// bottom right
			vertexData->color = sprite.color;
			vertexData->uv = {uvRect.x + uvRect.w, uvRect.y - uvRect.h};
			vertexData->zRotation = angle;
			x = halfWidth;
			y = -halfHeight;
			vertexData->position = 	
				Vec3{(x * c - y * s) + sprite.position.x, (x * s + y * c) + sprite.position.y,	z};
			vertexData++;

			// top right
			vertexData->color = sprite.color;
			vertexData->uv = { uvRect.x + uvRect.w, uvRect.y};
			vertexData->zRotation = angle;
			x = halfWidth;
			y = halfHeight;
			vertexData->position = 
				Vec3{(x * c - y * s) + sprite.position.x, (x * s + y * c) + sprite.position.y,	z};

			render::setBufferSubData(spriteBatchData.vertexBuffer,
					(void*)vertices, 
					sizeof(vertices),
					spriteBatchData.spriteCount * sizeof(vertices));

			checkGlError();
			spriteBatchData.spriteCount++;
		}

		void spriteBatchFlush()
		{
			//TODO(marcio): Flush buffer when sprite buffer is full and game is still pushing sprites
		}

		void spriteBatchEnd()
		{
			clearGlError();
			glBindTexture(GL_TEXTURE_2D, spriteBatchData.material.texture.id);
			glUseProgram(spriteBatchData.material.shader);
			glBindVertexArray(spriteBatchData.vao);
			render::bindBuffer(spriteBatchData.indexBuffer);
			// Draw
			glDrawElements(GL_TRIANGLES, 6 * spriteBatchData.spriteCount, GL_UNSIGNED_SHORT, 0);

			checkGlError();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			checkGlError();
			//TODO: sort draw calls per material 
		}

    void spriteBatchSetFont(const ldk::FontAsset& font)
    {
      fontAsset = font;
    }

    Vec2 spriteBatchText(Vec3& position, float scale, Vec4& color, const char* text)
    {
      if (fontAsset.gliphData == nullptr)
        return {-1,-1};

      char c;
      const char* ptrChar = text;
      Sprite sprite;
      sprite.color = color;

      ldk::FontGliphRect* gliphList = fontAsset.gliphData;
      uint32 advance = 0;

      if ( scale < 0 ) scale = 1.0f;

      Vec2 textSize = {};
      //submit each character as an individual sprite
      while ((c = *ptrChar) != 0)
      {
        FontGliphRect* gliph;

        // avoid indexing undefined characters
        if (c < fontAsset.firstCodePoint || c > fontAsset.lastCodePoint)
        {
          gliph = &(gliphList[fontAsset.defaultCodePoint]);
        }
        else
        {
          c = c - fontAsset.firstCodePoint;
          gliph = &(gliphList[c]);
        }

        //calculate advance
        sprite.position = position;
        sprite.position.x += advance;
        advance += gliph->w * scale;
        sprite.width = gliph->w * scale; 
        sprite.height = gliph->h * scale;
        sprite.srcRect = {gliph->x, gliph->y, gliph->w, gliph->h};
        ++ptrChar;
        render::spriteBatchSubmit(sprite);

        //TODO: account for multi line text
        textSize.x += sprite.width;
        textSize.y = MAX(textSize.y, sprite.height); 	
      }
      return textSize;
    }
	} // namespace render
} // namespace ldk


