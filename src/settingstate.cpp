#include "settingstate.h"
#include "global.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <OgreOggSound/OgreOggSound.h>

Settings g_settings = {};

static ControllerType controllers[] = {
        CONTROLLER_AI,
        CONTROLLER_KEYBOARD,
        CONTROLLER_KINECT
};

struct AiStrengthTranslator {
    typedef std::string         internal_type;
    typedef AiStrength  external_type;

    // Converts a string to bool
    boost::optional<external_type> get_value(const internal_type& str)
    {
        if (!str.empty())
        {
            using boost::algorithm::iequals;

            if (iequals(str, "STRONG"))
                return boost::optional<external_type>(AI_STRONG);
            else
                return boost::optional<external_type>(AI_WEAK);
        }
        else
            return boost::optional<external_type>(boost::none);
    }

    // Converts a bool to string
    boost::optional<internal_type> put_value(const external_type& b)
    {
        return boost::optional<internal_type>(b == AI_STRONG ? "STRONG" : "WEAK");
    }
};

struct ControllerTranslator {
    typedef std::string    internal_type;
    typedef ControllerType external_type;

    // Converts a string to bool
    boost::optional<external_type> get_value(const internal_type& str)
    {
        if (!str.empty())
        {
            using boost::algorithm::iequals;

            if (iequals(str, "KEYBOARD"))
                return boost::optional<external_type>(CONTROLLER_KEYBOARD);
            else if (iequals(str, "KINECT"))
                return boost::optional<external_type>(CONTROLLER_KINECT);
            else
                return boost::optional<external_type>(CONTROLLER_AI);
        }
        else
            return boost::optional<external_type>(boost::none);
    }

    // Converts a bool to string
    boost::optional<internal_type> put_value(const external_type& b)
    {
        switch(b) {
            case CONTROLLER_KEYBOARD:
                return boost::optional<internal_type>("KEYBOARD");
            case CONTROLLER_KINECT:
                return boost::optional<internal_type>("KINECT");
            default:
                return boost::optional<internal_type>("AI");
        }
    }
};

/*  Specialize translator_between. Specialization must be in boost::property_tree
    namespace. */
namespace boost {
    namespace property_tree {

        template<typename Ch, typename Traits, typename Alloc>
        struct translator_between<std::basic_string< Ch, Traits, Alloc >, AiStrength>
        {
            typedef AiStrengthTranslator type;
        };


        template<typename Ch, typename Traits, typename Alloc>
        struct translator_between<std::basic_string< Ch, Traits, Alloc >, ControllerType>
        {
            typedef ControllerTranslator type;
        };

    } // namespace property_tree
} // namespace boost

void SettingState::enter() {
    OgreOggSound::OgreOggSoundManager& soundManager = OgreOggSound::OgreOggSoundManager::getSingleton();
    soundManager.getSound("menu_loop")->play();
    setupWindows();
}

void SettingState::exit() {
    OgreOggSound::OgreOggSoundManager& soundManager = OgreOggSound::OgreOggSoundManager::getSingleton();
    soundManager.getSound("menu_loop")->stop();
}

void SettingState::loadSettings() {
    typedef boost::property_tree::iptree ptree;
    using boost::property_tree::ini_parser_error;

    ptree pt;
    try {
        read_ini("settings.cfg", pt);
    } catch(ini_parser_error& e) {
        Ogre::LogManager::getSingleton().logMessage("Error reading settings.cfg", Ogre::LML_CRITICAL);

        g_settings.aiStrength = AI_WEAK;
        g_settings.soundOn = true;
        g_settings.previewOn = true;
        g_settings.useOldCalibration = false;
        g_settings.showBallGhosts = true;
        g_settings.skip2D = false;
        g_settings.controllerP1 = CONTROLLER_KINECT;
        g_settings.controllerP2 = CONTROLLER_AI;
        return;
    }

    g_settings.showBallGhosts = pt.get<bool>("kinect_pong.showBallGhosts", true);
    g_settings.soundOn = pt.get<bool>("kinect_pong.sound", true);
    g_settings.previewOn = pt.get<bool>("kinect_pong.preview", true);
    g_settings.useOldCalibration = pt.get<bool>("kinect_pong.oldCalibration", false);
    g_settings.skip2D = pt.get<bool>("kinect_pong.skip2d", false);
    g_settings.aiStrength = pt.get<AiStrength>("kinect_pong.aiStrength");

    g_settings.controllerP1 = pt.get<ControllerType>("kinect_pong.P1");
    g_settings.controllerP2 = pt.get<ControllerType>("kinect_pong.P2");
}

