
/*
* Copyright (C) 2008 Collecte Localisation Satellites (CLS), France (31)
*
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

#include <cstdlib>
#include <cstdio>
#include <cstring> 

#include "brathl_error.h" 
#include "brathl.h" 
//#define BRAT_INTERNAL

#include "Stl.h" 

#include "TraceLog.h" 
#include "Tools.h" 
#include "Exception.h" 

#include "Criteria.h" 
#include "CriteriaPass.h" 


using namespace brathl;



namespace brathl
{

const string FORMAT_INT_PASS = "%d";

//-------------------------------------------------------------
//------------------- CCriteriaPass class --------------------
//-------------------------------------------------------------


CCriteriaPass::CCriteriaPass() 
{
  Init();
}

//----------------------------------------

CCriteriaPass::~CCriteriaPass()
{

}
//-------------------------------------------------------------
void CCriteriaPass::Init()
{
  m_key = CCriteria::PASS;
}
//----------------------------------------
CCriteriaPass* CCriteriaPass::GetCriteria(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaPass* criteria = dynamic_cast<CCriteriaPass*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaPass::GetCriteria - dynamic_cast<CCriteriaPass*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaPass",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}

//-------------------------------------------------------------
void CCriteriaPass::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteria::Dump(fOut);

  fOut << "==> Dump a CCriteriaPass Object at "<< this << endl;
  fOut << "==> END Dump a CCriteriaPass Object at "<< this << endl;

  fOut << endl;

}

//-------------------------------------------------------------
//------------------- CCriteriaPassString class --------------------
//-------------------------------------------------------------

const string CCriteriaPassString::m_delimiter = ",";



CCriteriaPassString::CCriteriaPassString() 
{
  Init();
}

//-------------------------------------------------------------
CCriteriaPassString::CCriteriaPassString(CCriteriaPassString& c) 
{
  Init();
  Set(c);
}


//-------------------------------------------------------------
CCriteriaPassString::CCriteriaPassString(CCriteriaPassString* c) 
{
  Init();
  Set(*c);
}

//-------------------------------------------------------------
CCriteriaPassString::CCriteriaPassString(const string& passes, const string& delimiter /* = CCriteriaPassString::m_delimiter */) 
{
  Init();
  Set(passes, delimiter);
}

//-------------------------------------------------------------
CCriteriaPassString::CCriteriaPassString(const CStringArray& array) 
{
  Init();
  Set(array);

}

//----------------------------------------

CCriteriaPassString::~CCriteriaPassString()
{

}

//----------------------------------------

void CCriteriaPassString::Init()
{

  SetDefaultValue();

}
//----------------------------------------
const CCriteriaPassString& CCriteriaPassString::operator=(CCriteriaPassString& c)
{

  Set(c);

  return *this;
}
//-------------------------------------------------------------
void CCriteriaPassString::Set(CCriteriaPassString& c) 
{
  Set(c.m_passes);
}


//----------------------------------------
string CCriteriaPassString::GetAsText(const string& delimiter /* = CCriteriaPassString::m_delimiter */)
{

  return m_passes.ToString(delimiter, false);

}


//----------------------------------------
CCriteriaPassString* CCriteriaPassString::GetCriteria(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaPassString* criteria = dynamic_cast<CCriteriaPassString*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaPassString::GetCriteria - dynamic_cast<CCriteriaPassString*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaPassString",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}


//----------------------------------------
void CCriteriaPassString::SetDefaultValue()
{
  m_passes.RemoveAll();
}

//----------------------------------------
bool CCriteriaPassString::IsDefaultValue()
{
  return m_passes.empty();
}

//-------------------------------------------------------------
void CCriteriaPassString::ExtractPass(const string& passes, CStringArray& arrayPass, const string& delimiter /*= CCriteriaPassString::m_delimiter*/)
{

  CStringArray array;
  
  array.ExtractStrings(passes, delimiter);

  CCriteriaPassString::ExtractPass(array, arrayPass);


}
//-------------------------------------------------------------
void CCriteriaPassString::ExtractPass(const CStringArray& array, CStringArray& arrayPass) 
{
  CStringArray::const_iterator it;

  for ( it = array.begin( ); it != array.end( ); it++ )
  {
    string str = CTools::StringTrim(*it);
    if (!(str.empty()))
    {
      arrayPass.Insert(str);
    }
  }
}

//-------------------------------------------------------------
void CCriteriaPassString::Set(const string& passes, const string& delimiter /*= CCriteriaPassString::m_delimiter*/)
{

  CCriteriaPassString::ExtractPass(passes, m_passes, delimiter);

}
//-------------------------------------------------------------
void CCriteriaPassString::Set(const CStringArray& array) 
{
  
  CCriteriaPassString::ExtractPass(array, m_passes);

}

//----------------------------------------

bool CCriteriaPassString::Intersect(const string& passes, CStringArray& intersect)
{
  
  CStringArray array;

  CCriteriaPassString::ExtractPass(passes, array);

  return Intersect(array, intersect);
}

//----------------------------------------
bool CCriteriaPassString::Intersect(CStringArray& passes, CStringArray& intersect)
{
  return m_passes.Intersect(passes, intersect);
}

//----------------------------------------
void CCriteriaPassString::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaPass::Dump(fOut);

  fOut << "==> Dump a CCriteriaPassString Object at "<< this << endl;
  m_passes.Dump(fOut);
  fOut << "==> END Dump a CCriteriaPassString Object at "<< this << endl;

  fOut << endl;

}
//-------------------------------------------------------------
//------------------- CCriteriaPassInt class --------------------
//-------------------------------------------------------------


