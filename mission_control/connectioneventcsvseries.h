#ifndef CONNECTIONEVENTCSVSERIES_H
#define CONNECTIONEVENTCSVSERIES_H

#include <QObject>
#include <QTimerEvent>

#include "core/csvrecorder.h"
#include "core/channel.h"

namespace Soro {

class ConnectionEventCsvSeries: public QObject, public CsvDataSeries
{
    Q_OBJECT
public:
    ConnectionEventCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void driveChannelStateChanged(Channel::State state);
    void mainChannelStateChanged(Channel::State state);
};

} // namespace Soro

#endif // CONNECTIONEVENTCSVSERIES_H
