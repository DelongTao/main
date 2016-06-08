#ifndef SCHEDULER_DLG_H
#define SCHEDULER_DLG_H

#if defined(QT_VERSION) && (QT_VERSION < 0x050000)
#include <QtGui/QToolBar>
#else
#include <QtWidgets/QToolBar>
#endif

#include "ui_SchedulerDialog.h"
#include "new-gui/Common/DataModels/TaskProcessor.h"


class CConsoleApplicationPaths;
class CSchedulerApplication;
class CBasicLogShell;
class CProcessesTable;



class CSchedulerDialog : public QDialog, private Ui::CSchedulerDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

	// types

	using base_t = QDialog;

	using uid_t = CBratTask::uid_t;

	struct post_execution_handler_wrapper_t;

	using post_execution_handler_t = void(CSchedulerDialog::*)( int exit_code, QProcess::ExitStatus exitStatus, post_execution_handler_wrapper_t *phandler );

	struct post_execution_handler_wrapper_t
	{
		CBratTask *task;
		post_execution_handler_t next_post_execution_handler;
		int isubtask;
	};

    // static members

    static const auto smSCHEDULER_TIMER_INTERVAL = 30000;
    static const auto smCHECK_CONFIGFILE_TIMER_INTERVAL = 5000;

	static QString MakeWindowTitle( const QString &title = QString() );	//returns the application name if title is empty


    // instance data members

	CProcessesTable *mProcessesTable = nullptr;
	int mProcessesTableIndex = -1;
	bool mSyncProcessExecuting = false;

    CBasicLogShell *mLogFrame = nullptr;
	int mLogFrameIndex = -1;
	QHash< int, QColor >	mSeverityToColorTable;
	QHash< int, QString >	mSeverityToPromptTable;

    const CConsoleApplicationPaths *mAppPaths;
	std::string mSchedulerFilePath;

	QTimer mSchedulerTimer;
	QTimer mCheckConfigFileTimer;
	QDateTime mLastCheck;

    QMutex mTasksMutex;


	// construction / destruction

    void CreateMenuBar();
    void CreateLogTable();
    void CreateProcessesTable();
	
	template< typename... Args > 
	void SetTablesProperties( Args... args );
    
	void UpdateTasksTables();

public:
    CSchedulerDialog( CSchedulerApplication &a, QWidget *parent = 0);

    virtual ~CSchedulerDialog();


	// operations

protected:
	void StartTimers();
	void StopTimers();

	void SetItemProcessData( QTableWidget *table, int index, CBratTask *task );
	CBratTask* GetItemProcessData( QTableWidget *table, int index ) const;

    void UpdateTasksTable( const CMapBratTask &data, QTableWidget *table );

	bool RemoveTaskFromTable( QTableWidget *table, const CBratTask *task );
	bool RemoveTaskFromTableAndMap( QTableWidget *table, CBratTask *task );
	bool RemoveTasksFromTableAndMap( QTableWidget *table );
	void InsertTaskInTable( QTableWidget *table, CBratTask *task );

	bool ExecuteAsync( CBratTask *task, int isubtask = - 1 );
	void AsyncTaskFinished( int exit_code, QProcess::ExitStatus exitStatus, post_execution_handler_wrapper_t *phandler );
	void AsyncSubtaskFinished( int exit_code, QProcess::ExitStatus exitStatus, post_execution_handler_wrapper_t *phandler );


	// signals / slots

    virtual void closeEvent( QCloseEvent *event ) override;

signals:
	void AsyncProcessExecution( bool executing );
	void SyncProcessExecution( bool executing );

public slots:

    void action_ToggleTimers_slot( bool checked );
    void action_ViewConfig_slot();
    void action_UserManual_slot();
    void action_About_slot();

	void SchedulerTimerTimeout();
	void CheckConfigFileTimerTimeout();

private slots:

	void HandlePeerMessage( const QString& );
	void HandleProcessFinished( int exit_code, QProcess::ExitStatus exitStatus, bool sync, void *user_data );

    void on_mRemovePendTaskButton_clicked();
    void on_mClearPendTaskButton_clicked();
    void on_mRemoveEndedTaskButton_clicked();
    void on_mClearEndedTaskButton_clicked();
    void on_mShowEndedLogButton_clicked();

	void HandleSelectedPendingProcessChanged();
	void HandleSelectedEndedProcessChanged();
};

#endif // SCHEDULER_DLG_H
