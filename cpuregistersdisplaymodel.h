#ifndef CPUREGISTERSDISPLAYMODEL_H
#define CPUREGISTERSDISPLAYMODEL_H

#include <map>
#include <vector>
#include <string>
#include <QAbstractTableModel>

#include "processor.h"

class CPURegistersDisplayModel: public QAbstractTableModel
{
    Q_OBJECT

    std::map<std::string, std::string> registersData;
    std::vector<std::string> registersList;
public:
    CPURegistersDisplayModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void addRegister(std::string reg);

    void updateRegister(std::string reg, uint64_t value);
    void updateRegister(std::string reg, std::string value);

    void updateAll(processor *cpu);
};

#endif // CPUREGISTERSDISPLAYMODEL_H
