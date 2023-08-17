#include "qsframewriter.h"

QSFrameWriter::QSFrameWriter(std::string filename, QSVideo::VideoParams params)
{
    auto pos = filename.find_last_of('.');
    const auto t_filename = filename.substr(0, pos - 1) + "_ts.csv";
    t_out.open(t_filename, std::fstream::ios_base::out);
    if(!t_out.is_open())
    {
        qDebug() << "Couldn't open timestamp file!";
        throw 1;    //TODO: add proper exception
    }
    v_out.open(filename, ((int*)&params.fourcc)[0], params.fps, {params.fwidth, params.fheight});
    if(!t_out.is_open())
    {
        qDebug() << "Couldn't open video file!";
        throw 1;    //TODO: add proper exceptio
    }
}

void    QSFrameWriter::write(const QSVideo::Frame &fr)
{
    if(fr.empty())
        return;
    t_out << fr.frame << ' ' << fr.time << '\n';
    v_out.write(fr.mat);
}
