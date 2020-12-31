#include <SFML/Graphics.hpp>
#include "Engine.h"
#include "Scripts.h"


int main(int argc, char* argv[])
{
    Script esc;
    {
        std::string ScriptDir = "Scripts", ext = ".txt", TexturesDir = "Textures";
        if (argc > 1)
        {
            ScriptDir = argv[1];
        }
        Script creation(ScriptDir, ext, TexturesDir);
        esc = creation;
    }

    launchScriptOutput lso = esc.LaunchScript("boot");
    Engine eng(lso.title,lso.width,lso.height,lso.camX,lso.camY,lso.fpsLimit);
    esc.System(eng);
    eng.SetCharacterSpeed(1.0);
    eng.Loop();

    return 0;
}