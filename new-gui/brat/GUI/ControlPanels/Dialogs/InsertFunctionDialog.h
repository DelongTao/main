#if !defined GUI_CONTROLPANELS_DIALOGS_INSERTFUNCTIONDIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_INSERTFUNCTIONDIALOG_H


#include <QDialog>

//class BBBB;


class CInsertFunctionDialog : public QDialog
{
#if defined (__APPLE__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
#endif

    Q_OBJECT;

#if defined (__APPLE__)
#pragma clang diagnostic pop
#endif

    /////////////////////////////
    //	types
    /////////////////////////////

    using base_t = QDialog;

    /////////////////////////////
    // static data
    /////////////////////////////

    /////////////////////////////
    // instance data
    /////////////////////////////

    /////////////////////////////
    //construction / destructuion
    /////////////////////////////

public:
    CInsertFunctionDialog( QWidget *parent );

    virtual ~CInsertFunctionDialog();

    /////////////////////////////
    // getters / setters / testers
    /////////////////////////////

    /////////////////////////////
    // remaining methods
    /////////////////////////////

};


#endif      // GUI_CONTROLPANELS_DIALOGS_INSERTFUNCTIONDIALOG_H