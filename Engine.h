#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
//#define ENGINE_DEBUG

struct TextureInfo { std::string FileName; int width, heigth, piece; };

class Drawning
{
private:
    int TextureId;
    std::vector<sf::Texture> *StackOTexture;
    std::vector<std::vector<int>> animations;
    std::vector<int> animationFPS;
    sf::Sprite spr;
    sf::Vector2f realPos;
    float reqResX, reqResY; int animCounter = 0;
    bool flipX, flipY;
    int selectedAnimationId = 0;
public:
    /*Creation*/
    void init(std::vector<sf::Texture>* StackOTexture, sf::Vector2i reqRes = { 100,100 })
    {
        this->StackOTexture = StackOTexture; TextureId = 0; this->reqResX = reqRes.x; this->reqResY = reqRes.y;
        std::vector<int> startAnimation; startAnimation.push_back(0);
        animations.push_back(startAnimation);
        animationFPS.push_back(60);
    }

    Drawning() {/*EMPTYCREATOR*/}
    Drawning(std::vector<sf::Texture> *StackOTexture, sf::Vector2i reqRes = { 100,100 }) { init(StackOTexture, reqRes); }

    void setReqRes(int sX, int sY) { reqResX = sX; reqResY = sY; }
    void defineAnimation(std::vector<int> animation, int fps) 
    { 
        bool trueAnimation = true;
        /* Animation Control */
        for (int i = 0; i < animation.size(); i++)
        {
            if (animation[i] > StackOTexture[0].size() - 1) { std::cout << "Animation's " << i << ". frame is too big." << std::endl; trueAnimation = false; }
        }if (trueAnimation) { animations.push_back(animation); animationFPS.push_back(fps); }
    }
    void setAnimation(std::vector<int> animation, int fps , int id) 
    { 
        bool trueAnimation = true;
        /* Animation Control */
        for (int i = 0; i < animation.size(); i++)
            if (animation[i] > StackOTexture[0].size() - 1)
            {
                std::cout << std::endl << "ERROR:" << "Animation's " << i << ". frame is too big." << std::endl;
                trueAnimation = false;
            }
        if (id > animations.size() - 1) { std::cout << "Animation ID " << id << " is too big" << std::endl; trueAnimation = false; }
        if (trueAnimation) { animations[id] = animation; animationFPS[id] = fps; }
    }
    void selectAnimation(int id) { selectedAnimationId = id; }
    sf::Sprite* getSprite() { return &spr; }
    void setPosition(sf::Vector2f pos) { realPos = pos; }
    void addPosition(sf::Vector2f pos) { realPos += pos; }
    sf::Vector2f getPos() { return realPos; }

    std::vector<sf::Texture>* getTextures() const { return StackOTexture; }
    void setFlipping(bool flipX, bool flipY) { this->flipX = flipX; this->flipY = flipY; }
    void a()
    {
        animCounter++; if (animCounter > animationFPS[selectedAnimationId]) animCounter = 0;
        float fps = animationFPS[selectedAnimationId];
        float animsize = animations[selectedAnimationId].size();
        float fAC = animCounter;
        int result = fAC * animsize / (fps);
#ifdef ENGINE_DEBUG
        std::cout << "Result For Animation " << selectedAnimationId << ":" << result << std::endl;
#endif //ENGINE_DEBUG
        TextureId = animations[selectedAnimationId][result >= animations[selectedAnimationId].size() ? animations[selectedAnimationId].size()-1 : result ];
    }

    /*
    * Drawing
    */
    sf::Sprite d() 
    { 
        float x = StackOTexture[0][TextureId].getSize().x; float y = StackOTexture[0][TextureId].getSize().y;
        spr.setTexture(StackOTexture[0][TextureId]);
        spr.setScale({ flipX ? -reqResX / x : reqResX / x, flipY ? -reqResY / y : reqResY / y });
        spr.setPosition(
            flipX ? realPos.x + reqResX : realPos.x,
            flipY ? realPos.y + reqResY : realPos.y
        );
#ifdef ENGINE_DEBUG
        std::cout << "Trying to get d: " << TextureId << std::endl;
#endif //ENGINE_DEBUG
        return spr;
    }
};

class Engine
{
private:
    sf::RenderWindow EngineWindow;
    sf::Vector2i camera;
    /*
    * Stack O' Textures
    */
    std::vector<std::vector<sf::Texture>> textures;
    std::vector<Drawning> drawnings;
    Drawning newDrawning;
    sf::Texture background;
    Drawning character;
    int fpsLimit;

