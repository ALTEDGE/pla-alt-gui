var targetDirectoryPage = null;

function Component()
{
	installer.gainAdminRights();
    component.loaded.connect(this, this.installerLoaded);
}

Component.prototype.createOperations = function()
{
	component.createOperations();
    component.addOperation("CreateShortcut",
                           "@TargetDir@\\PLA_FPS.exe",
                           "@DesktopDir@\\PLA FPS.lnk",
                           "workingDirectory=@TargetDir@");

    component.addOperation("CreateShortcut",
                           "@TargetDir@\\PLA_FPS.exe",
                           "@StartMenuDir@\\PLA FPS.lnk",
                           "workingDirectory=@TargetDir@");
		
	component.addOperation("Execute", "cmd", "/C", "@TargetDir@\\drivers\\dpinst-amd64.exe", "/Q", "||", "ver");
	component.addOperation("Execute", "cmd", "/C", "@TargetDir@\\drivers\\vc_redist.x64.exe", "/quiet", "/norestart", "||", "ver");
}

Component.prototype.installerLoaded = function()
{
    installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
    installer.addWizardPage(component, "TargetWidget", QInstaller.TargetDirectory);

    targetDirectoryPage = gui.pageWidgetByObjectName("DynamicTargetWidget");
    targetDirectoryPage.windowTitle = "Choose Installation Directory";
    targetDirectoryPage.description.setText("Please select where PLA FPS will be installed:");
    targetDirectoryPage.targetDirectory.textChanged.connect(this, this.targetDirectoryChanged);
    targetDirectoryPage.targetDirectory.setText(installer.value("TargetDir"));
    targetDirectoryPage.targetChooser.released.connect(this, this.targetChooserClicked);

    gui.pageById(QInstaller.ComponentSelection).entered.connect(this, this.componentSelectionPageEntered);
}

Component.prototype.targetChooserClicked = function()
{
    var dir = QFileDialog.getExistingDirectory("", targetDirectoryPage.targetDirectory.text);
    targetDirectoryPage.targetDirectory.setText(dir);
}

Component.prototype.targetDirectoryChanged = function()
{
    var dir = targetDirectoryPage.targetDirectory.text;
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Existing installation detected and will be overwritten.</p>");
    }
    else if (installer.fileExists(dir)) {
        targetDirectoryPage.warning.setText("<p style=\"color: red\">Installing in existing directory. It will be wiped on uninstallation.</p>");
    }
    else {
        targetDirectoryPage.warning.setText("");
    }
    installer.setValue("TargetDir", dir);
}

Component.prototype.componentSelectionPageEntered = function()
{
    var dir = installer.value("TargetDir");
    if (installer.fileExists(dir) && installer.fileExists(dir + "/maintenancetool.exe")) {
        installer.execute(dir + "/maintenancetool.exe", "--script=" + dir + "/scripts/auto_uninstall.qs");
    }
}
