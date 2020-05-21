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
#include "exfatModel.h"
#include "exnotepad.h"


#ifdef linux
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
typedef int DWORD;
#endif
exfatDlg::exfatDlg(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags),exfatModel(this),dia(this),bgcopyTh(this),m_progesssize(0)
{
	ui.setupUi(this);
	setAcceptDrops(true);
	//InModel.setRootPath("C:");
#if USEXDISK==0
	QString fileName = QFileDialog::getOpenFileName(
		this, 
		tr("open a file."),
		"./", 
		tr("File img(*.img);;All files(*.*)"));
	if (fileName.isEmpty()) {
		QMessageBox::warning(this, "Warning!", "Failed to open the Img!");
		close();
		return;
	}
	exfatModel.addRootDevice(fileName,EXFTDRIVE);
#else
	exfatModel.addRootDevice(XDISKDEFAULTKEY,EXFTDRIVE);
#endif
	
	//ui.tv_main->setModel(&InModel);
	//ui.tv_main->setRootIndex(InModel.index("C:"));
	ui.tv_main->setModel(&exfatModel);
	ui.tv_main->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tv_main->setAlternatingRowColors(true);
	ui.tv_main->setEditTriggers(QTreeView::NoEditTriggers);			//单元格不能编辑    
	ui.tv_main->setSelectionBehavior(QTreeView::SelectRows);			//一次选中整行    
	ui.tv_main->setSelectionMode(QTreeView::SingleSelection);        //单选，配合上面的整行就是一次选单行
	ui.tv_main->setColumnWidth(0,250);
	dia.setModal(true);
	dia.setWindowTitle("Progress Dialog Test");//设置窗口标题
	dia.setLabelText("Updating...");//提示标签
	connect(&bgcopyTh,SIGNAL(updateSize(qlonglong )),this,SLOT(updateCpSize(qlonglong)));
	connect(&bgcopyTh,SIGNAL(updateProg(qlonglong)),this,SLOT(updateCpProg(qlonglong)));
	connect(&bgcopyTh,SIGNAL(copyDone()),this,SLOT(copyDone()));
	connect(&bgcopyTh,SIGNAL(warningMsg(QString & ,QString&)),this,SLOT(showWarningMsg(QString & ,QString&)));
}

void exfatDlg::showWarningMsg(QString & title,QString & msg)
{
	
	QMessageBox::warning(this, title, msg,QMessageBox::Ok,QMessageBox::Ok);
}

void exfatDlg::copyDone()
{
	dia.close();
	QModelIndex curidx = ui.tv_main->currentIndex();
	QModelIndex curheadidx = curidx.sibling(curidx.row(),0);
	ui.tv_main->collapse(curheadidx);
	ui.tv_main->expand(curheadidx);
}

void exfatDlg::updateCpSize(qlonglong tsize)
{
	//dia.set();
	dia.setMaximum(1000);//设置最大值
	m_progesssize = tsize;		

}

void exfatDlg::updateCpProg(qlonglong prog)
{
	//dia.close();
	dia.setValue(prog*1000/m_progesssize);//设置进度条数值
}

void exfatDlg::dragEnterEvent(QDragEnterEvent * enterEvent)
{
	const QMimeData * eventdata = enterEvent->mimeData();
	if(eventdata->hasUrls()){
		enterEvent->accept();
	}
}

void exfatDlg::dragMoveEvent(QDragMoveEvent * event)
{
	//qDebug() << event;
	QPoint noewpos = event->pos();
	//qDebug() << "move pos: " <<  noewpos;
	QPoint globalpos = this->mapToGlobal(noewpos);
	//qDebug() << "move global pos: " << globalpos;
	QPoint ctlpos = ui.tv_main->mapFromGlobal(globalpos);
	//qDebug() << "move ctl pos : " << ctlpos;
	ctlpos.operator-=(QPoint(0,ui.tv_main->header()->height()));
	//qDebug() << "move new ctl pos : " << ctlpos;
	QModelIndex curIndex = ui.tv_main->indexAt(ctlpos);      //当前点击的元素的index 
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index    
	if(index.isValid()){
		ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
		if(pItemData->fstype == EXFTFILE){
			ui.tv_main->setCurrentIndex(index.parent());
		}
		else{
			ui.tv_main->expand(index);
			ui.tv_main->setCurrentIndex(index);
		}

	}
}

