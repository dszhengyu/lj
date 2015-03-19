#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QMessageBox>

class MessageBox
{
public:
    explicit MessageBox(const QString &te, const QString &ti = "Warning", const QString &pic = "images/error.png") :
        title (ti), text(te)
    {
        QMessageBox aboutMessage(QMessageBox::NoIcon, ti, te);
        aboutMessage.setIconPixmap(QPixmap(pic));
        aboutMessage.exec();
    }

private:
    QString title;
    QString text;
    QString picture;
};

#endif // MESSAGEBOX_H
