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

#include "brathl.h"

#include "new-gui/Common/tools/Trace.h"
#include "new-gui/Common/tools/Exception.h"
#include "Tools.h"

using namespace brathl;

#include "BratProcess.h"

using namespace processes;

#include "Workspace.h"
#include "Constants.h"
#include "Formula.h"





static const std::string DEFAULT_UNIT_COUNT = "count";

static const std::string DEFAULT_STEP_LATLON_ASSTRING = "1/3";
static const std::string DEFAULT_STEP_TIME_ASSTRING = "1"; 
static const std::string DEFAULT_STEP_GENERAL_ASSTRING = "1";



//-------------------------------------------------------------
//------------------- CFormula class --------------------
//-------------------------------------------------------------

//----------------------------------------
void CFormula::SetDefaultMinMax()
{
	setDefaultValue( m_minValue );
	setDefaultValue( m_maxValue );

	switch ( m_dataType )
	{
		case CMapTypeData::eTypeOpLongitude:
		{
			SetMinValue( ConvertToFormulaUnit( -180 ) );
			SetMaxValue( ConvertToFormulaUnit( 180 ) );
			break;
		}
		case CMapTypeData::eTypeOpLatitude:
		{
			SetMinValue( ConvertToFormulaUnit( -90 ) );
			SetMaxValue( ConvertToFormulaUnit( 90 ) );
			break;
		}
		default:
		{
			break;
		}
	}
}

//----------------------------------------
std::string CFormula::GetMinValueAsText()
{
	return GetValueAsString( m_minValue );
}
//----------------------------------------
std::string CFormula::GetMaxValueAsText()
{
	return GetValueAsString( m_maxValue );
}
//----------------------------------------
std::string CFormula::GetFormatString()
{
	static const char *FMT_FLOAT_XY = "%-#.10g";
	static const char *FMT_FLOAT_LATLON = "%-#.15g";

	std::string format;

	if ( IsLatLonDataType() )
	{
		CUnit unit( m_unit.BaseUnit() );
		if ( unit == m_unit )
		{
			format = FMT_FLOAT_LATLON;
		}
		else
		{
			format = FMT_FLOAT_XY;
		}

	}
	else if ( IsTimeDataType() )
	{
		; // "" as default format for date time
	}
	else
	{
		format = FMT_FLOAT_XY;
	}

	return format;
}
//----------------------------------------
std::string CFormula::GetValueAsString( double value )
{
	auto SprintfFormat = []( const std::string &format, double value )
	{
		char buffer[ 128 ];

		sprintf( buffer, format.c_str(), value );

		return std::string( buffer );
	};

	///

	if ( IsTimeDataType() )
		return GetAsDateString( value );

	return CTools::TrailingZeroesTrim( SprintfFormat( GetFormatString(), value ) );
}
//----------------------------------------
void CFormula::SetDataType(int32_t typeField, const CUnit& unit, CProduct* product)
{

  switch (typeField)
  {
  case CMapTypeField::eTypeOpAsX :
    {
      if (unit.IsDate())
      {
        SetDataType(CMapTypeData::eTypeOpT);
      }
      else if (unit.IsCompatible(CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE))
      {
        SetDataType(CMapTypeData::eTypeOpLongitude);

        if (product != nullptr)
        {
          if ( (product->IsLatitudeFieldName((const char *)m_name.c_str())) || (CTools::FindNoCase((const char *)m_name.c_str(), "lat") == 0) )
          {
            SetDataType(CMapTypeData::eTypeOpLatitude);
          }
        }
      }
      else
      {
        SetDataType(CMapTypeData::eTypeOpX);
      }

      break;
    }
  case CMapTypeField::eTypeOpAsY :
    {
      if (unit.IsDate())
      {
        SetDataType(CMapTypeData::eTypeOpT);
      }
      else if (unit.IsCompatible(CLatLonPoint::m_DEFAULT_UNIT_LATITUDE))
      {
        SetDataType(CMapTypeData::eTypeOpLatitude);
        if (product != nullptr)
        {
          if ( product->IsLongitudeFieldName( m_name ) || ( CTools::FindNoCase(m_name, "lon") == 0) )
          {
            SetDataType(CMapTypeData::eTypeOpLongitude);
          }
        }

      }
      else
      {
        SetDataType(CMapTypeData::eTypeOpY);
      }
      break;
    }
  case CMapTypeField::eTypeOpAsField :
      SetDataType(CMapTypeData::eTypeOpData);
      break;
  case CMapTypeField::eTypeOpAsSelect :
      SetDataType(CMapTypeData::eTypeOpData);
      break;
  default :
      SetDataType(CMapTypeData::eTypeOpData);
      break;
  }

}

//----------------------------------------
std::string CFormula::GetFilterAsLabel()
{
	auto FORMAT_FILTER_LABEL_NONE = []( const std::string &s )
	{
		return "No filter to apply on '" + s + "'";
	};

	auto FORMAT_FILTER_LABEL = []( const std::string &s1, const std::string &s2 )
	{
		return "Filter to apply on '" + s1 + "': " + s2;
	};


	if ( m_filter == CMapTypeFilter::eFilterNone )
	{
		return FORMAT_FILTER_LABEL_NONE( GetName() );
	}
	else
	{
		return FORMAT_FILTER_LABEL( GetName(), GetFilterAsString() );
	}
}

//----------------------------------------
std::string CFormula::GetResolutionAsLabel( bool hasFilter )
{
	auto FORMAT_LOESS_CUT_OFF_LABEL = []( const std::string &s )
	{
		return "Loess cut-off: " + s;
	};

	auto FORMAT_INTERVAL_LABEL = []( const std::string &s )
	{
		return "Number of intervals: " + s;
	};

	auto FORMAT_XY_RESOLUTION_LABEL = []( const std::string &s1, const std::string &s2, const std::string &s3, const std::string &s4, const std::string &s5 )
	{
		return "Min.: " + s1 + "   Max.: " + s2 + "   " + s3 + "   Step: " + s4 + "   " + s5;
	};


	std::string loessCutOffLabel;
	std::string intervalLabel;

	if ( hasFilter )
	{
		if ( isDefaultValue( m_loessCutOff ) )
		{
			loessCutOffLabel = FORMAT_LOESS_CUT_OFF_LABEL( "Not defined" );
		}
		else
		{
			loessCutOffLabel = FORMAT_LOESS_CUT_OFF_LABEL( GetLoessCutOffAsText() );
		}
	}

	if ( isDefaultValue( m_interval ) )
	{
		intervalLabel = FORMAT_INTERVAL_LABEL( "Not defined" );
	}
	else
	{
		intervalLabel = FORMAT_INTERVAL_LABEL( GetIntervalAsText() );
	}

	return FORMAT_XY_RESOLUTION_LABEL
		( 
		isDefaultValue( m_minValue ) ? "Not defined" : GetMinAsString(),
		isDefaultValue( m_maxValue ) ? "Not defined" : GetMaxAsString(),
		loessCutOffLabel,
		GetStep(),
		intervalLabel 
		);
}

//----------------------------------------
std::string CFormula::GetMinAsString()
{
  return GetMinValueAsText();
}

//----------------------------------------
std::string CFormula::GetMaxAsString()
{
  return GetMaxValueAsText();
}