const string CCriteriaPassInt::m_delimiter = " ";  


CCriteriaPassInt::CCriteriaPassInt() 
{
  Init();
}

//-------------------------------------------------------------
CCriteriaPassInt::CCriteriaPassInt(CCriteriaPassInt& c) 
{
  Init();
  Set(c);
}
//-------------------------------------------------------------
CCriteriaPassInt::CCriteriaPassInt(CCriteriaPassInt* c) 
{
  Init();
  Set(*c);
}

//-------------------------------------------------------------
CCriteriaPassInt::CCriteriaPassInt(int32_t from, int32_t to) 
{
  Init();
  Set(from, to);
}
//-------------------------------------------------------------
CCriteriaPassInt::CCriteriaPassInt(const string& from, const string& to) 
{
  Init();
  Set(from, to);
}

//-------------------------------------------------------------
CCriteriaPassInt::CCriteriaPassInt(const CStringArray& array) 
{
  Init();
  Set(array);

}


//----------------------------------------

CCriteriaPassInt::~CCriteriaPassInt()
{

}

//----------------------------------------

void CCriteriaPassInt::Init()
{

  SetDefaultValue();

}
//----------------------------------------
const CCriteriaPassInt& CCriteriaPassInt::operator=(CCriteriaPassInt& c)
{

  Set(c);

  return *this;
}
//-------------------------------------------------------------
void CCriteriaPassInt::Set(CCriteriaPassInt& c) 
{
  Set(c.m_from, c.m_to);
}

//----------------------------------------
string CCriteriaPassInt::GetAsText(const string& delimiter /* = CCriteriaPassInt::m_delimiter */)
{
  string format = FORMAT_INT_PASS;
  format.append(delimiter);
  format.append(FORMAT_INT_PASS);

  return CTools::Format(format.c_str(), m_from, m_to);

}

//----------------------------------------
void CCriteriaPassInt::SetDefaultValue()
{
  CTools::SetDefaultValue(m_from);
  CTools::SetDefaultValue(m_to);

}
//----------------------------------------
CCriteriaPassInt* CCriteriaPassInt::GetCriteria(CBratObject* ob, bool withExcept /*= true*/)
{
  CCriteriaPassInt* criteria = dynamic_cast<CCriteriaPassInt*>(ob);
  if (withExcept)
  {
    if (criteria == NULL)
    {
      CException e("CCriteriaPassInt::GetCriteria - dynamic_cast<CCriteriaPassInt*>(ob) returns NULL"
                   "object seems not to be an instance of CCriteriaPassInt",
                   BRATHL_LOGIC_ERROR);
      throw (e);
    }
  }
  
  return criteria;

}

//----------------------------------------
bool CCriteriaPassInt::IsDefaultValue()
{
  return (CTools::IsDefaultValue(m_from) || CTools::IsDefaultValue(m_to));
}


//-------------------------------------------------------------
void CCriteriaPassInt::Adjust() 
{
  if (!(CTools::IsDefaultValue(m_from)) && !(CTools::IsDefaultValue(m_to))) 
  {
    if (m_from > m_to)
    {
      int32_t tmp = m_from;
      m_from = m_to;
      m_to = tmp;
    }
  } 
  else if ((CTools::IsDefaultValue(m_from)) && !(CTools::IsDefaultValue(m_to))) 
  {
    m_from = m_to;
  } 
  else if (!(CTools::IsDefaultValue(m_from)) && (CTools::IsDefaultValue(m_to))) 
  {
    m_to = m_from;
  }

}
//-------------------------------------------------------------
void CCriteriaPassInt::SetTo(int32_t to) 
{
  m_to = to;

  Adjust();

}
//-------------------------------------------------------------
void CCriteriaPassInt::SetTo(const string& to)
{
  m_to = CTools::StrToInt(to);

  Adjust();
}
//-------------------------------------------------------------
void CCriteriaPassInt::SetFrom(int32_t from) 
{
  m_from = from;

  Adjust();
}

//-------------------------------------------------------------
void CCriteriaPassInt::SetFrom(const string& from)
{
  m_from = CTools::StrToInt(from);

  Adjust();
}

