
namespace ldk
{
	namespace render
	{
		static inline GLenum getGlEnum(render::GpuBuffer::Type type)
		{
				switch(type)
				{
					case GpuBuffer::Type::VERTEX:
					case GpuBuffer::Type::VERTEX_DYNAMIC:
					case GpuBuffer::Type::VERTEX_STREAM:
						return GL_ARRAY_BUFFER;

					case GpuBuffer::Type::INDEX:
						return GL_ELEMENT_ARRAY_BUFFER;

					case GpuBuffer::Type::UNIFORM:
						return GL_UNIFORM_BUFFER;
						break;
					default:
						return GL_INVALID_ENUM;
				};

		}

		static inline GLenum getGlEnum(render::GpuBufferLayout::Type type)
		{
			switch(type)
			{
				case render::GpuBufferLayout::Type::INT8:
					return GL_BYTE;
				case render::GpuBufferLayout::Type::UINT8:
					return GL_UNSIGNED_BYTE;
				case render::GpuBufferLayout::Type::INT16:
					return GL_SHORT;
				case render::GpuBufferLayout::Type::UINT16:
					return GL_UNSIGNED_SHORT;
				case render::GpuBufferLayout::Type::FLOAT32:
					return GL_FLOAT;
				default:
						return GL_INVALID_ENUM;
			}
		}

		static inline GLenum getGlBufferUsage(render::GpuBuffer::Type type)
		{
			switch(type)
				{
					case GpuBuffer::Type::INDEX:
					case GpuBuffer::Type::VERTEX:
						return GL_STATIC_DRAW;
					
					case GpuBuffer::Type::UNIFORM:
					case GpuBuffer::Type::VERTEX_DYNAMIC:
						return GL_DYNAMIC_DRAW;
					
					case GpuBuffer::Type::VERTEX_STREAM:
						return GL_STREAM_DRAW;
					
					default:
						return GL_INVALID_ENUM;
				};

		}

		render::GpuBuffer createBuffer(render::GpuBuffer::Type type, 
				size_t size, const render::GpuBufferLayout* layout, uint32 layoutCount, void* data)
		{
			// find ideal buffer target and memory strategy for the given buffer type
			render::GpuBuffer buffer;
			buffer.target = getGlEnum(type);
			buffer.usage = getGlBufferUsage(type);
			
			glGenBuffers(1, &buffer.id);
			glBindBuffer(buffer.target, buffer.id);

			// set buffer layout
			for (uint32 i = 0; i < layoutCount; i++)
			{
				const render::GpuBufferLayout& attrib = layout[i];
				LogInfo("Enabling attrib #%d" , attrib.index);
				glEnableVertexAttribArray(attrib.index);

				GLenum type = getGlEnum(attrib.type);
				if ( type == GL_INVALID_ENUM )
				{
					LogError("Invalid attribute type for buffer layout");
				}

				glVertexAttribPointer(attrib.index, attrib.size, type, GL_FALSE,
						(GLsizei) attrib.stride,
						(const GLvoid*) attrib.start);
			}
			
			glBufferData(buffer.target, size, (const GLvoid*) data, buffer.usage);
			glBindBuffer(buffer.target, 0);
			return buffer;
		}

		void setBufferSubData(const render::GpuBuffer& buffer, void* data, size_t dataSize, uint32 offset)
		{
			glBufferSubData(buffer.target, offset, dataSize, (const GLvoid*) data);
		}

		void setBufferData(const render::GpuBuffer& buffer, void* data, size_t dataSize)
		{
			glBufferData(buffer.target, dataSize, (const GLvoid*) data, buffer.usage);
		}

		inline void bindBuffer(const render::GpuBuffer& buffer)
		{
			glBindBuffer(buffer.target, buffer.id);
		}

		inline void unbindBuffer(const render::GpuBuffer& buffer)
		{
			glBindBuffer(buffer.target, 0);
		}

		void deleteBuffer(render::GpuBuffer& buffer)
		{
			glDeleteBuffers(1, &buffer.id);
			buffer.id = -1;
		}
	}
}

