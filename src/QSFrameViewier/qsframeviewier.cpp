#include "qsframeviewier.h"
#include "qpainter.h"

QSFrameViewier::QSFrameViewier(QWidget* parent) : QLabel{parent},
    video(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(defMinSize, defMinSize);
    setAlignment(Qt::AlignCenter);
    redrawTimer = new QTimer(this);
    redrawTimer->setSingleShot(false);
    redrawTimer->setInterval(15);
    connect(redrawTimer, &QTimer::timeout,
            this, &QSFrameViewier::redraw);
    setScaledContents(true);
}

QSFrameViewier::~QSFrameViewier()
{
    delete redrawTimer;
    if(video != nullptr)
        delete video;
}

void QSFrameViewier::setFrame(const QString &key)
{
    if(video != nullptr)
    {
        //video->pauseUser();
        delete video;
    }
    try {
        video = new QSVideoUser(key, this);
    }  catch (...) {
        //TODO: add proper handler
        qDebug() << "Couldn't open frame!";
        return;
    }
    endFrame = video->getNumberOfFrames();
    endTime = video->getVideoLength();
    if(!endFrame)
    {
        //TODO: add lock for rewinds
    }
    videoReady();
    //handlerIndex = video->addUser();
    //if module are inactive
    /*video->setActive(true);
    prevDrawTime = qs::micros();
    if(video->getFrame(currentFrame) == QSFrame::QSV_READY)
        currentTime = currentFrame.time;
    else
        currentTime = 0;
    redraw();*/
}

void QSFrameViewier::redraw()
{
    if(video == nullptr)
        return;
    if(video->getState() == QSVideo::QSV_STOP)
    {
        delete video;
        video = nullptr;
        return;
    }
    auto mc = qs::micros();
    if(isHidden())
    {
        prevDrawTime = mc;
        return;
    }
    video->getFrameT(currentFrame,
                    currentTime + (mc - prevDrawTime)*speed);
    //video->getFrame(currentFrame);
    if(!currentFrame.empty())
    {
        currentTime += (mc - prevDrawTime)*speed;
        actualFrameRate = 1000000.f/(mc - prevDrawTime);
        prevDrawTime = mc;
        //TODO: rename this bullshit
        emit newFrame(currentFrame.time);
        update();
    }
    /*else
    {
        qDebug() << "empty";
    }*/
}

void QSFrameViewier::update_frame()
{
    QSVideo::Frame f;
    video->getFrame(currentFrame);
    if(currentFrame.empty())
        return;
    currentTime = currentFrame.time;
    emit newFrame(currentFrame.time);
    update();
}

void QSFrameViewier::videoReady()
{
    prevDrawTime = qs::micros();
    video->getFrame(currentFrame);
    if(!currentFrame.empty())
        currentTime = currentFrame.time;
    else
        currentTime = 0;
    redraw();
}

/*void QSFrameViewier::resizeEvent(QResizeEvent* event)
{
    if(currentFrame.mat.empty())
        return;
    QStyle* _style = style();
    QPainter painter(this);
    QRect cr = contentsRect();
    drawFrame(&painter);
    cr.adjust(margin(), margin(), -margin(), -margin());
    int align = QStyle::visualAlignment(layoutDirection(), alignment());
    cv::Mat out;
    cv::Size2i sz;
    float widgetAspectRatio = ((float)event->size().width())/event->size().height();
    float frameAspectRatio = ((float)currentFrame.mat.cols)/currentFrame.mat.rows;
    if(widgetAspectRatio > frameAspectRatio)
    {
        sz.height = event->size().height();
        sz.width = event->size().height() * frameAspectRatio;
    }
    else
    {
        sz.width = event->size().width();
        sz.height = event->size().width() / frameAspectRatio;
    }
    cv::resize(currentFrame.mat, out, {(int)(sz.width*devicePixelRatioF()),
                (int)(sz.height*devicePixelRatioF())}, 0, 0, cv::INTER_AREA);
    QPixmap pix = QPixmap::fromImage(cvMatToQImage(out),
            Qt::ImageConversionFlag::NoFormatConversion);
    pix.setDevicePixelRatio(devicePixelRatioF());
    QStyleOption opt;
    opt.initFrom(this);
    if(!isEnabled())
        pix= _style->generatedIconPixmap(QIcon::Disabled, pix, &opt);
    _style->drawItemPixmap(&painter, cr, align, pix);
}*/

