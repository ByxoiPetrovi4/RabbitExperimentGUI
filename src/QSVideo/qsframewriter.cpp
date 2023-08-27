#include "qsframewriter.h"

QSFrameWriter::QSFrameWriter(std::string filename, QSVideo::VideoParams params)
{
    auto pos = filename.find_last_of('.');
    const auto t_filename = filename.substr(0, pos) + "_ts.csv";
    t_out.open(t_filename, std::fstream::ios_base::out);
    if(!t_out.is_open())
    {
        qDebug() << "Couldn't open timestamp file!";
        throw 1;    //TODO: add proper exception
    }
    v_out.open(filename, cv::CAP_GSTREAMER, v_out.fourcc('M', 'J', 'P', 'G'), params.fps, {params.fwidth, params.fheight});
    v_out.set(cv::VIDEOWRITER_PROP_QUALITY, 80);
    if(!t_out.is_open())
    {
        qDebug() << "Couldn't open video file!";
        throw 1;    //TODO: add proper exceptio
    }
}

void    QSFrameWriter::write(const QSVideo::Frame &fr)
{
    t_out << frameCount << ' ' << fr.time << '\n';
    v_out.write(fr.mat);
    frameCount++;
}
