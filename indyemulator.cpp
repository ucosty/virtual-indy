#include "indyemulator.h"

#include "hpc3.h"
#include "mc.h"

#include <iostream>

IndyEmulator::IndyEmulator(QObject *parent) : QObject(parent)
{
    this->mb = new memory_bus();
    this->p = new processor(mb);

    this->mem1 = new memory(256 * 1024 * 1024, true);
    mb -> register_memory(0x08000000, mem1->get_size(), mem1);
    mb -> register_memory(0xffffffff88000000, mem1->get_size(), mem1); // KSEG0
    mb -> register_memory(0xffffffffa8000000, mem1->get_size(), mem1); // KSEG1
    mb -> register_memory(0, 512 * 1024, mem1); // needed for exception vectors

    this->mem2 = new memory(256 * 1024 * 1024, true);
    mb -> register_memory(0x20000000, mem2->get_size(), mem2);

    this->m_prom = new rom("test.bin");
    mb -> register_memory(0xffffffff1fc00000, m_prom->get_size(), m_prom);
    mb -> register_memory(0xffffffff9fc00000, m_prom->get_size(), m_prom); // KSEG0
    mb -> register_memory(0xffffffffbfc00000, m_prom->get_size(), m_prom); // KSEG1

    this->pmc = new mc(p);
    mb -> register_memory(0xffffffff1fa00000, pmc->get_size(), pmc);
    mb -> register_memory(0xffffffff9fa00000, pmc->get_size(), pmc); // KSEG0
    mb -> register_memory(0xffffffffbfa00000, pmc->get_size(), pmc); // KSEG1

    this->hpc = new hpc3("sram.dat");
    mb -> register_memory(0xffffffff1fb00000, hpc->get_size(), hpc);
    mb -> register_memory(0xffffffff9fb00000, hpc->get_size(), hpc); // KSEG0
    mb -> register_memory(0xffffffffbfb00000, hpc->get_size(), hpc); // KSEG1

    this->isRunning = false;
}

IndyEmulator::~IndyEmulator() {
    delete this->p;
    delete this->mb;
    delete this->pmc;
    delete this->mem1;
    delete this->mem2;
    delete this->m_prom;
    delete this->hpc;
}

void IndyEmulator::execute() {
    p->reset();
    while(this->isRunning) {
        p->tick();
        count++;
        if(count % 1000000 == 0) {
            emit emitPC(count);
        }

    }
}

void IndyEmulator::singleStep() {
    p->tick();
    emit emitPC(count);
    count++;
}

void IndyEmulator::start() {
    this->isRunning = true;
}

void IndyEmulator::stop() {
    this->isRunning = false;
}

processor *IndyEmulator::getProcessor() {
    return this->p;
}
