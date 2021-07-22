#include "programtab.h"

#include "controller.h"
#include "profile.h"
#include "thresholdsetter.h"

ProgramTab::ProgramTab(QWidget *parent) :
    SavableTab(parent),
    lSelectJoystick("SELECT JOYSTICK", this),
    lEnterKeyOrMacro("ENTER KEY\nOR\nMACRO COMMAND", this),
    lVector1("VECTOR 1 COMMANDS", this),
    lVector2("VECTOR 2 COMMANDS", this),
    lJoystick("JOYSTICK", this),
    lSequencer("X2 VECTOR\nSEQUENCER", this),
    lButtonAction("JOYSTICK\nBUTTON ACTION", this),
    separator(this),
    lJoystickGuide(this),
    pixJoystick("assets/joystick.png"),
    pixPJoystick("assets/joystick-large.png"),
    pixArrow("assets/arrow.png"),
    lArrowDown(this),
    pixArrowDown("assets/arrow-down.png"),
    useLeftJoystick("LEFT AUX JOYSTICK", this),
    useRightJoystick("RIGHT AUX JOYSTICK", this),
    usePrimaryJoystick("PRIMARY JOYSTICK", this),
    pgButtons(this),
    joystickButton(this),
    useDiagonals("ENABLE DIAGONAL MOVEMENT\n *STEER WITH JOYSTICK", this),
    useSequencer("ENABLE SEQUENCER", this),
    useLatch("TOGGLE On-Off", this),
    configSave("SAVE", this),
    configCancel("CANCEL", this),
    configThreshold("TRIGGER SETTINGS", this),
    leftData(Controller::Left),
    rightData(Controller::Right),
    primaryData(Controller::Primary),
    keyAssignDialog(this),
    thresholdDialog(this, parent)
{
    // Control placement
    lSelectJoystick.setGeometry(133, 140, 150, 20);
    usePrimaryJoystick.setGeometry(133, 170, 150, 24);
    useLeftJoystick.setGeometry(40, 210, 150, 24);
    useRightJoystick.setGeometry(220, 210, 150, 24);
    configThreshold.setGeometry(140, 250, 140, 24);
    useSequencer.setGeometry(142, 305, 150, 20);
    joystickButton.setGeometry(140, 353, 140, 24);
    useLatch.setGeometry(154, 381, 140, 30);
    useDiagonals.setGeometry(116, 423, 200, 30);
    configSave.setGeometry(695, 430, 80, 20);
    configCancel.setGeometry(785, 430, 80, 20);
    separator.setGeometry(0, 134, 900, 1);
    separator.setStyleSheet("background: #777");

    // Additional visual setups
    lSelectJoystick.setAlignment(Qt::AlignCenter);
    lSelectJoystick.setStyleSheet("font-size: 13px");
    useLeftJoystick.setLayoutDirection(Qt::RightToLeft);
    useRightJoystick.setLayoutDirection(Qt::RightToLeft);
    usePrimaryJoystick.setLayoutDirection(Qt::RightToLeft);

    lJoystick.setGeometry(5, 235, 102, 50);
    lSequencer.setGeometry(5, 290, 102, 50);
    lButtonAction.setGeometry(5, 351, 102, 50);
    lJoystick.setAlignment(Qt::AlignCenter);
    lSequencer.setAlignment(Qt::AlignCenter);
    lButtonAction.setAlignment(Qt::AlignCenter);
    for (int i = 0; i < 3; ++i) {
        lArrow[i].setParent(this);
        lArrow[i].setPixmap(pixArrow);
        lArrow[i].setGeometry(98, 247 + i * 53, 30, 32);
    }
    lArrowDown.setPixmap(pixArrowDown);
    lArrowDown.setGeometry(200, 378, 20, 10);

    // Set up PG buttons and joystick images
    lEnterKeyOrMacro.setGeometry(400, 145, 112, 60);
    lEnterKeyOrMacro.setAlignment(Qt::AlignCenter);
    lJoystickGuide.setPixmap(pixPJoystick);
    lJoystickGuide.setGeometry(410, 205, 108, 117);
    for (int i = 0; i < 8; i++) {
        auto button = new QPushButton(QString("MAP PG_") + static_cast<char>('1' + i), this);
        button->setGeometry(11 + 114 * i, 10, 80, 20);
        button->setCheckable(true);
        pgButtons.addButton(button, i);

        lPGJoystick[i].setParent(this);
        lPGJoystick[i].setPixmap(pixJoystick);
        lPGJoystick[i].setGeometry(7 + 114 * i, 28, 108, 117);
    }
    pgButtons.setExclusive(true);

    for (int i = 0; i < 8; i++) {
        lKeySlots[i].setParent(this);
        lKeySlots[i].setText(std::to_string(i + 1).c_str());
        lKeySlots[i].setGeometry(530, 171 + 28 * i, 20, 24);

        lKeySlots2[i].setParent(this);
        lKeySlots2[i].setText(std::to_string(i + 1).c_str());
        lKeySlots2[i].setGeometry(705, 171 + 28 * i, 20, 24);
    }

    // Set up key slots
    lVector1.setGeometry(550, 145, 140, 25);
    lVector2.setGeometry(725, 145, 140, 25);
    lVector1.setAlignment(Qt::AlignCenter);
    lVector2.setAlignment(Qt::AlignCenter);
    for (int i = 0; i < 16; i++) {
        auto button = new QPushButton("", this);
        button->setGeometry(i < 8 ? 550 : 725, 170 + 28 * (i % 8), 140, 24);
        button->setStyleSheet("text-align: left; padding: 2px");
        button->setProperty("i", i);
        keySlots.addButton(button, i);
    }

    // Connect signals/slots
    connect(&pgButtons, SIGNAL(buttonClicked(int)), this, SLOT(updateControls()));
    connect(&keySlots, SIGNAL(buttonClicked(int)), this, SLOT(assignSlot(int)));
    connect(&joystickButton, SIGNAL(released()), this, SLOT(assignButton()));
    connect(&useLeftJoystick, SIGNAL(released()), this, SLOT(updateControls()));
    connect(&useRightJoystick, SIGNAL(released()), this, SLOT(updateControls()));
    connect(&usePrimaryJoystick, SIGNAL(released()), this, SLOT(updateControls()));
    connect(&useDiagonals, SIGNAL(toggled(bool)), this, SLOT(setDiagonals(bool)));
    connect(&useSequencer, SIGNAL(toggled(bool)), this, SLOT(setSequencer(bool)));
    connect(&useLatch, SIGNAL(toggled(bool)), this, SLOT(setButtonSticky(bool)));
    connect(&keyAssignDialog, SIGNAL(keyPressed(Key)), this, SLOT(keyPressed(Key)));
    connect(&configSave, SIGNAL(released()), this, SLOT(saveSettings()));
    connect(&configCancel, SIGNAL(released()), this, SLOT(loadSettings()));
    connect(&configThreshold, SIGNAL(released()), this, SLOT(openThresholdSettings()));

    // Assign default states
    usePrimaryJoystick.setChecked(true);
    pgButtons.button(0)->setChecked(true);

    loadSettings();
}

