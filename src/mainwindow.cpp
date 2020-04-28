#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qclipboard.h>

#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Variate To QSettings"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getKeyByName(const QString &name)
{
    auto key = name;
    if(ui->radioButton_removeM->isChecked())
    {
        if(key.startsWith("m"))
            key = key.remove(0,1);
    }else if(ui->radioButton_removeM_->isChecked())
    {
        if(key.startsWith("m_"))
            key = key.remove(0,2);
    }else if(ui->radioButton_removeCustom->isChecked())
    {
        auto removeText = ui->lineEdit_removeCustom->text();
        if(key.startsWith(removeText))
            key = key.remove(0,removeText.size());
    }

    return key;
}


QString MainWindow::packSettingsSetValue(STVariate var, const QString &objName)
{
    auto key = getKeyByName(var.name);

    QString value = var.name;
    if(!objName.isEmpty())
    {
        value = objName + "." + var.name;
    }
    if(ui->checkBox_addNote->isChecked())
        return QString("settings.setValue(\"%1\",%2); %3")
                .arg(key).arg(value).arg(var.note);
    return QString("settings.setValue(\"%1\",%2);").arg(key).arg(value);
}

QString MainWindow::packSettingsValue(STVariate var, const QString &objName)
{
    auto key = getKeyByName(var.name);

    QString value = var.name;
    if(!objName.isEmpty())
    {
        value = objName + "." + var.name;
    }

    if(ui->checkBox_addNote->isChecked())
        return QString("%2 = settings.value(\"%1\",%2).value<%3>(); %4")
                .arg(key).arg(value).arg(var.type).arg(var.note);
    return QString("%2 = settings.value(\"%1\",%2).value<%3>();")
            .arg(key).arg(value).arg(var.type);
}

QString MainWindow::toSetValueText(const QString &text)
{
    if(text.isEmpty() && !text.contains("value(") && !text.contains("="))
    {
        return text;
    }

    int index = text.indexOf("(");
    int end = 0;
    if(text.contains(","))
        end = text.indexOf(",",index);
    else
        end = text.indexOf(")",index);
    auto name = text.mid(index + 1,end - index - 1);

    index = text.indexOf("=");
    auto value = text.mid(0,index).trimmed();

    end = text.indexOf(".");
    auto settings = text.mid(index + 1,end - index - 1).trimmed();

    return QString("%1.setValue(%2,%3);").arg(settings).arg(name).arg(value);
}

QString MainWindow::toValueText(const QString &text)
{
    if(text.isEmpty() &&
            (!text.contains(".setValue(") || !text.contains("->setValue(")) &&
            !text.contains(",") &&
            !text.contains(")"))
    {
        return text;
    }

    int index = text.indexOf("(");
    int end = text.indexOf(",",index);
    auto name = text.mid(index + 1,end - index - 1);

    index = text.indexOf(")");
    auto value = text.mid(end + 1,index - end - 1);

    end = text.indexOf(".");
    auto settings = text.mid(0,end).trimmed();

    return QString("%1 = %2.value(%3,%1).value<>();").arg(value).arg(settings).arg(name);
}

void MainWindow::variateToSetting(const QString &text)
{
    auto list = getVariateTypeAndeName(text);

    foreach (auto var, list) {
        if(mStructMap.contains(var.type))
        {
            mCurStructList.append(var);
        }else
        {
            mSetValueList << packSettingsSetValue(var);
            mValueList << packSettingsValue(var);
        }
    }
}

void MainWindow::splitStruct()
{
    mStructMap.clear();
    auto source = ui->plainTextEdit_struct->toPlainText();
    getStructContentText(source);
}

