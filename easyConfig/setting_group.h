#ifndef SETTING_GROUP_H
#define SETTING_GROUP_H

#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

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
    vector<SettingItem*> & getVisibleItems() { return visibleItems_; }
    SettingItem * getSelectedItem() const { 
        if (visibleItems_.empty()) return nullptr;
        return visibleItems_[selectedIndex_];
    }
    bool selectPreviousItem() {
        if (visibleItems_.empty()) return false;
        if (selectedIndex_ == 0) selectedIndex_ = visibleItems_.size() - 1;
        else selectedIndex_--; 
        return true;
    }
    bool selectNextItem() {
        if (visibleItems_.empty()) return false;
        if (selectedIndex_ == visibleItems_.size() - 1) selectedIndex_ = 0;
        else selectedIndex_++; 
        return true;
    }
    unsigned int getSelectedIndex() const { 
        if (visibleItems_.size() == 0) throw std::logic_error("no item 2");
        return selectedIndex_; 
    }
    unsigned int getDisplayTopIndex() const { 
        if (visibleItems_.size() == 0) throw std::logic_error("no item 4");
        return displayTopIndex_; 
    }
    void setDisplayTopIndex(unsigned int index) {
        if (visibleItems_.size() == 0) throw std::logic_error("no item 5");
        if (index >= visibleItems_.size()) throw std::out_of_range("invalid item index");
        displayTopIndex_ = index;
    }
    void UpdateVisibleItems(const vector<string> & mode) {
        visibleItems_.clear();
        for (auto & item : items_) {
            item->UpdateVisible(mode);
            if (item->isVisible()) {
                visibleItems_.push_back(item);
            }
        }
    }
private:
    const string name_, outputFilename_;
    vector<SettingItem*> items_;
    vector<SettingItem*> visibleItems_;
    unsigned int selectedIndex_ = 0;
    unsigned int displayTopIndex_ = 0;
};

#endif // SETTING_GROUP_H