#include "macrotab.h"
#include "controller.h"
#include "mainwindow.h"
#include "macrorecorder.h"
#include "profile.h"

#include <QMessageBox>

#include <algorithm>
#include <thread>

MacroTab::MacroTab(QWidget *parent) :
    SavableTab(parent),
    lMacro("MACRO", this),
    lMacroName("MACRO NAME", this),
    lDelayPrompt("ENTER DELAY (ms):", this),
    macroList(this),
    macroName("Macro 1", this),
    macroAdd("+", this),
    macroDelete(this),
    actionList(this),
    actionEdit("EDIT", this),
    actionRemove(this),
    actionUp(this),
    actionDown(this),
    actionInsert("INSERT", this),
    delayNone("NO DELAY", this),
    delayFixed("FIXED DELAY", this),
    delayRecord("RECORD DELAY", this),
    delayBeginRecord("RECORD", this),
    delayValue(this),
    configSave("SAVE", this),
    configCancel("CANCEL", this),
    keyGrabber(this),
    recorder(this),
    ignoreNextMacroChange(false)
{
    macroDelete.setIcon(QIcon("assets/macro-trash.png"));
    actionRemove.setIcon(QIcon("assets/macro-trash.png"));
    actionUp.setIcon(QIcon("assets/macro-up.png"));
    actionDown.setIcon(QIcon("assets/macro-down.png"));
    delayBeginRecord.setIcon(QIcon("assets/macro-record.png"));

    // Set geometry
    lMacro.setGeometry(70, 40, 200, 20);
    macroList.setGeometry(70, 60, 200, 20);
    macroAdd.setGeometry(70, 85, 92, 20);
    macroDelete.setGeometry(178, 85, 92, 20);
    lMacroName.setGeometry(70, 125, 200, 20);
    macroName.setGeometry(70, 145, 200, 20);
    delayRecord.setGeometry(70, 215, 200, 20);
    delayFixed.setGeometry(70, 245, 200, 20);
    delayNone.setGeometry(70, 275, 200, 20);
    delayBeginRecord.setGeometry(70, 305, 200, 20);
    lDelayPrompt.setGeometry(70, 305, 200, 20);
    delayValue.setGeometry(70, 340, 200, 20);
    actionEdit.setGeometry(320, 40, 90, 20);
    actionRemove.setGeometry(460, 40, 90, 20);
    actionUp.setGeometry(600, 40, 90, 20);
    actionDown.setGeometry(740, 40, 90, 20);
    actionList.setGeometry(320, 70, 510, 255);
    actionInsert.setGeometry(750, 335, 80, 20);
    configSave.setGeometry(660, 400, 80, 20);
    configCancel.setGeometry(750, 400, 80, 20);

    // Enforce minimum 5ms delay
    delayValue.setValidator(new QIntValidator(5, INT32_MAX));
    delayValue.setText("5");

    actionList.setToolTip("Double click to toggle press/release");

    // Connect signals/slots
    connect(Profile::instance(), SIGNAL(profileChanged()), this, SLOT(loadSettings()));
    connect(&keyGrabber, SIGNAL(keyPressed(Key)), this, SLOT(keyPressed(Key)));
    connect(&macroAdd, SIGNAL(released()), this, SLOT(createNewMacro()));
    connect(&macroDelete, SIGNAL(released()), this, SLOT(deleteMacro()));
    connect(&actionUp, SIGNAL(released()), this, SLOT(moveKeyUp()));
    connect(&actionDown, SIGNAL(released()), this, SLOT(moveKeyDown()));
    connect(&actionRemove, SIGNAL(released()), this, SLOT(removeKey()));
    connect(&actionInsert, SIGNAL(released()), this, SLOT(insertKey()));
    connect(&actionEdit, SIGNAL(released()), this, SLOT(editKeybind()));
    connect(&actionList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(editKey()));
    connect(&macroList, SIGNAL(currentTextChanged(QString)), this, SLOT(changeCurrentMacro(QString)));
    connect(&macroName, SIGNAL(returnPressed()), this, SLOT(changeName()));

    connect(&delayBeginRecord, SIGNAL(released()), this, SLOT(beginRecord()));
    connect(&delayValue, SIGNAL(editingFinished()), this, SLOT(applyDelayValue()));
    connect(&recorder, SIGNAL(recordFinished(ActionList&)), this, SLOT(finishedRecording(ActionList&)));

    connect(&delayNone, SIGNAL(released()), this, SLOT(delayChange()));
    connect(&delayFixed, SIGNAL(released()), this, SLOT(delayChange()));
    connect(&delayRecord, SIGNAL(released()), this, SLOT(delayChange()));

    connect(&configSave, SIGNAL(released()), this, SLOT(saveSettings()));
    connect(&configCancel, SIGNAL(released()), this, SLOT(loadSettings()));
}