void ProgramTab::showEvent(QShowEvent *event)
{
    updateControls();

    if (event != nullptr)
        event->accept();
}

bool ProgramTab::isModified(void) const
{
    return primaryData.isModified() || leftData.isModified() ||
        rightData.isModified();
}

void ProgramTab::saveSettings(void)
{
    leftData.save();
    rightData.save();
    primaryData.save();
    Controller::save(Profile::current());
}

void ProgramTab::loadSettings(void)
{
    if (isModified()) {
        leftData.revert();
        rightData.revert();
        primaryData.revert();
    }

    showEvent(nullptr);
}

void ProgramTab::openThresholdSettings(void)
{
    thresholdDialog.show();
}

void ProgramTab::updateControls(void)
{
    // Hide PG buttons if primary joystick isn't selected
    bool primary = usePrimaryJoystick.isChecked();
    for (auto& button : pgButtons.buttons())
        button->setEnabled(primary);

    // Set key slots and diagonal check
    if (primary) {
        thresholdDialog.setJoystick("PRIMARY");
        for (int i = 0; i < 16; i++) {
            auto text = primaryData->getText(pgButtons.checkedId(), i);
            keySlots.button(i)->setText(text);
            keySlots.button(i)->setToolTip(text);
        }
        auto text = primaryData->getText(pgButtons.checkedId(), 16);
        joystickButton.setText(text);
        joystickButton.setToolTip(text);

        useDiagonals.setChecked(primaryData->getDiagonals());
        useSequencer.setChecked(primaryData->getSequencing());
        useLatch.setChecked(primaryData->getButtonSticky());
        setDiagonals(primaryData->getDiagonals());
        setSequencer(primaryData->getSequencing());
        setButtonSticky(primaryData->getButtonSticky());
    } else if (useLeftJoystick.isChecked()) {
        thresholdDialog.setJoystick("LEFT");

        for (int i = 0; i < 16; i++) {
            auto text = leftData->getText(i);
            keySlots.button(i)->setText(text);
            keySlots.button(i)->setToolTip(text);
        }
        auto text = leftData->getText(16);
        joystickButton.setText(text);
        joystickButton.setToolTip(text);

        bool diag = leftData->getDiagonals();
        useDiagonals.setChecked(diag);
        useSequencer.setChecked(leftData->getSequencing());
        useLatch.setChecked(leftData->getButtonSticky());
        setDiagonals(diag);
        setSequencer(leftData->getSequencing());
        setButtonSticky(leftData->getButtonSticky());
    } else {
        thresholdDialog.setJoystick("RIGHT");

        for (int i = 0; i < 16; i++) {
            auto text = rightData->getText(i);
            keySlots.button(i)->setText(text);
            keySlots.button(i)->setToolTip(text);
        }
        auto text = rightData->getText(16);
        joystickButton.setText(text);
        joystickButton.setToolTip(text);

        bool diag = rightData->getDiagonals();
        useDiagonals.setChecked(diag);
        useSequencer.setChecked(rightData->getSequencing());
        useLatch.setChecked(rightData->getButtonSticky());
        setDiagonals(diag);
        setSequencer(rightData->getSequencing());
        setButtonSticky(rightData->getButtonSticky());
    }

    // Set tool tips in case key slot can't fit text
    for (int i = 0; i < 16; i++)
        keySlots.button(i)->setToolTip(keySlots.button(i)->text());
}