void SettingState::saveSettings() {
    typedef boost::property_tree::iptree ptree;

    ptree pt;
    pt.put("kinect_pong.aiStrength", g_settings.aiStrength);
    pt.put("kinect_pong.showBallGhosts", g_settings.showBallGhosts);
    pt.put("kinect_pong.sound", g_settings.soundOn);
    pt.put("kinect_pong.preview", g_settings.previewOn);
    pt.put("kinect_pong.oldCalibration", g_settings.useOldCalibration);
    pt.put("kinect_pong.skip2d", g_settings.skip2D);
    pt.put("kinect_pong.P1", g_settings.controllerP1);
    pt.put("kinect_pong.P2", g_settings.controllerP2);

    write_ini("settings.cfg", pt);
}

void SettingState::setupWindows() {
    using namespace CEGUI;
    WindowManager& wmgr = WindowManager::getSingleton();
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();

    SchemeManager::getSingleton().createFromFile("TaharezLook.scheme");
    ImageManager::getSingleton().loadImageset("AlfiskoSkin.imageset", "ImageSets");
    guiContext.getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");
    guiContext.getMouseCursor().setVisible(true);
    Window* root = wmgr.loadLayoutFromFile("SettingsMenu.layout");
    guiContext.setRootWindow(root);

    ListboxItem* selected = nullptr;
    Combobox* p1Control = dynamic_cast<Combobox*>(root->getChild("P1Control"));
    ListboxItem* item = new ListboxTextItem("Keyboard", 0, controllers+1);
    if(g_settings.controllerP1 == *(static_cast<ControllerType*>(item->getUserData())))
        selected = item;
    p1Control->addItem(item);
    item = new ListboxTextItem("Kinect", 0, controllers+2);
    if(g_settings.controllerP1 == *(static_cast<ControllerType*>(item->getUserData())))
        selected = item;
    p1Control->addItem(item);

    if(selected)
        p1Control->setItemSelectState(selected, true);

    selected = nullptr;

    Combobox* p2Control = dynamic_cast<Combobox*>(root->getChild("P2Control"));
    item = new ListboxTextItem("AI", 0, controllers);
    if(g_settings.controllerP2 == *(static_cast<ControllerType*>(item->getUserData())))
        selected = item;
    p2Control->addItem(item);
    item = new ListboxTextItem("Keyboard", 0, controllers+1);
    if(g_settings.controllerP2 == *(static_cast<ControllerType*>(item->getUserData())))
        selected = item;
    p2Control->addItem(item);

    if(selected)
        p2Control->setItemSelectState(selected, true);

    RadioButton* weakAi = dynamic_cast<RadioButton*>(root->getChild("weakAI"));
    weakAi->setSelected(g_settings.aiStrength == AI_WEAK);
    RadioButton* strongAi = dynamic_cast<RadioButton*>(root->getChild("strongAI"));
    strongAi->setSelected(g_settings.aiStrength == AI_STRONG);

    ToggleButton* sound = dynamic_cast<ToggleButton*>(root->getChild("Sound"));
    sound->setSelected(g_settings.soundOn);

    ToggleButton* ghosts = dynamic_cast<ToggleButton*>(root->getChild("Ghost"));
    ghosts->setSelected(g_settings.showBallGhosts);

    ToggleButton* preview = dynamic_cast<ToggleButton*>(root->getChild("Preview"));
    preview->setSelected(g_settings.previewOn);

    ToggleButton* oldcalib = dynamic_cast<ToggleButton*>(root->getChild("OldCalibration"));
    oldcalib->setSelected(g_settings.useOldCalibration);

    ToggleButton* skip2d = dynamic_cast<ToggleButton*>(root->getChild("Skip2D"));
    skip2d->setSelected(g_settings.skip2D);

    Window* saveBtn = root->getChild("Save");
    Window* cancelBtn = root->getChild("Cancel");

    p1Control->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&SettingState::handleP1Selected, this));
    p2Control->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&SettingState::handleP2Selected, this));
    saveBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&SettingState::handleSaveClick, this));
    cancelBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&SettingState::handleCancelClick, this));
}

