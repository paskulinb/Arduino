#ifndef BCron_h
#define BCron_h

#include "BlockSystem.h"
#include "BRtc.h"
#include "BlockBase.h"

/** This class is using the idea of Linux Cron.
 *  It takes a string as input parameter according to Linux Cron specifications:
 *  "* * * * *" === [minute] [hour] [day] [month] [weekday]\n
 *  It supports \b lists, \b ranges and \b steps.\n
 *  \li List example: "2,15,22,54 * * * *"
 *  \li Range example: "15-35 * * * *"
 *  \li Step example: "0/5 12-22/2 * * *"
 *
 *  BCron is event driven Block. It automatically binds to \c B_EVT_RTC_MINUTE event, signaled by BRtc.
 *  If BCron is initialized with \c send_event=TRUE, it will signal the \c B_EVT_CRON_JOB event when cron time happens.
 *  If \c job_follow is set, BCron will call \c BlockSpider(job_follow) to trigger following Block.
 */
class BCron : public BlockBase
{
private:
    enum T_unit {MINUTE, HOUR, DAY, MONTH, WDAY};

    struct Range {
        Range();
        int8_t r1;     //(default = 0)
        int8_t r2;     //(default = -1 -> undefined; means max. value of certain unit (23 hours or 59 minutes) in case of defined 'step')
        uint8_t step;  //(default = 0)
    };

    struct BCronUnits {   // * * * * *
        Range* RLists[5] = {NULL,NULL,NULL,NULL,NULL};
        uint8_t RListsLength[5] = {0,0,0,0,0};
    };

    BCronUnits Units;
    BlockSystem* system;
    BRtc* rtc;
    bool send_event;

public:
    /** \c output[0]=1 during Cron job time, otherwise \c 0.*/
    float output[1];

	/** Constructor.
     *  BCron needs the BlockSystem to use events, and BRtc for time reference.
	 *  @param [in] system Pointer to BlockSystem object.
	 *  @param [in] rtc Pointer to BRtc object.
	 *  @param [in] send_event If \c true, BCron will signal \c B_EVT_CRON_JOB event.
	 */
    BCron(BlockSystem* system, BRtc* rtc, bool send_event = false);
    
private:
    /** Check if current BRtc time complies to Cron job setting.
     *  @return \c true, if time complies, \c false otherwise
     */
    int8_t checkNow(void); //return 1 (true) when DateTime complies to Cron job init, 0 (false) otherwise
    
public:
    /** Configures the BCron datetime filter.
	 *  @param [in] cron_str String with cron definition. (eg.: "* * * * *" for every minute cron job)
     */
    bool set(char* cron_str);

private:
    /** Event handler catching \c B_EVT_RTC_MINUTE event.
     */
    void onEvent(event_t event_flags);
};
#endif