void ProgramTab::setSequencer(bool enabled)
{
    // Use inc to skip showing diagonal key slots if we're using diagonal
    // movement instead.
    bool diag;

    if (usePrimaryJoystick.isChecked()) {
        diag = primaryData->getDiagonals();
        primaryData->setSequencing(enabled);
    } else if (useLeftJoystick.isChecked()) {
        diag = leftData->getDiagonals();
        leftData->setSequencing(enabled);
    } else {
        diag = rightData->getDiagonals();
        rightData->setSequencing(enabled);
    }

    unsigned int inc = 1 + (enabled & diag);
    for (int i = 8; i < 16; i += inc)
        keySlots.button(i)->setVisible(enabled);

    lVector1.setVisible(enabled);
    lVector2.setVisible(enabled);
    for (int i = 0; i < 8; ++i)
        lKeySlots2[i].setVisible(enabled && lKeySlots[i].isVisible());
}

void ProgramTab::setDiagonals(bool enabled)
{
    bool seq;

    if (usePrimaryJoystick.isChecked()) {
        seq = primaryData->getSequencing();
        primaryData->setDiagonals(enabled);
    } else if (useLeftJoystick.isChecked()) {
        seq = leftData->getSequencing();
        leftData->setDiagonals(enabled);
    } else {
        seq = rightData->getSequencing();
        rightData->setDiagonals(enabled);
    }

    // Hide diagonal key slots if diagonals are enabled
    for (int i = 1; i < (seq ? 16 : 8); i += 2) {
        keySlots.button(i)->setVisible(!enabled);
        if (i < 8) {
            lKeySlots[i].setVisible(!enabled);
            if (lVector1.isVisible())
                lKeySlots2[i].setVisible(!enabled);
        }
    }
}

void ProgramTab::setButtonSticky(bool enabled)
{
    if (usePrimaryJoystick.isChecked())
        primaryData->setButtonSticky(enabled);
    else if (useLeftJoystick.isChecked())
        leftData->setButtonSticky(enabled);
    else
        rightData->setButtonSticky(enabled);
}

void ProgramTab::assignButton(void)
{
    assignSlot(16);
}

void ProgramTab::assignSlot(int index)
{
    // Remember what slot was selected
    assigningSlot = index;
    keyAssignDialog.show();
}

void ProgramTab::keyPressed(Key key)
{
    // Set the key
    if (usePrimaryJoystick.isChecked())
        primaryData->setPGKey(pgButtons.checkedId(), assigningSlot, key);
    else if (useLeftJoystick.isChecked())
        leftData->setKey(assigningSlot, key);
    else
        rightData->setKey(assigningSlot, key);

    // Update key slot text
    if (assigningSlot == 16) {
        joystickButton.setText(key.toString());
        joystickButton.setToolTip(key.toString());
    } else {
        keySlots.button(assigningSlot)->setText(key.toString());
        keySlots.button(assigningSlot)->setToolTip(key.toString());
    }
}
