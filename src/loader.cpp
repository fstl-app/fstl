#include "loader.h"
#include "mesh.h"

Loader::Loader(QObject* parent, const QString& filename)
    : QThread(parent), filename(filename)
{
    // Nothing to do here
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

        // Skip the rest of the buffer
        file.read(75);

        // Assume we're on a little-endian system for simplicity
        uint32_t tri_count;
        file.read(reinterpret_cast<char*>(&tri_count), sizeof(tri_count));

        if (file.size() != 84 + tri_count*50)
        {
            emit error_bad_stl();
            return;
        }
    }

    emit got_mesh(Mesh::load_stl(filename));
    emit loaded_file(filename);
}
