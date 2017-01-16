/*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#if !defined GUI_CONTROLPANELS_DIALOGS_DATASET_INTERPOLATION_DIALOG_H
#define GUI_CONTROLPANELS_DIALOGS_DATASET_INTERPOLATION_DIALOG_H


#include <QDialog>

class CTextWidget;


class CDatasetInterpolationDialog : public QDialog
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

	CTextWidget *mHelpText = nullptr;
    QDialogButtonBox *mButtonBox = nullptr;
	QDateTimeEdit *mDateTimeEdit = nullptr;
	QListWidget *mFieldsList = nullptr;

	//...domain data

	const std::vector< std::string > &mFieldMNamesList;
	std::string mDataModeDITimeName;
	QDateTime mDataModeDIDateTime;


    /////////////////////////////
    //construction / destruction
    /////////////////////////////

	void Setup();
	void CreateWidgets();
public:
    CDatasetInterpolationDialog( const std::vector< std::string > &list, const std::string &name, const QDateTime &dt, QWidget *parent );

    virtual ~CDatasetInterpolationDialog();


    ///////////////////////////////
    // getters / setters / testers
    ///////////////////////////////

	const std::string& DataModeDITimeName() const { return mDataModeDITimeName; }

	const QDateTime& DataModeDIDateTime() const { return mDataModeDIDateTime; }


    /////////////////////////////
    // remaining methods
    /////////////////////////////

protected:
	
	virtual void  accept() override;


protected slots:

};


#endif      // GUI_CONTROLPANELS_DIALOGS_DATASET_INTERPOLATION_DIALOG_H