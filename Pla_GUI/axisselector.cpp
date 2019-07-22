#include "axisselector.h"

AxisSelector::AxisSelector(QWidget *parent) :
    QWidget(parent),
    selectLabel("SELECT JOYSTICK", this),
    selectPrimary("PRIMARY JOYSTICK", this),
    selectLeft("LEFT AUX JOYSTICK", this),
    selectRight("RIGHT AUX JOYSTICK", this),
    enableDiagonal("USE DIAGONALS", this),
    mapPGGroup(this)
{
    // Restrict widget size
    setFixedSize(320, 380);

    // Set positions
    selectLabel.setGeometry(30, 220, 150, 20);
    selectPrimary.setGeometry(30, 250, 150, 20);
    selectLeft.setGeometry(30, 280, 150, 20);
    selectRight.setGeometry(30, 310, 150, 20);
    enableDiagonal.setGeometry(30, 190, 150, 20);

    // Right-align checkboxes
    selectPrimary.setLayoutDirection(Qt::RightToLeft);
    selectLeft.setLayoutDirection(Qt::RightToLeft);
    selectRight.setLayoutDirection(Qt::RightToLeft);

    selectPrimary.setChecked(true);

    // Setup signals/slots
    connect(&selectPrimary, SIGNAL(released()), this, SLOT(onJoystickChange()));
    connect(&selectLeft, SIGNAL(released()), this, SLOT(onJoystickChange()));
    connect(&selectRight, SIGNAL(released()), this, SLOT(onJoystickChange()));

    // Create the 8 PG buttons and add them to the button group
    for (int i = 0; i < 8; i++) {
        auto b = new QPushButton(QString("MAP PG_") + static_cast<char>(i + '1'), this);
        b->setGeometry(200, 40 + 22 * i, 80, 20);
        b->setCheckable(true);
        mapPGGroup.addButton(b, i);
    }

    // Only allow one PG to be selected at a time
    mapPGGroup.setExclusive(true);

    connect(&mapPGGroup, SIGNAL(buttonReleased(int)), this, SLOT(onRelease(int)));
    mapPGGroup.button(0)->setChecked(true);

    connect(&enableDiagonal, SIGNAL(clicked(bool)), this, SLOT(onDiagonalClick(bool)));
}

void AxisSelector::onRelease(int b)
{
    // Signal that this PG group is now selected
    emit released(b);
}

void AxisSelector::onJoystickChange(void)
{
    // Hide PG buttons if not on the primary joystick
    auto show = selectPrimary.isChecked();
    for (auto& b : mapPGGroup.buttons())
        b->setVisible(show);

    // Signal that a different joystick has been selected
    emit joystickChanged();
}

void AxisSelector::onDiagonalClick(bool enable)
{
    emit diagonalChecked(enable);
}

bool AxisSelector::left(void) const
{
    return selectLeft.isChecked();
}

bool AxisSelector::primary(void) const
{
    return selectPrimary.isChecked();
}

bool AxisSelector::right(void) const
{
    return selectRight.isChecked();
}