//----------------------------------------
double CFormula::GetStepAsDouble( std::string &errorMsg )
{
	CExpression expression;
	CExpressionValue exprValue;
	double result = 0.0;

	SetStepToDefaultAsNecessary();

	bool bOk = CFormula::SetExpression( m_step, expression, errorMsg );
	if ( !bOk )
	{
		//wxMessageBox(errorMsg,		//             "Warning",		//              wxOK | wxCENTRE | wxICON_EXCLAMATION);
		SetStepToDefault();
		SetExpression( m_step, expression, errorMsg );
	}

	if ( expression.GetFieldNames()->size() != 0 )
	{
		//errorMsg = "Fieldnames are not allowed for step in formula";		//	wxMessageBox( errorMsg,		//	"Warning",		//	wxOK | wxCENTRE | wxICON_EXCLAMATION );
		SetStepToDefault();
		SetExpression( m_step, expression, errorMsg );
	}

	try
	{
		exprValue = expression.Execute();
		result = *( exprValue.GetValues() );
	}
	catch ( CException& e )
	{
		errorMsg += 		
			"Formula '"
			+ m_name
			+ "':\nSyntax for step ('"
			+ m_step
			+ "') is not valid\nReason:\n"
			+ e.what()
			+ "\n";

		//wxMessageBox( errorMsg, "Warning", wxOK | wxICON_EXCLAMATION );

		result = CTools::m_defaultValueDOUBLE;
	}
	catch ( std::exception& e )
	{
		errorMsg = 		
			"Formula '"
			+ m_name
			+ "':\nSyntax for step ('"
			+ m_step
			+ "') is not valid\nReason:\n"
			+ e.what(),

		//wxMessageBox( errorMsg, "Warning", wxOK | wxICON_EXCLAMATION );

		result = CTools::m_defaultValueDOUBLE;
	}
	catch ( ... )
	{
		errorMsg = 		
			"Formula '"
			+ m_name
			+ "':\nSyntax for step ('"
			+ m_step
			+ "') is not valid\n",

		//wxMessageBox( errorMsg, 		//	"Warning",		//	wxOK | wxICON_EXCLAMATION );

		result = CTools::m_defaultValueDOUBLE;
	}

	return result;
}
//----------------------------------------
double CFormula::GetStepAsDouble( const std::string& step, std::string errorMsg )
{
	CExpression expression;
	CExpressionValue exprValue;
	double result = 0.0;
	if ( step.empty() )
	{
		return CTools::m_defaultValueDOUBLE;
	}

	bool bOk = CFormula::SetExpression( step, expression, errorMsg );

	if ( !bOk )
	{
		//wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_EXCLAMATION );

		SetExpression( DEFAULT_STEP_GENERAL_ASSTRING, expression, errorMsg );
	}

	if ( expression.GetFieldNames()->size() != 0 )
	{
		errorMsg = "Fieldnames are not allowed for step in formula";

		//wxMessageBox( errorMsg, "Warning", wxOK | wxCENTRE | wxICON_EXCLAMATION );

		SetExpression( DEFAULT_STEP_GENERAL_ASSTRING, expression, errorMsg );
	}

	try
	{

		exprValue = expression.Execute();
		result = *( exprValue.GetValues() );
	}
	catch ( CException& e )
	{
		errorMsg = 
			"Syntax for step ('"
			+ step
			+ "') is not valid\nReason:\n"
			+ e.what(),

		//wxMessageBox( errorMsg,			"Warning",			wxOK | wxICON_EXCLAMATION );

		result = CTools::m_defaultValueDOUBLE;
	}
	catch ( std::exception& e )
	{
		errorMsg = 
			"Syntax for step ('"
			+ step
			+ "') is not valid\nReason:\n"
			+ e.what(),

		//wxMessageBox( errorMsg, "Warning", wxOK | wxICON_EXCLAMATION );

		result = CTools::m_defaultValueDOUBLE;
	}
	catch ( ... )
	{
		errorMsg = 
			"Syntax for step ('"
			+ step
			+ "') is not valid\n",

		//wxMessageBox( errorMsg, "Warning", wxOK | wxICON_EXCLAMATION );

		result = CTools::m_defaultValueDOUBLE;
	}

	return result;
}

//----------------------------------------
std::string CFormula::GetDefaultUnit()
{
  return GetDefaultUnit(m_dataType);
}
//----------------------------------------
std::string CFormula::GetDefaultUnit(int32_t dataType)
{
  CUnit unit;

  switch (dataType)
  {
    case CMapTypeData::eTypeOpData:
      //unit = DEFAULT_UNIT_COUNT.c_str();
      break;
    case CMapTypeData::eTypeOpLongitude:
      unit = CLatLonPoint::m_DEFAULT_UNIT_LONGITUDE;
      break;
    case CMapTypeData::eTypeOpLatitude:
      unit = CLatLonPoint::m_DEFAULT_UNIT_LATITUDE;
      break;
    case CMapTypeData::eTypeOpT:
      unit = CDate::m_DEFAULT_UNIT_SECOND;
      break;
    default:
      break;
  }

  return unit.GetText().c_str();

}

//----------------------------------------
bool CFormula::CheckExpressionUnits(const std::string& exprStr, const std::string& record, const std::string& strUnitExpr, CProduct* product, std::string& errorMsg)
{
  bool bOk = true;

  if (product == nullptr)
  {
    return bOk;
  }


  CExpression expr;
  bOk &= CFormula::SetExpression(exprStr, expr, errorMsg);

  if (!bOk)
  {
    return bOk;
  }

  if (!expr.HasFieldNames())
  {
    return bOk;
  }

  if (expr.HasConstants())
  {
    return bOk;
  }
  if (expr.GetNbFieldNames() > 1)
  {
    return bOk;
  }
  //Expression contains only one field

  std::string fieldName = expr.GetFieldNames()->at(0).c_str();

  CField* field = product->FindFieldByName((const char *)fieldName.c_str(), (const char *)record.c_str(), false, nullptr, true);

  if (field == nullptr)
  {
    return bOk;
  }

  CUnit unitExpr;

  try
  {
    unitExpr = (const char *)strUnitExpr.c_str();
  }
  catch (CException& e)
  {
    errorMsg += ( std::string("The unit of the expression ('") + strUnitExpr + "') is not valid.\nReason\n'" + e.what() + "'" );
    return false;
  }

  if (CFormula::IsDefaultUnitData(unitExpr))
  {
    return bOk;
  }

  std::string msgErrorUnits;
  if (! unitExpr.IsCompatible(field->GetUnit(), &msgErrorUnits) )
  {
    if (!msgErrorUnits.empty())
    {
      errorMsg += msgErrorUnits;
    }
    else
    {
      errorMsg += (
		  std::string("The unit of the field '") 
		  + fieldName
		  + "': '"
		  + field->GetUnit()
		  + "' is not compatible with the unit of the expression: '"
		  + strUnitExpr
		  + "'\n" );
    }
    bOk = false;

  }

  return bOk;
}
//----------------------------------------
bool CFormula::CheckFieldNames(const CExpression& expr, const std::string& record, CProduct* product, CStringArray& fieldNamesNotFound)
{
  if (product == nullptr)
  {
    return true;
  }

  return product->CheckFieldNames(expr, (const char *)record.c_str(), fieldNamesNotFound);
}

