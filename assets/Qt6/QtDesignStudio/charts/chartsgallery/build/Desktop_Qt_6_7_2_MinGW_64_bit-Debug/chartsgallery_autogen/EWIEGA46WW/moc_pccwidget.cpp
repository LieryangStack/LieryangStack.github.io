/****************************************************************************
** Meta object code from reading C++ file 'pccwidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../pccwidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pccwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSPccWidgetENDCLASS_t {};
constexpr auto qt_meta_stringdata_CLASSPccWidgetENDCLASS = QtMocHelpers::stringData(
    "PccWidget",
    "updateChartSettings",
    "",
    "updateSerieSettings",
    "updateSliceSettings",
    "handleSliceClicked",
    "QPieSlice*",
    "slice",
    "showFontDialog",
    "appendSlice",
    "insertSlice",
    "removeSlice"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSPccWidgetENDCLASS[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x0a,    1 /* Public */,
       3,    0,   63,    2, 0x0a,    2 /* Public */,
       4,    0,   64,    2, 0x0a,    3 /* Public */,
       5,    1,   65,    2, 0x0a,    4 /* Public */,
       8,    0,   68,    2, 0x0a,    6 /* Public */,
       9,    0,   69,    2, 0x0a,    7 /* Public */,
      10,    0,   70,    2, 0x0a,    8 /* Public */,
      11,    0,   71,    2, 0x0a,    9 /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject PccWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<ContentWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSPccWidgetENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSPccWidgetENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSPccWidgetENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PccWidget, std::true_type>,
        // method 'updateChartSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateSerieSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateSliceSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleSliceClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QPieSlice *, std::false_type>,
        // method 'showFontDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'appendSlice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'insertSlice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'removeSlice'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void PccWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PccWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateChartSettings(); break;
        case 1: _t->updateSerieSettings(); break;
        case 2: _t->updateSliceSettings(); break;
        case 3: _t->handleSliceClicked((*reinterpret_cast< std::add_pointer_t<QPieSlice*>>(_a[1]))); break;
        case 4: _t->showFontDialog(); break;
        case 5: _t->appendSlice(); break;
        case 6: _t->insertSlice(); break;
        case 7: _t->removeSlice(); break;
        default: ;
        }
    }
}

const QMetaObject *PccWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PccWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSPccWidgetENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return ContentWidget::qt_metacast(_clname);
}

int PccWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ContentWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
