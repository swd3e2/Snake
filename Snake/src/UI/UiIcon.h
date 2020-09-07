#pragma once

#include "UiElement.h"
#include "Graphics/Renderer/Texture2D.h"
#include "UI/UiRenderer.h"

class UiIcon : public UiElement
{
public:
	bool hovered = false;
	bool selected = false;
private:
	std::unique_ptr<Texture2D> icon_texture;
public:
	UiIcon(Texture2D* texture)
	{
		icon_texture.reset(texture);
		EventSystem::instance()->addEventListener<UiIcon, LeftMouseClickEvent>(new ClassEventDelegate(this, &UiIcon::onLeftMouseClick));
	}

	virtual void render(UiRenderer* ui_renderer) override
	{
		if (!visible) return;

		Renderer* renderer = ui_renderer->renderer;

		ui_renderer->ui_icon_shader->bind(renderer->getContext());

		ColorBufferData data = { color, bg_color, true, hovered, selected };
		ui_renderer->buffer->update(&data);
		ui_renderer->buffer->bind(renderer->getContext());

		ui_renderer->blend_state->bind(renderer->getContext());
		vBuffer->bind(ui_renderer->renderer->getContext());
		icon_texture->bindToUnit(0, ui_renderer->renderer->getContext());

		renderer->draw(6);

		ui_renderer->ui_shader->bind(renderer->getContext());
		renderer->setDefaultBlendState();
	}

	void onLeftMouseClick(Event* event)
	{
		if (hovered) {
			selected = true;
			return;
		} else {
			selected = false;
		}
	}
};