#include "Graphics/Renderer/ConstantBuffer.h"

class OpenGLConstantBuffer : public ConstantBuffer 
{
private:
	unsigned int ubo = 0;
public:
    OpenGLConstantBuffer(int position, size_t size, void* data);
	virtual void update(void* data) override;
	virtual void bind(RenderContext* context) override;
};