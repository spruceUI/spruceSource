#include "setting_item.h"

#include <algorithm>
#include <cstdlib>
#include <string>

#include "global.h"

namespace {
// split string with delimiter into vector of tokens
vector<string> split(const string &s, const string &delimiter) {
  string copy_s = s;
  vector<string> tokens;
  size_t pos = 0;
  string token;
  while ((pos = copy_s.find(delimiter)) != string::npos) {
    token = copy_s.substr(0, pos);
    tokens.push_back(token);
    copy_s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(copy_s);

  return tokens;
}

// search element in vector and return zero-based index if found
// return -1 if no element is find
template <typename T> int find(vector<T> v, T s) {
  auto it = find(v.begin(), v.end(), s);

  if (it != v.end())
    return it - v.begin();
  else
    return -1;
}

// run command and get output from stdout
// return empty string if error occurs
string exec(const string &cmd) {
  char buffer[1024];
  std::string result = "";
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    return "";
  try {
    while (fgets(buffer, sizeof buffer, pipe) != NULL) {
      result += buffer;
    }
  } catch (...) {
  }
  pclose(pipe);
  return result;
}
} // namespace

SettingItem::SettingItem(const string &infoCommand,
                         const vector<string> &modeTags)
    : description_(exec(global::replaceAliases(infoCommand))),
      commandsString_(infoCommand), modeTags_(modeTags), isInfoText_(true) {
  if (description_.empty()) {
    isInitOK_ = false;
    return;
  }

  // create texture for description text
  descriptionTexture_ =
      new TextTexture(description_, global::font, global::minor_text_color);

  isInitOK_ = true;
}

SettingItem::SettingItem(const string &id, const string &description,
                         const string &optionsString,
                         const string &displayValuesString,
                         const string &selectedValue,
                         const string &commandsString,
                         const string &infoCommandString,
                         const vector<string> &modeTags)
    : id_(id), description_(description), optionsString_(optionsString),
      displayValuesString_(displayValuesString), selectedValue_(selectedValue),
      commandsString_(commandsString), infoCommandString_(infoCommandString),
      modeTags_(modeTags) {
  options_ = split(optionsString_, "|");
  displayValues_ = split(displayValuesString_, "|");
  if (!commandsString_.empty())
    commands_ = split(commandsString_, "|");

  if (options_.size() < 2 || displayValues_.size() < 2 ||
      options_.size() != displayValues_.size() ||
      (commands_.size() > 1 && commands_.size() != options_.size())) {
    errorMessage_ = "invalid number of options or commands " + commands_.size();
    return;
  }

  // try to find index of the selected value
  int index = find(displayValues_, selectedValue_);
  // if selected value not found in displayValues, assume it is command for
  // source value
  if (index < 0) {
    // try run command and get result as selected value
    sourceCommandString_ = selectedValue_;
    string cmd = global::replaceAliases(sourceCommandString_);
    selectedValue_ = exec(cmd);
    index = find(displayValues_, selectedValue_);

    if (selectedValue_.empty() || index < 0) {
      errorMessage_ = "invalid option value";
      return;
    }
  }

  // store selected inde to private fields
  oldSelectedIndex_ = selectedIndex_ = static_cast<unsigned int>(index);

  // create texture for description text
  descriptionTexture_ =
      new TextTexture(description_, global::font, global::text_color);

  // create other textures
  updateTextures();

  isInitOK_ = true;
}

void SettingItem::UpdateVisible(const vector<string> &modes) {
  // set to visible if there is no mode tag
  if (modeTags_.empty()) {
    isVisible_ = true;
    return;
  }

  // otherwise match tag to given modes
  isVisible_ = false;
  for (string &tag : modeTags_) {
    if (find(modes, tag) != -1) {
      isVisible_ = true;
      return;
    }
  }
}

void SettingItem::setMinorText(const string &text) {
  minorText_ = text;

  if (minorTextTexture_ != nullptr) {
    delete minorTextTexture_;
    minorTextTexture_ = nullptr;
  }

  if (!minorText_.empty()) {
    minorTextTexture_ = new TextTexture(
        minorText_, global::font, global::minor_text_color,
        TextureAlignment::topLeft, (global::screen_height - 120) * 2);
  }
}

void SettingItem::updateTextures() {
  if (isInfoText_)
    return;

  // delete old value texture
  if (valueTexture_ != nullptr)
    delete valueTexture_;

  // create new texture
  valueTexture_ = new TextTexture(displayValues_[selectedIndex_], global::font,
                                  global::text_color);

  // try run command to get minor info message
  if (!infoCommandString_.empty()) {
    // delete old texture
    if (minorTextTexture_ != nullptr) {
      delete minorTextTexture_;
      minorTextTexture_ = nullptr;
    }

    // get new info text and create texture
    string cmd = global::replaceAliases(infoCommandString_, selectedIndex_,
                                        selectedValue_);
    minorText_ = exec(cmd);
    if (!minorText_.empty()) {
      minorTextTexture_ = new TextTexture(
          minorText_, global::font, global::minor_text_color,
          TextureAlignment::topLeft, (global::screen_height - 120) * 2);
    }
  }
}

void SettingItem::renderDescription(int x, int y) const {
  if (minorTextTexture_ != nullptr) {
    descriptionTexture_->render(x, y + 4);
    minorTextTexture_->render(x, y + descriptionTexture_->getHeight() - 4);
  } else {
    descriptionTexture_->render(x, y + 10);
  }
}

void SettingItem::renderValue(int x, int y) const {
  if (isInfoText_)
    return;

  if (minorTextTexture_ != nullptr) {
    int offsetY = (getHeight() - valueTexture_->getHeight()) / 2;
    valueTexture_->render(x, y + offsetY);
  } else {
    valueTexture_->render(x, y + 10);
  }
}

void SettingItem::selectPreviousValue() {
  if (isInfoText_)
    return;

  if (selectedIndex_ == 0)
    selectedIndex_ = options_.size();
  selectedIndex_--;

  selectedValue_ = displayValues_[selectedIndex_];

  updateTextures();
}

void SettingItem::selectNextValue() {
  if (isInfoText_)
    return;

  selectedIndex_++;
  if (selectedIndex_ >= options_.size())
    selectedIndex_ = 0;

  selectedValue_ = displayValues_[selectedIndex_];

  updateTextures();
}

bool SettingItem::isOnOffSetting() const {
  if (isInfoText_)
    return false;

  return displayValues_.size() == 2 && displayValues_[0] == "on" &&
         displayValues_[1] == "off";
}

bool SettingItem::isRunOffSetting() const {
  if (isInfoText_)
    return false;

  return displayValues_.size() == 2 && displayValues_[0] == "run" &&
         displayValues_[1] == "off";
}
