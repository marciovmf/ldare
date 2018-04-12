
namespace ldk
{
	namespace renderer
	{
		static inline GLenum getGlEnum(renderer::GpuBuffer::Type type)
		{
				switch(type)
				{
					case Buffer::Type::VERTEX:
					case Buffer::Type::VERTEX_DYNAMIC:
					case Buffer::Type::VERTEX_STREAM:
						return GL_ARRAY_BUFFER;

					case Buffer::Type::INDEX:
						return GL_ELEMENT_ARRAY_BUFFER;

					case Buffer::Type::UNIFORM:
						return GL_UNIFORM_BUFFER;
						break;
					default:
						return GL_INVALID_ENUM;
				};

		}

		static inline GLenum getGlEnum(renderer::GpuBufferLayout::Type type)
		{
			switch(type)
			{
				case renderer::GpuBufferLayout::Type::INT8:
					return GL_BYTE;
				case renderer::GpuBufferLayout::Type::UINT8:
					return GL_UNSIGNED_BYTE;
				case renderer::GpuBufferLayout::Type::INT16:
					return GL_SHORT;
				case renderer::GpuBufferLayout::Type::UINT16:
					return GL_UNSIGNED_SHORT;
				case renderer::GpuBufferLayout::Type::FLOAT32:
					return GL_FLOAT;
				default:
						return GL_INVALID_ENUM;
			}
		}

		static inline GLenum getGlBufferUsage(renderer::GpuBuffer::Type type)
		{
			switch(type)
				{
					case Buffer::Type::INDEX:
					case Buffer::Type::VERTEX:
						return GL_STATIC_DRAW;
					
					case Buffer::Type::UNIFORM:
					case Buffer::Type::VERTEX_DYNAMIC:
						return GL_DYNAMIC_DRAW;
					
					case Buffer::Type::VERTEX_STREAM:
						return GL_STREAM_DRAW;
					
					default:
						return GL_INVALID_ENUM;
				};

		}

		renderer::GpuBuffer createBuffer(renderer::GpuBuffer::Type type, 
				size_t size, const renderer::GpuBufferLayout* layout, uint32 layoutCount, void* data)
		{
			// find ideal buffer target and memory strategy for the given buffer type
			renderer::GpuBuffer buffer;
			buffer.GL.target = getGlEnum(type);
			buffer.GL.usage = getGlBufferUsage(type);
			
			glGenBuffers(1, &buffer.GL.id);
			glBindBuffer(buffer.GL.target, buffer.GL.id);

			// set buffer layout
			for (uint32 i = 0; i < layoutCount; i++)
			{
				const BufferLayout& attrib = layout[i];
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
			
			glBufferData(buffer.GL.target, size, (const GLvoid*) data, buffer.GL.usage);
			glBindBuffer(buffer.GL.target, 0);
			return buffer;
		}

		void setBufferData(const renderer::GpuBuffer& buffer, void* data, size_t dataSize, uint32 offset)
		{
			glBufferSubData(buffer.GL.target, offset, dataSize, (const GLvoid*) data);
		}

		void setBufferData(const renderer::GpuBuffer& buffer, void* data, size_t dataSize)
		{
			glBufferData(buffer.GL.target, dataSize, (const GLvoid*) data, buffer.GL.usage);
		}

		inline void bindBuffer(const renderer::GpuBuffer& buffer)
		{
			glBindBuffer(buffer.GL.target, buffer.GL.id);
		}

		inline void unbindBuffer(const renderer::GpuBuffer& buffer)
		{
			glBindBuffer(buffer.GL.target, 0);
		}

		void deleteBuffer(renderer::GpuBuffer& buffer)
		{
			glDeleteBuffers(1, &buffer.GL.id);
			buffer.GL.id = -1;
		}
	}
}

