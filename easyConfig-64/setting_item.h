#ifndef SETTING_ITEM_H
#define SETTING_ITEM_H

#include <string>
#include <vector>

#include "text_texture.h"

using std::string;
using std::vector;

class SettingItem {
public:
  explicit SettingItem(const string &infoCommand,
                       const vector<string> &modeTags);

  explicit SettingItem(const string &id, const string &description,
                       const string &optionsString,
                       const string &displayValuesString,
                       const string &selectedValue,
                       const string &commandsString,
                       const string &infoCommandString,
                       const vector<string> &modeTags);

  void UpdateVisible(const vector<string> &modes);
  void renderDescription(int offsetX, int offsetY) const;
  void renderValue(int offsetX, int offsetY) const;
  void selectPreviousValue();
  void selectNextValue();
  bool isOnOffSetting() const;
  bool isRunOffSetting() const;

  const string &getID() const { return id_; }
  const string &getDescription() const { return description_; }
  const string &getOptionsString() const { return optionsString_; }
  const string &getDisplayValuesString_() const { return displayValuesString_; }
  const string &getSelectedValue() const { return selectedValue_; }
  const string &getCommandsString() const { return commandsString_; }
  const vector<string> &getOptions() const { return options_; }
  const vector<string> &getDisplayValues() const { return displayValues_; }
  const vector<string> &getCommands() const { return commands_; }
  const vector<string> &getModeTags() const { return modeTags_; }
  const string &getSourceCommandString() const { return sourceCommandString_; }
  const string &getInfoCommandString() const { return infoCommandString_; }
  const string &getMinorText() const { return minorText_; }
  void setMinorText(const string &text);
  unsigned int getSelectedIndex() const { return selectedIndex_; }
  unsigned int getOldSelectedIndex() const { return oldSelectedIndex_; }
  bool IsInitOK() const { return isInitOK_; }
  bool isInfoText() const { return isInfoText_; }
  bool isVisible() const { return isVisible_; }
  const string &getErrorMessage() const { return errorMessage_; }
  TextTexture *getDescriptionTexture() const { return descriptionTexture_; }
  TextTexture *getValueTexture() const { return valueTexture_; }
  TextTexture *getMinorTextTexture() const { return minorTextTexture_; }
  int getHeight() const {
    if (minorTextTexture_ != nullptr) {
      return descriptionTexture_->getHeight() + minorTextTexture_->getHeight();
    } else {
      return descriptionTexture_->getHeight() + 20;
    }
  }
  int getValueOffsetY() const {
    if (minorTextTexture_ != nullptr) {
      return (getHeight() - valueTexture_->getHeight()) / 2;
    } else {
      return 10;
    }
  }

private:
  const string id_;
  const string description_;
  const string optionsString_;
  const string displayValuesString_;
  string selectedValue_;
  const string commandsString_;
  string infoCommandString_;
  vector<string> modeTags_;
  vector<string> options_;
  vector<string> displayValues_;
  vector<string> commands_;
  unsigned int selectedIndex_, oldSelectedIndex_;
  bool isInitOK_ = false;
  bool isInfoText_ = false;
  bool isVisible_ = true;
  string sourceCommandString_;
  string minorText_;
  string errorMessage_;
  TextTexture *descriptionTexture_ = nullptr;
  TextTexture *valueTexture_ = nullptr;
  TextTexture *minorTextTexture_ = nullptr;

  void updateTextures();
};

#endif // SETTING_ITEM_H