    /*Controls*/
    bool aWalking = false, dWalking = false; float speed = 1;
public:
    /*
    * Engine Creation
    */
    Engine(std::string Title = "Engine", int resX = 800, int resY = 600, int camX = 0, int camY = 0, int fpsLimit = 60)
        : EngineWindow(sf::VideoMode(resX, resY), Title), camera(camX, camY) {
        EngineWindow.setFramerateLimit(fpsLimit); this->fpsLimit = fpsLimit;
    }
    /*
    * Binding
    */
    void PushBackStackOTextures(std::vector<sf::Texture> s) { textures.push_back(s); }
    void SetCharacterSpeed(float speed) { this->speed = speed; }
    void bindTexturesToNewDrawning(int StackOTextureId)
    {
        if (StackOTextureId > textures.size() - 1) { std::cout << "Binding position error. ID:" << StackOTextureId << " is very big." << std::endl; }
        else
        {
            newDrawning.init(&textures[StackOTextureId]);
            std::cout << StackOTextureId << " binded to new Drawning!" << std::endl;
        }
    }
    void bindCharacterDrawning(int StackOTextureId)
    {
        if (StackOTextureId > textures.size() - 1) { std::cout << "Binding position error. ID:" << StackOTextureId << " is very big." << std::endl; }
        else
        {
            character.init(&textures[StackOTextureId]);
            std::cout << StackOTextureId << " binded to characterSlot!" << std::endl;
        }
    }
    void addTexturesToId(int id, std::vector<sf::Texture> s) { textures[id].insert(textures[id].end(), s.begin(), s.end()); }
    void bindBackground(int StackOTextureId)
    {
        if (StackOTextureId > textures.size() - 1) { std::cout << "Binding position error. ID:" << StackOTextureId << " is very big." << std::endl; }
        else
        {
            textures[StackOTextureId][0].setRepeated(true);
            background = textures[StackOTextureId][0];
            std::cout << StackOTextureId << " binded to background!" << std::endl;
        }
    }
    void bindDrawningPosition(int POSX, int POSY) { newDrawning.setPosition({ (float)POSX, (float)POSY }); std::cout << "New drawning position binded as " << POSX << " " << POSY << std::endl; }
    void bindNewTo(std::string id)
    {
        std::cout << "New drawning is binding to ";
        if (id == "DRAWNING") { drawnings.push_back(newDrawning); std::cout << "DRAWNING" << std::endl; }
    }
    void setCharacterAnimation(std::vector<int> animation, int id, int fps)
    {
        character.setAnimation(animation, fps, id);
    }
    void defineCharacterAnimation(std::vector<int> animation, int fps)
    {
        character.defineAnimation(animation, fps);
    }
    void setDrawningAnimation(std::vector<int> animation, int AnimationId, int DrawningId, int fps)
    {
        if (DrawningId > drawnings.size() - 1) std::cout << "Drawning id " << DrawningId << " is too big." << std::endl;
        else { drawnings[DrawningId].setAnimation(animation, fps, AnimationId); }
    }
    void defineDrawningAnimation(std::vector<int> animation, int DrawningId, int fps)
    {
        if (DrawningId > drawnings.size() - 1) std::cout << "Drawning id " << DrawningId << " is too big." << std::endl;
        else { drawnings[DrawningId].defineAnimation(animation, fps); }
    }
    std::vector<Drawning> getDrawnings() const { return drawnings; }


    /*
    * Draw with camera pos functions
    */
    void draw(sf::Texture drawning, sf::Vector2f pos = { 0,0 }) {
        sf::Sprite spr; spr.setTexture(drawning); spr.setPosition(pos.x - camera.x, pos.y - camera.y);
        EngineWindow.draw(spr);
    }
    void drawAllOScreen(sf::Texture drawning)
    {
        sf::Sprite spr; spr.setTexture(drawning); spr.setPosition(0, 0);
        spr.setScale((float)EngineWindow.getSize().x / (float)drawning.getSize().x, (float)EngineWindow.getSize().y / (float)drawning.getSize().y);
        EngineWindow.draw(spr);
    }
    void draw(sf::Sprite drawning)
    {
        drawning.setPosition(drawning.getPosition().x - camera.x, drawning.getPosition().y - camera.y);
        EngineWindow.draw(drawning);
    }

    /*
    * Easy Draw
    */
    void Draw()
    {
        /* Animation Things */
        character.a();
        for (int i = 0; i < drawnings.size(); i++) drawnings[i].a();

        /* Drawing */
        drawAllOScreen(background);
        for (Drawning a : drawnings) draw(a.d());
        draw(character.d());
    }

    /*
    * Easy Event
    */
    void Event(sf::Event event)
    {
        if (event.type == sf::Event::Closed)
            EngineWindow.close();
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::A)
            {
                character.setFlipping(true, false);
                character.selectAnimation(1);
                aWalking = true;
            }
            else if (event.key.code == sf::Keyboard::D)
            {
                character.setFlipping(false, false);
                character.selectAnimation(1);
                dWalking = true;
            }
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            if (event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D)
            {
                character.selectAnimation(0); aWalking = dWalking = false;
            }
        }
    }

    /*
    * Boolean Controls
    */
    void Bools()
    {
        if (aWalking)
            character.addPosition({ -speed, 0 });
        else if (dWalking)
            character.addPosition({ speed, 0 });
    }

    /*
    * Looping Event
    */
    void EventLoop()
    {
        sf::Event event;
        while (EngineWindow.pollEvent(event))
        {
            Event(event);
        }
    }

    /*
    * Engine Loop
    */
	void Loop()
	{
        std::cout << "Trying to run!" << std::endl;
        while (EngineWindow.isOpen())
        {
            EventLoop();
            Bools();
            EngineWindow.clear();
            Draw();
            EngineWindow.display();
        }
	}
};