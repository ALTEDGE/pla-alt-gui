#ifndef SAVABLETAB_H
#define SAVABLETAB_H

#include <QWidget>

/**
 * @class SavableTab
 * @brief Allows tabs to tell if changes to settings were made, and provides
 * common controls to save/discard/load those settings.
 */
class SavableTab : public QWidget
{
    Q_OBJECT

public:
    SavableTab(QWidget *parent = nullptr)
        : QWidget(parent) {}

    /**
     * Saves settings.
     */
    virtual void saveSettings(void) {}

    /**
     * Loads settings, discarding changes.
     */
    virtual void loadSettings(void) {}

    /**
     * Returns true if the tab was modified.
     */
    virtual bool isModified(void) const { return false; }
};

#endif // SAVABLETAB_H