//----------------------------------------
bool CFormula::CheckFieldNames(const std::string& exprStr, const std::string& record, CProduct* product, std::string& exprStrOut, std::string& errorMsg)
{
  bool bOk = true;

  CExpression expr;
  bOk &= CFormula::SetExpression(exprStr, expr, errorMsg);

  if (!bOk)
  {
    return bOk;
  }

  CExpression exprOut;
  bOk &= CFormula::CheckFieldNames(expr, record, product, exprOut, errorMsg);

  try
  {
    exprStrOut = exprOut.AsString().c_str();
  }
  catch (CException& e)
  {
    errorMsg += ( std::string( "Syntax is not valid\nReason:\n" ) + e.what() );
    bOk = false;

  }

  return bOk;
}
//----------------------------------------
bool CFormula::CheckFieldNames(const CExpression& expr, const std::string& record, CProduct* product, CExpression& exprOut, std::string& errorMsg)
{
  if (product == nullptr)
  {
    return true;
  }

  try
  {
    exprOut = expr;
  }
  catch (CException& e)
  {
	  errorMsg += ( std::string("Syntax is not valid\nReason:\n") + e.what() );
    return false;

  }


  CStringArray fieldNamesNotFound;
  //Try to correct lower/upper case error in field names.
  product->ReplaceNamesCaseSensitive(expr, fieldNamesNotFound, exprOut);

  //Add record name to field names
  std::string errorString;
  bool recordNameAdded = product->AddRecordNameToField(expr, (const char *)record.c_str(), exprOut, errorString);

  if (!recordNameAdded)
  {
    errorMsg += "\n";
    errorMsg += errorString;
    errorMsg += "\n";
  }


  fieldNamesNotFound.RemoveAll();
  bool bOk = CFormula::CheckFieldNames(exprOut, record, product, fieldNamesNotFound);


/*
  CStringArray fieldNamesNotFound;
  bool bOk = CFormula::CheckFieldNames(expr, record, product, fieldNamesNotFound);

  //Try to correct lower/upper case error in field names.

  if (!bOk)
  {
    product->ReplaceNamesCaseSensitive(expr, fieldNamesNotFound, exprOut);
    fieldNamesNotFound.RemoveAll();
    bOk = CFormula::CheckFieldNames(exprOut, record, product, fieldNamesNotFound);
  }

  std::string errorString;

  bool recordNameAdded = product->AddRecordNameToField(expr, record.c_str(), exprOut, errorString);

  if (!recordNameAdded)
  {
    errorMsg.Append("\n");
    errorMsg.Append(errorString.c_str());
    errorMsg.Append("\n");
  }

  fieldNamesNotFound.RemoveAll();
  bOk = CFormula::CheckFieldNames(exprOut, record, product, fieldNamesNotFound);
*/
  if (!bOk)
  {
    errorMsg += "Fields below are unknown:\n\n";
    CStringArray::const_iterator it;

    for (it = fieldNamesNotFound.begin() ; it != fieldNamesNotFound.end() ; it++)
    {
      errorMsg += "\t";
      errorMsg += *it;
      errorMsg += "\n";
    }
  }

  return bOk;
}
//----------------------------------------
bool CFormula::CheckExpression( CWorkspaceFormula *wks, const std::string& value, const std::string& record, std::string& errorMsg, std::string* strUnitExpr,
                               const CStringMap* aliases /* = nullptr*/, CProduct* product /* = nullptr*/, std::string* valueOut /*= nullptr*/)
{

  std::string tmp = value;
  rtrim( tmp );
  ltrim( tmp );
  if (tmp.empty())
  {
    errorMsg += "Data expression is empty. You have to set it.";
    return false;
  }


  bool bOk = true;
  std::string str = (const char *)value.c_str();
  uint32_t numberVarsExpanded = 0;

  // Group fields aliases and formulas aliases in a single map.
  // Add fields aliases first then formulas aliases
  // If aliases are duplicated (fields / formulas): fields aliases will be overidden by formulas aliases
  //CStringMap allAliases;
  //CProduct::GroupAliases(product, aliases, allAliases);

  const CStringMap* fieldAliases = CProduct::GetAliasesAsString(product);


  if ((aliases != nullptr) || (fieldAliases != nullptr))
  {

    CStringArray aliasesFound;

    try
    {
      CTools::FindAliases((const char *)value.c_str(), aliasesFound, true, "%", true, aliases, fieldAliases, true);
    }
    catch(CException& e)
    {
      errorMsg += ( std::string("Unable to find aliases (formulas and fields) from:\n" ) + value + "\n.\nReason\n'" + e.what() + "'" );
      return false;
    }
    //CTrace::GetInstance()->SetTraceLevel(5);
    //aliasesFound.Dump();

    CStringArray aliasesFoundUnique;
    aliasesFoundUnique.InsertUnique(aliasesFound);

      if (wks != nullptr)
      {
        wks->AmendFormulas(aliasesFoundUnique, product, record);
      }

    // Expand fields aliases and formulas aliases
    std::string errorString;
    str = CTools::ExpandVariables((const char *)value.c_str(), aliases, fieldAliases, true, '%', &numberVarsExpanded, false, &errorString);
    if (!errorString.empty())
    {
      errorMsg += ( std::string("Unable to expand aliases (formulas and fields) from:\n" ) + value + "\n.\nReason\n'" + errorString + "'" );    
      return false;
    }

  }

  //CExpression expr;
  //bOk &= CFormula::SetExpression(str.c_str(), expr, errorMsg);

  //CExpression exprOut;
  std::string expStrOut;
  bOk &= CFormula::CheckFieldNames(str.c_str(), record, product, expStrOut, errorMsg);

  if (bOk)
  {
    if (valueOut != nullptr)
    {
      if (numberVarsExpanded == 0)
      {
        *valueOut = expStrOut;
      }
      else
      {
        //Try to correct lower/upper case error in field names.
        std::string out;
        product->ReplaceNamesCaseSensitive((const char *)value.c_str(), out);

        //Add record name to field names
        std::string out2 = out;
        std::string errorString;
        product->AddRecordNameToField(out2, (const char *)record.c_str(), out, errorString);


        *valueOut = out.c_str();
      }

    }
  }
  if ((bOk) && (strUnitExpr != nullptr))
  {
    bOk &= CFormula::CheckExpressionUnits(expStrOut, record, *strUnitExpr, product, errorMsg);
  }


  return bOk;
}

//----------------------------------------
bool CFormula::SetExpression(const char* value, CExpression& expr, std::string& errorMsg)
{
  std::string str(value);
  return SetExpression(str, expr, errorMsg);

}
//----------------------------------------
bool CFormula::SetExpression( const std::string& value, CExpression& expr, std::string& errorMsg )
{
	bool bOk = true;

	try
	{
		expr.SetExpression( (const char *)value.c_str() );
	}
	catch ( CException& e )
	{
		errorMsg = errorMsg + "Syntax is not valid\nReason:\n" + e.what();
		bOk = false;
	}

	return bOk;
}
//----------------------------------------
bool CFormula::GetFields(CStringArray& fields, std::string& errorMsg, const CStringMap* aliases /* = nullptr*/, const CStringMap* fieldAliases /*= nullptr*/)
{
  CExpression expr;

  bool bOk = CFormula::SetExpression(GetDescription(true, aliases, fieldAliases), expr, errorMsg);
  if (!bOk)
  {
    return bOk;
  }
  // Copy fields names contained in expression
  fields = *(expr.GetFieldNames());

  return bOk;
}

//----------------------------------------
bool CFormula::CheckExpression(CWorkspaceFormula *wks, std::string& errorMsg, const std::string& record,
                               const CStringMap* aliases /* = nullptr*/, CProduct* product /* = nullptr*/, std::string* valueOut /*= nullptr*/)
{

  std::string stringExpr = GetDescription(true);

  // Empty expression is allowed for selection criteria
  if ((this->GetType() == CMapTypeField::eTypeOpAsSelect) && (stringExpr.empty()))
  {
    return true;
  }

  std::string str;
  std::string valueOutTemp;
  std::string strUnit = m_unit.AsString(false).c_str();

  bool bOk = CFormula::CheckExpression(wks, stringExpr, record, str, &strUnit, aliases, product, &valueOutTemp);

  rtrim( valueOutTemp );

  if ( bOk && !str_cmp( stringExpr, valueOutTemp) && valueOutTemp.empty() )
  {
    SetDescription(valueOutTemp);
  }

  if (valueOut != nullptr)
  {
    *valueOut = GetDescription(true);
  }

  if (!bOk)
  {
    errorMsg += ( std::string("\nFormula '") + m_name + "':\n\t" + str + "\n" );
  }

  return bOk;
}

//----------------------------------------
double CFormula::LonNormal360(double value)
{

  CUnit unit((const char *)GetDefaultUnit(CMapTypeData::eTypeOpLongitude).c_str());
  unit.SetConversionFrom(m_unit);

  value = unit.Convert(value);
  value = CLatLonPoint::LonNormal360(value);

  unit	= m_unit.BaseUnit();
  unit.SetConversionTo(m_unit);

  value	= unit.Convert(value);

  return value;

}
//----------------------------------------

bool CFormula::ControlResolution( std::string& errorMsg )
{
	bool bOk = true;

	if ( !IsXYType() )
		return bOk;

	std::string str;
	if ( isDefaultValue( m_minValue ) )
	{
		str += "\n\tMin. value is not defined.";
		bOk &= false;
	}

	if ( isDefaultValue( m_maxValue ) )
	{
		str += "\n\tMax. value is not defined.";
		bOk &= false;
	}

	if ( isDefaultValue( m_interval ) )
	{
		str += "\n\tNumber of intervals is not defined.";
		bOk &= false;
	}

	if ( !isDefaultValue( m_minValue ) || !isDefaultValue( m_maxValue ) )
	{
		bOk &= CtrlMinMaxValue( errorMsg );
	}

	if ( !bOk )
	{
		errorMsg += ( std::string( "\nFormula '" ) + m_name + "' - Errors on resolution:" + str + "\n" );
	}

	return bOk;
}
//----------------------------------------
bool CFormula::CtrlMinMaxValue(std::string& errorMsg)
{
  bool bOk = true;
  double min = m_minValue;
  double max = m_maxValue;

  if (IsLonDataType())
  {
    if (min >= max)
    {
      max = LonNormal360(max);
      //formula.SetMaxValue(max);
      //ctrlMax->SetValue(max);
    }
  }
  if (min >= max)
  {
    errorMsg += "\n\tMinimun value must be strictly less than Maximum value.",
    bOk = false;
  }

  return bOk;
}


