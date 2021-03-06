#include "GlobalDialList.h"

int GlobalDialList::EnumFromNameAndType(std::string name, int type){

  // Setup Type Container
  if (fTypeEnumCont.find(type) == fTypeEnumCont.end()){
    std::map<std::string, int> temp;
    temp[name] = 0;

    fTypeEnumCont[type] = temp;
  }

  // Get Type Container
  std::map<std::string, int> enumcont = fTypeEnumCont[type];
  //  LOG(FIT) << "Getting Enum From name and type " << name << " " << type << std::endl;

  // Check name is in container, if its not add it
  if (enumcont.find(name) == enumcont.end()){
    //    LOG(FIT) << " Name not found in Enum type " << type << std::endl;
    int index = enumcont.size();
    enumcont[name] = index;
    //    LOG(FIT) << "Returning new index of " << index << std::endl;
    fTypeEnumCont[type][name] = index;
    return index;
  }

  return enumcont[name];
}

void GlobalDialList::RegisterDialEnum(std::string name, int type, int nuisenum){

  if (std::find(fAllDialNames.begin(), fAllDialNames.end(), name) != fAllDialNames.end()){
    return;
  }

  LOG(FIT) << "Registed Dial Enum : " << name << " " << type << " " << nuisenum << std::endl;
  fAllDialNames.push_back(name);
  fAllDialTypes.push_back(type);
  fAllDialEnums.push_back(nuisenum);
}




/// Singleton functions
GlobalDialList& Reweight::DialList(){
  return GlobalDialList::Get();
}

GlobalDialList* GlobalDialList::m_diallistInstance = NULL;

GlobalDialList& GlobalDialList::Get(void){
  if (!m_diallistInstance) m_diallistInstance = new GlobalDialList;
  return *m_diallistInstance;
}