void QSFrameViewier::paintEvent(QPaintEvent* event)
{
    if(currentFrame.mat.empty())
        return;
    QStyle* _style = style();
    QPainter painter(this);
    QRect cr = contentsRect();
    drawFrame(&painter);
    cr.adjust(margin(), margin(), -margin(), -margin());
    int align = QStyle::visualAlignment(layoutDirection(), alignment());
    cv::Mat out;
    cv::Size2i sz;
    if(zp.state == ZoomParams::Zoom)
    {
        zp.zoomMatrix = cv::Mat(currentFrame.mat, zp.area);
        widgetAspectRatio = ((float)size().width())/size().height();
        frameAspectRatio = ((float)zp.zoomMatrix.cols)/zp.zoomMatrix.rows;
        if(widgetAspectRatio > frameAspectRatio)
        {
            sz.height = size().height();
            sz.width = size().height() * frameAspectRatio;
        }
        else
        {
            sz.width = size().width();
            sz.height = size().width() / frameAspectRatio;
        }
        cv::resize(zp.zoomMatrix, out, {(int)(sz.width*devicePixelRatioF()),
                    (int)(sz.height*devicePixelRatioF())}, 0, 0, cv::INTER_AREA);
        QPixmap pix = QPixmap::fromImage(cvMatToQImage(out),
                Qt::ImageConversionFlag::NoFormatConversion);
        pix.setDevicePixelRatio(devicePixelRatioF());
        QStyleOption opt;
        opt.initFrom(this);
        if(!isEnabled())
            pix= _style->generatedIconPixmap(QIcon::Disabled, pix, &opt);
        _style->drawItemPixmap(&painter, cr, align, pix);
        return;
    }
    widgetAspectRatio = ((float)size().width())/size().height();
    frameAspectRatio = ((float)currentFrame.mat.cols)/currentFrame.mat.rows;
    if(widgetAspectRatio > frameAspectRatio)
    {
        sz.height = size().height();
        sz.width = size().height() * frameAspectRatio;
    }
    else
    {
        sz.width = size().width();
        sz.height = size().width() / frameAspectRatio;
    }
    cv::resize(currentFrame.mat, out, {(int)(sz.width*devicePixelRatioF()),
                (int)(sz.height*devicePixelRatioF())}, 0, 0, cv::INTER_AREA);
    QPixmap pix = QPixmap::fromImage(cvMatToQImage(out),
            Qt::ImageConversionFlag::NoFormatConversion);
    pix.setDevicePixelRatio(devicePixelRatioF());
    QStyleOption opt;
    opt.initFrom(this);
    if(!isEnabled())
        pix= _style->generatedIconPixmap(QIcon::Disabled, pix, &opt);
    _style->drawItemPixmap(&painter, cr, align, pix);
}

void QSFrameViewier::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(zp.state != ZoomParams::NoZoom)
            return;
        if(currentFrame.empty())
            return;
        cv::Point2i p = {event->pos().x(), event->pos().y()};
        cv::Size sz = {size().width(), size().height()};
        float conv_coef;
        if(widgetAspectRatio > frameAspectRatio)
        {
            sz.height = size().height();
            sz.width = size().height() * frameAspectRatio;
            conv_coef = (float(currentFrame.mat.cols))/size().width();
            p.x = p.x - (size().width() - sz.width)/2;
        }
        else
        {
            sz.width = size().width();
            sz.height = size().width() / frameAspectRatio;
            conv_coef = (float(currentFrame.mat.cols))/size().width();
            p.y = p.y - (size().height() - sz.height)/2;
        }
        p = p * conv_coef;
        if(p.x < 0 || p.x >= currentFrame.mat.cols ||
                p.y < 0 || p.y >= currentFrame.mat.rows)
            return;
        zp.state = ZoomParams::ZoomChoosing;
        zp.firstPoint = p;
    } else if(event->button() == Qt::RightButton)
    {
        zp.state = ZoomParams::NoZoom;
        update();
    }
}

