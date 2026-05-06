#pragma once

#include <QDebug>

#ifdef QT_DEBUG
    #define DEBUG_LOG(category, message) qDebug() << "[" << category << "]" << message
#else
    #define DEBUG_LOG(category, message) (void)0
#endif

#define DEBUG_LOG_INFO(category, message) qInfo() << "[" << category << "]" << message
#define DEBUG_LOG_WARN(category, message) qWarning() << "[" << category << "]" << message
#define DEBUG_LOG_ERROR(category, message) qCritical() << "[" << category << "]" << message