void MacroTab::showEvent(QShowEvent *event)
{
    if (macroList.count() == 0)
        loadSettings();

    if (event != nullptr)
        event->accept();
}

bool MacroTab::isModified(void) const
{
    return Macro::modified(currentName.toStdString(), currentMacro) ||
        Macro::delayType(currentName.toStdString()) != currentDelay;
}

void MacroTab::saveSettings(void)
{
    Macro::replace(macroName.text().toStdString(), currentMacro);
    Macro::setDelayType(macroName.text().toStdString(), currentDelay);

    Macro::save(Profile::current());
}

void MacroTab::loadSettings(void)
{
    // Load macros from the config file
    Macro::load(Profile::current());
    reloadMacroList();
}

void MacroTab::reloadMacroList(void)
{
    auto names = Macro::getNames();
    if (names.empty()) {
        Macro::get("Macro 1");
        names.emplace_back("Macro 1");
    }

    macroList.blockSignals(true);
    macroList.clear();
    for (const auto& name : names)
        macroList.addItem(name.c_str());
    macroList.blockSignals(false);

    loadMacro(names.front().c_str());
}

void MacroTab::changeName(void)
{
    auto newName = macroName.text();

    if (currentName == newName || newName.isEmpty())
        return;

    Macro::rename(currentName.toStdString(), newName.toStdString());

    // Update controls
    // Ensure ignoreNext... is always true so a save prompt doesn't come up
    // setCurrentText() will return it to false
    ignoreNextMacroChange = true;
    macroList.removeItem(macroList.currentIndex());
    ignoreNextMacroChange = true;
    macroList.addItem(newName);
    ignoreNextMacroChange = true;
    macroList.setCurrentText(newName);
    currentName = newName;

    Macro::save(Profile::current());
}

void MacroTab::changeCurrentMacro(QString name)
{
    // Don't bother with a prompt if it isn't needed
    if (ignoreNextMacroChange) {
        ignoreNextMacroChange = false;
        return;
    }

    if (!currentName.isEmpty() && isModified()) {
        auto choice = QMessageBox::warning(this, "Unsaved Macro",
            "Would you like to save your changes?", QMessageBox::Yes,
            QMessageBox::No, QMessageBox::Cancel);

        switch (choice) {
        case QMessageBox::Yes:
            saveSettings();
            break;
        case QMessageBox::No:
            loadMacro(name);
            break;
        default:
            // Ignore the change back to currentName, so a second
            // save prompt doesn't appear
            ignoreNextMacroChange = true;
            macroList.setCurrentText(currentName);
            break;
        }
    } else {
        loadMacro(name);
    }
}

void MacroTab::loadMacro(const QString& name)
{
    currentName = name;
    currentMacro = Macro::get(name.toStdString());

    // Select delay type
    currentDelay = Macro::delayType(name.toStdString());
    delayNone.setChecked(currentDelay == Macro::NoDelay);
    delayFixed.setChecked(currentDelay == Macro::FixedDelay);
    delayRecord.setChecked(currentDelay == Macro::RecordedDelay);

    // Show appropriate controls
    lDelayPrompt.setVisible(delayFixed.isChecked());
    delayValue.setVisible(delayFixed.isChecked());

    // Load fixed delay value
    if (currentDelay == Macro::FixedDelay)
        delayValue.setText(std::to_string(currentMacro.front().delay.count()).c_str());

    macroName.setText(name);
    macroList.setCurrentText(name);

    reloadMacro();
}

