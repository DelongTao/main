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
#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"

#include "brathl.h"
#include "display/BitSet32.h"

#include "Operation.h"
#include "gui/Display.h"
#include "Workspace.h"



const std::string CWorkspace::m_configName = "setup.ini";

const std::string CWorkspace::m_keyDelimiter = ".";


//-------------------------------------------------------------
//------------------- CWorkspace class --------------------
//-------------------------------------------------------------

//----------------------------------------
std::string CWorkspace::GetRootKey()
{
	std::string result;

	CStringArray array;
	std::string key = (const char *)m_key.c_str();
	array.ExtractStrings( key, m_keyDelimiter );

	if ( array.size() >= 1 )
		result = array[ 0 ].c_str();

	return result;
}
//----------------------------------------
bool CWorkspace::DeleteConfigFile()
{
	bool bOk = m_config->DeleteAll();

	InitConfig();

	return bOk;
}
//----------------------------------------
void CWorkspace::InitConfig()
{
	//m_configFileName.Assign( m_path/*.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR )*/ + "/" + CWorkspace::m_configName );
	m_configFileName = NormalizedPath( m_path + "/" + CWorkspace::m_configName );
	delete m_config;
	m_config = new CConfiguration( m_configFileName );
	//m_config = new CConfiguration( m_configFileName.GetFullPath().ToStdString() );
	//m_config = new CConfiguration( "", "", m_configFileName.GetFullPath().ToStdString(), "", wxCONFIG_USE_LOCAL_FILE );
}
//----------------------------------------
bool CWorkspace::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, bool flush )
{
	UNUSED( errorMsg );

	if ( m_config == nullptr )
		return true;

	bool bOk = DeleteConfigFile() && SaveCommonConfig();

	if ( flush )
		m_config->Flush();

	return bOk;
}
//----------------------------------------
bool CWorkspace::SaveCommonConfig( bool flush )
{
	bool bOk = true;

	if ( m_config == nullptr )
		return true;

	m_config->SetPath( "/" + GROUP_GENERAL );
	bOk &= m_config->Write( ENTRY_WKSNAME, m_name );
	m_config->SetPath( "/" + GROUP_GENERAL );
	bOk &= m_config->Write( ENTRY_WKSLEVEL, m_level );

	if ( flush )
		m_config->Flush();

	return bOk;
}
//----------------------------------------
bool CWorkspace::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	UNUSED( errorMsg );

	return LoadCommonConfig();
}
//----------------------------------------
bool CWorkspace::LoadCommonConfig()
{
	if ( m_config == nullptr )
		return false;

	m_config->SetPath( "/" + GROUP_GENERAL );
	m_name = "";

	return 
		m_config->Read( ENTRY_WKSNAME, &m_name ) &&
		m_config->Read( ENTRY_WKSLEVEL, &m_level );
}


//----------------------------------------
//bool CWorkspace::SetPath( const wxFileName& path, bool createPath )
//{
//	m_path.AssignDir( path.GetPath() );
//	m_path.Normalize();
//
//	if ( !m_path.IsDir() )
//		return false;
//
//	bool bOk = true;
//	if ( !m_path.DirExists() && createPath )
//	{
//		bOk = m_path.Mkdir( 0777, wxPATH_MKDIR_FULL );
//	}
//
//	InitConfig();
//
//	return bOk;
//}
//
//----------------------------------------
//bool CWorkspace::SetPath( const std::string& path, bool createPath )
//{
//	wxFileName pathTmp;
//	if ( path.empty() )
//		return false;
//
//	pathTmp.AssignDir( path );
//	//return SetPath( pathTmp, createPath );
//
//	m_path.AssignDir( pathTmp.GetPath() );
//	m_path.Normalize();
//
//	if ( !m_path.IsDir() )
//		return false;
//
//	bool bOk = true;
//	if ( !m_path.DirExists() && createPath )
//	{
//		bOk = m_path.Mkdir( 0777, wxPATH_MKDIR_FULL );
//	}
//
//	InitConfig();
//
//	return bOk;
//}

