#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
#else
#include <QMainWindow>
#endif
#include <qmap>
#include <qdebug.h>
namespace Ui {
class MainWindow;
}

struct STVariate
{
  QString type;
  QString name;
  QString note;
  STVariate()
  {
      type = "";
      name = "";
      note = "";
  }
  void printf()
  {
      qDebug() << QString("variate( %1,%2,%3 )").arg(type).arg(name).arg(note);
  }
};

typedef QMap<QString,QString> QStringMap;
typedef QPair<QString,QString> QStringPair;
typedef QList<QStringPair> QStringPairList;
typedef QList<STVariate> STVariateList;
typedef QMap<QString,STVariate> STVariateMap;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_pushButton_transform_clicked();
    void on_pushButton_clearSource_clicked();

    void on_pushButton_copySetValue_clicked();
    void on_pushButton_tfValue_clicked();
    void on_pushButton_clearSetValue_clicked();

    void on_pushButton_copyValue_clicked();
    void on_pushButton_tfSetValue_clicked();
    void on_pushButton_clearValue_clicked();

    void on_radioButton_removeCustom_clicked(bool checked);

    void on_pushButton_clearStruct_clicked();
    void on_pushButton_pasteStruct_clicked();

    void on_pushButton_pasteSource_clicked();

private:
    QString getKeyByName(const QString &name);
    QString packSettingsSetValue(STVariate var,const QString &objName = "");
    QString packSettingsValue(STVariate var,const QString &objName = "");
private:
    Ui::MainWindow *ui;
    QString toSetValueText(const QString &text);
    QString toValueText(const QString &text);
    void variateToSetting(const QString &text);

    void splitStruct();
    void getStructContentText(const QString &text);

    QStringPair getTypeAndName(const QString &text);
    STVariateList getVariateTypeAndeName(const QString &text);
    QStringList checkMultiVariate(const QString &text);
    QStringList checkVariateArray(const QString &text);
    QStringList getQSettingsHeader(QString name = "");
    QString getSettingBeginGroup(const QString &text);
    QString getSettingEndGroup();
    void toSettingsFromStructList(STVariateList varList);

    //key:为struct name value:结构体里变量声明集合
    QMap<QString,STVariateMap> mStructMap;
    STVariateList mCurStructList;
    QStringList mSetValueList;
    QStringList mValueList;
};

#endif // MAINWINDOW_H
