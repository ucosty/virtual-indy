#ifndef INDYEMULATOR_H
#define INDYEMULATOR_H

#include <QObject>

#include "processor.h"
#include "memory_bus.h"
#include "memory.h"
#include "rom.h"

class IndyEmulator : public QObject
{
    Q_OBJECT

    memory_bus *mb;
    processor *p;

    memory *mem1;
    memory *mem2;
    rom *m_prom;
    memory *pmc;
    memory *hpc;

    bool isRunning;
    unsigned long long count = 0;

    uint64_t previousRA = 0;
public:
    explicit IndyEmulator(QObject *parent = 0);
    ~IndyEmulator();

    processor *getProcessor();

signals:
    void emitPC(unsigned long long count);

public slots:
    void execute();
    void singleStep();
    void start();
    void stop();
};

#endif // INDYEMULATOR_H