void exfatDlg::dropEvent(QDropEvent * event)
{
	auto src = event->source();
	//event->setDropAction(Qt::LinkAction);
	//event->accept();
	const QMimeData * eventdata = event->mimeData();
	//qDebug() << eventdata;
	QPoint noewpos = event->pos();
	//qDebug() << "evebt pos: " <<  noewpos;
	QPoint globalpos = this->mapToGlobal(noewpos);
	//qDebug() << "global pos: " << globalpos;
	QPoint ctlpos = ui.tv_main->mapFromGlobal(globalpos);
	
	//qDebug() << "ctl pos : " << ctlpos;
	ctlpos.operator-=(QPoint(0,ui.tv_main->header()->height()));
	//qDebug() << "new ctl pos : " << ctlpos;
	QModelIndex curIndex = ui.tv_main->indexAt(ctlpos);      //当前点击的元素的index 
	QModelIndex index = curIndex.sibling(curIndex.row(),0); //同一行第一列元素的index    
	if(index.isValid() && eventdata->hasUrls()){
		
		QList<QUrl>urlList=eventdata->urls();    //获取URL列表
		if(urlList.isEmpty())return ;
		//将其中第一个URL表示为本地文件路径
		QString fileName=urlList.at(0).toLocalFile();
		if(fileName.isEmpty())return;
 
		if(!fileName.isEmpty())                 //若文件路径不为空
		{
			QFile file(fileName);  //建立QFile对象并只读方式打开该文件
			//窗口标题更改为显示文件路径
			//setWindowTitle(tr("%1 - %2").arg(fileName).arg("DND File"));
			if(!file.open(QIODevice::ReadOnly)){
				QFileInfo fileinfo(fileName);
				if(!fileinfo.isDir())
					return; 
			}
			else{
				file.close();
			}
			//QTextStream in(&file); //建立文本流对象
			//qDebug() << in.readAll(); //1.纯代码时使用。将文件所有内容读入编辑器
			//textEdit->setText(in.readAll());   //2.纯代码时使用。将文件所有内容读入编辑器
			ExfatFSPrivate* pItemData = static_cast<ExfatFSPrivate*>(index.internalPointer());
			if(pItemData->fstype == EXFTFILE){
				pItemData = static_cast<ExfatFSPrivate*>(index.parent().internalPointer());
			}
			//qDebug() << "edit here" << index;
			//qDebug() << pItemData->absPath;
			
			
			dia.setMinimum(0);//设置最小值
			dia.setMaximum(100);//设置最大值
			dia.setValue(0);//设置进度条数值

			//如何实时更新进度条？
			//创建一个新线程或通过使用信号与槽获取实时的value值，后台不断的下载或者更新
			//通过set和get方法传递value值，最后通过setValue()实时更新进度条
			
			bgcopyTh.setWorkMode(WMCPTOEXF);
			QList<QString> selfiles ;
			selfiles << fileName;
			bgcopyTh.setSelPath(selfiles);
			if(pItemData->fstype==EXFTDRIVE){
				QString tmpStr = "/";

				bgcopyTh.setTarget(tmpStr);
			}
			else
				bgcopyTh.setTarget(pItemData->absPath);
			bgcopyTh.setExfat(pItemData->m_pexfatRoot);
			bgcopyTh.start();
			dia.exec();
		}

		

	}

	

}

exfatDlg::~exfatDlg()
{

}

void exfatDlg::sltLoad()
{

}

void exfatDlg::sltFormat()
{
#if USEXDISK==0
	QString filename = QInputDialog::getText(this,tr("Input"),tr("New FileSystem File Name")).trimmed();
	if(filename.isEmpty()){
		QMessageBox::warning(this, tr("Name Err"), tr("FileName is Empty"),QMessageBox::Ok,QMessageBox::Ok);
		return;
	}
	QString fssizestr = QInputDialog::getText(this,tr("Input"),tr("New Filesystem Size(GB) between 2 to 64")).trimmed();
	int fssize = fssizestr.toInt();
	if(fssize>=64 || fssize<=2){
		QMessageBox::warning(this, tr("Num Err"), tr("Size Err,pls input a number between 2 to 64"),QMessageBox::Ok,QMessageBox::Ok);
		return;
	}
	qint64 fssizeg = fssize * 1024;
	fssizeg *= 1024LL * 1024LL;
#if linux
#if 0
	QFile fp;
	fp.setFileName(filename);
	fp.open(QIODevice::WriteOnly | QIODevice::Truncate );
	fp.seek(fssizeg);
	fp.write("\0");
	fp.flush();
	fp.close();
#else
	int fd = open(filename.toStdString().c_str(),O_WRONLY | O_CREAT | O_TRUNC ,0777);
	if( -1 == fd){
		perror("create file fail");
		return ;
	}
	printf("create file %s success\n",filename.toStdString().c_str());
	long long ret = lseek(fd,fssizeg,SEEK_END);
	if( -1 == ret){
		perror("lseek file fail");
		return;
	}
	ret = write(fd,"\0",1);
	if(ret != 1){
		perror("write 1 byte failed!");
	}
	::close(fd);
#endif
#else
	//qDebug() << "here";
	Q
	DWORD nWritten = 0;

	HANDLE hFile = CreateFileA(filename.toStdString().c_str(),GENERIC_WRITE|GENERIC_READ,          
		FILE_SHARE_READ|FILE_SHARE_WRITE,         
		NULL,         
		CREATE_ALWAYS,         
		0,         
		NULL);     
	DWORD dwTemp;     
	DeviceIoControl(hFile,         
		FSCTL_SET_SPARSE,          
		NULL,         
		0,         
		NULL,         
		0,        
		&dwTemp,         
		NULL); 
	LARGE_INTEGER li;

	li.QuadPart = fssizeg;

	li.LowPart = SetFilePointer (hFile,
		li.LowPart,
		&li.HighPart,
		FILE_BEGIN);

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError()!= NO_ERROR)
	{
		li.QuadPart = -1;
	}

	
	//SetFilePointer(hFile, fssizeg, NULL, FILE_BEGIN);     
	WriteFile(hFile,         "\0",         1,         &nWritten,         NULL);     
	SetEndOfFile(hFile);     
	CloseHandle(hFile);

