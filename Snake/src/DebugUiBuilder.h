#pragma once

#include "UI/UiSystem.h"
#include "UI/Widgets/UiBlock.h"
#include "Import/Texture/RawTexture.h"
#include "UI/UiIcon.h"
#include "UI/UiIconMenu.h"

class DebugUiBuilder
{
public:
	void build(UiSystem* ui_system)
	{
		UiBlock* block = new UiBlock();
		{
			block->color = glm::vec4(0.9019607, 0.9215686, 0.91764705, 1.0);
			block->bg_color = glm::vec4(0.9019607, 0.9215686, 0.91764705, 1.0);

			UiConstraints constraints;
			constraints.setWidth({ ValueType::Relative, 100 });
			constraints.setHeight({ ValueType::Absolute, 20 });
			constraints.setXPos({ XPosition::Center });
			constraints.setYPos({ YPosition::Bottom });

			ui_system->add(ui_system->getRootElement(), block, constraints);
		}
		
		UiBlock* entity_list = new UiBlock();
		entity_list->visible = false;

		{
			entity_list->color = glm::vec4(0.9019607, 0.9215686, 0.91764705, 1.0);
			entity_list->bg_color = glm::vec4(0.9019607, 0.9215686, 0.91764705, 1.0);

			UiConstraints constraints2;
			constraints2.setWidth({ ValueType::Relative, 20 });
			constraints2.setHeight({ ValueType::Relative, 20 });
			constraints2.setXPos({ XPosition::Center });
			constraints2.setYPos({ YPosition::Center });

			ui_system->add(ui_system->getRootElement(), entity_list, constraints2);
		}

		buildUiList(ui_system);
	}

	void buildUiList(UiSystem* ui_system)
	{
		UiIconMenu* block1 = new UiIconMenu();
		{
			block1->bg_color = glm::vec4(0.9019607, 0.9215686, 0.91764705, 1.0);

			UiConstraints constraints2;
			constraints2.setWidth({ ValueType::Relative, 2 });
			constraints2.setHeight({ ValueType::Relative, 60 });
			constraints2.setXPos({ XPosition::Left });
			constraints2.setYPos({ YPosition::Center });

			ui_system->add(ui_system->getRootElement(), block1, constraints2);
		}

		{
			RawTexture raw_icon_texture("icon/pencil.png");
			raw_icon_texture.import();

			Texture2D* icon_texture = Texture2D::create(
				raw_icon_texture.getWidth(),
				raw_icon_texture.getHeight(), 0,
				raw_icon_texture.getData(),
				TextureFormat::RGBA8,
				TextureFlags::TF_ShaderResource | TextureFlags::TF_GenerateMips,
				2
			);
			icon_texture->generateMips();

			UiIcon* icon = new UiIcon(icon_texture);
			{
				icon->color = glm::vec4(0.9019607, 0.9215686, 0.91764705, 1.0);
				icon->bg_color = glm::vec4(0.9019607, 0.9215686, 0.91764705, 1.0);

				UiConstraints constraints;
				constraints.setWidth({ ValueType::Absolute, 42 });
				constraints.setHeight({ ValueType::Absolute, 42 });
				constraints.setXPos({ XPosition::Center });
				constraints.setYPos({ YPosition::Top });

				ui_system->add(block1, icon, constraints);
			}
		}
		{
			RawTexture raw_icon_texture1("icon/entity.png");
			raw_icon_texture1.import();

			Texture2D* icon_texture1 = Texture2D::create(
				raw_icon_texture1.getWidth(),
				raw_icon_texture1.getHeight(), 0,
				raw_icon_texture1.getData(),
				TextureFormat::RGBA8,
				TextureFlags::TF_ShaderResource | TextureFlags::TF_GenerateMips,
				2
			);
			icon_texture1->generateMips();

			UiConstraints constraints1;
			constraints1.setWidth({ ValueType::Absolute, 42 });
			constraints1.setHeight({ ValueType::Absolute, 42 });
			constraints1.setXPos({ XPosition::Center });
			constraints1.setYPos({ YPosition::Top });

			UiIcon* icon1 = new UiIcon(icon_texture1);
			ui_system->add(block1, icon1, constraints1);
		}
	}
};