bool CWorkspace::SetPath( const std::string& path, bool createPath )
{
	if ( path.empty() )
		return false;

	m_path = NormalizedPath( path );

	//pathTmp.AssignDir( path );
	////return SetPath( pathTmp, createPath );

	//m_path.AssignDir( pathTmp.GetPath() );
	//m_path.Normalize();

	//if ( !IsDir( m_path ) )
	//	return false;

	bool bOk = IsDir( m_path ) || ( createPath && MakeDirectory( m_path ) );

	InitConfig();		//TODO: even on error???

	return bOk;
}
//----------------------------------------
bool CWorkspace::Rmdir()
{
	return DeleteDirectory( m_path );
	//RmdirRecurse();
	//return m_path.Rmdir();
}
//----------------------------------------
//bool CWorkspace::RmdirRecurse()
//{
//  CDirTraverserDeleteFile traverserDelFile;
//  CDirTraverserDeleteDir traverserDelDir;
//
//  //path name given to wxDir is locked until wxDir object is deleted
//  wxDir dir;
//
//  dir.Open(GetPathName());
//  dir.Traverse(traverserDelFile);
//
//  dir.Open(GetPathName());
//  dir.Traverse(traverserDelDir);
//  return (dir.HasFiles() == false) && (dir.HasSubDirs() == false);
//}
//----------------------------------------
void CWorkspace::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	fOut << "==> Dump a CWorkspace Object at " << this << std::endl;
	fOut << "m_name = " << m_name << std::endl;
	fOut << "m_path object at = " << &m_path << std::endl;
	fOut << "m_path = " << m_path << std::endl;
	fOut << "m_configFileName object at = " << &m_configFileName << std::endl;
	fOut << "m_configFileName = " << m_configFileName << std::endl;
	fOut << "m_config object at = " << m_config << std::endl;

	fOut << "==> END Dump a CWorkspace Object at " << this << std::endl;

	fOut << std::endl;
}


//-------------------------------------------------------------
//------------------- CWorkspaceDataset class --------------------
//-------------------------------------------------------------

//----------------------------------------
bool CWorkspaceDataset::Import( CWorkspace* wks, std::string &errorMsg )
{
	if ( wks == nullptr )
		return true;


	CWorkspaceDataset* wksToImport = dynamic_cast<CWorkspaceDataset*>( wks );
	if ( wksToImport == nullptr )
		return true;

	if ( m_importBitSet == nullptr || !m_importBitSet->m_bitSet.test( IMPORT_DATASET_INDEX ) )
		return true;

	for ( CObMap::const_iterator it = wksToImport->GetDatasets()->begin(); it != wksToImport->GetDatasets()->end(); it++ )
	{
		CDataset* datasetImport = dynamic_cast<CDataset*>( it->second );
		if ( datasetImport == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceDataset::Import\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDataset";

			//wxMessageBox( ,				"Error",				wxOK | wxCENTRE | wxICON_ERROR );

			return false;
		}

		CDataset* dataset = GetDataset( datasetImport->GetName() );
		if ( dataset != nullptr )
		{
			errorMsg +=
				"Dataset to import '"
				+ datasetImport->GetName()
				+ "':\nUnable to process, a dataset with the same name already exists\n"
				+ "Import cancelled";

			//wxMessageBox( 				"Warning",				wxOK | wxICON_EXCLAMATION );

			return false;
		}

		m_datasets.Insert( (const char *)datasetImport->GetName().c_str(), new CDataset( *datasetImport ) );

	}
	return true;
}

//----------------------------------------
// femm: Apparently not used...
bool CWorkspaceDataset::CheckFiles( std::string &errorMsg )
{
	bool bOk = true;
	CObMap::iterator it;

	for ( it = m_datasets.begin(); it != m_datasets.end(); it++ )
	{
		CDataset* dataset = dynamic_cast<CDataset*>( it->second );
		if ( dataset == nullptr )
		{
			errorMsg +=
				"ERROR in  CWorkspaceDataset::CheckFiles\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDataset";

			return false;
		}

		try
		{
			dataset->GetProductList()->CheckFiles();
		}
		catch ( CException& e )
		{
			bOk = false;
			errorMsg +=
				"Dataset '"
				+ dataset->GetName()
				+ "':\nUnable to process files. Please apply correction\n\nReason:\n"
				+ e.what()
				+ "\n";
		}
	}

	return bOk;
}
//----------------------------------------
bool CWorkspaceDataset::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, bool flush )
{
	if ( m_config == nullptr )
		return true;

	bool bOk = DeleteConfigFile() && SaveCommonConfig() && SaveConfigDataset( errorMsg );

	if ( flush )
		m_config->Flush();

	return bOk;
}
//----------------------------------------
bool CWorkspaceDataset::SaveConfigDataset( std::string &errorMsg )
{
	bool bOk = true;
	if ( m_config == nullptr )
		return true;

	std::string entry;
	int index = 0;
	for ( CObMap::iterator it = m_datasets.begin(); it != m_datasets.end(); it++ )
	{
		index++;
		CDataset* dataset = dynamic_cast<CDataset*>( it->second );
		if ( dataset == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceDataset::SaveConfigDataset\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDataset";
			return false;
		}

		m_config->SetPath( "/" + GROUP_DATASETS );

		bOk &= m_config->Write( ENTRY_DSNAME + n2s<std::string >( index ), dataset->GetName().c_str() );

		dataset->SaveConfig( m_config );
	}

	return bOk;
}