//----------------------------------------
bool CFormula::IsFieldType()
{
  return (GetType() == CMapTypeField::eTypeOpAsField);
}
//----------------------------------------
bool CFormula::IsXType()
{
  return (GetType() == CMapTypeField::eTypeOpAsX);
}
//----------------------------------------
bool CFormula::IsYType()
{
  return (GetType() == CMapTypeField::eTypeOpAsY);
}
//----------------------------------------
bool CFormula::IsXYType()
{
  return ( IsXType() ||
           IsYType() );
}
//----------------------------------------
bool CFormula::IsDataTypeSet()
{
  return (GetDataType() >= 0);
}
//----------------------------------------
bool CFormula::IsLatLonDataType()
{
  return ( IsLonDataType() ||
           IsLatDataType() );
}
//----------------------------------------
bool CFormula::IsLonDataType()
{
  return (GetDataType() == CMapTypeData::eTypeOpLongitude);
}
//----------------------------------------
bool CFormula::IsLatDataType()
{
  return (GetDataType() == CMapTypeData::eTypeOpLatitude);
}
//----------------------------------------
bool CFormula::IsTimeDataType()
{
  return (GetDataType() == CMapTypeData::eTypeOpT);
}
//----------------------------------------
bool CFormula::IsXYLatLon()
{
  return IsXYType() && IsLatLonDataType();
}
//----------------------------------------
bool CFormula::IsXYTime()
{
  return IsXYType() && IsTimeDataType();
}
//----------------------------------------
bool CFormula::ControlUnitConsistency(std::string& errorMsg)
{
  bool bOk = IsUnitCompatible();
  if (!bOk)
  {

    errorMsg += (
		std::string("\nFormula '" )
		+ m_name
		+ "': \nUnit '"
		+ GetUnitAsText()
		+ "' is not compatible with type '"
		+ GetDataTypeAsString()
		+ "'.\nYou have to change either unit, or type\n" 
		);
  }

  return bOk;
}
//----------------------------------------
bool CFormula::IsDefaultUnitData()
{
  return CFormula::IsDefaultUnitData(m_unit);
}
//----------------------------------------
bool CFormula::IsDefaultUnitData(const CUnit& unit)
{
  return CFormula::IsDefaultUnitData(unit.GetText().c_str());
}
//----------------------------------------
bool CFormula::IsDefaultUnitData(const std::string& unitText)
{
  return unitText.empty() || str_icmp( unitText, DEFAULT_UNIT_COUNT );
}
//----------------------------------------
bool CFormula::IsUnitCompatible()
{
  return IsUnitCompatible(m_dataType, m_unit);
}
//----------------------------------------
bool CFormula::IsUnitCompatible(const CUnit& unit)
{
  return IsUnitCompatible(m_dataType, unit);
}
//----------------------------------------
bool CFormula::IsUnitCompatible(const std::string& unitText)
{
  return IsUnitCompatible(m_dataType, unitText);
}
//----------------------------------------
bool CFormula::IsUnitCompatible(int32_t dataType, const CUnit& unit)
{
  bool bOk = true;

  //To avoid compilation error on Linux : 2 steps
  std::string defaultUnit = (const char *)GetDefaultUnit(dataType).c_str();
  CUnit unitDataType = defaultUnit;

  if (CFormula::IsDefaultUnitData(unitDataType))
  {
    return bOk;
  }

  try
  {
    bOk = unitDataType.IsCompatible(unit);
  }
  catch (CException& e)
  {
    e.what(); // to avoid warning compilation
    bOk = false;
  }

  return bOk;
}
//----------------------------------------
bool CFormula::IsUnitCompatible(int32_t dataType, const std::string& unitText)
{
  CUnit unit((const char *)GetDefaultUnit(dataType).c_str());

  return CFormula::IsUnitCompatible(dataType, unit);
}

//----------------------------------------
bool CFormula::IsDefaultUnit()
{
//  std::string unit(GetDefaultUnit(m_dataType).c_str());
//  return unit.CmpNoCase(m_unit.GetText().c_str()) == 0;
    return CFormula::IsDefaultUnit(m_unit.GetText().c_str(), m_dataType);
}

//----------------------------------------
bool CFormula::IsDefaultUnit(const char * value, int32_t dataType)
{

  std::string unit(GetDefaultUnit(dataType).c_str());

  return str_icmp( unit, std::string( value ) );
}
//----------------------------------------
void CFormula::SetDefaultUnit()
{

  m_unit = (const char *)GetDefaultUnit(m_dataType).c_str();

  SetStepToDefaultAsNecessary();

/*
  switch (m_dataType)
  {
    case CMapTypeData::typeOpData:
      m_unit = DEFAULT_UNIT_COUNT.c_str();
      break;
    case CMapTypeData::typeOpLongitude:
      m_unit = DEFAULT_UNIT_LONGITUDE.c_str();
      break;
    case CMapTypeData::typeOpLatitude:
      m_unit = DEFAULT_UNIT_LATITUDE.c_str();
      break;
    default:
      break;
  }
*/
}
//----------------------------------------
void CFormula::SetUnit( const std::string& value, std::string &errorMsg, const std::string& defaultValue, bool convertMinMax /*= false*/ )
{
	if ( convertMinMax )
	{
		ConvertToMinMaxFormulaBaseUnit( errorMsg );
	}

	try
	{
		if ( value.empty() )
		{
			SetDefaultUnit();
		}
		else
		{
			m_unit = (const char *)value.c_str();
		}
	}
	catch ( CException& e )
	{
			errorMsg +=
				"Formula '"
				+ m_name
				+ "' - Invalid unit '"
				+ value
				+ "'\nReason\n'"
				+ e.what()
				+ "'\n";

				//wxMessageBox(				"Warning",				wxOK | wxICON_EXCLAMATION );

		m_unit = (const char *)defaultValue.c_str();
	}

	SetStepToDefaultAsNecessary();

	if ( convertMinMax )
	{
		ConvertToMinMaxFormulaUnit( errorMsg );
	}

}
//----------------------------------------
void CFormula::SetUnit( const CUnit& value, std::string &errorMsg, const std::string& defaultValue, bool convertMinMax /*= false*/ )
{
	if ( convertMinMax )
	{
		if ( !IsTimeDataType() )
		{
			ConvertToMinMaxFormulaBaseUnit( errorMsg );
		}
	}

	try
	{
		m_unit = value;
	}
	catch ( CException& e )
	{
		errorMsg =
			"Formula '"
			+ m_name
			+ "' - Invalid unit '"
			+ value.GetText()
			+ "'\nReason\n'"
			+ e.what()
			+ "'\n";
		//wxMessageBox(		  "Warning",		  wxOK | wxICON_EXCLAMATION);

		m_unit = (const char *)defaultValue.c_str();
	}

	SetStepToDefaultAsNecessary();

	if ( convertMinMax )
	{
		if ( !IsTimeDataType() )
		{
			ConvertToMinMaxFormulaUnit( errorMsg );
		}
	}

}
//----------------------------------------
std::string CFormula::GetFieldPrefix(int32_t type)
{
  std::string result;
  switch (type)
  {
    case CMapTypeField::eTypeOpAsField:
      result = "FIELD";
      break;
    case CMapTypeField::eTypeOpAsX:
      result = "X";
      break;
    case CMapTypeField::eTypeOpAsY:
      result = "Y";
      break;
    default:
      result = "FIRLD";
      break;
  }
  return result;

}
//----------------------------------------
std::string CFormula::GetFieldPrefix()
{
  return GetFieldPrefix(m_type);
}
//----------------------------------------
std::string CFormula::GetExportAsciiFieldPrefix()
{
  return GetFieldPrefix(CMapTypeField::eTypeOpAsField);
}

