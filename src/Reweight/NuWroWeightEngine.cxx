#include "NuWroWeightEngine.h"

NuWroWeightEngine::NuWroWeightEngine(std::string name) {
#ifdef __NUWRO_REWEIGHT_ENABLED__

  // Setup the NEUT Reweight engien
  fCalcName = name;
  LOG(FIT) << "Setting up NuWro RW : " << fCalcName << std::endl;

  // Create RW Engine suppressing cout
  StopTalking();

  // Create the engine
  fNuwroRW = new nuwro::rew::NuwroReWeight();

  // Get List of Veto Calcs (For Debugging)
  std::string rw_engine_list =
      FitPar::Config().GetParS("FitWeight.fNuwroRW_veto");
  bool xsec_qel = rw_engine_list.find("nuwro_QEL") == std::string::npos;
  bool xsec_flag = rw_engine_list.find("nuwro_FlagNorm") == std::string::npos;
  bool xsec_res = rw_engine_list.find("nuwro_RES") == std::string::npos;

  // Add the RW Calcs
  if (xsec_qel)
    fNuwroRW->AdoptWghtCalc("nuwro_QEL", new nuwro::rew::NuwroReWeight_QEL);
  if (xsec_flag)
    fNuwroRW->AdoptWghtCalc("nuwro_FlagNorm",
                            new nuwro::rew::NuwroReWeight_FlagNorm);
  if (xsec_res)
    fNuwroRW->AdoptWghtCalc("nuwro_RES", new nuwro::rew::NuwroReWeight_SPP);

  // Set Abs Twk Config
  fIsAbsTwk = (FitPar::Config().GetParB("setabstwk"));

  // allow cout again
  StartTalking();
#else
  ERR(FTL) << "NUWRO RW NOT ENABLED! " << std::endl;
#endif
};

void NuWroWeightEngine::IncludeDial(std::string name, double startval) {
#ifdef __NUWRO_REWEIGHT_ENABLED__

  // Get RW Enum and name
  int nuisenum = Reweight::ConvDial(name, kNUWRO);

  // Initialise new vector
  fEnumIndex[nuisenum];
  fNameIndex[name];

  // Split by commas
  std::vector<std::string> allnames = GeneralUtils::ParseToStr(name, ",");
  for (uint i = 0; i < allnames.size(); i++) {
    std::string singlename = allnames[i];

    // Get Syst
    nuwro::rew::NuwroSyst_t gensyst = nuwro::rew::NuwroSyst::FromString(name);

    // Fill Maps
    int index = fValues.size();
    fValues.push_back(0.0);
    fNUWROSysts.push_back(gensyst);

    // Initialise Dial
    LOG(FIT) << "Adding NuWro Syst " << fNUWROSysts[index] << std::endl;
    fNuwroRW->Systematics().Add(fNUWROSysts[index]);

    if (fIsAbsTwk) {
      nuwro::rew::NuwroSystUncertainty::Instance()->SetUncertainty(
          fNUWROSysts[index], 1.0, 1.0);
    }

    // Setup index
    fEnumIndex[nuisenum].push_back(index);
    fNameIndex[name].push_back(index);
  }

  // Set Value if given
  if (startval != -999.9) {
    SetDialValue(name, startval);
  }
#endif
};

void NuWroWeightEngine::SetDialValue(int nuisenum, double val) {
#ifdef __NUWRO_REWEIGHT_ENABLED__
  std::vector<size_t> indices = fEnumIndex[nuisenum];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fNuwroRW->Systematics().SetSystVal(fNUWROSysts[indices[i]], val);
  }
#endif
}

void NuWroWeightEngine::SetDialValue(std::string name, double val) {
#ifdef __NUWRO_REWEIGHT_ENABLED__
  std::vector<size_t> indices = fNameIndex[name];
  for (uint i = 0; i < indices.size(); i++) {
    fValues[indices[i]] = val;
    fNuwroRW->Systematics().SetSystVal(fNUWROSysts[indices[i]], val);
  }
#endif
}

void NuWroWeightEngine::Reconfigure(bool silent) {
#ifdef __NUWRO_REWEIGHT_ENABLED__
  // Hush now...
  if (silent) StopTalking();

  // Reconf
  fNuwroRW->Reconfigure();

  // Shout again
  if (silent) StartTalking();
#endif
}

double NuWroWeightEngine::CalcWeight(BaseFitEvt* evt) {
  double rw_weight = 1.0;

#ifdef __NUWRO_REWEIGHT_ENABLED__
  // Skip Non GENIE
  if (evt->fType != kNUWRO) return 1.0;

#ifdef __USE_NUWRO_SRW_EVENTS__
  rw_weight = fNuwroRW->CalcWeight(evt->fNuwroSRWEvent, *evt->fNuwroParams);
#else
  // Call Weight calculation
  rw_weight = fNuwroRW->CalcWeight(evt->fNuwroEvent);
#endif
#endif

  // Return rw_weight
  return rw_weight;
}