//----------------------------------------
bool CWorkspaceDataset::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	UNUSED( errorMsg );

	return m_config && LoadCommonConfig() && LoadConfigDataset( errorMsg );
}

//----------------------------------------
bool CWorkspaceDataset::LoadConfigDataset( std::string &errorMsg )
{
	if ( m_config == nullptr )
		return true;

	m_config->SetPath( "/" + GROUP_DATASETS );

	long maxEntries = m_config->GetNumberOfEntries();
	std::string entry;
	std::string valueString;
	long i = 0;

	while ( m_config->GetNextEntry( entry, i ) )
	{
		valueString = m_config->Read( entry );
		InsertDataset( valueString );
	}

	int index = 0;
	for ( CObMap::iterator it = m_datasets.begin(); it != m_datasets.end(); it++ )
	{
		index++;
		CDataset* dataset = dynamic_cast< CDataset* >( it->second );
		assert__( dataset != nullptr );
		//{
		//	wxMessageBox( "ERROR in  CWorkspaceDataset::LoadConfigDataset\ndynamic_cast<CDataset*>(it->second) returns nullptr pointer"
		//		"\nList seems to contain objects other than those of the class CDataset",
		//		"Error",
		//		wxOK | wxCENTRE | wxICON_ERROR );
		//	return false;
		//}

		dataset->LoadConfig( m_config );
		if ( m_ctrlDatasetFiles )
		{
			std::vector< std::string > v;
			if ( !dataset->CtrlFiles(v) )
			{
				std::string s = Vector2String( v, "\n" );
				errorMsg += "Dataset '" + m_name + "':\n contains file '" + s + "' that doesn't exist\n";
				return false;
			}
			else
			{
				try
				{
					//Just to initialize 'product class' and 'product type'
					dataset->GetProductList()->CheckFiles( true );
				}
				catch ( CException& e )
				{
					UNUSED( e );
				}
			}
		}
	}

	return true;
}
//----------------------------------------
bool CWorkspaceDataset::RenameDataset(CDataset* dataset, const std::string& newName)
{
  if ( str_cmp( dataset->GetName(), newName) )
    return true;

  bool bOk = m_datasets.RenameKey((const char *)dataset->GetName().c_str(), (const char *)newName.c_str());
  if (bOk == false)
  {
   return false;
  }

  dataset->SetName(newName);

  return true;
}

//----------------------------------------
bool CWorkspaceDataset::DeleteDataset( CDataset* dataset )
{
	return m_datasets.Erase( (const char *)dataset->GetName().c_str() );
}

//----------------------------------------
std::string CWorkspaceDataset::GetDatasetNewName()
{
  int32_t i = m_datasets.size();
  std::string key;

  do
  {
    key = WKS_DATASET_NAME + "_" + n2s< std::string >( i + 1 );
    i++;
  }
  while (m_datasets.Exists((const char *)key.c_str()) != nullptr);

  return key;
}
//----------------------------------------
void CWorkspaceDataset::Close()
{
	for ( CObMap::iterator it = m_datasets.begin(); it != m_datasets.end(); it++ )
	{
		CDataset* dataset = dynamic_cast<CDataset*>( it->second );
		if ( dataset != nullptr )
		{
			//dataset->DeleteProduct();
		}
	}
}
//----------------------------------------
CDataset* CWorkspaceDataset::GetDataset(const std::string& name)
{
  return dynamic_cast<CDataset*>(m_datasets.Exists((const char *)name.c_str()));
}
//----------------------------------------
void CWorkspaceDataset::GetDatasetNames( std::vector< std::string >& array ) const
{
	for ( CObMap::const_iterator it = m_datasets.begin(); it != m_datasets.end(); it++ )
	{
		//std::string value = it->first;
		array.push_back( it->first.c_str() );
	}
}
//----------------------------------------
bool CWorkspaceDataset::InsertDataset( const std::string& name )
{
    if ( m_datasets.Exists(name) )
    {
        return false;
    }

    m_datasets.Insert(name, new CDataset(name));
    return true;
}
//----------------------------------------
void CWorkspaceDataset::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CWorkspace::Dump(fOut);

  fOut << "==> Dump a CWorkspaceDataset Object at "<< this << std::endl;

  fOut << "==> END Dump a CWorkspaceDataset Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CWorkspaceFormula class --------------------
