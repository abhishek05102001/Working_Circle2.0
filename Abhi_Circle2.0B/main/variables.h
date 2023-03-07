#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdbool.h>

struct SendFlags {
    bool BootNotification_sendflag;
    bool StatusNotification_sendflag;
    bool StatusNotification_sendflag2;
    bool HeartBeat_sendflag;
    bool MeterValues_sendflag;
    bool RemoteStart_sendflag;
    bool StartTransaction_sendflag;
    bool RemoteStop_sendflag;
    bool StopTransaction_sendflag;
    bool send_offline_transactions_flag;
};

struct RecvFlags {
    bool BootNotification_recflag;
    bool StatusNotification_recflag;
    bool StatusNotification_recflag2;
    bool HeartBeat_recflag;
    bool MeterValues_recflag;
    bool RemoteStart_recflag;
    bool StartTransaction_recflag;
    bool RemoteStop_recflag;
    bool StopTransaction_recflag;
};

extern volatile bool waiting_for_remotestart;
extern volatile bool start_energymeter;
extern volatile bool offline_charge;
extern volatile bool online_charge;
extern volatile bool Internet_connected;

#endif
