#pragma once

#include "UiElement.h"
#include <vector>

class UiIconMenu : public UiElement
{
public:
	UiIconMenu()
	{
		EventSystem::instance()->addEventListener<UiIconMenu, LeftMouseClickEvent>(new ClassEventDelegate(this, &UiIconMenu::onLeftMouseClick));
	}


	virtual void render(UiRenderer* ui_renderer)
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

	void onLeftMouseClick(Event* event)
	{
		
	}
};