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

private:
	Ui::exNotepad ui;
};

#endif // EXNOTEPAD_H
