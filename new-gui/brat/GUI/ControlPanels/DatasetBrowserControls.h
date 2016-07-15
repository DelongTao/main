#ifndef GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
#define GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H


#include "DesktopControlPanel.h"


class CTextWidget;


// TODO (DELETE): old Brat code at gui/DatasetPanel.cpp //


/////////////////////////////////////////////////////////////////////////////////////
//								Dataset Browser
/////////////////////////////////////////////////////////////////////////////////////

class CDatasetBrowserControls : public CDesktopControlsPanel
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

	Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	//types

    using base_t = CDesktopControlsPanel;


	//instance data

#if defined(TEST_RADS)
	CStackedWidget *mBrowserStakWidget = nullptr;
    QAbstractButton *m_BrowseFilesButton = nullptr;
    QAbstractButton *m_BrowseRadsButton = nullptr;
#endif

    QTreeWidget *mDatasetTree = nullptr;

    QToolButton *mNewDataset = nullptr;
    QToolButton *mDeleteDataset = nullptr;
    QToolButton *mAddDir = nullptr;
    QToolButton *mAddFiles = nullptr;
    QToolButton *mRemove = nullptr;
    QToolButton *mClear = nullptr;

    CTextWidget *mFileDesc = nullptr;
    QGroupBox   *mFileDescGroup = nullptr;
    QListWidget *mFieldList = nullptr;
    CTextWidget *mFieldDesc = nullptr;


	//...doamin data

	const CApplicationPaths &mBratPaths;
    CWorkspaceDataset *mWDataset = nullptr;


    //construction / destruction

    void Wire();

public:
    explicit CDatasetBrowserControls( CModel &model, CDesktopManagerBase *manager, QWidget *parent = nullptr, Qt::WindowFlags f = 0 );

    virtual ~CDatasetBrowserControls()
    {}

    // access

    // operations
    void AddFiles(QStringList &paths_list);

    void DatasetChanged(QTreeWidgetItem *tree_item );
    void FileChanged(QTreeWidgetItem *file_item );

    QTreeWidgetItem *AddDatasetToTree(const QString &dataset_name);
    void FillFileTree(QTreeWidgetItem *current_dataset_item);
    void FillFieldList(QTreeWidgetItem *current_file_item);
    void ClearFieldList();

signals:
    void CurrentDatasetChanged( CDataset* );
    void DatasetsChanged( CDataset* );

public slots:
    void HandleWorkspaceChanged( CWorkspaceDataset *wksd );

    void HandleFieldChanged();
    void HandleNewDataset();
    void HandleDeleteDataset();
    void HandleAddFiles();
    void HandleAddDir();
    void HandleRemoveFile();
    void HandleClearFiles();
    void HandleTreeItemChanged();

protected slots:

    void HandleDatasetExpanded(); // Resizes DatasetTree when datasets are expanded
    void HandleRenameDataset(QTreeWidgetItem *dataset_item, int col);
};






#endif // GUI_CONTROL_PANELS_DATASETBROWSERCONTROLS_H
