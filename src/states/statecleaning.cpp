#include "statecleaning.h"
#include "global.h"

#include <QDir>

NBStateCleaning::NBStateCleaning(QObject *parent) : NBState(parent)
{

}

NBStateCleaning::~NBStateCleaning()
{
    // deletes nothing!!
}


void NBStateCleaning::run()
{
#ifdef QT_NO_DEBUG
    QDir d(GlobalConfig::BuildPath);

    if (!d.exists()) {
        emit finished();
        return;
    }

    QDir d_up = d;

#define DETECT_FAIL_RUN(a) \
    do {\
        if (!a) {\
            emit error();\
            return;\
        }\
    } while (false)

    DETECT_FAIL_RUN(d_up.cdUp());
    DETECT_FAIL_RUN(d.removeRecursively());
    DETECT_FAIL_RUN(d_up.mkpath(GlobalConfig::BuildPath));

#undef DETECT_FAIL_RUN

#endif
    emit finished();
}

void NBStateCleaning::shutUp()
{
    // does nothing!!!
}