bool SettingState::handleSaveClick(const CEGUI::EventArgs& args) {
    using namespace CEGUI;
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();
    Window* root = guiContext.getRootWindow();

    g_settings.showBallGhosts = dynamic_cast<ToggleButton*>(root->getChild("Ghost"))->isSelected();
    g_settings.soundOn = dynamic_cast<ToggleButton*>(root->getChild("Sound"))->isSelected();
    g_settings.previewOn = dynamic_cast<ToggleButton*>(root->getChild("Preview"))->isSelected();
    g_settings.useOldCalibration = dynamic_cast<ToggleButton*>(root->getChild("OldCalibration"))->isSelected();
    g_settings.skip2D = dynamic_cast<ToggleButton*>(root->getChild("Skip2D"))->isSelected();

    RadioButton* weakAi = dynamic_cast<RadioButton*>(root->getChild("weakAI"));
    if(!weakAi->isSelected())
        g_settings.aiStrength = AI_STRONG;
    else
        g_settings.aiStrength = AI_WEAK;

    ListboxItem* item = dynamic_cast<Combobox*>(root->getChild("P1Control"))->getSelectedItem();
    if(item)
        g_settings.controllerP1 = *static_cast<ControllerType*>(item->getUserData());

    item = dynamic_cast<Combobox*>(root->getChild("P2Control"))->getSelectedItem();
    if(item)
        g_settings.controllerP2 = *static_cast<ControllerType*>(item->getUserData());

    OgreOggSound::OgreOggSoundManager& soundManager = OgreOggSound::OgreOggSoundManager::getSingleton();
    if(g_settings.soundOn)
        soundManager.unmuteAllSounds();
    else
        soundManager.muteAllSounds();

    saveSettings();
    popGameState();
    return false;
}

bool SettingState::handleCancelClick(const CEGUI::EventArgs& args) {
    popGameState();
    return false;
}

bool SettingState::handleP1Selected(const CEGUI::EventArgs& args) {
    using namespace CEGUI;
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();
    Window* root = guiContext.getRootWindow();

    ListboxItem* itemP2 = nullptr;
    Combobox *p2ctl = dynamic_cast<Combobox*>(root->getChild("P2Control"));
    for(size_t idx = 0; idx < p2ctl->getItemCount(); idx++) {
        ListboxItem* i2 = p2ctl->getListboxItemFromIndex(idx);
        if(*static_cast<ControllerType*>(i2->getUserData()) == CONTROLLER_KEYBOARD) {
            itemP2 = i2;
            break;
        }
    }

    ListboxItem* item = dynamic_cast<Combobox*>(root->getChild("P1Control"))->getSelectedItem();
    if(*static_cast<ControllerType*>(item->getUserData()) == CONTROLLER_KEYBOARD && itemP2) {
        p2ctl->removeItem(itemP2);
    } else if(!itemP2) {
        p2ctl->addItem(new ListboxTextItem("Keyboard", 0, controllers+1));
    }
    return false;
}

bool SettingState::handleP2Selected(const CEGUI::EventArgs& args) {
    using namespace CEGUI;
    GUIContext& guiContext = System::getSingleton().getDefaultGUIContext();
    Window* root = guiContext.getRootWindow();

    ListboxItem* itemP1 = nullptr;
    Combobox *p1ctl = dynamic_cast<Combobox*>(root->getChild("P1Control"));
    for(size_t idx = 0; idx < p1ctl->getItemCount(); idx++) {
        ListboxItem* i2 = p1ctl->getListboxItemFromIndex(idx);
        if(*static_cast<ControllerType*>(i2->getUserData()) == CONTROLLER_KEYBOARD) {
            itemP1 = i2;
            break;
        }
    }

    ListboxItem* item = dynamic_cast<Combobox*>(root->getChild("P2Control"))->getSelectedItem();
    if(*static_cast<ControllerType*>(item->getUserData()) == CONTROLLER_KEYBOARD && itemP1) {
        p1ctl->removeItem(itemP1);
    } else if(!itemP1) {
        p1ctl->addItem(new ListboxTextItem("Keyboard", 0, controllers+1));
    }
    return false;
}