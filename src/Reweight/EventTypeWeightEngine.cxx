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

//#define DEBUG_OSC_WE

#include "EventTypeWeightEngine.h"

#include <limits>

/*enum nuTypes {
  kNuebarType = -1,
  kNumubarType = -2,
  kNutaubarType = -3,
  kNueType = 1,
  kNumuType = 2,
  kNutauType = 3,
};

nuTypes GetNuType(int pdg) {
  switch (pdg) {
    case 16:
      return kNutauType;
    case 14:
      return kNumuType;
    case 12:
      return kNueType;
    case -16:
      return kNutaubarType;
    case -14:
      return kNumubarType;
    case -12:
      return kNuebarType;
    default: { THROW("Attempting to convert \"neutrino pdg\": " << pdg); }
  }
}*/

EventTypeWeightEngine::EventTypeWeightEngine()
    :
      WeightCC0Pi(1.0),
      WeightCC1Pi(1.0),
      WeightCCOther(1.0){
  Config();
}

double EventTypeWeightEngine::GetWeightCC0Pi(void){
  return WeightCC0Pi;
}

double EventTypeWeightEngine::GetWeightCC1Pi(void){
  return WeightCC1Pi;
}

double EventTypeWeightEngine::GetWeightCCOther(void){
  return WeightCCOther;
}

void EventTypeWeightEngine::Config() {
  std::vector<nuiskey> EventTypeParam = Config::QueryKeys("parameter");

  if (EventTypeParam.size() < 1) {
    ERROR(WRN,
          "Event Type parameters specified but no EventTypeParam element "
          "configuring the experimental characteristics found. Pausing for "
          "10...");
    sleep(10);
    return;
  }
  QLOG(FIT, "N keys: " << EventTypeParam.size());
  QLOG(FIT, "name: " << EventTypeParam[0].GetS("name"));
  QLOG(FIT, "name: " << EventTypeParam[1].GetS("name"));
  QLOG(FIT, "name: " << EventTypeParam[2].GetS("name"));

/*  WeightCC0Pi = EventTypeParam[0].Has("WeightCC0Pi") ? EventTypeParam[0].GetD("WeightCC0Pi") : WeightCC0Pi;

  WeightCC1Pi = EventTypeParam[0].Has("WeightCC1Pi") ? EventTypeParam[0].GetD("WeightCC1Pi") : WeightCC1Pi;

  WeightCCOther = EventTypeParam[0].Has("WeightCCOther") ? EventTypeParam[0].GetD("WeightCCOther") : WeightCCOther;
  */
 
  for(int i = 0; i < EventTypeParam.size();++i){
    if(EventTypeParam[i].GetS("type") != "eventtype_parameter") continue;
    double nom = EventTypeParam[i].GetD("nominal");
    IncludeDial(EventTypeParam[i].GetS("name"), nom);
    SetDialValue(EventTypeParam[i].GetS("name"), nom);
  }
  QLOG(FIT, "Configured event type weighter:");
//Change below to if size < 1
/*  params[0] = WeightCC0Pi;
  params[1] = WeightCC1Pi;
  params[2] = WeightCCOther;
*/
  QLOG(FIT, "\tWeightCC0Pi;  : " << params[0]);
  QLOG(FIT, "\tWeightCC1Pi;  : " << params[1]);
  QLOG(FIT, "\tWeightCCOther;: " << params[2]);

}

void EventTypeWeightEngine::IncludeDial(std::string name, double startval) {
#ifdef DEBUG_EVENTTYPE_WE
  std::cout << "IncludeDial: " << name << " at " << startval << std::endl;
#endif
  int dial = SystEnumFromString(name);
  if (!dial) {
    THROW("EventTypeWeightEngine passed dial: " << name
                                          << " that it does not understand.");
  }
  params[dial - 1] = startval;
}

void EventTypeWeightEngine::SetDialValue(int nuisenum, double val) {
#ifdef DEBUG_EVENTTYPE_WE
  std::cout << "SetDial: " << (nuisenum % 1000) << " at " << val << std::endl;
#endif
  fHasChanged = (params[(nuisenum % 1000) - 1] - val) >
                std::numeric_limits<double>::epsilon();
  params[(nuisenum % 1000) - 1] = val;
}
void EventTypeWeightEngine::SetDialValue(std::string name, double val) {
#ifdef DEBUG_EVENTTYPE_WE
  std::cout << "SetDial: " << name << " at " << val << std::endl;
#endif
  int dial = SystEnumFromString(name);
  if (!dial) {
    THROW("EventTypeWeightEngine passed dial: " << name
                                          << " that it does not understand.");
  }

  fHasChanged =
      (params[dial - 1] - val) > std::numeric_limits<double>::epsilon();
  params[dial - 1] = val;
}

