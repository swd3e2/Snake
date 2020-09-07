#pragma once

#include "Input/InputManager.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Graphics/Renderer.h"
#include "EventSystem/EventSystem.h"
#include <functional>

class UiRenderer;

class UiElement
{
public:
	UiElement* parent;
	std::vector<UiElement*> childs;
	glm::vec4 color;
	glm::vec4 bg_color;

	std::unique_ptr<VertexBuffer> vBuffer;

	int width, height;
	int x_start, y_start;

	int internal_width_occupied = 0;
	int internal_height_occupied = 0;

	std::function<void()> onHover;
	std::function<void()> onClick;

	bool hovered = false;
	bool visible = true;
public:
	UiElement()
	{
		EventSystem::instance()->addEventListener<UiElement, LeftMouseClickEvent>(new ClassEventDelegate<UiElement>(this, &UiElement::onMouseClick));
	}

	virtual void render(UiRenderer* renderer) {}

	virtual void update(const MousePos& mouse_pos)
	{
		if (mouse_pos.x > x_start && mouse_pos.x < x_start + width && mouse_pos.y > y_start && mouse_pos.y < y_start + height) {
			hovered = true;
			if (onHover) {
				onHover();
			} 
		} else {
			hovered = false;
		}

		for (auto& it : childs) {
			it->update(mouse_pos);
		}
	}

	void add(UiElement* element) 
	{
		childs.push_back(element);
		internal_height_occupied += element->height;
	}

	void onMouseClick(Event* event)
	{
		if (hovered && onClick) {
			onClick();
		}
	}
};