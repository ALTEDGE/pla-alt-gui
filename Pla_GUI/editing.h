#ifndef EDITING_H
#define EDITING_H

/**
 * @class Editing
 * @brief Allows data objects to be modified, then compared or reverted to its
 * original state.
 *
 * Upon construction, the given object is saved to a copy. Later, code can use
 * isModified() to see if changes were made, and then either keep the changes
 * or revert to the original state.
 *
 * The object is modified first, saved later, rather than modified-on-save.
 * This allows controller settings to be viewed in real time.
 */
template<typename T>
class Editing
{
public:
    /**
     * Creates an Editing object with the given data object.
     * @param obj The data object to keep track of
     */
    Editing(T& obj)
        : object(obj), original(object) {}

    /**
     * Provides access to the object, so that it can be modified.
     */
    T* operator->(void) {
        return &object;
    }

    /**
     * Checks if the data object has changed since its last saved state.
     * @return True if modified
     */
    bool isModified(void) const {
        return object != original;
    }

    /**
     * "Saves" changes to the data.
     *
     * Changes to the object were already made; this function just updates
     * the copy.
     */
    void save(void) {
        // Get original copy up-to-date
        original = object;
    }

    /**
     * Reverts the data object to its last saved state.
     */
    void revert(void) {
        object = original;
    }

private:
    // A reference to the data object
    T& object;

    // A copy of the data object
    T original;
};

#endif // EDITING_H