void MacroTab::reloadMacro(void)
{
    // List keys
    actionList.clear();
    for (auto &k : currentMacro) {
        QString text (k.press ? "Prs. " : "Rel. ");
        text += k.key.toString();
        text += QString(" (") + std::to_string(k.delay.count()).c_str() + "ms)";
        actionList.addItem(text);
    }
}

void MacroTab::createNewMacro(void)
{
    std::string name ("Macro ");
    unsigned int i;

    // Create a unique name
    auto names = Macro::getNames();
    for (i = 1; std::find(names.begin(), names.end(), name + std::to_string(i)) != names.end(); i++);
    name += std::to_string(i);

    // Create the macro
    macroList.addItem(name.c_str());
    loadMacro(name.c_str());
}

void MacroTab::deleteMacro(void)
{
    Macro::remove(macroList.currentText().toStdString());
    ignoreNextMacroChange = true;
    macroList.removeItem(macroList.currentIndex());
    reloadMacroList();
}

void MacroTab::moveKeyUp(void)
{
    // Move data
    auto row = getCurrentActionListRow();
    if (row > 0 && row < currentMacro.size())
        std::swap(currentMacro[row], currentMacro[row - 1]);

    actionList.moveCurrentUp();
}

void MacroTab::moveKeyDown(void)
{
    // Move data
    auto row = getCurrentActionListRow();
    if (row + 1 < currentMacro.size())
        std::swap(currentMacro[row], currentMacro[row + 1]);

    actionList.moveCurrentDown();
}

void MacroTab::insertKey(void)
{
    // Get the macro's key list
    auto row = getCurrentActionListRow();
    int index = row < currentMacro.size() ? static_cast<int>(row) : 0;

    // Add a new key
    currentMacro.emplace(currentMacro.begin() + index);
    actionList.insertItem(index, currentMacro[index].key.toString());
}

void MacroTab::removeKey(void)
{
    auto row = getCurrentActionListRow();
    if (row < currentMacro.size())
        currentMacro.erase(currentMacro.begin() + row);

    actionList.removeCurrent();
}

void MacroTab::editKey(void)
{
    auto row = getCurrentActionListRow();
    if (row < currentMacro.size()) {
        currentMacro.at(row).press ^= true;
        reloadMacro();
    }
}

void MacroTab::editKeybind(void)
{
    keyGrabber.show();
}

void MacroTab::keyPressed(Key key)
{
    auto row = getCurrentActionListRow();
    if (row < currentMacro.size()) {
        currentMacro[row].key = key;
        actionList.item(row)->setText(key.toString());
        reloadMacro();
    }
}

void MacroTab::delayChange(void)
{
    currentDelay = delayNone.isChecked() ? Macro::NoDelay :
        (delayFixed.isChecked() ? Macro::FixedDelay : Macro::RecordedDelay);

    lDelayPrompt.setVisible(delayFixed.isChecked());
    delayValue.setVisible(delayFixed.isChecked());
    applyDelayValue();
}

void MacroTab::applyDelayValue(void)
{
    if (!delayRecord.isChecked()) {
        auto delay = std::chrono::milliseconds(
            delayFixed.isChecked() ? delayValue.text().toInt() : 0);
        std::for_each(currentMacro.begin(), currentMacro.end(),
                      [delay](auto& k) { k.delay = delay; });
    }

    reloadMacro();
}

void MacroTab::beginRecord(void)
{
    recorder.show();
}

void MacroTab::finishedRecording(ActionList &ks)
{
    currentMacro = ks;
    applyDelayValue();
}
