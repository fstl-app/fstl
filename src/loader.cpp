#include "loader.h"
#include "mesh.h"

Loader::Loader(QObject* parent, const QString& filename)
    : QThread(parent), filename(filename)
{
}

void Loader::run()
{
    emit got_mesh(Mesh::load_stl(filename));
}
