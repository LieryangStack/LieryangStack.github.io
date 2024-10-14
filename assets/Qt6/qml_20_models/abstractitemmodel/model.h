#ifndef MODEL_H
#define MODEL_H

#include <QAbstractListModel>
#include <QStringList>


class Animal
{
public:
    Animal(const QString &type, const QString &size);

    QString type() const;
    QString size() const;

private:
    QString m_type;
    QString m_size;
};



class AnimalModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum AnimalRoles {
        TypeRole = Qt::UserRole + 1,
        SizeRole
    };

    AnimalModel(QObject *parent = nullptr);

    /* 自定义函数，添加新行 */
    void addAnimal(const Animal &animal);

    /* QAbstractItemModel 虚函数rowCount，获取该模型现在有多少个项item（或者行row） */
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    /* QAbstractItemModel 虚函数data，获取该 index位置的数据  */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<Animal> m_animals;
};

#endif // MODEL_H
