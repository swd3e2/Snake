#pragma once

enum class RendererCommandType {
	DrawIndexed
};

class RendererCommand {
public:
	virtual RendererCommandType getType() = 0;
};

class DrawIndexed : public RendererCommand {
public:
	virtual RendererCommandType getType() {
		return RendererCommandType::DrawIndexed;
	}
};