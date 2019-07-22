#ifndef MACROTAB_H
#define MACROTAB_H

#include "key.h"
#include "keygrabber.h"
#include "macro.h"
#include "macrorecorder.h"
#include "savabletab.h"

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QShowEvent>
#include <wwWidgets/QwwListWidget>

/**
 * @class MacroTab
 * @brief Provides controls for creating and modifying macros.
 */
class MacroTab : public SavableTab
{
    Q_OBJECT

public:
    explicit MacroTab(QWidget *parent = nullptr);

    bool isModified(void) const;

public slots:
    void saveSettings(void);
    void loadSettings(void);

private slots:
    /**
     * Creates a new macro with a "Macro x" name.
     */
    void createNewMacro(void);

    /**
     * Loads the given macro.
     * @param name The macro to load
     */
    void loadMacro(const QString& name);

    /**
     * Saves/discards changes, then loads the given macro.
     */
    void changeCurrentMacro(QString name);

    /**
     * Moves the selected key up one in actionList.
     */
    void moveKeyUp(void);

    /**
     * Moves the selected key down one in actionList.
     */
    void moveKeyDown(void);

    /**
     * Inserts a new key into actionList.
     */
    void insertKey(void);

    /**
     * Removes the selected key from the actionList.
     */
    void removeKey(void);

    /**
     * Edits the selected key.
     */
    void editKey(void);

    /**
     * Captures key presses from the KeyGrabber dialog box.
     */
    void keyPressed(Key key);

    /**
     * Changes delay type to the selected type.
     */
    void delayChange(void);

    /**
     * When fixed delay is selected, applies the entered delay to all key
     * entries.
     */
    void applyDelayValue(void);

    /**
     * Changes the current macro name to what's been typed in.
     */
    void changeName(void);

    /**
`    * Begins recording the macro.
     */
    void beginRecord(void);

    /**
     * Sets the macro's contents to what was recorded.
     * @param ks The recorded ActionList
     */
    void finishedRecording(ActionList& ks);

private:
    void showEvent(QShowEvent *event);

    /**
     * Updates controls with new macro values.
     */
    void reloadMacro(void);

    inline std::pair<unsigned int, bool> getCurrentActionListRow(void) {
        int row = actionList.currentIndex().row();
        return { static_cast<unsigned int>(row), row >= 0 };
    }

    // "MACRO NAME";
    QLabel lMacroName;
    // "ENTER DELAY: "
    QLabel lDelayPrompt;

    // Macro list controls

    QComboBox macroList;
    QLineEdit macroName;
    QPushButton macroAdd;

    // Macro content controls

    QwwListWidget actionList;
    QPushButton actionEdit;
    QPushButton actionRemove;
    QPushButton actionUp;
    QPushButton actionDown;
    QPushButton actionInsert;

    // Macro delay controls

    QRadioButton delayNone;
    QRadioButton delayFixed;
    QRadioButton delayRecord;

    QPushButton delayBeginRecord;
    QLineEdit delayValue;

    // Other

    QPushButton configSave;
    QPushButton configCancel;

    KeyGrabber keyGrabber;
    MacroRecorder recorder;

    ActionList currentMacro;
    QString currentName;
    int currentDelay;

    bool ignoreNextMacroChange;
};

#endif // MACROTAB_H