//-------------------------------------------------------------

//----------------------------------------
void CWorkspaceFormula::AmendFormulas(const CStringArray& keys, CProduct* product, const std::string& record)
{
  m_formulas.Amend(keys, product, record);
  //SaveConfigPredefinedFormula();
  SaveConfigFormula();

}

//----------------------------------------
void CWorkspaceFormula::SetFormulasToImport(const std::vector< std::string >& array)
{
  m_formulasToImport.clear();
  m_formulasToImport = array;
}

//----------------------------------------
bool CWorkspaceFormula::IsFormulaToImport( const std::string& name )
{
	const size_t n = m_formulasToImport.size();
	for ( size_t i = 0; i < n; i++ )
		if ( str_icmp( name, m_formulasToImport[ i ] ) )
			return true;

	return false;
}
//----------------------------------------
bool CWorkspaceFormula::Import( CWorkspace* wks, std::string &errorMsg )
{
	if ( wks == nullptr )
		return true;

	CWorkspaceFormula* wksToImport = dynamic_cast<CWorkspaceFormula*>( wks );
	if ( wksToImport == nullptr )
		return true;

	if ( m_importBitSet == nullptr )
		return true;

	if ( m_importBitSet->m_bitSet.test( IMPORT_FORMULA_INDEX ) == false )
		return true;

	size_t countImport = m_formulasToImport.size();

	for ( CObMap::iterator it = wksToImport->GetFormulas()->begin(); it != wksToImport->GetFormulas()->end(); it++ )
	{
		CFormula* formulaImport = dynamic_cast<CFormula*>( it->second );
		assert__( formulaImport != nullptr );
		//{
		//  wxMessageBox("ERROR in  CWorkspaceFormula::Import\ndynamic_cast<CFormula*>(it->second) returns nullptr pointer"
		//               "\nList seems to contain objects other than those of the class CFormula",
		//               "Error",
		//                wxOK | wxCENTRE | wxICON_ERROR);
		//  return false;
		//}
		if ( formulaImport->IsPredefined() )
			continue;

		std::string formulaNewName = formulaImport->GetName();

		// if import array is not empty : test if import or not
		// else import all formulas
		if ( countImport > 0 && !IsFormulaToImport( formulaNewName ) )
			continue;

		CFormula* formula = GetFormula( formulaImport->GetName() );
		if ( formula != nullptr )
		{
			CFormula* formulaTmp = nullptr;
			do
			{
				formulaNewName += "_from_";
				formulaNewName += wksToImport->GetRootKey();
				formulaTmp = GetFormula( formulaNewName );

			} while ( formulaTmp != nullptr );

			errorMsg +=
				"Import formula '"
				+ formulaImport->GetName()
				+ "':\n\nA formula with the same name already exists.\n\nImported formula has been renamed to:\n\n '"
				+ formulaNewName
				+ "'\n";
		}

		CFormula* newFormula = new CFormula( *formulaImport );
		newFormula->SetName( formulaNewName );

		m_formulas.Insert( (const char *)newFormula->GetName().c_str(), newFormula, false );
	}

	return true;
}

