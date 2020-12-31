#pragma once
#include <iostream>
#include <fstream>
#include "Engine.h"
#include <string.h>

bool is_number(const std::string s)
{
	return(strspn(s.c_str(), "-.0123456789") == s.size());
}

#define LOG(x) std::cout << x << std::endl;
struct launchScriptOutput { std::string title; int width, height, camX, camY, fpsLimit; };

class Script
{
private:
	std::string scriptDir,ext, textureDir, systemFile;
public:
	Script(std::string scriptDir, std::string ext, std::string textureDir) 
	{
		this->scriptDir = scriptDir; this->ext = ext; this->textureDir = textureDir; 
	}
	Script() {/*EMPTYSTARTER*/}

	std::string getDirScript(std::string scriptName) { return scriptDir + "/" + scriptName + ext; }
	std::string getDirTexture(std::string txtrName) { return textureDir + "/" + txtrName; }

	/*
	* Loading Textures Script
	*/
	void TextureScript(Engine &engine, std::string TextureScriptFile)
	{
		std::fstream file(getDirScript(TextureScriptFile)); std::string update;
		while (true)
		{
			file >> update;
			if (update == "END") break;
			else if (update == "stack")
			{
				std::vector<sf::Texture> wllAdd;
				file >> update;
				if (update == "->")
				{
					int imgResX, imgResY;
					file >> update >> imgResX >> imgResY;
					std::cout << update << " parsing and stacking!" << std::endl;
					sf::Texture txtr; txtr.loadFromFile(getDirTexture(update));
					for (int i = 0; i < txtr.getSize().x / imgResX; i++) for (int j = 0; j < txtr.getSize().y / imgResY; j++)
					{
						sf::Texture loadingTexture; loadingTexture.loadFromFile(getDirTexture(update), sf::IntRect(i * imgResX, j * imgResY,imgResX, imgResY));
						wllAdd.push_back(loadingTexture);
					}
					engine.PushBackStackOTextures(wllAdd);
				}
				else if (update == "=>")
				{
					int imgResX, imgResY, location;
					file >> location >> update >> imgResX >> imgResY;
					std::cout << update << " parsing and adding to stack " << location << "!" << std::endl;
					sf::Texture txtr; txtr.loadFromFile(getDirTexture(update));
					for (int i = 0; i < txtr.getSize().x / imgResX; i++) for (int j = 0; j < txtr.getSize().y / imgResY; j++)
					{
						sf::Texture loadingTexture; loadingTexture.loadFromFile(getDirTexture(update), sf::IntRect(i * imgResX, j * imgResY, imgResX, imgResY));
						wllAdd.push_back(loadingTexture);
					}
					engine.addTexturesToId(location,wllAdd);
				}
				else 
				{
					int numberOfTextures = atoi(update.c_str());
					for (int i = 0; i < numberOfTextures; i++)
					{
						file >> update;
						std::cout << update << " stacking [" << i << "]!" << std::endl;
						sf::Texture texture; texture.loadFromFile(getDirTexture(update));
						wllAdd.push_back(texture);
					}
					engine.PushBackStackOTextures(wllAdd);
				}
			}
		}
	}

	/*
	* Binding Textures Script
	*/
	void BindingScript(Engine& engine, std::string BindingScriptFile)
	{
		std::fstream file(getDirScript(BindingScriptFile)); std::string update;
		while (true)
		{
			file >> update;
			if (update == "END") break;
			else if (update == "bind")
			{
				int info1,info2;
				file >> update;
				if (update == "characterSlot") { file >> info1;  engine.bindCharacterDrawning(info1); }
				else if (update == "background") { file >> info1; engine.bindBackground(info1); }
				else if (update == "POS") { file >> info1 >> info2;  engine.bindDrawningPosition(info1, info2); }
				else if (update == "NEW") { file >> info1; engine.bindTexturesToNewDrawning(info1); }
				else engine.bindNewTo(update);
			}
		}
	}

	/*
	* Binding Animations Script
	*/
	void AnimationScript(Engine& engine, std::string AnimationScriptFile)
	{
		std::fstream file(getDirScript(AnimationScriptFile)); std::string update;
		while (true)
		{
			std::string mainCommand;
			file >> mainCommand;
			if (mainCommand == "END") { std::cout << AnimationScriptFile << "'s end is came. Exiting..." << std::endl; break; }
			else if(mainCommand == "character" || mainCommand == "drawning")
			{
				std::string id = "", command = "", subCommand = ""; int fps = 60, DrawningId = 0; std::vector<int> animation;
				if (mainCommand == "character") { file >> fps >> id; std::cout << "Character seed fps:" << fps << " id:" << id << std::endl; }
				else if (mainCommand == "drawning") {
					file >> fps >> DrawningId >> id;
					std::cout << "Drawning seed fps:" << fps << " DrawningId:" <<  DrawningId <<" id:" << id << std::endl;
				}
				file >> command >> subCommand;
				if (subCommand == "R")
				{
					int r1, r2; file >> r1 >> r2;
					for (int i = r1; i <= r2; i++) animation.push_back(i);
				}
				else
				{
					for (int i = 0; i < atoi(subCommand.c_str()); i++)
					{
						int a; file >> a;
						animation.push_back(a);
					}
				}
				if (id == "NEW")
				{
					if (mainCommand == "character") engine.defineCharacterAnimation(animation, fps);
					else if (mainCommand == "drawning") engine.defineDrawningAnimation(animation, DrawningId, fps);
				}
				else
				{
					if (mainCommand == "character") engine.setCharacterAnimation(animation, atoi(id.c_str()), fps);
					else if (mainCommand == "drawning") engine.setDrawningAnimation(animation, atoi(id.c_str()), DrawningId, fps);
				}

			}
		}
	}
	
	/*
	* Boot Script
	*/
	launchScriptOutput LaunchScript(std::string scriptName)
	{
		std::fstream file(getDirScript(scriptName)); launchScriptOutput output;
		while (true)
		{
			std::string update;
			int iup;
			file >> update;
			if (update == "END") break;
			else if (update == "FPS") { file >> iup; output.fpsLimit = iup; LOG("FPS OPTIONS"); }
			else if (update == "RESOLUTION") { file >> iup; output.width = iup; file >> iup; output.height = iup; LOG("Window resolution selected:"<<output.width<<" "<<output.height); }
			else if (update == "CAMERA") { file >> iup; output.camX = iup; file >> iup; output.camY = iup; LOG("Camera start position selected:" << output.camX << " " << output.camY); }
			else if (update == "TITLE") { file >> update; output.title = update; LOG("Title selected:"<<update); }
			else if (update == "TEXTURES") { file >> update; textureDir = update; LOG("Texture directory selected:"<<update); }
			else if (update == "SYSTEM") { file >> update; systemFile = update; LOG("System file selected:"<<update); }

		}
		return output;
	}

	/*
	* System Script
	*/
	void System(Engine &engine)
	{
		std::fstream file(getDirScript(systemFile));
		while (true)
		{
			std::string update; file >> update;
			if (update == "BINDING") { file >> update; LOG(update << " executing for binding."); BindingScript(engine, update); }
			else if (update == "STACKING") { file >> update; LOG(update << " executing for texture stacking.");  TextureScript(engine, update); }
			else if (update == "ANIMATION") { file >> update; LOG(update << " executing for animation loading.");  AnimationScript(engine, update); }
			else if (update == "END") break;
		}
	}
};