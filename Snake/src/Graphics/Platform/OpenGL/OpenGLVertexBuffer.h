#include "Graphics/Renderer/VertexBuffer.h"

class OpenGLVertexBuffer : public VertexBuffer {
private:
    unsigned int vao = 0;
    unsigned int vbo = 0;
public:
    OpenGLVertexBuffer(int size, int stride, void* data);
	virtual void bind(RenderContext* context) override;
};