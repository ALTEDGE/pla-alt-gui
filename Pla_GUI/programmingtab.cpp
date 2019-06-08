#include "programmingtab.h"
#include "controllerroutine.h"
#include "keygrabber.h"
#include "mainwindow.h"

ProgrammingTab::ProgrammingTab(QWidget *parent) :
    QWidget(parent),
    axisSel(this),
    keyDialog(this),
    selectedPG("", this),
    instruction("ENTER KEY OR MACRO COMMANDS", this)
{
    // Create the 8 action assigning buttons
    for (unsigned int i = 0; i < 8; i++) {
        actions[i] = new QPushButton("", this);
        actions[i]->setGeometry(300, 40 + 22 * i, 140, 20);
        actions[i]->setStyleSheet("text-align: left; padding: 2px");
        // Tie the action index to the button
        actions[i]->setProperty("i", i);

        connect(actions[i], SIGNAL(pressed()), this, SLOT(setAction()));
    }

    // Set geometries
    selectedPG.setGeometry(300, 10, 60, 20);
    instruction.setGeometry(360, 10, 200, 20);

    // Set PG
    currentPG = 0;
    setPG(currentPG);

    // Connect signals/slots
    connect(&axisSel, SIGNAL(released(int)), this, SLOT(setPG(int)));
    connect(&axisSel, SIGNAL(joystickChanged()), this, SLOT(joystickChanged()));
    connect(&keyDialog, SIGNAL(keyPressed(Key)), this, SLOT(keyPressed(Key)));
    connect(&axisSel, SIGNAL(diagonalChecked(bool)), this, SLOT(diagonalChecked(bool)));
}

void ProgrammingTab::setAction(void)
{
    // Find the pressed button
    for (unsigned int i = 0; i < 8; i++) {
        if (actions[i]->isDown()) {
            lastPressed = actions[i];
            break;
        }
    }

    // Prompt for keypress
    keyDialog.show();
}

void ProgrammingTab::diagonalChecked(bool state)
{
    if (axisSel.left())
        Controller::Left.setDiagonals(state);
    else if (axisSel.primary())
        Controller::Primary.setDiagonals(state);
    else if (axisSel.right())
        Controller::Right.setDiagonals(state);
}

void ProgrammingTab::keyPressed(Key key)
{
    // Get the action index
    int i = lastPressed->property("i").toInt();

    // Assign the keystroke according to the selected joystick
    if (axisSel.left()) {
        Controller::Left.setKey(i, key);
        // Update the button's text
        lastPressed->setText(key.toString());
    } else if (axisSel.primary()) {
        Controller::Primary.setPGKey(currentPG, i, key);
        lastPressed->setText(key.toString());
    } else if (axisSel.right()) {
        Controller::Right.setKey(i, key);
        lastPressed->setText(key.toString());
    }
    // Set tool tip in case button text is too long
    lastPressed->setToolTip(lastPressed->text());

    // Save current stuff
    Controller::save(MainWindow::settings);
}

// This function updates the key assigning buttons; if the left or right auxilary
// joystick is selected then the argument will do nothing.
// TODO probably give the function a better name
void ProgrammingTab::loadPG(int n)
{
    currentPG = n;
    if (axisSel.left()) {
        // Set buttons to left auxilary's actions
        for (unsigned int i = 0; i < 8; i++) {
            actions[i]->setText(Controller::Left.getText(i));
            actions[i]->setToolTip(actions[i]->text());
        }
    } else if (axisSel.primary()) {
        // Set buttons to the current PG's actions
        for (unsigned int i = 0; i < 8; i++) {
            actions[i]->setText(Controller::Primary.getText(currentPG, i));
            actions[i]->setToolTip(actions[i]->text());
        }
    } else if (axisSel.right()) {
        // Set buttons to right auxilary's actions
        for (unsigned int i = 0; i < 8; i++) {
            actions[i]->setText(Controller::Right.getText(i));
            actions[i]->setToolTip(actions[i]->text());
        }
    }
}

void ProgrammingTab::setPG(int n)
{
    selectedPG.setText(QString("MAP_PG") + QChar(n + '1'));
    loadPG(n);
}

void ProgrammingTab::joystickChanged(void)
{
    loadPG(currentPG);

    QString text (axisSel.left() ? "L. AUX" : (axisSel.right() ? "R. AUX" :
        QString("MAP_PG") + QChar(currentPG + '1')));
    selectedPG.setText(text);
}
