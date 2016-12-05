#include <QApplication>
#include <QThread>
#include <QObject>
#include "debuggerform.h"
#include "indyemulator.h"

class EmulatorStarter: public QObject {
    IndyEmulator *emulator;
    QThread *counterThread;
public:
    EmulatorStarter(IndyEmulator *emulator, QThread *counterThread) {
        this->emulator = emulator;
        this->counterThread = counterThread;

        connect(counterThread, SIGNAL(started()), emulator, SLOT(execute()));
        connect(emulator, SIGNAL(finished()), counterThread, SLOT(quit()));
        connect(emulator, SIGNAL(finished()), emulator, SLOT(deleteLater()));
    }

    void start() {
        counterThread->start();
    }
};

int main(int argc, char **argv) {
    QApplication app (argc, argv);

    // Create the counter thread
    IndyEmulator *emulator = new IndyEmulator();
    QThread *counterThread = new QThread();

    emulator->moveToThread(counterThread);
    EmulatorStarter emulatorStarter(emulator, counterThread);
    DebuggerForm debuggerForm(emulator);
    debuggerForm.show();

    emulatorStarter.start();
    return app.exec();
}


