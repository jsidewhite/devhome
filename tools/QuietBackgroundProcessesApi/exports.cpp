#include "pch.h"

// stl
#include <algorithm>
#include <condition_variable>
#include <chrono>
#include <future>
#include <mutex>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_set>
#include <vector>

// windows
//#include <nt.h>
//#include <ntrtl.h>
//#include <nturtl.h>
#include "windows.h"

// wil
//#include <wil/com.h>
//#include <wrl/implements.h>
//#include <wil/resultmacros.h>
//#include <wil/resource.h>
//#include <wil/result.h>
//#include <wil/token_helpers.h>
//#include <wil/win32_helpers.h>

// other
#include <shellapi.h>
#include "windows.h"
#include <ActivityCoordinator.h>
#include "werapi.h"

#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"
#include "propkey.h"
//#include "propkeyp.h"

// crm
//#include <resourcemanagercrm.h>

// wnf stuff
//#include "power.h"
//#include "wnfnamesp.h"

// power stuff
//#include "ntpoapi_p.h"
#include "powrprof.h" // DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS

// power saver (RM_QUIET_MODE_DATA)
//#include "crmtypes.h"

// process stuff
#include <tlhelp32.h>

// battery stuff
#include "poclass.h"


extern "C" HRESULT SetGameMode(bool enable)
{
    int pid = (int)enable;
    //THROW_IF_NTSTATUS_FAILED(RtlPublishWnfStateData(WNF_RM_GAME_MODE_ACTIVE, NULL, &pid, sizeof(pid), NULL));
    return E_FAIL;
}
