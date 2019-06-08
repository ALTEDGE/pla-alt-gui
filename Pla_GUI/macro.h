#ifndef MACRO_H
#define MACRO_H

#include "key.h"

#include <map>
#include <string>
#include <vector>

/**
 * @class Action
 * @brief Contains a key action and other data that can be chained together in
 * a macro.
 *
 * Each Action in a macro can either start another macro, or press/release a
 * key. Each Action also has a settable delay to provide space between other
 * actions.
 */
struct Action {
    Key key;
    bool press;
    unsigned int delay;

    Action(Key k = Key(), bool p = true, unsigned int d = 0)
        : key(k), press(p), delay(d) {}

    bool operator==(const Action& other) const {
        return key == other.key && press == other.press && delay ==
            other.delay;
    }
};
//using ActionList = std::vector<Action>;

class ActionList : public std::vector<Action>
{
public:
    ActionList(void)
        : std::vector<Action>(), delayType(0) {}

    int getDelayType(void) const { return delayType; }
    void setDelayType(int type) { delayType = type; }
private:
    int delayType;
};

/**
 * @class Macro
 * @brief Provides functions to create and manage macros.
 */
class Macro
{
public:
    // Constants to specify delay type
    constexpr static const int NoDelay = 0;
    constexpr static const int FixedDelay = 1;
    constexpr static const int RecordedDelay = 2;

    /**
     * Gets the macro with the given name.
     * If no macro with the name existed, it is created.
     * @param name The macro's name
     * @return The macro's ActionList
     */
    static ActionList& get(const std::string& name);

    /**
     * Gets a list of all macro names.
     */
    static std::vector<std::string> getNames(void);

    /**
     * Checks if the macro exists.
     * @param name The macro's name
     * @return True if it exists
     */
    static bool exists(const std::string& name);

    static bool modified(const std::string& name, ActionList keys);

    /**
     * Renames the given macro.
     * @param oldName The macro's current name
     * @param newName The macro's new name
     */
    static void rename(const std::string& oldName, const std::string& newName);

    /**
     * Replaces a macro's contents with the given ActionList.
     * @param name The macro's name
     * @param keys The ActionList to replace the current one
     */
    static void replace(const std::string& name, ActionList& keys);

    /**
     * Gets the type of delay of the current macro.
     * @param config The config file that contains macros
     * @param name The macro's name
     * @return One of the delay types defined above
     */
    static int delayType(const std::string& name);

    /**
     * Sets the macro's delay type, saving it instantly.
     * @param config The config file containing macros
     * @param name The macro's name
     * @param type One of the delay types defined above
     */
    static void setDelayType(const std::string& name, int type);

    /**
     * Runs through the given macro if it exists.
     */
    static void fire(const std::string& name);

    /**
     * Loads macros from the given config.
     */
    static void load(QSettings& settings);

    /**
     * Saves macros to the given config.
     */
    static void save(QSettings& settings);

private:
    static std::map<std::string, ActionList> macros;
};

#endif // MACRO_H