void QSFrameViewier::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(zp.state != ZoomParams::ZoomChoosing)
            return;
        if(currentFrame.empty())
            return;
        cv::Point2i p = {event->pos().x(), event->pos().y()};
        cv::Size sz = {size().width(), size().height()};
        float conv_coef;
        if(widgetAspectRatio > frameAspectRatio)
        {
            sz.height = size().height();
            sz.width = size().height() * frameAspectRatio;
            conv_coef = (float(currentFrame.mat.cols))/size().width();
            p.x = p.x - (size().width() - sz.width)/2;
        }
        else
        {
            sz.width = size().width();
            sz.height = size().width() / frameAspectRatio;
            conv_coef = (float(currentFrame.mat.cols))/size().width();
            p.y = p.y - (size().height() - sz.height)/2;
        }
        p = p * conv_coef;
        if(p.x < 0 || p.x >= currentFrame.mat.cols ||
                p.y < 0 || p.y >= currentFrame.mat.rows)
        {
            zp.state = ZoomParams::NoZoom;
            return;
        }
        zp.secondPoint = p;
        if(zp.firstPoint.x > zp.secondPoint.x)
        {
            zp.area.x = zp.secondPoint.x;
            zp.area.width = zp.firstPoint.x - zp.secondPoint.x;
        }
        else
        {
            zp.area.x = zp.firstPoint.x;
            zp.area.width = zp.secondPoint.x - zp.firstPoint.x;
        }
        if(zp.firstPoint.y > zp.secondPoint.y)
        {
            zp.area.y = zp.secondPoint.y;
            zp.area.height = zp.firstPoint.y - zp.secondPoint.y;
        }
        else
        {
            zp.area.y = zp.firstPoint.y;
            zp.area.height = zp.secondPoint.y - zp.firstPoint.y;
        }
        qDebug() << zp.area.x << " " << zp.area.y << " " << zp.area.width << " " << zp.area.height << " ";
        if(zp.area.width>100&&zp.area.height>100)
        {
            zp.state = ZoomParams::Zoom;
            update();
        }
        else
            zp.state = ZoomParams::NoZoom;
    }
}

void QSFrameViewier::pausePlay()
{
    if(video==nullptr)
        return;
    auto mc = qs::micros();
    if(await)
    {
        prevDrawTime = mc;
        await = !await;
        redraw();
        redrawTimer->start();
    }
    else
    {
        prevPauseTime = mc;
        redrawTimer->stop();
        await = !await;
    }
}

void QSFrameViewier::pause()
{
    if(video==nullptr)
        return;
    if(await)
        return;
    prevPauseTime = qs::micros();
    redrawTimer->stop();
    await = !await;
}

void QSFrameViewier::play()
{
    if(video==nullptr)
        return;
    if(!await)
        return;
    prevDrawTime = qs::micros();
    await = !await;
    redraw();
    redrawTimer->start();
}

void QSFrameViewier::moveT(int64_t time)
{
    if(video==nullptr)
        return;
    currentTime += time;
    if(currentTime > endTime)
        currentTime = endTime;
    if(currentTime < 0)
        currentTime = 0;
    video->rewindT(currentTime);
    if(await)
        QTimer::singleShot(100, this, SLOT(update_frame()));
}

void QSFrameViewier::moveF(int64_t frame)
{
    if(video==nullptr)
        return;
    auto cf = currentFrame.frame + frame - 1;
    video->rewindF(cf);
    if(await)
        QTimer::singleShot(100, this, SLOT(update_frame()));
}

void QSFrameViewier::setT(qs::TimeT time)
{
    if(video==nullptr)
        return;
    currentTime = time;
    video->rewindT(currentTime);
    if(await)
        QTimer::singleShot(100, this, SLOT(update_frame()));
}

void QSFrameViewier::setF(qs::FrameT frame)
{
    if(video==nullptr)
        return;
    auto cf = frame;
    if(cf > endFrame)
        cf = endFrame;
    if(cf < 0)
        cf = 0;
    video->rewindF(cf);
    if(await)
        QTimer::singleShot(100, this, SLOT(update_frame()));
}

void QSFrameViewier::setSpeed(float speed)
{
    if(video==nullptr)
        return;
}
