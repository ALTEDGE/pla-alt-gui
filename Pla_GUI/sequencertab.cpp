#include "controller.h"
#include "profile.h"
#include "sequencertab.h"

SequencerTab::SequencerTab(QWidget *parent) :
    QWidget(parent),
    currentPG(0),
    axisSel(this),
    keyDialog(this),
    selectedActions("", this),
    instruction("ENTER KEY OR MACRO COMMANDS", this),
    vector1("VECTOR 1 COMMAND", this),
    vector2("VECTOR 2 COMMAND", this),
    enableSequencer("Enable Sequencer", this),
    activeAction(nullptr)
{
    // Setup the action assigning buttons
    for (int i = 0; i < 16; i++) {
        actions[i] = new QPushButton("", this);
        actions[i]->setGeometry(i < 8 ? 300 : 450, 80 + 22 * (i < 8 ? i : i - 8), 140, 20);
        actions[i]->setStyleSheet("text-align: left; padding: 2px");
        actions[i]->setProperty("i", i);

        connect(actions[i], SIGNAL(pressed()), this, SLOT(setAction()));
    }

    // Set geometries
    selectedActions.setGeometry(300, 30, 60, 20);
    instruction.setGeometry(360, 30, 200, 20);
    vector1.setGeometry(300, 60, 140, 20);
    vector1.setAlignment(Qt::AlignCenter);
    vector2.setGeometry(450, 60, 140, 20);
    vector2.setAlignment(Qt::AlignCenter);
    enableSequencer.setGeometry(300, 10, 200, 20);

    // TODO load state from setting
    enableSequencer.setChecked(false);
    setPG(currentPG);

    // Connect signals/slots
    connect(&enableSequencer, SIGNAL(toggled(bool)), this, SLOT(sequencerToggled(bool)));
    connect(&axisSel, SIGNAL(released(int)), this, SLOT(setPG(int)));
    connect(&axisSel, SIGNAL(joystickChanged()), this, SLOT(joystickChanged()));
    connect(&keyDialog, SIGNAL(keyPressed(Key)), this, SLOT(keyPressed(Key)));
}

void SequencerTab::sequencerToggled(bool state)
{
    Controller::setSequencing(state);
}

void SequencerTab::setAction(void)
{
    // Find the pressed button
    for (unsigned int i = 0; i < 16; i++) {
        if (actions[i]->isDown()) {
            activeAction = actions[i];
            break;
        }
    }

    keyDialog.show();
}

void SequencerTab::keyPressed(Key key)
{
    int i = activeAction->property("i").toInt();

    // Set action for the enabled joystick
    if (axisSel.left()) {
        Controller::Left.setKey(i, key);
        activeAction->setText(key.toString());
    } else if (axisSel.primary()) {
        Controller::Primary.setPGKey(currentPG, i, key);
        activeAction->setText(key.toString());
    } else if (axisSel.right()) {
        Controller::Right.setKey(i, key);
        activeAction->setText(key.toString());
    }
    activeAction->setToolTip(activeAction->text());

    // Save current stuff
    Controller::save(Profile::current());
}

void SequencerTab::updateActionButtons(int n)
{
    currentPG = n;
    if (axisSel.left()) {
        for (int i = 0; i < 16; i++) {
            actions[i]->setText(Controller::Left.getText(i));
            actions[i]->setToolTip(actions[i]->text());
        }
    } else if (axisSel.primary()) {
        for (int i = 0; i < 16; i++) {
            actions[i]->setText(Controller::Primary.getText(currentPG, i));
            actions[i]->setToolTip(actions[i]->text());
        }
    } else if (axisSel.right()) {
        for (int i = 0; i < 16; i++) {
            actions[i]->setText(Controller::Right.getText(i));
            actions[i]->setToolTip(actions[i]->text());
        }
    }
}

void SequencerTab::setPG(int n)
{
    selectedActions.setText(QString("MAP_PG") + QChar(n + '1'));
    updateActionButtons(n);
}

void SequencerTab::joystickChanged(void)
{
    updateActionButtons(currentPG);

    QString text (axisSel.left() ? "L. AUX" : (axisSel.right() ? "R. AUX" :
        QString("MAP_PG") + QChar(currentPG + '1')));
    selectedActions.setText(text);
}
