#include "secmode.h"

SecModeDlg::SecModeDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	
}

SecModeDlg::~SecModeDlg()
{

}

void SecModeDlg::setKey(QString & key)
{
	ui.le_Key->setText(key);
}

int SecModeDlg::GetKeyMode()
{
	if(ui.rb_AES->isChecked()){
		return 1;
	}
	else if(ui.rb_SM4->isChecked()){
		return 2;
	}
	return 0;
}