//----------------------------------------
bool CWorkspaceFormula::AddFormula( CFormula& formula, std::string &errorMsg )
{
	return m_formulas.InsertUserDefined_ReplacePredefinedNotAllowed( formula, errorMsg );
}
//----------------------------------------
void CWorkspaceFormula::RemoveFormula(const std::string& name)
{
  m_formulas.Erase((const char *)name.c_str());
}
//----------------------------------------
std::string CWorkspaceFormula::GetCommentFormula(const std::string& name)
{
  return m_formulas.GetCommentFormula(name);
}
//----------------------------------------
std::string CWorkspaceFormula::GetDescFormula(const std::string& name, bool alias)
{
  return m_formulas.GetDescFormula(name, alias);
}
//----------------------------------------
CFormula* CWorkspaceFormula::GetFormula(const std::string& name)
{
  return dynamic_cast<CFormula*>(m_formulas.Exists((const char *)name.c_str()));
}
//----------------------------------------
const CFormula* CWorkspaceFormula::GetFormula( CMapFormula::const_iterator it ) const
{
	return dynamic_cast<const CFormula*>( it->second );
}
CFormula* CWorkspaceFormula::GetFormula(CMapFormula::iterator it)
{
  return dynamic_cast<CFormula*>(it->second);
}
//----------------------------------------
void CWorkspaceFormula::GetFormulaNames(CStringMap& stringMap, bool getPredefined /* = true*/, bool getUser /* = true*/)
{
  CMapFormula::iterator it;
  stringMap.RemoveAll();

  for (it = m_formulas.begin() ; it != m_formulas.end() ; it++)
  {
    CFormula* formula = GetFormula(it);
    if (formula != nullptr)
    {
      if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
      {
        stringMap.Insert((const char *)formula->GetName().c_str(), (const char *)formula->GetDescription().c_str());
      }
    }
  }
}
//----------------------------------------
void CWorkspaceFormula::GetFormulaNames( std::vector< std::string >& array, bool getPredefined /* = true*/, bool getUser /* = true*/ )
{
	for ( CMapFormula::iterator it = m_formulas.begin(); it != m_formulas.end(); it++ )
	{
		CFormula* formula = GetFormula( it );
		if ( formula != nullptr )
		{
			if ( ( getPredefined && formula->IsPredefined() ) || ( getUser && !formula->IsPredefined() ) )
			{
				array.push_back( it->first );
			}
		}
	}
}

