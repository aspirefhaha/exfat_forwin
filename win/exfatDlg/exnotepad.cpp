#include "exnotepad.h"
#include "exfat.h"
#include <qmessagebox.h>

exNotepad::exNotepad(ExfatFSPrivate*exinfo ,QWidget *parent)
	: QWidget(parent),m_pexinfo(exinfo)
{
	ui.setupUi(this);
	//setWindowModality(Qt::ApplicationModal);        //设置阻塞类型
	//setAttribute(Qt::WA_ShowModal, true);           //属性设置 true:模态 false:非模态
	//setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::Dialog);
	setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
	struct exfat * ef = m_pexinfo->m_pexfatRoot;
	struct exfat_node * node ;
	char utf8str[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
	exfat_utf16_to_utf8(utf8str,(const le16_t *)m_pexinfo->absPath.data(),EXFAT_UTF8_NAME_BUFFER_MAX,exinfo->absPath.length());
	int lookupret = exfat_lookup(ef,&node,utf8str);
	if(lookupret == 0){
		unsigned char * buf = (unsigned char *)malloc(node->size+1);
		memset(buf,0,node->size+1);
		ssize_t readsize = exfat_generic_pread(ef,node,buf,node->size,0);
		if(readsize >0){
			QString helstr = QString::fromUtf8((const char *)buf);
			ui.te_main->setText(helstr);
		}
		exfat_flush_node(ef,node);
		exfat_put_node(ef,node);
		free(buf);
	}
}

exNotepad::~exNotepad()
{

}

void exNotepad::sltSave()
{
	QString text = ui.te_main->toPlainText();
	//text.replace(QString("\n"), QString("\r\n"));
	if(m_pexinfo != NULL){
		struct exfat * ef = m_pexinfo->m_pexfatRoot;
		char utf8str[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
		exfat_utf16_to_utf8(utf8str,(const le16_t *)m_pexinfo->absPath.data(),EXFAT_UTF8_NAME_BUFFER_MAX,m_pexinfo->absPath.length());
		struct exfat_node * node ;
		int lookupret = exfat_lookup(ef,&node,utf8str);
		if(lookupret == 0){
			QByteArray utf8data = text.toUtf8();
			int datalen = utf8data.length();
			if(datalen < node->size){
				exfat_truncate(ef,node,datalen,true);
			}
			ssize_t writedata = exfat_generic_pwrite(ef,node,utf8data.data(),datalen,0);
			if(writedata != datalen){
				QMessageBox::warning(this, tr("Save Err"), tr("Size Err"),QMessageBox::Ok,QMessageBox::Ok);
				exfat_put_node(ef,node);
				return;
			}
			exfat_flush_node(ef,node);
			exfat_put_node(ef,node);
			this->close();
		}
		else{
			QMessageBox::warning(this, tr("Save Err"), tr("Filesystem Has Changed, Reopen File and Edit Again!"),QMessageBox::Ok,QMessageBox::Ok);
		}
	}
	else{
		QMessageBox::warning(this, tr("Edit Err"), tr("Opened Info err!"),QMessageBox::Ok,QMessageBox::Ok);
	}
	
}