void MainWindow::getStructContentText(const QString &text)
{
    if(text.contains("struct"))
    {
        auto index = text.indexOf("struct");
        auto begin = text.indexOf("{",index);
        auto end = text.indexOf("}",begin);

        auto structText = text.mid(index,begin - index);
        auto contentText = text.mid(begin + 1,end - begin - 1);
        auto residueText = text.mid(end + 1);

        structText = structText.trimmed();
        contentText = contentText.trimmed();
        residueText = residueText.trimmed();

        auto structPair = getTypeAndName(structText);
        if(!structPair.second.isEmpty())
        {
            STVariateMap varMap;
            auto contentList = contentText.split("\n",QString::SkipEmptyParts);
            foreach (auto content, contentList) {
                auto varList = getVariateTypeAndeName(content);
                foreach (auto var, varList) {
                    varMap.insert(var.name,var);
                }
            }

            mStructMap.insert(structPair.second,varMap);
        }

        getStructContentText(residueText);
    }
}

QStringPair MainWindow::getTypeAndName(const QString &text)
{
    QStringPair pair;
    if(!text.isEmpty() && text.contains(" "))
    {
        QStringList list = text.split(" ",QString::SkipEmptyParts);
        if(list.size() == 2)
        {
            auto type = list.at(0).trimmed();
            auto name = list.at(1).trimmed();
            pair = qMakePair(type,name);
            return pair;
        }
    }

    pair = qMakePair(QString(),QString());
    return pair;
}

STVariateList MainWindow::getVariateTypeAndeName(const QString &text)
{
    STVariateList list;
    if(!text.isEmpty() && text.contains(";"))
    {
        auto temp = text;
        temp = temp.trimmed();
        auto tempList = temp.split(";",QString::SkipEmptyParts);

        QString noteText = "";
        if(tempList.size() > 0)
        {
            auto note = tempList.last();
            note = note.trimmed();
            if(note.startsWith("/"))
            {
                noteText = note;
            }
        }

        foreach (auto variate, tempList)
        {
            variate = variate.trimmed();
            auto pair = getTypeAndName(variate);
            if(!pair.first.isEmpty() && !pair.second.isEmpty())
            {
                auto varList = checkMultiVariate(pair.second);
                foreach (auto var, varList)
                {
                    auto arrayList = checkVariateArray(var);
                    foreach (auto array, arrayList)
                    {
                        STVariate variate;
                        variate.type = pair.first;
                        variate.name = array;
                        variate.note = noteText;

                        list.append(variate);
                    }
                }
            }
        }
    }

    return list;
}

QStringList MainWindow::checkMultiVariate(const QString &text)
{
    if(text.contains(","))
    {
        return text.split(",",QString::SkipEmptyParts);
    }

    return QStringList() << text;
}

QStringList MainWindow::checkVariateArray(const QString &text)
{
    if(text.contains("[") && text.contains("]"))
    {
        int begin = text.indexOf("[");
        int end = text.indexOf("]");
        auto nameText = text.mid(0,begin);
        auto numberText = text.mid(begin + 1,end - begin - 1);
        numberText = numberText.trimmed();
        bool isOk;
        auto number = numberText.toInt(&isOk);
        if(isOk)
        {
            QStringList list;
            for (int i = 0; i < number; ++i)
            {
                auto key = QString("%1[%2]").arg(nameText).arg(i);
                list << key;
            }

            return list;
        }
    }

    return QStringList() << text;
}

QStringList MainWindow::getQSettingsHeader(QString name)
{
    if(name.isEmpty())
        name = "config";

    QStringList list;
    list << "auto path = QString(\"xxx.ini\");";
    list << "QSettings settings(path);";
    list << "settings.setIniCodec(\"UTF-8\");";
    list << QString("settings.beginGroup(\"%1\");").arg(name);
    return list;
}

QString MainWindow::getSettingBeginGroup(const QString &text)
{
    return QString("settings.beginGroup(\"%1\");").arg(text);
}

QString MainWindow::getSettingEndGroup()
{
    return QString("settings.endGroup();");
}