//----------------------------------------
bool CFormula::LoadConfigDesc(CConfiguration *config, const std::string& path)
{
  bool bOk = true;
  if (config == nullptr)
  {
    return true;
  }

  config->SetPath("/" + path);

  m_description = config->Read(m_name, m_description);

  return true;
}
//----------------------------------------
bool CFormula::LoadConfig( CConfiguration *config, std::string &errorMsg, const std::string& pathSuff )
{
	bool bOk = true;
	std::string valueString;

	if ( config == nullptr )
	{
		return true;
	}
	std::string path = m_name;
	if ( pathSuff.empty() == false )
	{
		path += "_" + pathSuff;
	}

	config->SetPath( "/" + path );

	m_description = config->Read( ENTRY_DEFINE, m_description );
	m_comment = config->Read( ENTRY_COMMENT, m_comment );

	valueString = config->Read( ENTRY_UNIT, m_unit.GetText().c_str() );
	SetUnit( valueString, errorMsg, "" );

	valueString = config->Read( ENTRY_FIELDTYPE, GetTypeAsString() );
	if ( valueString.empty() )
	{
		m_type = CMapTypeField::eTypeOpAsField;
	}
	else
	{
		m_type = CMapTypeField::GetInstance().NameToId( valueString );
	}

	valueString = config->Read( ENTRY_DATATYPE, GetDataTypeAsString() );
	if ( valueString.empty() )
	{
		m_dataType = CMapTypeData::eTypeOpData;
	}
	else
	{
		m_dataType = CMapTypeData::GetInstance().NameToId( valueString );
	}

	m_title = config->Read( ENTRY_TITLE, m_title );

	valueString = config->Read( ENTRY_FILTER, GetFilterAsString() );
	if ( valueString.empty() )
	{
		m_filter = CMapTypeFilter::eFilterNone;
	}
	else
	{
		m_filter = CMapTypeFilter::GetInstance().NameToId( valueString );
	}
	if ( IsTimeDataType() )
	{
		valueString = config->Read( ENTRY_MINVALUE );
		SetMinValueFromDateString( valueString );

		valueString = config->Read( ENTRY_MAXVALUE );
		SetMaxValueFromDateString( valueString );
	}
	else
	{
		config->Read( ENTRY_MINVALUE, &m_minValue, CTools::m_defaultValueDOUBLE );
		config->Read( ENTRY_MAXVALUE, &m_maxValue, CTools::m_defaultValueDOUBLE );
	}

	// 3.3.1 note: wxWidgets asserts if value >= INT_MAX, but CTools::m_defaultValueINT32 is INT_MAX, so,
	// if value is not read and the default is used, we have a failed assertion. This was not changed, to
	// avoid unknown implications.

	config->Read( ENTRY_INTERVAL, &m_interval, CTools::m_defaultValueINT32 );
	m_step = config->Read( ENTRY_STEP, DEFAULT_STEP_GENERAL_ASSTRING );
	if ( m_step.empty() )
	{
		m_step = DEFAULT_STEP_GENERAL_ASSTRING;
	}

	config->Read( ENTRY_LOESSCUTOFF, &m_loessCutOff, CTools::m_defaultValueINT32 );

	valueString = config->Read( ENTRY_DATA_MODE );
	if ( valueString.empty() )
	{
		m_dataMode = -1;
	}
	else
	{
		m_dataMode = CMapDataMode::GetInstance().NameToId( valueString );
	}

	return true;
}
//----------------------------------------
bool CFormula::SaveConfigDesc( CConfiguration *config, const std::string& path)
{
  bool bOk = true;
  if (config == nullptr)
  {
    return true;
  }

  config->SetPath("/" + path);

  bOk &= config->Write(m_name, GetDescription(true));
  //bOk &= config->Write(ENTRY_COMMENT, m_comment);

  return true;
}

//----------------------------------------
bool CFormula::SaveConfigPredefined( CConfiguration *config, const std::string& pathSuff)
{
  bool bOk = true;
  if (config == nullptr)
  {
    return true;
  }
  std::string path = m_name;
  if (pathSuff.empty() == false)
  {
    path += "_" + pathSuff;
  }
  config->SetPath("/" + path);

  bOk &= config->Write(ENTRY_DEFINE, GetDescription(true));
  bOk &= config->Write(ENTRY_COMMENT, GetComment(true));
  bOk &= config->Write(ENTRY_UNIT, m_unit.GetText().c_str());

  return true;
}
//----------------------------------------
bool CFormula::SaveConfig( CConfiguration *config, const std::string& pathSuff )
{
  if (m_predefined)
  {
    return SaveConfigPredefined(config, pathSuff);
  }

  bool bOk = true;
  if (config == nullptr)
  {
    return true;
  }
  std::string path = m_name;
  if (pathSuff.empty() == false)
  {
    path += "_" + pathSuff;
  }
  config->SetPath("/" + path);

  bOk &= config->Write(ENTRY_DEFINE, GetDescription(true));
  bOk &= config->Write(ENTRY_COMMENT, GetComment(true));
  bOk &= config->Write(ENTRY_UNIT, m_unit.GetText().c_str());
  bOk &= config->Write(ENTRY_FIELDTYPE, GetTypeAsString());
  bOk &= config->Write(ENTRY_DATATYPE, GetDataTypeAsString());
  bOk &= config->Write(ENTRY_TITLE, m_title);
  bOk &= config->Write(ENTRY_FILTER, GetFilterAsString());
  if (isDefaultValue(m_minValue) == false)
  {
    if (IsTimeDataType())
    {
      bOk &= config->Write(ENTRY_MINVALUE, GetMinValueAsDateString());
    }
    else
    {
      bOk &= config->Write(ENTRY_MINVALUE, CTools::Format("%.15g", m_minValue).c_str());
    }
  }
  if (isDefaultValue(m_maxValue) == false)
  {
    if (IsTimeDataType())
    {
      bOk &= config->Write(ENTRY_MAXVALUE, GetMaxValueAsDateString());
    }
    else
    {
      bOk &= config->Write(ENTRY_MAXVALUE, CTools::Format("%.15g", m_maxValue).c_str());
    }
  }
  if (isDefaultValue(m_interval) == false)
  {
    bOk &= config->Write(ENTRY_INTERVAL, m_interval);
  }
  if (m_step.empty() == false)
  {
    bOk &= config->Write(ENTRY_STEP, m_step);
  }
  if (isDefaultValue(m_loessCutOff) == false)
  {
    bOk &= config->Write(ENTRY_LOESSCUTOFF, m_loessCutOff);
  }
  bOk &= config->Write(ENTRY_DATA_MODE,
                       GetDataModeAsString());

  return true;
}
//----------------------------------------
std::string CFormula::GetDescription(bool removeCRLF /* = false */, const CStringMap* formulaAliases /* = nullptr*/, const CStringMap* fieldAliases /* = nullptr*/) const
{
  if (removeCRLF == false)
  {
    return m_description;
  }

  std::string str = m_description;
  replace( str, "\n", " ");
  
  if ((formulaAliases != nullptr) || (fieldAliases != nullptr)) 
  {
    str = CTools::ExpandVariables(str, formulaAliases, fieldAliases, true, '%', nullptr, false);
  }

  return str;
}
//----------------------------------------
std::string CFormula::GetComment(bool removeCRLF)
{
  if (removeCRLF == false)
  {
    return m_comment;
  }

  std::string str = m_comment;
  replace( str, "\n", " ");
  return str;
}
//----------------------------------------
std::string CFormula::GetAlias()
{
	return std::string( "%{" ) + m_name + "}";
}

//----------------------------------------
void CFormula::SetMinValue(CDate& d)
{
  if (d.IsDefaultValue())
  {
    setDefaultValue(m_minValue);
    return;
  }

  m_minValue = d.Value();
}
//----------------------------------------
void CFormula::SetMinValueFromDateString(const std::string& value)
{
  CDate d(value.c_str());
  SetMinValue(d);
}
//----------------------------------------
void CFormula::SetMaxValue(CDate& d)
{
  if (d.IsDefaultValue())
  {
    setDefaultValue(m_maxValue);
    return;
  }

  m_maxValue = d.Value();
}
//----------------------------------------
void CFormula::SetMaxValueFromDateString(const std::string& value)
{
  CDate d(value.c_str());
  SetMaxValue(d);
}
//----------------------------------------
void CFormula::ConvertToMinMaxFormulaBaseUnit( std::string &errorMsg )
{
  if (!IsTimeDataType())
  {
    m_minValue = ConvertToFormulaBaseUnit(m_minValue);
    m_maxValue = ConvertToFormulaBaseUnit(m_maxValue);
  }

  ComputeInterval( errorMsg );
}
//----------------------------------------
void CFormula::ConvertToMinMaxFormulaUnit( std::string &errorMsg )
{
  if (!IsTimeDataType())
  {
    m_minValue = ConvertToFormulaUnit(m_minValue);
    m_maxValue = ConvertToFormulaUnit(m_maxValue);
  }

  ComputeInterval( errorMsg );
}
//----------------------------------------
bool CFormula::ConvertToFormulaBaseUnit(double in, double& out)
{
  out = m_unit.ConvertToBaseUnit(in);

  return !isDefaultValue(out);;

}
//----------------------------------------
double CFormula::ConvertToFormulaBaseUnit(double in)
{
  double out;
  ConvertToFormulaBaseUnit(in, out);

  return out;
}

