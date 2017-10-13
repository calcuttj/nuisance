// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "FitLogger.h"

#include "FitEvent.h"

#include "PhysConst.h"

#include "WeightEngineBase.h"

#include "WeightUtils.h"

#include <cmath>

class EventTypeWeightEngine : public WeightEngineBase {
  enum params {

    WeightCC0Pi_idx = 0,
    WeightCC1Pi_idx,
    WeightCCOther_idx,
  };


  //******************************* Event Type params ******************************
  double WeightCC0Pi;
  double WeightCC1Pi;
  double WeightCCOther;

  /// Holds current value of oscillation parameters.
  double params[3];

 public:
  EventTypeWeightEngine();

  /// Configures Event Weight parameters from input xml file.
  ///
  /// Event type parameters configured from EventTypeParam XML element as:
  /// <nuisance>
  /// <EventTypeParam WeightCC0Pi="XX" WeightCC1Pi="XX" WeightCCOther="XX"/>
  /// </nuisance>

  void Config();

  //Returning weights
  double GetWeightCC0Pi(void);
  double GetWeightCC1Pi(void);
  double GetWeightCCOther(void);

  // Functions requiring Override
  void IncludeDial(std::string name, double startval);

  void SetDialValue(int nuisenum, double val);
  void SetDialValue(std::string name, double val);

  bool IsDialIncluded(std::string name);
  bool IsDialIncluded(int nuisenum);

  double GetDialValue(std::string name);
  double GetDialValue(int nuisenum);

  void Reconfigure(bool silent);

  bool NeedsEventReWeight();

  double CalcWeight(BaseFitEvt* evt);
  double CalcWeight(double ENu, int PDGNu, int TargetPDGNu = -1);

  static int SystEnumFromString(std::string const& name);

  void Print();
};
