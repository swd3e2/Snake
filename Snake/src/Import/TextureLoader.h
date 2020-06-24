#pragma once

#include "Common/Singleton.h"
#include "Common/Storage.h"
#include "Texture/RawTexture.h"
#include "Graphics/Renderer/Texture2D.h"
#include "FileSystem/File.h"

class TextureLoader : public Singleton<TextureLoader> {
private:
	Storage<Texture2D> storage;
public:
	TextureLoader() {
		storage.startUp();
	}

	std::shared_ptr<Texture2D> loadFromFile(const std::string& filename) {
		if (filename.size() == 0) {
			return nullptr;
		}

		File file(filename);
		if (file.exists()) {
			return nullptr;
		}

		if (storage.has(filename)) {
			return storage.get(filename);
		} else {
			return this->loadTexture(filename);
		}
	}

	std::shared_ptr<Texture2D> loadTexture(const std::string& filename) {
		std::shared_ptr<RawTexture> importTtexture = std::make_shared<RawTexture>(filename);
		importTtexture->import();

		std::shared_ptr<Texture2D> texture;
		texture.reset(Texture2D::create(
			importTtexture->getWidth(),
			importTtexture->getHeight(),
			0,
			importTtexture->getData(),
			importTtexture->getChannels() == 4 ? TextureFormat::RGBA8 : TextureFormat::RGB8
		));

		return texture;
	}
};