#ifndef SETTING_GROUP_H
#define SETTING_GROUP_H

#include <string>
#include <vector>
#include <stdexcept>

using std::string;
using std::vector;

#include "setting_item.h"

class SettingGroup {
public:
    SettingGroup(const string & name, const string & outputFilename = "") 
        : name_(name), outputFilename_(outputFilename) {}

    const string & getName() const { return name_; }
    const string & getOutputFilename() const { return outputFilename_; }
    vector<SettingItem*> & getItems() { return items_; }
    unsigned int getSize() const { return items_.size(); }
    SettingItem * getSelectedItem() const { 
        if (items_.size() == 0) throw std::logic_error("no item");
        return items_[selectedIndex_];
    }
    unsigned int getSelectedIndex() const { 
        if (items_.size() == 0) throw std::logic_error("no item");
        return selectedIndex_; 
    }
    void setSelectedIndex(unsigned int index) {
        if (items_.size() == 0) throw std::logic_error("no item");
        if (index >= items_.size()) throw std::out_of_range("invalid item index");
        selectedIndex_ = index;
    }
    unsigned int getDisplayTopIndex() const { 
        if (items_.size() == 0) throw std::logic_error("no item");
        return displayTopIndex_; 
    }
    void setDisplayTopIndex(unsigned int index) {
        if (items_.size() == 0) throw std::logic_error("no item");
        if (index >= items_.size()) throw std::out_of_range("invalid item index");
        displayTopIndex_ = index;
    }    
private:
    const string name_, outputFilename_;
    vector<SettingItem*> items_;
    unsigned int selectedIndex_ = 0;
    unsigned int displayTopIndex_ = 0;
};

#endif // SETTING_GROUP_H