//----------------------------------------
bool CWorkspaceFormula::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, bool flush)
{
	UNUSED( errorMsg );

  bool bOk = true;

  if (m_config == nullptr)
  {
    return true;
  }

  bOk &= DeleteConfigFile();

  bOk &= SaveCommonConfig();
  bOk &= SaveConfigFormula();

  if (flush)
  {
    m_config->Flush();
  }

  return bOk;

}
//----------------------------------------
bool CWorkspaceFormula::SaveConfigFormula()
{
  bool bOk = true;
  if (m_config == nullptr)
  {
    return true;
  }

  m_formulas.SaveConfig(m_config, false);

  return bOk;
}
//----------------------------------------
bool CWorkspaceFormula::SaveConfigPredefinedFormula()
{
	wxFileName formulaPath;
	formulaPath.Assign( CTools::GetDataDir().c_str(), CMapFormula::m_predefFormulaFile );
	formulaPath.Normalize();

	//wxFileConfig* config = new wxFileConfig( wxEmptyString, wxEmptyString, formulaPath.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE );
	CConfiguration config( formulaPath.GetFullPath().ToStdString() );

	return config.DeleteAll() && m_formulas.SaveConfig( &config, true );
}
//----------------------------------------
bool CWorkspaceFormula::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	return m_config && LoadCommonConfig() && LoadConfigFormula( errorMsg );
}
//----------------------------------------
bool CWorkspaceFormula::LoadConfigFormula( std::string &errorMsg )
{
	return !m_config || m_formulas.InsertUserDefined( m_config, errorMsg );
}
//----------------------------------------
void CWorkspaceFormula::Dump(std::ostream& fOut /* = std::cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CWorkspace::Dump(fOut);

  fOut << "==> Dump a CWorkspaceFormula Object at "<< this << std::endl;

  fOut << "==> END Dump a CWorkspaceFormula Object at "<< this << std::endl;

  fOut << std::endl;

}

//-------------------------------------------------------------
//------------------- CWorkspaceOperation class --------------------
//-------------------------------------------------------------
//----------------------------------------
bool CWorkspaceOperation::Import( CWorkspace* wks, std::string &errorMsg )
{
	if ( wks == nullptr )
	{
		return true;
	}

	CWorkspaceOperation* wksToImport = dynamic_cast<CWorkspaceOperation*>( wks );
	if ( wksToImport == nullptr )
	{
		return true;
	}

	if ( m_importBitSet == nullptr )
	{
		return true;
	}

	if ( m_importBitSet->m_bitSet.test( IMPORT_OPERATION_INDEX ) == false )
	{
		return true;
	}


	CObMap::iterator it;
	for ( it = wksToImport->GetOperations()->begin(); it != wksToImport->GetOperations()->end(); it++ )
	{
		COperation* operationImport = dynamic_cast<COperation*>( it->second );
		if ( operationImport == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceOperation::Import\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class COperation\n";

			return false;
		}

		COperation* operation = GetOperation( operationImport->GetName() );
		if ( operation != nullptr )
		{
			errorMsg += 
				"Operation to import '"
				+ operationImport->GetName()
				+ "':\nUnable to process, an operation with the same name already exists\n"
				+ "Import cancelled\n";

			return false;
		}

		m_operations.Insert( operationImport->GetName(), new COperation( *operationImport ) );
	}

	return true;
}

//----------------------------------------
bool CWorkspaceOperation::RenameOperation( COperation* operation, const std::string& newName )
{
	if ( str_cmp( operation->GetName(), newName ) )
		return true;

	if ( !m_operations.RenameKey( operation->GetName(), newName ) )
		return false;

	operation->SetName( newName );

	return true;
}
//----------------------------------------
bool CWorkspaceOperation::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, bool flush )		//flush = true
{
	if ( m_config == nullptr )
		return true;

	bool bOk = DeleteConfigFile() && SaveCommonConfig() && SaveConfigOperation( errorMsg, wkso );

	if ( flush )
		m_config->Flush();

	return bOk;
}

//----------------------------------------
bool CWorkspaceOperation::SaveConfigOperation( std::string &errorMsg, CWorkspaceOperation *wkso )
{
	bool bOk = true;
	if ( m_config == nullptr )
	{
		return true;
	}

	std::string entry;

	CObMap::iterator it;

	int32_t index = 0;

	for ( it = m_operations.begin(); it != m_operations.end(); it++ )
	{
		index++;
		COperation* operation = dynamic_cast<COperation*>( it->second );
		if ( operation == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceOperation::SaveConfigOperation\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class COperation\n";

			return false;
		}

		m_config->SetPath( "/" + GROUP_OPERATIONS );

		bOk &= m_config->Write( ENTRY_OPNAME + n2s< std::string >( index ), operation->GetName() );

		operation->SaveConfig( m_config, wkso );
	}

	return bOk;
}

//----------------------------------------
bool CWorkspaceOperation::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
  return !m_config || ( LoadCommonConfig() && LoadConfigOperation( errorMsg, wks, wkso ) );
}

//----------------------------------------
bool CWorkspaceOperation::LoadConfigOperation( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	bool bOk = true;
	if ( m_config == nullptr )
		return true;

	m_config->SetPath( "/" + GROUP_OPERATIONS );

	long maxEntries = m_config->GetNumberOfEntries();
	std::string entry;
	std::string valueString;
	long i = 0;

	do
	{
		bOk = m_config->GetNextEntry( entry, i );
		if ( bOk )
		{
			valueString = m_config->Read( entry );
			InsertOperation( valueString );
		}
	} while ( bOk );

	CObMap::iterator it;

	int32_t index = 0;

	for ( it = m_operations.begin(); it != m_operations.end(); it++ )
	{
		index++;
		COperation* operation = dynamic_cast<COperation*>( it->second );
		if ( operation == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceOperation::LoadConfigOperation\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class COperation";

			return false;
		}

		operation->LoadConfig( m_config, errorMsg, wks, wkso );
	}

	return true;
}
//----------------------------------------
std::string CWorkspaceOperation::GetOperationNewName()
{
  int32_t i = m_operations.size();
  std::string key;

  do
  {
    key = WKS_OPERATION_NAME + "_" + n2s< std::string >( i + 1 );
    i++;
  }
  while (m_operations.Exists((const char *)key.c_str()) != nullptr);

  return key;
}

COperation* CWorkspaceOperation::GetOperation( const std::string& name )
{
	return dynamic_cast<COperation*>( m_operations.Exists( name ) );
}
//----------------------------------------
std::string CWorkspaceOperation::GetOperationCopyName( const std::string& baseName )
{
	int32_t i = 0;
	std::string key;
	do
	{
		if ( i == 0 )
		{
			key = baseName + "_copy";
		}
		else
		{
			key = baseName + "_copy" + n2s<std::string>( i );
		}
		
		i++;

	} while ( m_operations.Exists( key ) != nullptr );

	return key;
}

//----------------------------------------
bool CWorkspaceOperation::InsertOperation(const std::string &name)
{
    if (m_operations.Exists(name))
    {
        return false;
    }

    m_operations.Insert(name, new COperation(name));
    return true;
}
//----------------------------------------
bool CWorkspaceOperation::InsertOperation(const std::string &name, COperation* operationToCopy, CWorkspaceOperation *wkso )
{
    if (m_operations.Exists(name))
    {
        return false;
    }

    // Create and copy the operation
    COperation* newOperation = new COperation(*operationToCopy);

    // Set the correct names
    newOperation->SetName(name);
    newOperation->InitOutput( wkso );
    newOperation->InitExportAsciiOutput( wkso );

    m_operations.Insert(name, newOperation);
    return true;
}
//----------------------------------------
void CWorkspaceOperation::GetOperationNames( std::vector< std::string >& array )
{
	for ( CObMap::iterator it = m_operations.begin(); it != m_operations.end(); it++ )
		array.push_back( it->first );
}
//----------------------------------------
bool CWorkspaceOperation::UseDataset( const std::string& name, std::string &errorMsg, CStringArray* operationNames /*= nullptr*/ )
{
	CObMap::iterator it;
	bool useDataset = false;
	for ( it = m_operations.begin(); it != m_operations.end(); it++ )
	{
		COperation* operation = dynamic_cast<COperation*>( it->second );
		if ( operation == nullptr )
		{
			errorMsg += "ERROR in  CWorkspaceOperation::UseDataset\ndynamic_cast<COperation*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class COperation\n";

			return false;
		}

		bool useIt = operation->UseDataset( name );
		useDataset |= useIt;

		if ( operationNames != nullptr )
		{
			if ( useIt )
			{
				operationNames->Insert( (const char *)operation->GetName().c_str() );
			}

		}
		else
		{
			if ( useDataset )
			{
				break;
			}
		}
	}

	return useDataset;
}
//----------------------------------------
bool CWorkspaceOperation::DeleteOperation(COperation* operation)
{
 bool bOk = m_operations.Erase((const char *)operation->GetName().c_str());

 return bOk;
}

