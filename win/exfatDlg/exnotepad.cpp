#include "exnotepad.h"
#include "exfat.h"

exNotepad::exNotepad(ExfatFSPrivate*exinfo ,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//setWindowModality(Qt::ApplicationModal);        //设置阻塞类型
	//setAttribute(Qt::WA_ShowModal, true);           //属性设置 true:模态 false:非模态
	//setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::Dialog);
	setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
	struct exfat * ef = exinfo->m_pexfatRoot;
	struct exfat_node * node ;
	char utf8str[MAX_PATH]={0};
	exfat_utf16_to_utf8(utf8str,(const le16_t *)exinfo->absPath.data(),MAX_PATH,exinfo->absPath.length());
	int lookupret = exfat_lookup(ef,&node,utf8str);
	if(lookupret == 0){
		char * buf = new char[node->size];
		memset(buf,0,node->size);
		ssize_t readsize = exfat_generic_pread(ef,node,buf,node->size,0);
		if(readsize >0){
			ui.te_main->setText(buf);
		}
		exfat_put_node(ef,node);
		delete[] buf;
	}
}

exNotepad::~exNotepad()
{

}
