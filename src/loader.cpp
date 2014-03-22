#include "loader.h"
#include "mesh.h"

Loader::Loader(QObject* parent, const QString& filename)
    : QThread(parent), filename(filename)
{
}

void Loader::run()
{
    QTime timer;
    timer.start();
    emit got_mesh(Mesh::load_stl(filename));
    qDebug() << "Time taken:" << timer.elapsed();

    emit loaded_file(filename);
}
