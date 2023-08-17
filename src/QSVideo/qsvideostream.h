#ifndef QSVIDEOSOURCE_H
#define QSVIDEOSOURCE_H

#include <chrono>
#include <iostream>
#include <QThread>
#include "qsvideo.h"

class QSVideoStream : public QSVideo
{
    Q_OBJECT
protected:
    std::chrono::time_point<std::chrono::high_resolution_clock>
                            nextRead;

    QSVideoStream(QString key, QObject *parent=nullptr);
    ///Add user for this videostream
    int                     addUser();
    ///Pause user with specified id
    void                    pauseUser(int user_id);
    ///Resume work for selected user
    void                    resumeUser(int user_id);
public:
    ///Update function for stream update
    virtual UStatus         update() = 0;
private:
signals:
    void                    endOfFile();
};

#endif // QSVIDEOSOURCE_H