#endif

	exfatModel.setFsFilename(filename);
#else
	QString tmpQStr = XDISKDEFAULTKEY;
	exfatModel.setFsFilename(tmpQStr);
#endif
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

void exfatDlg::sltDelete(bool sel)
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
		
		char selfilename[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
		QString selabsname ;
		selabsname = pItemData->absPath;

				
		exfat_utf16_to_utf8(selfilename,(const le16_t *)selabsname.data(),EXFAT_UTF8_NAME_BUFFER_MAX,selabsname.length());
		struct exfat_node * pnode;
		if(exfat_lookup(pItemData->m_pexfatRoot,&pnode,selfilename)==0){
			if(exfat_unlink(pItemData->m_pexfatRoot,pnode)==0){
				exfat_put_node(pItemData->m_pexfatRoot,pnode);
				exfat_cleanup_node(pItemData->m_pexfatRoot,pnode);
				QMessageBox::information(this, selabsname, tr("Delete Ok!"),QMessageBox::Ok,QMessageBox::Ok);
			}
		}
		
		QModelIndex rbindex = index.sibling(index.row(),3);
		exfatModel.notifyChange(index,rbindex);
	}
}

void exfatDlg::sltRename(bool sel)
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
		QString dirname = QInputDialog::getText(this,tr("Input"),tr("New Name")).trimmed();
		if(!dirname.isEmpty()){
			char newdirname[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
			char olddirname[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
			QString newabsname ;
			QString oldabsname;
			dirname.replace("/" ,"");
			dirname = dirname.trimmed();
			
			if(pItemData->fstype == EXFTDRIVE){
				//newabsname = "/" + dirname;
				return;
			}
			else{
				oldabsname = pItemData->absPath;

				newabsname = pItemData->absPath;
				
				newabsname.truncate(newabsname.lastIndexOf("/"));
				newabsname.append( "/");
				newabsname.append( dirname);	
			}
			exfat_utf16_to_utf8(newdirname,(const le16_t *)newabsname.data(),EXFAT_UTF8_NAME_BUFFER_MAX,newabsname.length());
			exfat_utf16_to_utf8(olddirname,(const le16_t *)oldabsname.data(),EXFAT_UTF8_NAME_BUFFER_MAX,oldabsname.length());
			if(exfat_rename(pItemData->m_pexfatRoot,olddirname,newdirname)!=0){
				QMessageBox::warning(this, tr("Rename Err"), tr("Rename Failed!"),QMessageBox::Ok,QMessageBox::Ok);
			}
		}
		
		QModelIndex rbindex = index.sibling(index.row(),3);
		exfatModel.notifyChange(index,rbindex);
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
			char newdirname[EXFAT_UTF8_NAME_BUFFER_MAX]={0};
			QString newabsname ;
			dirname.replace("/" ,"");
			dirname = dirname.trimmed();
			
			if(pItemData->fstype == EXFTDRIVE){
				newabsname = "/" + dirname;
			}
			else{
				newabsname = pItemData->absPath + "/" + dirname;	
			}
			exfat_utf16_to_utf8(newdirname,(const le16_t *)newabsname.data(),EXFAT_UTF8_NAME_BUFFER_MAX,newabsname.length());
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
			menu.addAction(tr("Rename"),this,SLOT(sltRename(bool)));
			menu.addAction(tr("Delete"),this,SLOT(sltDelete(bool)));
			break;
		case EXFTDIR:
			menu.addAction(tr("Add Dir"), this, SLOT(sltAddDir(bool)));  
			menu.addAction(tr("Add File"), this, SLOT(sltAddFile(bool)));  
			menu.addAction(tr("Rename"),this,SLOT(sltRename(bool)));
			break;
		}  
	}    
	menu.exec(QCursor::pos());  //显示菜单

}