bool EventTypeWeightEngine::IsDialIncluded(std::string name) {
  return SystEnumFromString(name);
}
bool EventTypeWeightEngine::IsDialIncluded(int nuisenum) {
  return ((nuisenum % 1000) > 0) && ((nuisenum % 1000) < 6);
}

double EventTypeWeightEngine::GetDialValue(std::string name) {
  int dial = SystEnumFromString(name);
  if (!dial) {
    THROW("EventTypeWeightEngine passed dial: " << name
                                          << " that it does not understand.");
  }
  return params[dial - 1];
}
double EventTypeWeightEngine::GetDialValue(int nuisenum) {
  if (!(nuisenum % 1000) || (nuisenum % 1000) > 3) {
    THROW("EventTypeWeightEngine passed dial enum: "
          << (nuisenum % 1000)
          << " that it does not understand, expected [1,3].");
  }
  return params[(nuisenum % 1000) - 1];
}

void EventTypeWeightEngine::Reconfigure(bool silent) { fHasChanged = false; };

bool EventTypeWeightEngine::NeedsEventReWeight() {
  if (fHasChanged) {
    return true;
  }
  return false;
}

double EventTypeWeightEngine::CalcWeight(BaseFitEvt* evt) {
/*  static bool Warned = false;
  if (evt->probe_E == 0xdeadbeef) {
    if (!Warned) {
      ERROR(WRN,
            "EventType weights asked for but using 'litemode' or "
            "unsupported generator input. Pasuing for 10...");
      sleep(10);
      Warned = true;
    }
    return 1;
  }

  return CalcWeight(evt->probe_E * 1E-3, evt->probe_pdg);*/
  return 1;
}

double EventTypeWeightEngine::CalcWeight(double ENu, int PDGNu, int TargetPDGNu) {
/*  if (LengthParam == 0xdeadbeef) {  // not configured.
    return 1;
  }
#ifdef __PROB3PP_ENABLED__
  int NuType = (ForceFromNuPDG != 0) ? ForceFromNuPDG : GetNuType(PDGNu);
  bp.SetMNS(params[theta12_idx], params[theta13_idx], params[theta23_idx],
            params[dm12_idx], params[dm23_idx], params[dcp_idx], ENu, true,
            NuType);

  int pmt = 0;
  double prob_weight = 1;
  TargetPDGNu = (TargetPDGNu == -1) ? (TargetNuType ? TargetNuType : NuType)
                                    : GetNuType(TargetPDGNu);

  if (LengthParamIsZenith) {  // Use earth density
    bp.DefinePath(LengthParam, 0);
    bp.propagate(NuType);
    pmt = 0;
    prob_weight = bp.GetProb(NuType, TargetPDGNu);
  } else {
    if (constant_density != 0xdeadbeef) {
      bp.propagateLinear(NuType, LengthParam, constant_density);
      pmt = 1;
      prob_weight = bp.GetProb(NuType, TargetPDGNu);
    } else {
      pmt = 2;
      prob_weight =
          bp.GetVacuumProb(NuType, TargetPDGNu, ENu * 1E-3, LengthParam);
    }
  }
#ifdef DEBUG_OSC_WE
  if (prob_weight != prob_weight) {
    THROW("Calculated bad prob weight: " << prob_weight << "(Osc Type: " << pmt
                                         << " -- " << NuType << " -> "
                                         << TargetPDGNu << ")");
  }
  if (prob_weight > 1) {
    THROW("Calculated bad prob weight: " << prob_weight << "(Osc Type: " << pmt
                                         << " -- " << NuType << " -> "
                                         << TargetPDGNu << ")");
  }

  std::cout << NuType << " -> " << TargetPDGNu << ": " << ENu << " = "
            << prob_weight << "%%." << std::endl;
#endif
  return prob_weight;
#else
  return 1;
#endif
*/
return 1;
}

int EventTypeWeightEngine::SystEnumFromString(std::string const& name) {
  if (name == "WeightCC0Pi") {
    return 1;
  } else if (name == "WeightCC1Pi") {
    return 2;
  } else if (name == "WeightCCOther") {
    return 3; 
  } else {
    return 0;
  }
}

void EventTypeWeightEngine::Print() {
  std::cout << "EventTypeWeightEngine: " << std::endl;

  std::cout << "\t WeightCC0Pi: " << params[WeightCC0Pi_idx] << std::endl;
  std::cout << "\t WeightCC1Pi: " << params[WeightCC1Pi_idx] << std::endl;
  std::cout << "\t WeightCCOther: " << params[WeightCCOther_idx] << std::endl;
}
