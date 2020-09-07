#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include "Graphics/Renderer/Texture2D.h"
#include "Graphics/Renderer/VertexBuffer.h"
#include "Graphics/Renderer/BlendState.h"
#include <glm/glm.hpp>
#include <map>
#include "vertex.h"
#include "Renderer.h"

class FontRenderer {
private:
	struct glyph_info {
		float x0, y0, x1, y1;	// coords of glyph in the texture atlas
		int x_off, y_off;		// left & top bearing when rendering
		int advance;			// x advance when rendering
	} info[128];

	VertexBuffer* vbuffer = nullptr;
	std::unique_ptr<Texture2D> font_atlas;
	BlendState* blend_state = nullptr;
public:
	FontRenderer()
	{
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		FT_Face face;
		if (FT_New_Face(ft, "font/Roboto-Medium.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(face, 0, 48);

		Texture2D* texture;
		
		std::vector<unsigned char> data;

		int max_dim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(128));
		int tex_width = 1;
		while (tex_width < max_dim) tex_width <<= 1;
		int tex_height = tex_width;

		// render glyphs to atlas
		char* pixels = (char*)calloc(tex_width * tex_height, 1);
		int pen_x = 0, pen_y = 0;

		for (int i = 0; i < 128; ++i) {
			FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LIGHT);
			FT_Bitmap* bmp = &face->glyph->bitmap;

			if (pen_x + bmp->width >= tex_width) {
				pen_x = 0;
				pen_y += ((face->size->metrics.height >> 6) + 1);
			}

			for (int row = 0; row < bmp->rows; ++row) {
				for (int col = 0; col < bmp->width; ++col) {
					int x = pen_x + col;
					int y = pen_y + row;
					pixels[y * tex_width + x] = bmp->buffer[row * bmp->pitch + col];
				}
			}

			// this is stuff you'd need when rendering individual glyphs out of the atlas
			info[i].x0 = pen_x;
			info[i].y0 = pen_y;
			info[i].x1 = pen_x + bmp->width;
			info[i].y1 = pen_y + bmp->rows;

			info[i].x0 = info[i].x0 / tex_width;
			info[i].y0 = info[i].y0 / tex_width;
			info[i].x1 = info[i].x1 / tex_width;
			info[i].y1 = info[i].y1 / tex_width;

			info[i].x_off = face->glyph->bitmap_left;
			info[i].y_off = face->glyph->bitmap_top;
			info[i].advance = face->glyph->advance.x >> 6;

			pen_x += bmp->width + 1;
		}

		font_atlas.reset(Texture2D::create(
			tex_width, 
			tex_height, 
			1, 
			pixels, 
			TextureFormat::R8, 
			TextureFlags::TF_ShaderResource
		));

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
		
		BlendStateDesc blendStateDesc;
		blendStateDesc.count = 1;

		blendStateDesc.blend[0].blendEnabled = true;
		blendStateDesc.blend[0].blendOp			= BlendOperation::ADD;
		blendStateDesc.blend[0].srcBlend		= BlendFunction::SRC_APLHA;
		blendStateDesc.blend[0].dstBlend		= BlendFunction::INV_SRC_APLHA;

		blendStateDesc.blend[0].blendOpAlpha	= BlendOperation::ADD;
		blendStateDesc.blend[0].srcBlendAlpha	= BlendFunction::ZERO;
		blendStateDesc.blend[0].dstBlendAlpha	= BlendFunction::ZERO;

		blend_state = BlendState::create(blendStateDesc);
	}

	void renderFont(const std::string& text, float x, float y, float scale)
	{
		Renderer* renderer = Renderer::instance();
		RenderContext* context = renderer->getContext();

		blend_state->bind(context);
		font_atlas->bindToUnit(0, context);

		/*temp->bindToUnit(0, context);
		renderer->draw(6);*/

		float xpos = x;
		float ypos = y;

		for (auto& ch : text) {
			glyph_info& character = info[ch];

			float w = (character.x1 - character.x0) * scale * 0.8f;
			float h = (character.y1 - character.y0) * scale;

			vertex vertices[6] = {
				{ xpos,     ypos + h, 0.0f,  character.x0, character.y0 },
				{ xpos,     ypos,     0.0f,  character.x0, character.y1 },
				{ xpos + w, ypos,     0.0f,  character.x1, character.y1 },

				{ xpos,     ypos + h, 0.0f,  character.x0, character.y0 },
				{ xpos + w, ypos,     0.0f,  character.x1, character.y1 },
				{ xpos + w, ypos + h, 0.0f,  character.x1, character.y0 }
			};

			vbuffer->update(vertices, 6, sizeof(vertex));
			vbuffer->bind(context);
			renderer->draw(6);

			xpos += ((float)character.advance / 1024.0f) * scale * 0.7f;
		}

		renderer->setDefaultBlendState();
		return;

		for (auto& ch : text) {	
			glyph_info& character = info[ch];

			float xpos = x + character.x_off;
			float ypos = y - character.y_off;

			float w = (character.x0 - character.x1) * scale;
			float h = (character.y0 - character.y1) * scale;

			vertex vertices[6] = {
				{ xpos,     ypos + h, 0.0f,  character.x0, character.y0 },
				{ xpos,     ypos,     0.0f,  character.x0, character.y1 },
				{ xpos + w, ypos,     0.0f,  character.x1, character.y1 },

				{ xpos,     ypos + h, 0.0f,  character.x0, character.y0 },
				{ xpos + w, ypos,     0.0f,  character.x1, character.y1 },
				{ xpos + w, ypos + h, 0.0f,  character.x1, character.y0 }
			};

			vbuffer->update(vertices, 6, sizeof(vertex));
			vbuffer->bind(context);
			renderer->draw(6);

			x += (character.advance >> 6) * scale;
		}
	}
};