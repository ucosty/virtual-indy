#ifndef DEBUGGERFORM_H
#define DEBUGGERFORM_H

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QStandardItem>

#include <string>

#include "ui_debuggerForm.h"
#include "indyemulator.h"
#include "cpuregistersdisplaymodel.h"
#include "processor.h"


class DebuggerForm : public QWidget
{
    Q_OBJECT

    Ui::DebuggerForm ui;
    IndyEmulator *emulator;

    CPURegistersDisplayModel *registersModel;

    processor *cpu;
    std::string disasmText;

    std::string toHex(uint64_t value);


    void setupBacktraceView();

    QStandardItemModel *backtraceModel;
public:
    explicit DebuggerForm(IndyEmulator *emulator, QWidget *parent = 0);

signals:

public slots:
};

#endif // DEBUGGERFORM_H