//----------------------------------------
void CWorkspaceOperation::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	CWorkspace::Dump( fOut );

	fOut << "==> Dump a CWorkspaceOperation Object at " << this << std::endl;

	fOut << "==> END Dump a CWorkspaceOperation Object at " << this << std::endl;

	fOut << std::endl;
}

//-------------------------------------------------------------
//------------------- CWorkspaceDisplay class --------------------
//-------------------------------------------------------------

bool CWorkspaceDisplay::Import( CWorkspace* wks, std::string &errorMsg )
{
	if ( wks == nullptr )
	{
		return true;
	}

	CWorkspaceDisplay* wksToImport = dynamic_cast<CWorkspaceDisplay*>( wks );
	if ( wksToImport == nullptr )
	{
		return true;
	}

	if ( m_importBitSet == nullptr )
	{
		return true;
	}

	if ( m_importBitSet->m_bitSet.test( IMPORT_DISPLAY_INDEX ) == false )
	{
		return true;
	}

	for ( CObMap::iterator it = wksToImport->GetDisplays()->begin(); it != wksToImport->GetDisplays()->end(); it++ )
	{
		CDisplay* displayImport = dynamic_cast<CDisplay*>( it->second );
		if ( displayImport == nullptr )
		{
			errorMsg += 
				"ERROR in  CWorkspaceDisplay::Import\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay";

			return false;
		}

		CDisplay* display = GetDisplay( displayImport->GetName().ToStdString() );
		if ( display != nullptr )
		{
			errorMsg += 
				"Operation to import '"
				+ displayImport->GetName()
				+ "':\nUnable to process, an operation with the same name already exists\n"
				+ "Import canceled\n";

			return false;
		}

		m_displays.Insert( (const char *)displayImport->GetName().c_str(), new CDisplay( *displayImport ) );
	}

	return true;
}
//----------------------------------------
bool CWorkspaceDisplay::UseOperation( const std::string& name, std::string &errorMsg, CStringArray* displayNames /*= nullptr*/ )
{
	CObMap::iterator it;
	bool useOperation = false;

	for ( it = m_displays.begin(); it != m_displays.end(); it++ )
	{
		CDisplay* display = dynamic_cast<CDisplay*>( it->second );
		if ( display == nullptr )
		{
			errorMsg += 
				"ERROR in  CWorkspaceDisplay::UseOperation\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay\n";

			return false;
		}

		bool useIt = display->UseOperation( name );
		useOperation |= useIt;

		if ( displayNames != nullptr )
		{
			if ( useIt )
			{
				displayNames->Insert( (const char *)display->GetName().c_str() );
			}
		}
		else
		{
			if ( useOperation )
			{
				break;
			}
		}
	}

	return useOperation;
}
//----------------------------------------
bool CWorkspaceDisplay::SaveConfig( std::string &errorMsg, CWorkspaceOperation *wkso, bool flush )
{
	if ( m_config == nullptr )
		return true;

	bool bOk = DeleteConfigFile() && SaveCommonConfig() && SaveConfigDisplay( errorMsg );

	if ( flush )
		m_config->Flush();

	return bOk;
}
//----------------------------------------
bool CWorkspaceDisplay::SaveConfigDisplay( std::string &errorMsg )
{
	bool bOk = true;
	if ( m_config == nullptr )
	{
		return true;
	}

	std::string entry;

	CObMap::iterator it;

	int32_t index = 0;

	for ( it = m_displays.begin(); it != m_displays.end(); it++ )
	{
		index++;
		CDisplay* display = dynamic_cast<CDisplay*>( it->second );
		if ( display == nullptr )
		{
			errorMsg += 
				"ERROR in  CWorkspaceDisplay::m_displays\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay";

			return false;
		}

		m_config->SetPath( "/" + GROUP_DISPLAY );

		bOk &= m_config->Write( ENTRY_DISPLAYNAME + n2s< std::string >( index ), display->GetName().ToStdString() );

		display->SaveConfig( m_config );
	}

	return bOk;
}

