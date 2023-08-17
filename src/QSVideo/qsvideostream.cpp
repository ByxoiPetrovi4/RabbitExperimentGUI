#include "qsvideostream.h"

QSVideoStream::QSVideoStream(QString key, QObject* parent) : QSVideo{key, parent}
{}

int QSVideoStream::addUser()
{
    mlock();
    info = *_getInfo();
    info.users_iters[info.users_all] = 0;
    info.users_all++;
    info.users_active++;
    *_getInfo() = info;
    munlock();
    return info.users_all-1;
}

void QSVideoStream::pauseUser(int user_id)
{
    mlock();
    info = *_getInfo();
    if(info.users_all <= user_id)
    {
        munlock();
        throw 1;    //TODO: add proper exception
    }
    info.users_iters[user_id] = -1;
    info.users_active--;
    *_getInfo() = info;
    munlock();
}

void QSVideoStream::resumeUser(int user_id)
{
    mlock();
    info = *_getInfo();
    if(info.users_all <= user_id)
    {
        munlock();
        throw 1;
    }
    if(info.users_iters[user_id]==-1)
    {
        munlock();
        return;
    }

}
