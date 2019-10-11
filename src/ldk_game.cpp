
namespace ldk
{

#define LDK_DEFAULT_GAME_WINDOW_TITLE "LDK Game"
#define LDK_DEFAULT_CONFIG_FILE "assets/ldk_game.cfg"

  LDKGameSettings loadGameSettings()
  {
    LDKGameSettings settings = {};
    settings.name = LDK_DEFAULT_GAME_WINDOW_TITLE;
    settings.displayWidth = 800;
    settings.displayHeight = 600;
    settings.aspect = 800/600;
    settings.fullScreen = false;
    settings.preallocMemorySize = 0;
    settings.showCursor = true;

    const ldk::VariantSectionRoot* root = ldk::configParseFile((const char8*) LDK_DEFAULT_CONFIG_FILE);

    if (root)
    {
      const ldk::VariantSection* sectionDisplay =
        ldk::configGetSection(root,"game");

      if (sectionDisplay != nullptr)
      {

        char*name;
        ldk::configGetString(sectionDisplay, 
            "name", &name);
        settings.name = name;

        ldk::configGetBool(sectionDisplay, 
            "fullScreen", &settings.fullScreen);
        ldk::configGetInt(sectionDisplay, 
            "displayWidth", &settings.displayWidth);
        ldk::configGetInt(sectionDisplay,
            "displayHeight", &settings.displayHeight);
        ldk::configGetFloat(sectionDisplay,
            "aspect", &settings.aspect);
      }
    }
    return settings;
  }

}
