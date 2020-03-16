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
	setAcceptDrops(true);
	//InModel.setRootPath("C:");
	exfatModel.addRootDevice(QString("fhaha.img"),EXFTDRIVE);
	//ui.tv_main->setModel(&InModel);
	//ui.tv_main->setRootIndex(InModel.index("C:"));
	ui.tv_main->setModel(&exfatModel);
	ui.tv_main->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tv_main->setAlternatingRowColors(true);
	ui.tv_main->setEditTriggers(QTreeView::NoEditTriggers);			//单元格不能编辑    
	ui.tv_main->setSelectionBehavior(QTreeView::SelectRows);			//一次选中整行    
	ui.tv_main->setSelectionMode(QTreeView::SingleSelection);        //单选，配合上面的整行就是一次选单行
	ui.tv_main->setColumnWidth(0,250);

}

void exfatDlg::dragEnterEvent(QDragEnterEvent * enterEvent)
{
	const QMimeData * eventdata = enterEvent->mimeData();
	if(eventdata->hasUrls()){
		enterEvent->accept();
	}
	
}

void exfatDlg::dropEvent(QDropEvent * event)
{
	auto src = event->source();
	//event->setDropAction(Qt::LinkAction);
	//event->accept();
	const QMimeData * eventdata = event->mimeData();
	qDebug() << eventdata->text();
	qDebug() << eventdata;
	QPoint noewpos = event->pos();
	qDebug() << "evebt pos: " <<  noewpos;
	QPoint globalpos = this->mapToGlobal(noewpos);
	qDebug() << "global pos: " << globalpos;
	QPoint ctlpos = ui.tv_main->mapFromGlobal(globalpos);
	
	qDebug() << "ctl pos : " << ctlpos;
	ctlpos.operator-=(QPoint(0,ui.tv_main->header()->height()));
	qDebug() << "new ctl pos : " << ctlpos;
	QModelIndex curIndex = ui.tv_main->indexAt(ctlpos);      //当前点击的元素的index 
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		//qDebug() << "edit here" << index;
		qDebug() << pItemData->absPath;
	}

	if(eventdata->hasUrls()){
		QList<QUrl>urlList=eventdata->urls();    //获取URL列表
       if(urlList.isEmpty())return ;
       //将其中第一个URL表示为本地文件路径
       QString fileName=urlList.at(0).toLocalFile();
       if(fileName.isEmpty())return;
 
       if(!fileName.isEmpty())                 //若文件路径不为空
       {
           QFile file(fileName);  //建立QFile对象并只读方式打开该文件
           //窗口标题更改为显示文件路径
           setWindowTitle(tr("%1 - %2").arg(fileName).arg("DND File"));
           if(!file.open(QIODevice::ReadOnly)) return; 
           
           QTextStream in(&file); //建立文本流对象
           //qDebug() << in.readAll(); //1.纯代码时使用。将文件所有内容读入编辑器
           //textEdit->setText(in.readAll());   //2.纯代码时使用。将文件所有内容读入编辑器
       }

	}

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
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		qDebug() << "edit here" << index;
		qDebug() << pItemData->absPath;
	}
}

void exfatDlg::sltAddFile(bool sel)
{
	QModelIndex curIndex = ui.tv_main->currentIndex();    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		qDebug() << "add file here" << index;
		qDebug() << pItemData->absPath;
	}
}

void exfatDlg::sltAddDir(bool sel)
{
	QModelIndex curIndex = ui.tv_main->currentIndex();    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		//qDebug() << "add dir here" << index;
		//qDebug() << pItemData->absPath;
		QModelIndex parent = index.parent();
		ui.tv_main->collapse(parent);
		ui.tv_main->expand(parent);
		QString dirname = QInputDialog::getText(this,tr("Input"),tr("New Dir Name")).trimmed();
		if(!dirname.isEmpty()){
			char newdirname[MAX_PATH]={0};
			QString newabsname ;
			dirname.replace(" " ,"");
			
			if(pItemData->fstype == EXFTDRIVE){
				newabsname = "/" + dirname;
			}
			else{
				newabsname = pItemData->absPath + "/" + dirname;	
			}
			exfat_utf16_to_utf8(newdirname,(const le16_t *)newabsname.data(),MAX_PATH,newabsname.length());
			if(exfat_mkdir(pItemData->m_pexfatRoot,newdirname)!=0){
				QMessageBox::warning(this, tr("Create Err"), tr("Create Dir Failed!"),QMessageBox::Ok,QMessageBox::Ok);
			}
		}
		
		QModelIndex rbindex = index.sibling(index.row(),3);
		exfatModel.notifyChange(index,rbindex);
	}
}

void exfatDlg::sltEdit(bool sel)
{
	QModelIndex curIndex = ui.tv_main->currentIndex();    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		qDebug() << "edit here" << index;
		
		editFile(index);
		//sltEdit(index);
	}
}

void exfatDlg::sltEdit(const QModelIndex & curIndex)
{
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index    
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
	menu.setStyleSheet(qss);    //可以给菜单设置样式   
	qDebug() << "right pos: " <<  pos;
	QModelIndex curIndex = ui.tv_main->indexAt(pos);      //当前点击的元素的index    
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //该行的第1列元素的index   

	if (index.isValid())    {        //可获取元素的文本、data,进行其他判断处理        
		//QStandardItem* item = mModel->itemFromIndex(index);        
		//QString text = item->text();        
		//QVariant data = item->data(Qt::UserRole + 1);        
		//...   
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());

		//添加一行菜单，进行展开        
		menu.addAction(tr("Properties"), this, SLOT(sltProperties(bool)));        
		menu.addSeparator();    //添加一个分隔线 
		switch(pItemData->fstype){
		case EXFTDRIVE:
			menu.addAction(tr("Add Dir"), this, SLOT(sltAddDir(bool)));  
			break;
		case EXFTFILE:
			menu.addAction(tr("Edit"), this, SLOT(sltEdit(bool)));  
			break;
		case EXFTDIR:
			menu.addAction(tr("Add Dir"), this, SLOT(sltAddDir(bool)));  
			menu.addAction(tr("Add File"), this, SLOT(sltAddFile(bool)));  
			break;
		}  
	}    
	menu.exec(QCursor::pos());  //显示菜单

}
