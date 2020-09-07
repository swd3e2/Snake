#pragma once

#include "UI/UiElement.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/vertex.h"
#include <iostream>
#include "UI/UiRenderer.h"

class UiBlock : public UiElement
{
private:
	bool hovered = false;
public:
	virtual void render(UiRenderer* ui_renderer) override
	{
		Renderer* renderer = ui_renderer->renderer;

		ColorBufferData data = { color, bg_color, false, false };
		ui_renderer->buffer->update(&data);
		ui_renderer->buffer->bind(renderer->getContext());

		vBuffer->bind(renderer->getContext());
		renderer->draw(6);

		// Render stuff
		UiElement::render(ui_renderer);
	}

	virtual void update(const MousePos& mouse_pos) override
	{
		if (mouse_pos.x > x_start && mouse_pos.x < x_start + width && mouse_pos.y > y_start && mouse_pos.y < y_start + height) {
			hovered = true;
		} else {
			hovered = false;
		}

		UiElement::update(mouse_pos);
	}
};