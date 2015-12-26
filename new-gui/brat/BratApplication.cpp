#include "stdafx.h"

#include "new-gui/Common/QtUtils.h"

#include "BratApplication.h"



CBratApplication::CBratApplication(int &argc, char **argv, bool GUIenabled, QString customConfigPath ) :	//customConfigPath = QString()
	base_t( argc, argv, GUIenabled, customConfigPath )
{
	// To be sure that number have always a decimal point (and not a comma
	// or something else)
	setlocale( LC_NUMERIC, "C" );

	/*
	wxFileName traceFileName;
	traceFileName.AssignDir(wxGetApp().GetExecPathName());
	traceFileName.SetFullName("BratDisplay.log");
	traceFileName.Normalize();
	CTrace::GetInstance(traceFileName.GetFullPath().c_str());
	*/

	if ( getenv( BRATHL_ENVVAR ) == NULL )
	{
		// Note that this won't work on Mac OS X when you use './BratDisplay' from within the Contents/MacOS directory of
		// you .app bundle. The problem is that in that case Mac OS X will change the current working directory to the
		// location of the .app bundle and thus the calculation of absolute paths will break
        CTools::SetDataDirForExecutable( ApplicationDirectories::instance().mExecutableFileName.c_str() /*this->argv()[ 0 ]*/ );
	}

	if ( !CTools::DirectoryExists( CTools::GetDataDir() ) )
	{
		std::cerr << "WARNING: " << CTools::GetDataDir() << " is not a valid directory" << std::endl;
		SimpleWarnBox( QString( "WARNING: " ) + CTools::GetDataDir().c_str() + " is not a valid directory\n\nAre you sure your " + BRATHL_ENVVAR + " environment variable is set correctly?");
		throw false;
	}

	std::string errorMsg;
	if ( !CTools::LoadAndCheckUdUnitsSystem( errorMsg ) )
	{
		std::cerr << errorMsg << std::endl;
		SimpleErrorBox( errorMsg.c_str() );
		throw false;
	}

	//vtkOutputWindow::GetInstance()->PromptUserOn();

	//femmTODO
	//
	m_config = new QSettings( QSettings::IniFormat, QSettings::UserScope, "GetOrgName()", applicationName() );

	/*IN CmdLineProcessor - begin
	try
	{
		if ( !GetCommandLineOptions( this->argc(), this->argv() ) )
			throw false;
		
		//if (CheckCommandLineOptions(CBratDisplayApp::argc, CBratDisplayApp::argv,
		//"BratDisplay - An application to display BRAT netCDF data",
		//keywordList,
		//m_paramFile))
		//{
		//return false;
		//}

		GetParameters();

		//Prepare();

#if !defined(WIN32) && !defined(_WIN32)

		//Progess dialogs displayed while plotting need this
		wxEventLoop *tmpEventLoop = new wxEventLoop();
		wxEventLoopBase::SetActive( tmpEventLoop );
#endif

		if ( m_isZFLatLon )
		{
			WorldPlot();
		}
		else if ( m_isYFX )
		{
			XYPlot();
		}
		else if ( m_isZFXY )
		{
			ZFXYPlot();
		}
		else
		{
			CException e( "CBratDisplayApp::OnInit - Only World Plot Data, XY Plot Data and ZFXY Plot Data are implemented", BRATHL_UNIMPLEMENT_ERROR );
			CTrace::Tracer( "%s", e.what() );
			throw ( e );
		}


#if !defined(WIN32) && !defined(_WIN32)

		wxEventLoopBase::SetActive( NULL );
		delete tmpEventLoop;
#endif

		m_internalData.RemoveAll();


		//for ( itData = m_internalData.begin(); itData != m_internalData.end(); itData++ )
		//{
		//CInternalFilesZFXY* zfxy = dynamic_cast<CInternalFilesZFXY*>(*it);
		//if (zfxy == NULL)
		//{
		//CException e("CBratDisplayApp::OnInit - Non-geographical data are not yet implemented", BRATHL_ERROR);
		//CTrace::Tracer("%s", e.what());
		//throw (e);

		//}
		//if (zfxy->IsGeographic() == false)
		//{
		//CException e("CBratDisplayApp::OnInit - Non-geographical data are not yet implemented", BRATHL_ERROR);
		//CTrace::Tracer("%s", e.what());
		//throw (e);

		//}

		//CreateWPlot(zfxy);
		//}


			//wxSize size;
			//wxPoint pos;
			//CWindowHandler::GetSizeAndPosition(size, pos);

			////CBratDisplayFrame *frame = new CBratDisplayFrame( NULL, -1, wxT("SuperApp"), wxPoint(20,20), wxSize(500,340) );
			//CWorldPlotFrame* frame = new CWorldPlotFrame( NULL, -1, wxT("BRAT World Plot"), pos, size);


			////--- ????????
			//m_dataForTestOnly = NULL;
			//m_dataForTestOnly = new CGeoMap(frame);
			////m_dataForTestOnly->m_plotProperty.m_projection = CMapProjection::GetInstance()->IdToName(VTK_PROJ2D_MOLLWEIDE);
			//frame->AddData(m_dataForTestOnly);
			////--- ????????


			//frame->SetCenterLongitude(m_centerLongitude);
			//frame->SetCenterLatitude(m_centerLatitude);
			//wxPostEvent(frame->GetPlotPlanel()->GetPlotPropertyTab(),
			//CCenterPointChangedEvent(wxEVT_CENTER_POINT_CHANGED,
			//m_centerLongitude,
			//m_centerLatitude));

			//frame->Raise();


			//frame->Show( TRUE );
	}
	catch ( CException &e )
	{
		SimpleErrorBox( e.what() );
		throw false;
	}
	catch ( std::exception &e )
	{
		SimpleErrorBox( e.what() );
		throw false;
	}
	catch ( ... )
	{
		SimpleErrorBox( "Unexpected error encountered" );
		throw false;
	}

	IN CmdLineProcessor - end */

#ifdef __WXMAC__
	// Make sure we become the foreground application
	// for when we have been spawned from another application (such as BratGui)
	MakeApplicationActive();
#endif

	//QgsDecorationGrid* mDecorationGrid = new QgsDecorationGrid( this );
	//addDecorationItem( mDecorationGrid );

    //setApplicationStyle( getAppOptions() );
}


//virtual 
CBratApplication::~CBratApplication()
{}




////////////////////////////////////////////////
//
////////////////////////////////////////////////

#include "moc_BratApplication.cpp"