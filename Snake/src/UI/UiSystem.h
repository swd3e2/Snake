#pragma once

#include "UiElement.h"
#include "UiRenderer.h"
#include "Input/InputManager.h"
#include "UiDisplay.h"
#include "UiConstraints.h"
#include <vector>
#include "ApplicationSettings.h"
#include "Graphics/vertex.h"
#include "EventSystem/EventSystem.h"

class UiSystem
{
private:
	InputManager* input_manager = nullptr;
	UiRenderer* ui_renderer = nullptr;
	UiElement* root = nullptr;
	ApplicationSettings* settings = nullptr;

	std::vector<UiElement*> elements;
public:
	UiSystem(Renderer* renderer, InputManager* inputManager, ApplicationSettings* settings) : input_manager(inputManager), settings(settings)
	{
		ui_renderer = new UiRenderer(renderer);
		root = new UiDisplay();

		root->width = settings->getWindowWidth();
		root->height = settings->getWindowHeight();
		root->y_start = 0;
		root->x_start = 0;
	}

	inline UiElement* getRootElement() { return root; }

	void update()
	{
		MousePos pos = input_manager->getMousePoistion();
		pos.y = settings->getWindowHeight() - pos.y;

		root->update(pos);
	}

	void render()
	{
		ui_renderer->render(root);
	}

	void add(UiElement* parent, UiElement* child, const UiConstraints& constraints)
	{
		child->width = constraints.getWidth().getNormalizedValue(parent->width);
		child->height = constraints.getHeight().getNormalizedValue(parent->height);

		child->x_start = constraints.getXPos().getNormalizedValue(parent->x_start, parent->width, child->width);
		child->y_start = constraints.getYPos().getNormalizedValue(parent->y_start, parent->height, child->height, parent->internal_height_occupied);

		float new_x_start = ((float)child->x_start / settings->getWindowWidth()) * 2.0f - 1.0f;
		float new_y_start = ((float)child->y_start / settings->getWindowHeight()) * 2.0f - 1.0f;
		float new_width = (float)child->width / settings->getWindowWidth() * 2.0f;
		float new_height = (float)child->height / settings->getWindowHeight() * 2.0f;

		vertex vertices[] = {
			vertex(new_x_start + new_width,  new_y_start + new_height,	0.0f, 1.0f, 0.0f),
			vertex(new_x_start + new_width,  new_y_start,				0.0f, 1.0f, 1.0f),
			vertex(new_x_start,				 new_y_start + new_height,	0.0f, 0.0f, 0.0f),
			
			vertex(new_x_start,				 new_y_start,				0.0f, 0.0f, 1.0f),
			vertex(new_x_start,				 new_y_start + new_height,	0.0f, 0.0f, 0.0f),
			vertex(new_x_start + new_width,	 new_y_start,				0.0f, 1.0f, 1.0f),
		};

		child->vBuffer.reset(VertexBuffer::create(6, sizeof(vertex), vertices));
		child->parent = parent;

		parent->add(child);

		elements.push_back(child);
	}
};