void MainWindow::toSettingsFromStructList(STVariateList varList)
{
    foreach (auto var, varList)
    {
        auto map = mStructMap.value(var.type);

        mSetValueList << getSettingBeginGroup(var.type + "." + var.name);
        mValueList << getSettingBeginGroup(var.type + "." + var.name);

        foreach (auto key, map.keys())
        {
            auto varr = map.value(key);
            if(mStructMap.contains(varr.type))
            {
                STVariateList tempList;
                tempList.append(varr);
                toSettingsFromStructList(tempList);
            }
            else
            {
                mSetValueList << packSettingsSetValue(varr,var.name);
                mValueList << packSettingsValue(varr,var.name);
            }

        }

        mSetValueList << getSettingEndGroup();
        mValueList << getSettingEndGroup();

    }
}

void MainWindow::on_pushButton_transform_clicked()
{
    splitStruct();

    mCurStructList.clear();
    mSetValueList.clear();
    mValueList.clear();

    if(ui->checkBox_addQSettings->isChecked())
    {
        auto list = getQSettingsHeader();
        mSetValueList.append(list);
        mValueList.append(list);
    }

    auto source = ui->plainTextEdit_source->toPlainText();
    foreach (auto text, source.split("\n"))
    {
        variateToSetting(text);
    }

    mSetValueList << getSettingEndGroup();
    mValueList << getSettingEndGroup();

    mSetValueList << "\n";
    mValueList << "\n";

    toSettingsFromStructList(mCurStructList);

    auto setValueText = mSetValueList.join("\n");
    auto valueText = mValueList.join("\n");

    ui->plainTextEdit_setValue->clear();
    ui->plainTextEdit_setValue->appendPlainText(setValueText);

    ui->plainTextEdit_value->clear();
    ui->plainTextEdit_value->appendPlainText(valueText);
}

void MainWindow::on_pushButton_clearSource_clicked()
{
    ui->plainTextEdit_source->clear();
}

void MainWindow::on_pushButton_copySetValue_clicked()
{
    qApp->clipboard()->setText(ui->plainTextEdit_setValue->toPlainText());
}

void MainWindow::on_pushButton_tfValue_clicked()
{
    auto source = ui->plainTextEdit_setValue->toPlainText();
    QStringList valueList;
    foreach (auto text, source.split("\n"))
    {
       valueList << toValueText(text);
    }

    auto valueText = valueList.join("\n");

    ui->plainTextEdit_value->clear();
    ui->plainTextEdit_value->appendPlainText(valueText);
    on_pushButton_copyValue_clicked();
}

void MainWindow::on_pushButton_clearSetValue_clicked()
{
    ui->plainTextEdit_setValue->clear();
}

void MainWindow::on_pushButton_copyValue_clicked()
{
    qApp->clipboard()->setText(ui->plainTextEdit_value->toPlainText());
}

void MainWindow::on_pushButton_tfSetValue_clicked()
{
    auto source = ui->plainTextEdit_value->toPlainText();
    QStringList setValueList;
    foreach (auto text, source.split("\n"))
    {
        setValueList << toSetValueText(text);
    }

    auto setValueText = setValueList.join("\n");

    ui->plainTextEdit_setValue->clear();
    ui->plainTextEdit_setValue->appendPlainText(setValueText);
    on_pushButton_copySetValue_clicked();
}

void MainWindow::on_pushButton_clearValue_clicked()
{
    ui->plainTextEdit_value->clear();
}

void MainWindow::on_radioButton_removeCustom_clicked(bool checked)
{
    ui->lineEdit_removeCustom->setEnabled(checked);
}


void MainWindow::on_pushButton_clearStruct_clicked()
{
    ui->plainTextEdit_struct->clear();
}

void MainWindow::on_pushButton_pasteStruct_clicked()
{
    ui->plainTextEdit_struct->clear();
    ui->plainTextEdit_struct->setPlainText(qApp->clipboard()->text());
}

void MainWindow::on_pushButton_pasteSource_clicked()
{
    ui->plainTextEdit_source->clear();
    ui->plainTextEdit_source->setPlainText(qApp->clipboard()->text());
}
