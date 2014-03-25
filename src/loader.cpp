#include "loader.h"
#include "mesh.h"

Loader::Loader(QObject* parent, const QString& filename)
    : QThread(parent), filename(filename)
{
}

void Loader::run()
{
    {   // Verify that this isn't an ascii stl file
        QFile file(filename);
        file.open(QIODevice::ReadOnly);
        if (file.read(5) == "solid")
        {
            emit error_ascii_stl();
            return;
        }
    }

    emit got_mesh(Mesh::load_stl(filename));
    emit loaded_file(filename);
}
