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
    useLeftJoystick("LEFT AUX JOYSTICK", this),
    useRightJoystick("RIGHT AUX JOYSTICK", this),
    usePrimaryJoystick("PRIMARY JOYSTICK", this),
    pgButtons(this),
    useDiagonals("ENABLE DIAGONAL\nMOVEMENT", this),
    useSequencer("ENABLE SEQUENCER", this),
    configSave("SAVE", this),
    configCancel("CANCEL", this),
    configThreshold("THRESHOLD SETTINGS", this),
    leftData(Controller::Left),
    rightData(Controller::Right),
    primaryData(Controller::Primary),
    keyAssignDialog(this),
    thresholdDialog(this)
{
    // Control placement
    lSelectJoystick.setGeometry(30, 190, 150, 20);
    lSelectedPG.setGeometry(300, 10, 60, 20);
    lEnterKeyOrMacro.setGeometry(370, 10, 200, 20);
    lVector1.setGeometry(320, 55, 140, 20);
    lVector2.setGeometry(470, 55, 140, 20);
    useLeftJoystick.setGeometry(30, 250, 150, 20);
    useRightJoystick.setGeometry(30, 280, 150, 20);
    usePrimaryJoystick.setGeometry(30, 220, 150, 20);
    useDiagonals.setGeometry(30, 160, 150, 20);
    useSequencer.setGeometry(30, 130, 150, 20);
    configSave.setGeometry(235, 320, 80, 20);
    configCancel.setGeometry(325, 320, 80, 20);
    configThreshold.setGeometry(30, 30, 130, 20);

    // Additional visual setups
    lVector1.setAlignment(Qt::AlignCenter);
    lVector2.setAlignment(Qt::AlignCenter);
    useLeftJoystick.setLayoutDirection(Qt::RightToLeft);
    useRightJoystick.setLayoutDirection(Qt::RightToLeft);
    usePrimaryJoystick.setLayoutDirection(Qt::RightToLeft);

    // Set up PG buttons
    for (int i = 0; i < 8; i++) {
        auto button = new QPushButton(QString("MAP PG_") + static_cast<char>('1' + i), this);
        button->setGeometry(200, 35 + 22 * i, 80, 20);
        button->setCheckable(true);
        pgButtons.addButton(button, i);
    }
    pgButtons.setExclusive(true);

    // Set up key slots
    for (int i = 0; i < 16; i++) {
        auto button = new QPushButton("", this);
        button->setGeometry(i < 8 ? 320 : 470, 75 + 22 * (i < 8 ? i : i - 8), 140, 20);
        button->setStyleSheet("text-align: left; padding: 2px");
        button->setProperty("i", i);
        keySlots.addButton(button, i);
    }

    for (int i = 0; i < 8; i++) {
        lKeySlots[i] = new QLabel(QString(std::to_string(i + 1).c_str()), this);
        lKeySlots[i]->setGeometry(305, 75 + 22 * i, 20, 20);
    }

    // Connect signals/slots
    connect(&pgButtons, SIGNAL(buttonClicked(int)), this, SLOT(updateControls()));
    connect(&keySlots, SIGNAL(buttonClicked(int)), this, SLOT(assignSlot(int)));
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
    // Get Editing objects up-to-date
    leftData.save();
    rightData.save();
    primaryData.save();

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
        lSelectedPG.setText(QString("MAP PG_") + static_cast<char>(pgButtons.checkedId() + '1'));
        for (int i = 0; i < 16; i++)
            keySlots.button(i)->setText(primaryData->getText(pgButtons.checkedId(), i));

        useDiagonals.setChecked(primaryData->getDiagonals());
        useSequencer.setChecked(primaryData->getSequencing());
        setDiagonals(primaryData->getDiagonals());
        setSequencer(primaryData->getSequencing());
    } else if (useLeftJoystick.isChecked()) {
        lSelectedPG.setText("L. AUX.");

        for (int i = 0; i < 16; i++)
            keySlots.button(i)->setText(leftData->getText(i));

        bool diag = leftData->getDiagonals();
        useDiagonals.setChecked(diag);
        useSequencer.setChecked(leftData->getSequencing());
        setDiagonals(diag);
        setSequencer(leftData->getSequencing());
    } else {
        lSelectedPG.setText("R. AUX.");

        for (int i = 0; i < 16; i++)
            keySlots.button(i)->setText(rightData->getText(i));

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
    keySlots.button(assigningSlot)->setText(key.toString());
    keySlots.button(assigningSlot)->setToolTip(key.toString());
}
