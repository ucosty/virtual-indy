#include "cpuregistersdisplaymodel.h"

#include <iostream>
#include <sstream>
#include <iomanip>

CPURegistersDisplayModel::CPURegistersDisplayModel(QObject *parent) :QAbstractTableModel(parent) {
    // 0 PC (because register 0 is otherwise always zero)
    this->addRegister("PC");

    // 1		$at		(assembler temporary) reserved by the assembler
    this->addRegister("AT");

    // 2-3		$v0 - $v1	(values) from expression evaluation and function results
    this->addRegister("V0");
    this->addRegister("V1");

    // 4-7		$a0 - $a3	(arguments) First four parameters for subroutine.  Not preserved across procedure calls
    this->addRegister("A0");
    this->addRegister("A1");
    this->addRegister("A2");
    this->addRegister("A3");

    // 8-15		$t0 - $t7	(temporaries) Caller saved if needed. Subroutines can use w/out saving.  Not preserved across procedure calls
    this->addRegister("T0");
    this->addRegister("T1");
    this->addRegister("T2");
    this->addRegister("T3");
    this->addRegister("T4");
    this->addRegister("T5");
    this->addRegister("T6");
    this->addRegister("T7");

    // 16-23	$s0 - $s7	(saved values) - Callee saved.  A subroutine using one of these must save original and restore it before exiting.  Preserved across procedure calls
    this->addRegister("S0");
    this->addRegister("S1");
    this->addRegister("S2");
    this->addRegister("S3");
    this->addRegister("S4");
    this->addRegister("S5");
    this->addRegister("S6");
    this->addRegister("S7");

    // 24-25	$t8 - $t9	(temporaries) Caller saved if needed. Subroutines can use w/out saving.  These are in addition to $t0 - $t7 above.  Not preserved across procedure calls.
    this->addRegister("T8");
    this->addRegister("T9");

    // 26-27	$k0 - $k1	reserved for use by the interrupt/trap handler
    this->addRegister("K0");
    this->addRegister("K1");

    // 28		$gp		global pointer.  Points to the middle of the 64K block of memory in the static data segment.
    this->addRegister("GP");

    // 29		$sp		stack pointer Points to last location on the stack.
    this->addRegister("SP");

    // 30		$s8/$fp		saved value / frame pointer Preserved across procedure calls
    this->addRegister("FP");

    // 31		$ra		return address
    this->addRegister("RA");
}

void CPURegistersDisplayModel::updateAll(processor *cpu) {
    updateRegister("PC", cpu->get_PC());

    for(int i = 1; i<32; i++) {
        this->updateRegister(registersList[i], cpu->get_register_64b_unsigned(i));


        int column = (i / 8) * 2;
        int row = i % 8;
        QModelIndex topLeft = createIndex(row, column);
        emit dataChanged(topLeft, topLeft);
    }

    for(int x = 1; x < 8; x += 2) {
        for(int y = 0; y<8; y++) {
            QModelIndex topLeft = createIndex(y, x);
            emit dataChanged(topLeft, topLeft);
        }
    }
}

int CPURegistersDisplayModel::rowCount(const QModelIndex & /*parent*/) const
{
   return 8;
}

int CPURegistersDisplayModel::columnCount(const QModelIndex & /*parent*/) const
{
    return registersList.size() / 4;
}

QVariant CPURegistersDisplayModel::data(const QModelIndex &index, int role) const
{
//    int superColumn = index.row() / 8;
    // SUPERCOLUMN 1      / SUPERCOLUMN 2      / SUPERCOLUMN 3      / SUPERCOLUMN 4
    // ---------------------------------------------------------------------------------
    // [Register / Value] / [Register / Value] / [Register / Value] / [Register / Value]
    // [Register / Value] / [Register / Value] / [Register / Value] / [Register / Value]
    // ....

    int row = index.row();
    int col = index.column();

    if (role == Qt::DisplayRole)
    {
        // Are we looking at a register name?
        if(index.column() % 2 == 0) {

            // Col = 0, 2, 4, 6
            int valueIndex = (col) * 4 + row;
            return QString::fromStdString(this->registersList[valueIndex]);
        } else {
            int valueIndex = (col - 1) * 4 + row;
            return QString::fromStdString(this->registersData.at(this->registersList[valueIndex]));


        }
    }
    return QVariant();
}


void CPURegistersDisplayModel::addRegister(std::string reg) {
    registersData[reg] = "0x0000000000000000";
    registersList.push_back(reg);
}


void CPURegistersDisplayModel::updateRegister(std::string reg, uint64_t value) {
    std::ostringstream oss;
    oss << "0x" << std::setw(16) << std::setfill('0') << std::uppercase << std::hex << value;
    this->updateRegister(reg, oss.str());
}

void CPURegistersDisplayModel::updateRegister(std::string reg, std::string value) {
    registersData[reg] = value;
}
