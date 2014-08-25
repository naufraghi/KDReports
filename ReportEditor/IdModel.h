#ifndef IDMODEL_H
#define IDMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include "editor_export.h"

class ReportData;

class EDITOR_EXPORT IdModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit IdModel(QObject* parent = 0);

    /**
      * \reimp
      */
    virtual int rowCount(const QModelIndex &parent = QModelIndex() ) const;

    /**
      * \reimp
      */
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /**
      * \reimp
      */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /**
      * \reimp
      */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /**
      * \reimp
      */
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;

    /**
      * \reimp
      */
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    /**
      * Set the model data
      */
    void setReportData(ReportData* reportData);

public slots:
    void slotAddId(const QString &id);
    void slotRemoveId(const QString &id);
    void slotModifyId(const QString &newId, const QString &oldId);

private:
    QString valueFromId(const QString& id) const;
    void insertIdValue(const QString &id, const QString &value);
    void removeFromId(const QString &id);
    void changeId(const QString &newId, const QString &oldId);

    ReportData* m_reportData;
    QList<QString> m_idList;
    QList<QString> m_valueList;
};

#endif // IDMODEL_H
