#ifndef EXNOTEPAD_H
#define EXNOTEPAD_H

#include "exfatModel.h"
#include <QWidget>
#include "ui_exnotepad.h"

class exNotepad : public QWidget
{
	Q_OBJECT

public:
	exNotepad(ExfatFSPrivate*exinfo ,QWidget *parent = 0);
	~exNotepad();

protected slots:
	void sltSave();

private:
	Ui::exNotepad ui;
	ExfatFSPrivate * m_pexinfo;
};

#endif // EXNOTEPAD_H
