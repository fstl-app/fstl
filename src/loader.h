#ifndef LOADER_H
#define LOADER_H

#include <QThread>

#include "mesh.h"

class Loader : public QThread
{
    Q_OBJECT
public:
    explicit Loader(QObject* parent, const QString& filename);
    void run();

signals:
    void got_mesh(Mesh* m);

private:
    const QString filename;

};

#endif // LOADER_H
