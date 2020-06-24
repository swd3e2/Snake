#pragma once

#include "Common/Singleton.h"
#include "Common/Storage.h"
#include "Texture/RawTexture.h"
#include "Graphics/Renderer/Texture2D.h"
#include "FileSystem/File.h"
#include "Configuration.h"

class TextureLoader : public Singleton<TextureLoader> {
private:
	Storage<Texture2D> storage;
	std::string assetFolder;
public:
	std::shared_ptr<Texture2D> loadFromFile(const std::string& filename) {
		if (filename.size() == 0) {
			return nullptr;
		}

		const std::string assetFolder = getAssetFolder();
		const std::string fullFilePath = assetFolder + "/" + filename;
		File file(fullFilePath);
		if (!file.exists()) {
			return nullptr;
		}

		if (storage.has(fullFilePath)) {
			return storage.get(fullFilePath);
		} else {
			return this->loadTexture(fullFilePath);
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
			importTtexture->getChannels() == 4 ? TextureFormat::RGBA8 : TextureFormat::RGB8,
			TextureFlags::TF_ShaderResource
		));

		storage.set(filename, texture);

		return texture;
	}
private:
	const std::string getAssetFolder() {
		if (Configuration::instance()->has("assets_dir") && assetFolder.size() == 0) {
			assetFolder = Configuration::instance()->get("assets_dir");
		}

		return assetFolder;
	}
};