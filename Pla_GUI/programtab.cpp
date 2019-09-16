#include "programtab.h"

#include "controller.h"
#include "profile.h"
#include "thresholdsetter.h"

ProgramTab::ProgramTab(QWidget *parent) :
    SavableTab(parent),
    lSelectJoystick("SELECT JOYSTICK", this),
    lSelectedPG("", this),
    lEnterKeyOrMacro("ENTER KEY OR MACRO COMMAND", this),
    lVector1("VECTOR 1 COMMAND", this),
    lVector2("VECTOR 2 COMMAND", this),
    lButton("BUTTON ACTION:", this),
    lJoystickGuide(this),
    pixJoystick("assets/joystick.png"),
    pixPJoystick("assets/pjoystick.png"),
    useLeftJoystick("LEFT AUX JOYSTICK", this),
    useRightJoystick("RIGHT AUX JOYSTICK", this),
    usePrimaryJoystick("PRIMARY JOYSTICK", this),
    pgButtons(this),
    joystickButton(this),
    useDiagonals("ENABLE DIAGONAL\nMOVEMENT", this),
    useSequencer("ENABLE SEQUENCER", this),
    configSave("SAVE", this),
    configCancel("CANCEL", this),
    configThreshold("THRESHOLD SETTINGS", this),
    leftData(Controller::Left),
    rightData(Controller::Right),
    primaryData(Controller::Primary),
    keyAssignDialog(this),
    thresholdDialog(this, parent)
{
    lJoystickGuide.setPixmap(pixJoystick);

    // Control placement
    lJoystickGuide.setGeometry(150, 40, 108, 117);
    lSelectJoystick.setGeometry(150, 180, 150, 20);
    usePrimaryJoystick.setGeometry(150, 210, 150, 24);
    useLeftJoystick.setGeometry(150, 235, 150, 24);
    useRightJoystick.setGeometry(150, 260, 150, 24);
    useSequencer.setGeometry(150, 290, 150, 20);
    useDiagonals.setGeometry(150, 315, 150, 30);
    configThreshold.setGeometry(150, 355, 150, 20);
    lSelectedPG.setGeometry(440, 40, 60, 20);
    lEnterKeyOrMacro.setGeometry(520, 40, 250, 20);
    lVector1.setGeometry(460, 70, 140, 20);
    lVector2.setGeometry(610, 70, 140, 20);
    configSave.setGeometry(365, 400, 80, 20);
    configCancel.setGeometry(455, 400, 80, 20);

    // Additional visual setups
    lVector1.setAlignment(Qt::AlignCenter);
    lVector2.setAlignment(Qt::AlignCenter);
    useLeftJoystick.setLayoutDirection(Qt::RightToLeft);
    useRightJoystick.setLayoutDirection(Qt::RightToLeft);
    usePrimaryJoystick.setLayoutDirection(Qt::RightToLeft);

    // Set up PG buttons
    for (int i = 0; i < 8; i++) {
        auto button = new QPushButton(QString("MAP PG_") + static_cast<char>('1' + i), this);
        button->setGeometry(330, 35 + 22 * i, 80, 20);
        button->setCheckable(true);
        pgButtons.addButton(button, i);
    }
    pgButtons.setExclusive(true);

    for (int i = 0; i < 8; i++) {
        lKeySlots[i] = new QLabel(QString(std::to_string(i + 1).c_str()), this);
        lKeySlots[i]->setGeometry(440, 100 + 28 * i, 20, 24);
    }

    // Set up key slots
    for (int i = 0; i < 16; i++) {
        auto button = new QPushButton("", this);
        button->setGeometry(i < 8 ? 460 : 610, 100 + 28 * (i < 8 ? i : i - 8), 140, 24);
        button->setStyleSheet("text-align: left; padding: 2px");
        button->setProperty("i", i);
        keySlots.addButton(button, i);
    }

    lButton.setGeometry(460, 350, 140, 24);
    lButton.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    // TODO match v.seq. buttons (align)
    joystickButton.setGeometry(610, 350, 140, 24);

    // Connect signals/slots
    connect(&pgButtons, SIGNAL(buttonClicked(int)), this, SLOT(updateControls()));
    connect(&keySlots, SIGNAL(buttonClicked(int)), this, SLOT(assignSlot(int)));
    connect(&joystickButton, SIGNAL(released()), this, SLOT(assignButton()));
    connect(&useLeftJoystick, SIGNAL(released()), this, SLOT(updateControls()));
    connect(&useRightJoystick, SIGNAL(released()), this, SLOT(updateControls()));
    connect(&usePrimaryJoystick, SIGNAL(released()), this, SLOT(updateControls()));
    connect(&useDiagonals, SIGNAL(toggled(bool)), this, SLOT(setDiagonals(bool)));
    connect(&useSequencer, SIGNAL(toggled(bool)), this, SLOT(setSequencer(bool)));
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
        button->setVisible(primary);

    // Set key slots and diagonal check
    if (primary) {
        //lJoystickGuide.setPixmap(pixPJoystick);
        thresholdDialog.setJoystick("PRIMARY");
        lSelectedPG.setText(QString("MAP PG_") + static_cast<char>(pgButtons.checkedId() + '1'));
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
        setDiagonals(primaryData->getDiagonals());
        setSequencer(primaryData->getSequencing());
    } else if (useLeftJoystick.isChecked()) {
        //lJoystickGuide.setPixmap(pixJoystick);
        thresholdDialog.setJoystick("LEFT");
        lSelectedPG.setText("L. AUX.");

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
        setDiagonals(diag);
        setSequencer(leftData->getSequencing());
    } else {
        //lJoystickGuide.setPixmap(pixJoystick);
        thresholdDialog.setJoystick("RIGHT");
        lSelectedPG.setText("R. AUX.");

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
        setDiagonals(diag);
        setSequencer(rightData->getSequencing());
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
        if (i < 8)
            lKeySlots[i]->setVisible(!enabled);
    }
}

void ProgramTab::assignButton(void)
{
    assignSlot(16);
}

void ProgramTab::assignSlot(int index)
{
    // Remember what slot was selected
    assigningSlot = index;
    Controller::setEnabled(false);
    keyAssignDialog.show();
}

void ProgramTab::keyPressed(Key key)
{
    // Set the key
    Controller::setEnabled(true);
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
