#include "macro.h"
#include "config.h"

#include <chrono>
#include <thread>

std::map<std::string, ActionList> Macro::macros;

ActionList& Macro::get(const std::string& name)
{
    // This will create the macro if it doesn't exist
    return macros[name];
}

std::vector<std::string> Macro::getNames(void)
{
    std::vector<std::string> names;

    for (const auto& m : macros)
        names.emplace_back(m.first);

    return names;
}

bool Macro::exists(const std::string& name)
{
    auto macro = macros.find(name);
    return macro != macros.end();
}

bool Macro::modified(const std::string &name, ActionList keys)
{
    auto macro = macros.find(name);
    if (macro == macros.end())
        return true;

    return macro->second != keys;
}

int Macro::delayType(const std::string& name)
{
    auto macro = macros.find(name);
    if (macro == macros.end())
        return NoDelay;

    return macro->second.getDelayType();
}

void Macro::setDelayType(const std::string& name, int type)
{
    auto macro = macros.find(name);
    if (macro == macros.end())
        return;

    macro->second.setDelayType(type);
}

void Macro::rename(const std::string& oldName, const std::string& newName)
{
    auto macro = macros.find(oldName);
    if (macro == macros.end())
        return;

    // Copy the ActionList to the new macro entry
    macros[newName] = (*macro).second;

    macros.erase(macro);
}

void Macro::replace(const std::string& name, ActionList& keys)
{
    auto macro = macros.find(name);
    if (macro == macros.end())
        macros.emplace(name, keys);
    else
        macros.at(name) = keys;
}

void Macro::fire(const std::string& name)
{
    auto macro = macros.find(name);
    if (macro == macros.end())
        return;

    // Loop through all Actions
    for (const auto& a : (*macro).second) {
        a.key.fire(a.press);
        std::this_thread::sleep_for(std::max(config::MinimumMacroDelay, a.delay));
    }
}

void Macro::load(QSettings& settings)
{
    macros.clear();

    settings.beginGroup("macros");
    for (const auto& macro : settings.childGroups()) {
        ActionList actions;

        settings.beginGroup(macro);

        actions.setDelayType(settings.value("delayType").toInt());

        // Load all keys
        int count = settings.childGroups().count();
        QString key ("key");
        for (int i = 0; i < count; i++) {
            settings.beginGroup(key + std::to_string(i).c_str());

            // Load key data
            Key k (settings);

            // Store key data, press/release, and delay
            actions.emplace_back(k, settings.value("press").toBool(),
                std::chrono::milliseconds(settings.value("delay").toUInt()));
            settings.endGroup();
        }
        settings.endGroup();

        // Add the macro to the list
        macros.emplace(macro.toStdString(), actions);
    }
    settings.endGroup();
}

void Macro::save(QSettings& settings)
{
    // Delete old macro list
    settings.remove("macros");

    settings.beginGroup("macros");

    // For each macro
    auto names = getNames();
    for (const auto& name : names) {
        settings.beginGroup(name.c_str());

        auto& vec = macros[name];
        settings.setValue("delayType", vec.getDelayType());

        // For each key
        QString key ("key");
        for (unsigned int i = 0; i < vec.size(); i++) {
            settings.beginGroup(key + std::to_string(i).c_str());

            vec[i].key.save(settings);
            settings.setValue("press", vec[i].press);
            settings.setValue("delay", static_cast<long long>(vec[i].delay.count()));

            settings.endGroup();
        }

        settings.endGroup();
    }

    settings.endGroup();
    settings.sync();
}
