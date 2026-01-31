#ifndef __WW_GLOBAL_P_H
#define __WW_GLOBAL_P_H

class QwwPrivate;
/**
 * \internal
 * @class QwwPrivatable
 */
class QwwPrivatable {
protected:
    QwwPrivatable(QwwPrivate *p);
    ~QwwPrivatable();
    QwwPrivate *d_ww_ptr;
};

#define WW_DECLARE_PRIVATE(Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(d_ww_ptr); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(d_ww_ptr); } \
    friend class Class##Private;

#define WW_DECLARE_PUBLIC(Class) \
    inline Class* q_func() { return static_cast<Class *>(q_ww_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ww_ptr); } \
    friend class Class;

/**
 * \internal
 * @class QwwPrivate
 * @class QwwPrivatable
 */
class QwwPrivate {
protected:
    QwwPrivate(QwwPrivatable *p) {
        q_ww_ptr = p;
    }
    QwwPrivatable *q_ww_ptr;
};

#include <QDrag>

class ColorDrag : public QDrag {
public:
    ColorDrag(QWidget *source, const QColor &color, const QString &name);
};

#include <QIcon>

namespace wwWidgets {
    QIcon icon(const QString &name, const QIcon &fallback = QIcon());
};

#endif