//----------------------------------------
bool CFormula::ConvertToFormulaUnit(double in, double& out)
{
  out = m_unit.ConvertToUnit(in);

  return !isDefaultValue(out);;
}
//----------------------------------------
double CFormula::ConvertToFormulaUnit(double in)
{
  double out;
  ConvertToFormulaUnit(in, out);
  return out;
}

//----------------------------------------
bool CFormula::ComputeInterval( std::string &errorMsg )
{
	bool bOk = true;

	if ( !IsXYType() )
	{
		return true;
	}

	/*
	  if (IsTimeDataType())
	  {
	  bOk = ComputeIntervalAsDate(showMsg);
	  }
	  else
	  {
	  bOk = ComputeIntervalAsDouble(showMsg);
	  }

	  return bOk;
	  */

	setDefaultValue( m_interval );

	if ( !IsXYType() )
	{
		return true;
	}

	if ( isDefaultValue( m_minValue ) && isDefaultValue( m_maxValue ) )
	{
		return true;
	}

	double interval = 0.0;
	double intervalTmp = 0.0;
	double step = GetStepAsDouble( errorMsg );

	if ( isDefaultValue( step ) )
	{
		return true;
	}

	double minValueDefault =  0.0;
	double maxValueDefault =  1.0;

	if ( IsLonDataType() )
	{
		ConvertToFormulaUnit( -180, minValueDefault );
		ConvertToFormulaUnit( 180, maxValueDefault );
	}
	else if ( IsLatDataType() )
	{
		ConvertToFormulaUnit( -90, minValueDefault );
		ConvertToFormulaUnit( 90, maxValueDefault );
	}
	else if ( IsTimeDataType() )
	{
		ConvertToFormulaUnit( 0, minValueDefault );
		ConvertToFormulaUnit( CDate::m_secInDay, maxValueDefault );
	}

	if ( isDefaultValue( m_minValue ) )
	{
		m_minValue = minValueDefault;
	}
	if ( isDefaultValue( m_maxValue ) )
	{
		m_maxValue = maxValueDefault;
	}

	if ( step < 0 )
	{
		step = -( step );
		replace( m_step, "-", "" );
	}

	if ( isZero( step ) )
	{
		SetStepToDefault();

		step = CFormula::GetStepAsDouble( m_step );
	}

	/*
	  CUnit	unit	= m_unit.BaseUnit();
	  unit.SetConversionTo(m_unit);
	  double minValue	= unit.Convert(m_minValue);
	  double maxValue	= unit.Convert(m_maxValue);
	  */
	double minValue	= m_minValue;
	double maxValue	= m_maxValue;

	if ( IsLonDataType() )
	{
		if ( minValue >= maxValue )
		{
			maxValue	= LonNormal360( maxValue );
		}
	}
	else if ( IsTimeDataType() )
	{
		CUnit	unit	= m_unit.BaseUnit();
		unit.SetConversionTo( m_unit );
		minValue	= unit.Convert( m_minValue );
		maxValue	= unit.Convert( m_maxValue );
	}


	//intervalTmp = ((m_maxValue - m_minValue) / step) / CDate::m_secInDay;
	intervalTmp = ( ( maxValue - minValue ) / step );
	//interval = CTools::Round( ((m_maxValue - m_minValue) / step) / CDate::m_secInDay);
	interval = CTools::Round( ( ( maxValue - minValue ) / step ) );

	m_interval = static_cast<int32_t>( CTools::Int( interval ) );

	if ( m_interval <= 0 )
	{
		m_interval = 1;
	}


	if ( !areEqual( interval, intervalTmp ) )
	{
		errorMsg +=
			"Formula '"
			+ m_name
			+ "':\nInterval was round up or down to the nearest integer value.\n";

			//wxMessageBox(			"Information",			wxOK | wxCENTRE | wxICON_INFORMATION );
	}

	return areEqual( interval, intervalTmp );
}
/*
//----------------------------------------
bool CFormula::ComputeIntervalAsDouble(bool showMsg)
{
  setDefaultValue(m_interval);

  if (!IsXYType())
  {
    return true;
  }
  if (isDefaultValue(m_minValue) && isDefaultValue(m_maxValue))
  {
    return true;
  }

  double interval = 0.0;
  double intervalTmp = 0.0;
  double step = GetStepAsDouble();

  if (isDefaultValue(step))
  {
    return true;
  }

  double minValueDefault =  0.0;
  double maxValueDefault =  0.0;

  if (IsLonDataType())
  {
    minValueDefault = -180 : -90;
    maxValueDefault = 180 : 90;
  }
  else if (IsLatDataType())
  {
    minValueDefault = -90;
    maxValueDefault = 90;
  }

  if (isDefaultValue(m_minValue))
  {
    m_minValue = minValueDefault;
  }
  if (isDefaultValue(m_maxValue))
  {
    m_maxValue = maxValueDefault;
  }

  if (step < 0)
  {
    step = -(step);
    m_step.Replace("-", "");
  }

  if (isZero(step))
  {
    SetStepToDefault();

    step = CFormula::GetStepAsDouble(m_step);
  }

  intervalTmp = (m_maxValue - m_minValue) / step;
  interval = CTools::Round( (m_maxValue - m_minValue) / step );

  m_interval = static_cast<int32_t>( CTools::Int(interval) );

  if (m_interval <= 0)
  {
    m_interval = 1;
  }

  if ( (showMsg) && (areEqual( interval, intervalTmp ) == false) )
  {
    wxMessageBox(std::string::Format("Formula '%s':\nInterval was round up or down to the nearest integer value.",
                                  m_name.c_str()),
                 "Information",
                  wxOK | wxCENTRE | wxICON_INFORMATION);
  }

  return areEqual( interval, intervalTmp );
}
*/
/*
//----------------------------------------
bool CFormula::ComputeIntervalAsDate(bool showMsg)
{
  setDefaultValue(m_interval);

  if (!IsXYType())
  {
    return true;
  }

  if (isDefaultValue(m_minValue) && isDefaultValue(m_maxValue))
  {
    return true;
  }

  double interval = 0.0;
  double intervalTmp = 0.0;
  double step = GetStepAsDouble();

  if (isDefaultValue(step))
  {
    return true;
  }

  double minValueDefault =  0.0;
  double maxValueDefault =  CDate::m_secInDay;

  if (isDefaultValue(m_minValue))
  {
    m_minValue = minValueDefault;
  }
  if (isDefaultValue(m_maxValue))
  {
    m_maxValue = maxValueDefault;
  }

  if (step < 0)
  {
    step = -(step);
    m_step.Replace("-", "");
  }

  if (isZero(step))
  {
    SetStepToDefault();

    step = CFormula::GetStepAsDouble(m_step);
  }


  CUnit	unit	= m_unit.BaseUnit();
  unit.SetConversionTo(m_unit);
  double minValue	= unit.Convert(m_minValue);
  double maxValue	= unit.Convert(m_maxValue);

  //intervalTmp = ((m_maxValue - m_minValue) / step) / CDate::m_secInDay;
  intervalTmp = ((maxValue - minValue) / step);
  //interval = CTools::Round( ((m_maxValue - m_minValue) / step) / CDate::m_secInDay);
  interval = CTools::Round( ((maxValue - minValue) / step));

  m_interval = static_cast<int32_t>( CTools::Int(interval) );

  if (m_interval <= 0)
  {
    m_interval = 1;
  }

  if ( (showMsg) && (areEqual( interval, intervalTmp ) == false) )
  {
    wxMessageBox(std::string::Format("Formula '%s':\nInterval was round up or down to the nearest integer value.",
                                  m_name.c_str()),
                 "Information",
                  wxOK | wxCENTRE | wxICON_INFORMATION);
  }

  return areEqual( interval, intervalTmp );
}
*/
//----------------------------------------
void CFormula::SetStepToDefaultAsNecessary()
{
  if (m_step.empty())
  {
    SetStepToDefault();
  }
}
//----------------------------------------
void CFormula::SetStepToDefault()
{
  m_step = GetDefaultStep();
}
//----------------------------------------
std::string CFormula::GetDefaultStep()
{
  if (!IsDataTypeSet())
  {
    return "";
  }

  if (IsLatLonDataType())
  {
    return DEFAULT_STEP_LATLON_ASSTRING;
  }
  else if (IsTimeDataType())
  {
    return DEFAULT_STEP_TIME_ASSTRING;
  }
  return DEFAULT_STEP_GENERAL_ASSTRING;
}

