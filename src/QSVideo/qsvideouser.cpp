#include "qsvideouser.h"

QSVideoUser::QSVideoUser(QString key, QObject *parent) : QSVideo{key, parent}
{
    mem = new QSharedMemory();
    mem->setNativeKey(key);
    if(!mem->isAttached())
        if(!mem->attach(QSharedMemory::ReadWrite))
            throw 1;//TODO: add proper exception
    info = *_getInfo();
    mlock();
    user_id = info.users_all;
    info.users_iters[user_id] = 0;
    info.users_active++;
    info.users_all++;
    *_getInfo() = info;
    type = info.type;
    munlock();
}

void QSVideoUser::stop()
{
    if(!user_active)
        return;
    mlock();
    info = *_getInfo();
    info.users_active--;
    info.users_iters[user_id] = -1;
    munlock();
}

void QSVideoUser::resume()
{
    if(user_active)
        return;
    mlock();
    info = *_getInfo();
    info.users_active++;
    info.users_iters[user_id] = 0;
    munlock();
}

void QSVideoUser::getFrame(QSVideo::Frame &out)
{
    mlock();
    info = *_getInfo();
    if(info.buffer_lenw == 0 || info.users_iters[user_id] >= info.buffer_lenw)
    {
        munlock();
        out._empty = true;
        return;
    }
    out = *_getFrameC(info.users_iters[user_id]);
    info.users_iters[user_id]++;
    *_getInfo() = info;
    munlock();
}

void        QSVideoUser::getFrameL(QSVideo::Frame &out)
{
    mlock();
    info = *_getInfo();
    if(info.buffer_lenw == 0)
    {
        munlock();
        out._empty = true;
        return;
    }
    out = *_getFrameC(info.buffer_lenw-1);
    info.users_iters[user_id] = info.buffer_lenw;
    *_getInfo() = info;
    munlock();
}

void QSVideoUser::getFrameT(QSVideo::Frame &out, qs::TimeT time)
{
    if(!mlock(2))
    {
        out._empty = true;
        return;
    }
    info = *_getInfo();
    qs::TimeT dif = -99999999999999;
    int nindex = 0;
    for(int i = 0; i < info.buffer_lenw; i++)
    {
        qs::TimeT ndif = time-_getFrameC(i)->time;
        if(ndif > dif && ndif < 0)
        {
            dif = ndif;
            nindex = i;
        }
    }
    if(dif == -99999999999999)
        out._empty = true;
    else
    {
        out = *_getFrameC(nindex);
        if(user_active)
            info.users_iters[user_id]=nindex+1;
        *_getInfo() = info;
    }
    munlock();
}
