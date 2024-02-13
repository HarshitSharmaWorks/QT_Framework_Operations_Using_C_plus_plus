#include <QApplication>
#include "QT_Tasks.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QT_Tasks qtTasks;
    qtTasks.show();
    return app.exec();
}
