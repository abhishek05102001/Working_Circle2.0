#include "variables.h"


volatile struct SendFlags sendFlags = {
    false, // BootNotification_sendflag
    false, // StatusNotification_sendflag
    false, // StatusNotification_sendflag2
    false, // HeartBeat_sendflag
    false, // MeterValues_sendflag
    false, // RemoteStart_sendflag
    false, // StartTransaction_sendflag
    false, // RemoteStop_sendflag
    false, // StopTransaction_sendflag
    false  // send_offline_transactions_flag
};

volatile struct RecvFlags recvFlags = {
    false, // BootNotification_recflag
    false, // StatusNotification_recflag
    false, // StatusNotification_recflag2
    false, // HeartBeat_recflag
    false, // MeterValues_recflag
    false, // RemoteStart_recflag
    false, // StartTransaction_recflag
    false, // RemoteStop_recflag
    false  // StopTransaction_recflag
};

volatile bool waiting_for_remotestart = true;
volatile bool start_energymeter = false;
volatile bool offline_charge = false;
volatile bool online_charge = false;
volatile bool Internet_connected = false;
