/**
 * @file runguard.h
 * @brief Utility for insuring only one instance is running.
 * Based on: https://stackoverflow.com/questions/5006547/qt-best-practice-for-a-single-instance-app-protection
 */
#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QCryptographicHash>
#include <QSharedMemory>
#include <QString>
#include <QSystemSemaphore>

class RunGuard {
public:
    RunGuard(const QString& key) :
        memoryLock(generateKeyHash(key, "_memLockKey"), 1)
    {
        auto memoryKey = generateKeyHash(key, "_sharedMemory");
        sharedMemory.setKey(memoryKey);

        memoryLock.acquire();
        QSharedMemory(memoryKey).attach();
        memoryLock.release();
    }

    ~RunGuard(void) {
        memoryLock.acquire();
        if (sharedMemory.isAttached())
            sharedMemory.detach();
        memoryLock.release();
    }

    bool alreadyRunning(void) {
        if (sharedMemory.isAttached())
            return false;
        memoryLock.acquire();
        bool running = sharedMemory.attach();
        if (!running)
            running = !sharedMemory.create(1);
        memoryLock.release();
        return running;
    }

private:
    QSharedMemory sharedMemory;
    QSystemSemaphore memoryLock;

    static QString generateKeyHash(const QString& key, const QString& salt) {
        return QCryptographicHash::hash(key.toUtf8() + salt.toUtf8(),
            QCryptographicHash::Sha1).toHex();
    }
};

#endif // RUNGUARD_H
