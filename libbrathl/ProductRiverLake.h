/*
 * 
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
#if !defined(_ProductRiverLake_h_)
#define _ProductRiverLake_h_

#include "Product.h"
using namespace brathl;

namespace brathl
{


/** \addtogroup product Products
  @{ */

/** 
  River & Lake product management class.


 \version 1.0
*/

class CProductRiverLake : public CProduct
{
	DECLARE_BASE_TYPE( CProduct );

public:

  static const std::string m_TIME_NAME;
  static const std::string m_TIME_DESC;
  static const std::string m_TIME_UNIT;

  static const std::string m_YEAR_NAME;
  static const std::string m_MONTH_NAME;
  static const std::string m_DAY_NAME;
  static const std::string m_HOUR_NAME;
  static const std::string m_MINUTE_NAME;

  static const std::string m_PROD_TYPE_RLH;
  static const std::string m_PROD_TYPE_RLA;

  /// Empty CProductRiverLake ctor
  CProductRiverLake();

  
  /** Creates new CProductRiverLake object
    \param fileName [in] : file name to be connected */
  CProductRiverLake(const std::string& fileName);
  
  /** Creates new CProductRiverLake object
    \param fileNameList [in] : list of file to be connected */
  CProductRiverLake( const CStringList &fileNameList, bool check_only_first_file );

  /// Destructor
  virtual ~CProductRiverLake();

  virtual void InitCriteriaInfo();

  ///Dump fonction
  virtual void Dump(std::ostream& fOut = std::cerr);


protected:

  virtual void InitDateRef() override;

  virtual bool Open() override;

  virtual void InitInternalFieldNamesForCombinedVariable( CStringList &listField, const std::string &record ) override;

  virtual void ReadBratFieldRecord( CField::CListField::iterator it, bool& skipRecord ) override;


private:

  void Init();


};

/** @} */

} //end namespace

#endif // !defined(_ProductRiverLake_h_)