//----------------------------------------
std::string CFormula::GetAsDateString( double seconds )
{
	CDate d;
	d.SetDate( seconds );

	if ( d.IsDefaultValue() )
		return "";

	return d.AsString();		; // "" as default format for date time
}



//----------------------------------------
void CFormula::Dump( std::ostream& fOut /* = std::cerr */ )
{
	if ( !CTrace::IsTrace() )
		return;


	fOut << "==> Dump a CFormula Object at " << this << std::endl;
	fOut << "m_name " << m_name;
	fOut << "m_description " << m_description;
	fOut << "==> END Dump a CFormula Object at " << this << std::endl;

	fOut << std::endl;
}

//-------------------------------------------------------------
//------------------- CMapFormula class --------------------
//-------------------------------------------------------------

std::string CMapFormula::m_predefFormulaFile = "Formulas.ini";

/*
//----------------------------------------
CMapFormula& CMapFormula::GetInstance()
{
 static CMapFormula instance;

 return instance;
}
*/
//----------------------------------------
bool CMapFormula::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapFormula::ValidName(const char* name)
{
  CFormula* value = dynamic_cast<CFormula*>(Exists(name));
  return (value != nullptr);
}

//----------------------------------------
bool CMapFormula::LoadConfig( std::string &errorMsg, bool predefined )
{
  return LoadConfig( m_config, errorMsg, predefined );
}
//----------------------------------------
bool CMapFormula::LoadConfig( CConfiguration *config, std::string &errorMsg, bool predefined, const std::string& pathSuff )
{
	bool bOk = true;

	if ( config == nullptr )
	{
		return false;
	}

	std::string path = GROUP_FORMULAS;
	if ( pathSuff.empty() == false )
	{
		path += "_" + pathSuff;
	}
	config->SetPath( "/" + path );

	//long maxEntries = config->GetNumberOfEntries();
	std::string entry;
	std::string valueString;
	std::string formulaName;
	long i = 0;

	do
	{
		bOk = config->GetNextEntry( entry, i );
		if ( bOk )
		{
			valueString = config->Read( entry );

			if ( !pathSuff.empty() )
			{
				//formulaName = valueString.Left(valueString.Length() - pathSuff.Length() - 1);
				formulaName = valueString.substr( 0, valueString.length() - pathSuff.length() - 1 );
			}
			else
			{
				formulaName = valueString;
			}
			CFormula* value = dynamic_cast<CFormula*>( Exists( (const char *)formulaName.c_str() ) );
			if ( value != nullptr )
			{
				Erase( formulaName );
			}

			Insert( formulaName, new CFormula( formulaName, predefined ) );
		}
	} while ( bOk );

	//int32_t index = 0;

	for ( CMapFormula::iterator it = begin(); it != end(); it++ )
	{
		//index++;
		CFormula* formula = dynamic_cast<CFormula*>( it->second );
		if ( formula == nullptr )
		{
			errorMsg +=
				"ERROR in  CMapFormula::LoadConfig\ndynamic_cast<CFormula*>(it->second) returns nullptr pointer"
				"\nList seems to contain objects other than those of the class CFormula.\n";

			//wxMessageBox(,                   "Error",                    wxOK | wxCENTRE | wxICON_ERROR);

			return false;
		}

		formula->LoadConfig( config, errorMsg, pathSuff );
	}
	return true;
}

//----------------------------------------
bool CMapFormula::SaveConfig( CConfiguration *config, bool predefined, const std::string& pathSuff )
{
	bool bOk = true;
	if ( config == nullptr )
		return false;

	int index = 0;
	for ( CMapFormula::iterator it = begin(); it != end(); it++ )
	{
		std::string path = GROUP_FORMULAS;
		if ( pathSuff.empty() == false )
		{
			path += "_" + pathSuff;
		}
		config->SetPath( "/" + path );

		CFormula* formula = dynamic_cast<CFormula*>( it->second );
		if ( formula != nullptr )
		{
			if ( formula->IsPredefined() != predefined )
				continue;

			index++;
			std::string formulaName = formula->GetName();
			if ( pathSuff.empty() == false )
			{
				formulaName += "_" + pathSuff;
			}

			config->Write( ENTRY_FORMULA + n2s<std::string>( index ), formulaName );

			formula->SaveConfig( config, pathSuff );
		}
	}
	return true;
}
//----------------------------------------
bool CMapFormula::InsertPredefined( std::string &errorMsg )
{
	std::string formulaPath = CTools::GetDataDir() + "/" + CMapFormula::m_predefFormulaFile;		//TODO check this conversion from wxFileName to std::string
	//wxFileName formulaPath;
	//formulaPath.Assign( CTools::GetDataDir().c_str(), CMapFormula::m_predefFormulaFile );
	//formulaPath.Normalize();

    delete m_config;
	//m_config = new CConfiguration( wxEmptyString, wxEmptyString, formulaPath.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE );
	m_config = new CConfiguration( formulaPath );

	return LoadConfig( errorMsg, true );
}
//----------------------------------------
bool CMapFormula::InsertUserDefined( CFormula* formula, std::string &errorMsg )
{
	std::string name = formula->GetName();

	CFormula* value = dynamic_cast<CFormula*>( Exists( name ) );
	if ( value != nullptr )
	{
		errorMsg +=
			"Formula '"
			+ value->GetName()
			+ "' already exists\nContent:\n"
			+ value->GetDescription()
			+ "\nIt will be replaced by:\n"
			+ formula->GetDescription()
			+ ".\n";

		value->SetDescription( formula->GetDescription() );
		value->SetComment( formula->GetComment() );
	}
	else
	{
		Insert( name, formula );
	}

	return true;
}
//----------------------------------------
bool CMapFormula::InsertUserDefined_ReplacePredefinedNotAllowed( CFormula& formula, std::string &errorMsg )
{
	std::string name = formula.GetName();

	CFormula* value = dynamic_cast<CFormula*>( Exists( name ) );
	if ( value != nullptr )
	{
		if ( value->IsPredefined() )
		{
			errorMsg +=
				"Formula '"
				+ value->GetName()
				+ "' already exists and is a predefined formula.\n"
				+ "You are not allowed to replace predefined formula.";

			return false;		//femm: returned true;
		}
		else
		{
			return InsertUserDefined( &formula, errorMsg );
		}
	}
	else
	{
		CFormula* formulaToInsert = new CFormula( formula );
		Insert( name, formulaToInsert );
	}
	return true;
}
//----------------------------------------
//bool CMapFormula::InsertUserDefined( const wxFileName& fileName )
//{
//    delete m_config;
//	m_config = new CConfiguration( fileName.GetFullPath().ToStdString() );
//	//m_config = new wxFileConfig( wxEmptyString, wxEmptyString, fileName.GetFullPath(), wxEmptyString, wxCONFIG_USE_LOCAL_FILE );
//
//	return LoadConfig( false );
//}
//----------------------------------------
bool CMapFormula::InsertUserDefined( CConfiguration *config, std::string &errorMsg )
{
	return LoadConfig( config, errorMsg, false );
}
//----------------------------------------
std::string CMapFormula::GetDescFormula( const std::string& name, bool alias )
{
	CFormula* value = dynamic_cast<CFormula*>( Exists( name ) );
	if ( value == nullptr )
		return "";

	std::string text = alias ? value->GetAlias() : value->GetDescription();
	return text;
}
/*
//----------------------------------------
bool CMapFormula::SetDescFormula(const std::string& name, const std::string& description)
{
  CFormula* value = dynamic_cast<CFormula*>(Exists(name.c_str()));
  if (value == nullptr)
  {
    return false;
  }

  value->SetDescription(description);
  return true;
}
*/
//----------------------------------------
std::string CMapFormula::GetCommentFormula( const std::string& name ) const
{
	CFormula* value = dynamic_cast<CFormula*>( Exists( name ) );
	if ( value == nullptr )
		return "";

	return value->GetComment();
}
//----------------------------------------
bool CMapFormula::SetCommentFormula(const std::string& name, const std::string& comment)
{
  CFormula* value = dynamic_cast<CFormula*>(Exists((const char *)name.c_str()));
  if (value == nullptr)
  {
    return false;
  }

  value->SetComment(comment);
  return true;
}

//----------------------------------------
CFormula* CMapFormula::GetFormula(int32_t type)
{
  CMapFormula::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    if (value->GetType() == type)
    {
      return value;
    }
  }

  return nullptr;
}
//----------------------------------------
void CMapFormula::InitFormulaDataMode(int32_t dataMode)
{
  CMapFormula::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value == nullptr)
    {
      continue;
    }

    if (value->GetDataMode() < 0)
    {
      value->SetDataMode(dataMode);
    }
  }

}