//----------------------------------------
bool CWorkspaceDisplay::LoadConfig( std::string &errorMsg, CWorkspaceDataset *wks, CWorkspaceOperation *wkso )
{
	return !m_config || ( LoadCommonConfig() && LoadConfigDisplay( errorMsg ) );
}
//----------------------------------------
bool CWorkspaceDisplay::LoadConfigDisplay( std::string &errorMsg )
{
	bool bOk = true;
	if ( m_config == nullptr )
	{
		return true;
	}

	m_config->SetPath( "/" + GROUP_DISPLAY );

	long maxEntries = m_config->GetNumberOfEntries();
	std::string entry;
	std::string valueString;
	long i = 0;
	do
	{
		bOk = m_config->GetNextEntry( entry, i );
		if ( bOk )
		{
			valueString = m_config->Read( entry );
			InsertDisplay( valueString );
		}
	} while ( bOk );

	CObMap::iterator it;

	int32_t index = 0;

	for ( it = m_displays.begin(); it != m_displays.end(); it++ )
	{
		index++;
		CDisplay* display = dynamic_cast<CDisplay*>( it->second );
		if ( display == nullptr )
		{
			errorMsg +=
				"ERROR in  CWorkspaceDisplay::LoadConfigOperation\ndynamic_cast<CDisplay*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CDisplay\n";

			return false;
		}

		display->LoadConfig( m_config );
	}

	return true;
}
//----------------------------------------
bool CWorkspaceDisplay::RenameDisplay(CDisplay* display, const std::string& newName)
{
  if (display->GetName().Cmp(newName) == 0)
  {
    return true;
  }

  bool bOk = m_displays.RenameKey((const char *)display->GetName().c_str(), (const char *)newName.c_str());
  if (bOk == false)
  {
   return false;
  }

  display->SetName(newName);

  return true;
}



CDisplay* CWorkspaceDisplay::GetDisplay( const std::string& name )
{
	return dynamic_cast<CDisplay*>( m_displays.Exists( name ) );
}


//----------------------------------------
std::string CWorkspaceDisplay::GetDisplayNewName()
{
  int32_t i = m_displays.size();
  std::string key;

  do
  {
    key = WKS_DISPLAY_NAME +"_" + n2s< std::string >( i + 1 );
    i++;
  }
  while (m_displays.Exists((const char *)key.c_str()) != nullptr);

  return key;
}
//----------------------------------------
bool CWorkspaceDisplay::InsertDisplay(const std::string &name)
{
    if (m_displays.Exists(name))
    {
        return false;
    }

    m_displays.Insert(name, new CDisplay(name));
    return true;
}
//----------------------------------------
void CWorkspaceDisplay::GetDisplayNames( std::vector< std::string >& array )
{
	for ( CObMap::iterator it = m_displays.begin(); it != m_displays.end(); it++ )
		array.push_back( it->first );
}
//----------------------------------------
bool CWorkspaceDisplay::DeleteDisplay( CDisplay* display )
{
	return m_displays.Erase( (const char *)display->GetName().c_str() );
}


//----------------------------------------
void CWorkspaceDisplay::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;

	CWorkspace::Dump( fOut );

	fOut << "==> Dump a CWorkspaceDisplay Object at " << this << std::endl;

	fOut << "==> END Dump a CWorkspaceDisplay Object at " << this << std::endl;

	fOut << std::endl;
}
