#include "debuggerform.h"
#include "ui_debuggerForm.h"
#include <string>
#include <iomanip>
#include "indyemulator.h"

DebuggerForm::DebuggerForm(IndyEmulator *emulator, QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    this->emulator = emulator;
    this->registersModel = new CPURegistersDisplayModel(0);

    connect(this->emulator, &IndyEmulator::emitPC, [=](unsigned long long count) {
        registersModel->updateAll(this->cpu);

        this->ui.registersTable->resizeColumnsToContents();

        uint32_t instruction;
        cpu->get_mem_32b(cpu->get_PC(), &instruction);
        this->ui.currentInstruction->setText(QString::fromStdString(cpu->decode_to_text(instruction)));


        // Add the instruction to the disassembler view

        std::string pc = this->toHex(cpu->get_PC());
        std::string disasmTextLine = "[" + std::to_string(count) + "] " + pc + ": " + cpu->decode_to_text(instruction);
        this->ui.disasmView->appendPlainText(QString::fromStdString(disasmTextLine));

    });

    connect(this->ui.singleStepButton, &QPushButton::clicked, [=]() {
        this->emulator->singleStep();
    });

    this->ui.registersTable->setModel(this->registersModel);
    this->ui.registersTable->horizontalHeader()->hide();
    this->ui.registersTable->verticalHeader()->hide();
    this->ui.registersTable->resizeColumnsToContents();

    // Prep the treeview
    this->setupBacktraceView();
    this->cpu = this->emulator->getProcessor();
}

std::string DebuggerForm::toHex(uint64_t value) {
    std::ostringstream oss;
    oss << "0x" << std::setw(16) << std::setfill('0') << std::uppercase << std::hex << value;
    return oss.str();
}

void DebuggerForm::setupBacktraceView() {
    this->backtraceModel = new QStandardItemModel();

    QStandardItem *item = this->backtraceModel->invisibleRootItem();

    QList<QStandardItem *> rowItems;
    rowItems << new QStandardItem("Toast");
    rowItems << new QStandardItem("Eggs");
    rowItems << new QStandardItem("Milk");
    rowItems << new QStandardItem("Squick");
    item->appendRow(rowItems);

    this->ui.backtrace->setModel(this->backtraceModel);
    this->ui.backtrace->expandAll();
    this->ui.backtrace->show();

//    QList<QStandardItem *> preparedRow =prepareRow("first", "second", "third");
//    QStandardItem *item = standardModel->invisibleRootItem();
//    // adding a row to the invisible root item produces a root element
//    item->appendRow(preparedRow);

//    QList<QStandardItem *> secondRow =prepareRow("111", "222", "333");
//    // adding a row to an item starts a subtree
//    preparedRow.first()->appendRow(secondRow);

//    treeView->setModel(standardModel);
//    treeView->expandAll();

}