//----------------------------------------
int32_t CMapFormula::CountDataFields()
{
  CMapFormula::iterator it;

  int32_t count = 0;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value != nullptr)
    {
      if (value->GetType() == CMapTypeField::eTypeOpAsField)
      {
        count++;
      }
    }
  }

  return count;

}
//----------------------------------------

bool CMapFormula::HasFilters()
{
  CMapFormula::iterator it;

  bool hasFilter = false;

  for (it = begin() ; it != end() ; it++)
  {
    CFormula* value = dynamic_cast<CFormula*>(it->second);
    if (value != nullptr)
    {
      if (value->GetType() == CMapTypeField::eTypeOpAsField)
      {
        if (value->GetFilter() != CMapTypeFilter::eFilterNone)
        {
          hasFilter = true;
          break;
        }
      }
    }
  }

  return hasFilter;
}
//----------------------------------------

void CMapFormula::Amend(const CStringArray& keys, CProduct* product, const std::string& record)
{
  if (product == nullptr)
  {
    return;
  }

  bool bOk = true;

  CStringArray::const_iterator itKey;

  for (itKey = keys.begin() ; itKey != keys.end() ; itKey++)
  {
    CFormula* value = dynamic_cast<CFormula*>(this->Exists(*itKey));
    if (value == nullptr)
    {
      continue;
    }

    std::string out;
    std::string errorMsg;
    std::string str = value->GetDescription(true);

    product->ReplaceNamesCaseSensitive((const char *)str.c_str(), out);

    if (out.empty())
    {
      continue;
    }

    if ( !str_cmp( str, out ) )
    {
      value->SetDescription(out);
    }

  }
}


//-------------------------------------------------------------
//------------------- CMapTypeFilter class --------------------
//-------------------------------------------------------------

CMapTypeFilter::CMapTypeFilter()
{
  Insert("NONE", eFilterNone);
  Insert("LOESS_SMOOTH", eFilterLoessSmooth);
  Insert("LOESS_EXTRAPOLATE", eFilterLoessExtrapolate);
  Insert("LOESS", eFilterLoess);
}

//----------------------------------------
CMapTypeFilter::~CMapTypeFilter()
{

}

//----------------------------------------
CMapTypeFilter& CMapTypeFilter::GetInstance()
{
 static CMapTypeFilter instance;

 return instance;
}

//----------------------------------------
bool CMapTypeFilter::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeFilter::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

std::string CMapTypeFilter::IdToName(uint32_t id)
{
  CMapTypeFilter::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeFilter::NameToId(const std::string& name)
{
  return Exists((const char *)name.c_str());
}


//-------------------------------------------------------------
//------------------- CMapTypeData class --------------------
//-------------------------------------------------------------

CMapTypeData::CMapTypeData()
{
  Insert("Data", eTypeOpData);
  Insert("Longitude", eTypeOpLongitude);
  Insert("Latitude", eTypeOpLatitude);
  Insert("X", eTypeOpX);
  Insert("Y", eTypeOpY);
  Insert("Z", eTypeOpZ);
  Insert("Date", eTypeOpT);
}

//----------------------------------------
CMapTypeData::~CMapTypeData()
{

}

//----------------------------------------
CMapTypeData& CMapTypeData::GetInstance()
{
 static CMapTypeData instance;

 return instance;
}

//----------------------------------------
bool CMapTypeData::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeData::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

std::string CMapTypeData::IdToName(uint32_t id)
{
  CMapTypeData::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeData::NameToId(const std::string& name)
{
  return Exists((const char *)name.c_str());
}

//-------------------------------------------------------------
//------------------- CMapTypeOp class --------------------
//-------------------------------------------------------------

CMapTypeOp::CMapTypeOp()
{
  Insert("Y=F(X)", eTypeOpYFX);
  Insert("Z=F(X,Y)", eTypeOpZFXY);
}

//----------------------------------------
CMapTypeOp::~CMapTypeOp()
{

}

//----------------------------------------
CMapTypeOp& CMapTypeOp::GetInstance()
{
 static CMapTypeOp instance;

 return instance;
}

//----------------------------------------
bool CMapTypeOp::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeOp::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}
//----------------------------------------

std::string CMapTypeOp::IdToName(uint32_t id)
{
  CMapTypeOp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeOp::NameToId(const std::string& name)
{
  return Exists((const char *)name.c_str());
}

/*
//-------------------------------------------------------------
//------------------- CMapTypeDisp class --------------------
//-------------------------------------------------------------

CMapTypeDisp::CMapTypeDisp()
{
  Insert("Y=F(X)", typeDispYFX);
  Insert("Z=F(Lat,Lon)", typeDispZFLatLon);
  Insert("Z=F(X,Y)", typeDispZFXY);
}

//----------------------------------------
CMapTypeDisp::~CMapTypeDisp()
{

}

//----------------------------------------
CMapTypeDisp& CMapTypeDisp::GetInstance()
{
 static CMapTypeDisp instance;

 return instance;
}

//----------------------------------------
bool CMapTypeDisp::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeDisp::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

std::string CMapTypeDisp::IdToName(uint32_t id)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeDisp::NameToId(const std::string& name)
{
  return Exists(name.c_str());
}

//----------------------------------------
void CMapTypeDisp::NamesToArrayString(wxArrayString& array)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      array.Add( (it->first).c_str());
    }
  }

}
//----------------------------------------
void CMapTypeDisp::NamesToComboBox(wxComboBox& combo)
{
  CMapTypeDisp::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (!isDefaultValue(value))
    {
      //combo.Insert((it->first).c_str(), value);
      combo.Append( (it->first).c_str());
    }
  }

}
//----------------------------------------

std::string CMapTypeDisp::Enum()
{
  CMapTypeDisp::iterator it;

  std::string result;

  for (it = begin() ; it != end() ; it++)
  {
    result.Append(std::string::Format("%s ==> %d  ", (it->first).c_str(), (it->second)));
  }

  return result;
}

*/
//-------------------------------------------------------------
//------------------- CMapTypeField class --------------------
//-------------------------------------------------------------

CMapTypeField::CMapTypeField()
{
  Insert("asField", eTypeOpAsField);
  Insert("asX", eTypeOpAsX);
  Insert("asY", eTypeOpAsY);
}

//----------------------------------------
CMapTypeField::~CMapTypeField()
{

}

//----------------------------------------
CMapTypeField& CMapTypeField::GetInstance()
{
 static CMapTypeField instance;

 return instance;
}

//----------------------------------------
bool CMapTypeField::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapTypeField::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

std::string CMapTypeField::IdToName(uint32_t id)
{
  CMapTypeField::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapTypeField::NameToId(const std::string& name)
{
  return Exists((const char *)name.c_str());
}

//-------------------------------------------------------------
//------------------- CMapDataMode class --------------------
//-------------------------------------------------------------

CMapDataMode::CMapDataMode()
{
  for (int32_t mode = CBratProcess::pctFIRST; mode <= CBratProcess::pctLAST; mode++)
  {
    Insert(CBratProcess::DataModeStr(CBratProcess::MergeDataMode(mode)), mode);
  }
}

//----------------------------------------

CMapDataMode::~CMapDataMode()
{

}

//----------------------------------------
CMapDataMode& CMapDataMode::GetInstance()
{
 static CMapDataMode instance;

 return instance;
}

//----------------------------------------
bool CMapDataMode::ValidName(const std::string& name)
{
  return ValidName(name.c_str());
}
//----------------------------------------
bool CMapDataMode::ValidName(const char* name)
{
  uint32_t value = Exists(name);
  return (!isDefaultValue(value));
}

//----------------------------------------

std::string CMapDataMode::IdToName(uint32_t id)
{
  CMapDataMode::iterator it;

  for (it = begin() ; it != end() ; it++)
  {
    uint32_t value = it->second;
    if (value == id)
    {
      return (it->first).c_str();
    }
  }

  return "";
}


//----------------------------------------
uint32_t CMapDataMode::NameToId(const std::string& name)
{
  return Exists((const char *)name.c_str());
}

//----------------------------------------
uint32_t CMapDataMode::GetDefault()
{
  return CBratProcess::pctMEAN;
}

//----------------------------------------
std::string CMapDataMode::GetDefaultAsString()
{
  return IdToName(GetDefault());
}
