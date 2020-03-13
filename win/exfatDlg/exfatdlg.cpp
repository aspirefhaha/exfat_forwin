#include "exfatdlg.h"

#include <QtGui>
#ifdef _DEBUG
#pragma comment(lib,"libexfat.lib")
#pragma comment(lib,"mkfs.lib")
#else
#pragma comment(lib,"libexfat.lib")
#pragma comment(lib,"mkfs.lib")
#endif
#include <qdebug.h>
#include "exnotepad.h"
exfatDlg::exfatDlg(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),exfatModel(this)
{
	ui.setupUi(this);
	//InModel.setRootPath("C:");
	exfatModel.addRootDevice(QString("fhaha.img"),EXFTDRIVE);
	//ui.tv_main->setModel(&InModel);
	//ui.tv_main->setRootIndex(InModel.index("C:"));
	ui.tv_main->setModel(&exfatModel);
	ui.tv_main->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tv_main->setAlternatingRowColors(true);
	ui.tv_main->setEditTriggers(QTreeView::NoEditTriggers);			//��Ԫ���ܱ༭    
	ui.tv_main->setSelectionBehavior(QTreeView::SelectRows);			//һ��ѡ������    
	ui.tv_main->setSelectionMode(QTreeView::SingleSelection);        //��ѡ�������������о���һ��ѡ����
	ui.tv_main->setColumnWidth(0,250);

}

exfatDlg::~exfatDlg()
{

}

void exfatDlg::sltFormat()
{
	//qDebug() << "here";
	exfatModel.resetfs();
}

void exfatDlg::editFile(const QModelIndex & index)
{
	ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
	exNotepad * pnote = new  exNotepad(pItemData,this);
	pnote->show();
	//exfatModel.editFile(editfile);
}

void exfatDlg::sltProperties(bool sel)
{
	QModelIndex curIndex = ui.tv_main->currentIndex();    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //ͬһ�е�һ��Ԫ�ص�index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		qDebug() << "edit here" << index;
		qDebug() << pItemData->absPath;
	}
}

void exfatDlg::sltAddFile(bool sel)
{
	QModelIndex curIndex = ui.tv_main->currentIndex();    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //ͬһ�е�һ��Ԫ�ص�index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		qDebug() << "add file here" << index;
		qDebug() << pItemData->absPath;
	}
}

void exfatDlg::sltAddDir(bool sel)
{
	QModelIndex curIndex = ui.tv_main->currentIndex();    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //ͬһ�е�һ��Ԫ�ص�index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		qDebug() << "add dir here" << index;
		qDebug() << pItemData->absPath;
	}
}

void exfatDlg::sltEdit(bool sel)
{
	QModelIndex curIndex = ui.tv_main->currentIndex();    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //ͬһ�е�һ��Ԫ�ص�index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		qDebug() << "edit here" << index;
		
		editFile(index);
		//sltEdit(index);
	}
}

void exfatDlg::sltEdit(const QModelIndex & curIndex)
{
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //ͬһ�е�һ��Ԫ�ص�index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		if(pItemData->fstype== EXFTFILE){
			qDebug() << "double should edit here" << index;
			
			editFile(index);
		}
		
		
		//sltEdit(index);
	}
	
}

void exfatDlg::sltContextMenu(const QPoint & pos)
{
	QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
				  QMenu::item{padding:3px 20px 3px 20px;}\
				  QMenu::indicator{width:13px;height:13px;}\
				  QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
				  QMenu::separator{height:1px;background:#757575;}"; 
	QMenu menu;    
	menu.setStyleSheet(qss);    //���Ը��˵�������ʽ     
	QModelIndex curIndex = ui.tv_main->indexAt(pos);      //��ǰ�����Ԫ�ص�index    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //���еĵ�1��Ԫ�ص�index   

	if (index.isValid())    {        //�ɻ�ȡԪ�ص��ı���data,���������жϴ���        
		//QStandardItem* item = mModel->itemFromIndex(index);        
		//QString text = item->text();        
		//QVariant data = item->data(Qt::UserRole + 1);        
		//...   
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());

		//���һ�в˵�������չ��        
		menu.addAction(tr("Properties"), this, SLOT(sltProperties(bool)));        
		menu.addSeparator();    //���һ���ָ��� 
		switch(pItemData->fstype){
		case EXFTFILE:
			menu.addAction(tr("Edit"), this, SLOT(sltEdit(bool)));  
			break;
		case EXFTDIR:
			menu.addAction(tr("Add Dir"), this, SLOT(sltAddDir(bool)));  
			menu.addAction(tr("Add File"), this, SLOT(sltAddFile(bool)));  
			break;
		}  
	}    
	menu.exec(QCursor::pos());  //��ʾ�˵�

}
