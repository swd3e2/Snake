#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include "Graphics/Renderer/Texture2D.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include <glm/glm.hpp>
#include <map>
#include "vertex.h"
#include "Renderer.h"

class FontRenderer {
private:
	VertexBuffer* vbuffer = nullptr;

	struct Character {
		Texture2D* texture; // ID текстуры глифа
		glm::ivec2 Size;      // Размеры глифа
		glm::ivec2 Bearing;   // Смещение верхней левой точки глифа
		int Advance;   // Горизонтальное смещение до начала следующего глифа
	};

	std::map<char, Character> Characters;
public:
	FontRenderer()
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		FT_Face face;
		if (FT_New_Face(ft, "font/PTS55F.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(face, 0, 48);
		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;


		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			Texture2D* texture = Texture2D::create(face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, face->glyph->bitmap.buffer, TextureFormat::R8, TextureFlags::TF_RenderTarget | TextureFlags::TF_ShaderResource, 1);

			// Now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters[c] = character;
		}

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		vertex vertices[6] = { 
			vertex(1.0f,  1.0f, 0.0f, 1.0f, 0.0f),
			vertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f),
			vertex(-1.0f,  1.0f, 0.0f, 0.0f, 0.0f),

			vertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f),
			vertex(-1.0f,  1.0f, 0.0f, 0.0f, 0.0f),
			vertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f), 
		};
		vbuffer = VertexBuffer::create(6, sizeof(vertex), vertices, true);
	}

	void renderFont(const std::string& text, float x, float y, float scale)
	{
		Renderer* renderer = Renderer::instance();
		RenderContext* context = renderer->getContext();

		for (auto& ch : text) {	
			Character& character = Characters[ch];

			float xpos = x + character.Bearing.x * scale;
			float ypos = y - (character.Size.y - character.Bearing.y) * scale;

			float w = character.Size.x * scale;
			float h = character.Size.y * scale;

			vertex vertices[6] = {
				{ xpos,     ypos + h, 0.0f,  0.0f, 0.0f },
				{ xpos,     ypos,     0.0f,  0.0f, 1.0f },
				{ xpos + w, ypos,     0.0f,  1.0f, 1.0f },

				{ xpos,     ypos + h, 0.0f,  0.0f, 0.0f },
				{ xpos + w, ypos,     0.0f,  1.0f, 1.0f },
				{ xpos + w, ypos + h, 0.0f,  1.0f, 0.0f }
			};

			vbuffer->update(vertices, 6, sizeof(vertex));
			vbuffer->bind(context);
			character.texture->bindToUnit(0, context);
			renderer->draw(6);

			x += (character.Advance >> 6) * scale;
		}
	}
};