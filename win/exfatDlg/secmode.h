#ifndef SECMODE_H
#define SECMODE_H

#include <QDialog>
#include "ui_secmode.h"

class SecModeDlg : public QDialog
{
	Q_OBJECT

public:
	SecModeDlg(QWidget *parent = 0);
	~SecModeDlg();

	void setKey(QString & key);
	int GetKeyMode();

//protected slots:
//	void sltSave();

private:
	Ui::Dialog ui;
};

#endif // SECMODE_H