//-------------------------------------------------------------
void CCriteriaPassInt::Set(int32_t from, int32_t to)
{
  SetFrom(from);
  SetTo(to);

}
//-------------------------------------------------------------
void CCriteriaPassInt::Set(const string& from, const string& to)
{
  Set(CTools::StrToInt(from), CTools::StrToInt(to));

}
//-------------------------------------------------------------
void CCriteriaPassInt::SetFromText(const string& values, const string& delimiter /* = CCriteriaPassInt::m_delimiter */)
{
  CStringArray array;
  array.ExtractStrings(values, delimiter);

  Set(array);

}

//-------------------------------------------------------------

void CCriteriaPassInt::Set(const CStringArray& array) 
{
  switch (array.size()) 
  {
    case 2: 
    {
      Set(array.at(0), array.at(1)); 
      break;
    }
    case 1: 
    {
      Set(array.at(0), array.at(0)); 
      break;
    }
    default:
    {
      break;
    }

  }
}


//----------------------------------------
bool CCriteriaPassInt::Intersect(CStringArray& array, CStringArray& intersect)
{

  CIntArray arrayIn;
  arrayIn.Insert(array);
  CIntArray arrayTmp;
  bool intersectFlag = Intersect(arrayIn, arrayTmp);
  if (intersectFlag) 
  {
    intersect.Insert(array);
  }

  return intersectFlag;

}
//----------------------------------------
bool CCriteriaPassInt::Intersect(CStringArray& array, CIntArray& intersect)
{

  CIntArray arrayIn;
  arrayIn.Insert(array);
  bool intersectFlag = Intersect(arrayIn, intersect);
  if (intersectFlag) 
  {
    intersect.Insert(array);
  }

  return intersectFlag;

}
//----------------------------------------
bool CCriteriaPassInt::Intersect(CIntArray& array, CStringArray& intersect)
{

  CIntArray arrayTmp;
  bool intersectFlag = Intersect(array, arrayTmp);
  if (intersectFlag) 
  {
    intersect.Insert(array);
  }

  return intersectFlag;
  
}
//----------------------------------------
bool CCriteriaPassInt::Intersect(CIntArray& array, CIntArray& intersect)
{
  bool intersectFlag = false;

  switch (array.size()) 
  {
    case 2: 
    {
      intersectFlag = Intersect(array.at(0), array.at(1), intersect); 
      break;
    }
    case 1: 
    {
      intersectFlag = Intersect(array.at(0), array.at(0), intersect); 
      break;
    }
    default:
    {
      break;
    }

  }

  return intersectFlag;
}

//----------------------------------------
bool CCriteriaPassInt::Intersect(int32_t otherFrom, int32_t otherTo, CStringArray& intersect)
{

  CIntArray array;
  bool intersectFlag = Intersect(otherFrom, otherTo, array);
  if (intersectFlag) 
  {
    intersect.Insert(array);
  }

  return intersectFlag;
}

//----------------------------------------
bool CCriteriaPassInt::Intersect(double otherFrom, double otherTo, CIntArray& intersect)
{
  return Intersect(static_cast<int32_t>(otherFrom), static_cast<int32_t>(otherTo), intersect);
}
//----------------------------------------
bool CCriteriaPassInt::Intersect(int32_t otherFrom, int32_t otherTo, CIntArray& intersect)
{
  
  if ((this->IsDefaultValue()) || (CTools::IsDefaultValue(otherFrom)) || (CTools::IsDefaultValue(otherTo))) {
      return false;
  }

  //  this    !-------------!
  //  other                    !-----!
  // OR
  //  this                 !-------------!
  //  other       !-----!
  if ((m_to < otherFrom) || (otherTo < m_from))
  {
    return false;
  }


  //  this    !------------......
  //  other          !-----......
  if (m_from <= otherFrom)  
  {
    intersect.Insert(otherFrom);
  }
  //  this          !---------......
  //  other     !-------------......
  else
  {
    intersect.Insert(m_from);
  }

  //  this    ....--------!
  //  other   ....-------------!
  if (m_to <= otherTo)  
  {
    intersect.Insert(m_to);
  }
  //  this    ....-------------!
  //  other   ....--------!
  else
  {
    intersect.Insert(otherTo);
  }

  return true;
}
//----------------------------------------
bool CCriteriaPassInt::Intersect(const string& from, const string& to, CStringArray& intersect)
{
  return Intersect(CTools::StrToInt(from), CTools::StrToInt(to), intersect);

}
//----------------------------------------
bool CCriteriaPassInt::Intersect(const string& from, const string& to, CIntArray& intersect)
{
  return Intersect(CTools::StrToInt(from), CTools::StrToInt(to), intersect);

}


//----------------------------------------
void CCriteriaPassInt::Dump(ostream& fOut /* = cerr */)
{
  if (CTrace::IsTrace() == false)
  {
    return;
  }

  CCriteriaPass::Dump(fOut);

  fOut << "==> Dump a CCriteriaPassInt Object at "<< this << endl;
  fOut << "m_from "<< m_from << endl;
  fOut << "m_to "<< m_to << endl;
  fOut << "==> END Dump a CCriteriaPassInt Object at "<< this << endl;

  fOut << endl;

}

} // end namespace
