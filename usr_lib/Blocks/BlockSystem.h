#ifndef BlockSystem_h
#define BlockSystem_h

#include <limits.h>
#include "BlockBase.h"

/*********************************************************************/
#define B_TMR_TYPE_ONTIMER     0  //just call the onTimer() method on listener block
#define B_TMR_TYPE_JOB         1  //call trigger() method
#define B_TMR_TYPE_JOB_FOLLOW  2  //call trigger() method and follow to 'follow_job' pointer for next job

#define B_TMR_MODE_SINGLESHOT      0
#define B_TMR_MODE_CONTINUOUS      1

#define B_TMR_FLAGS_SINGLESHOT        0x01
#define B_TMR_FLAGS_RUNNING           0x02
#define B_TMR_FLAGS_JOB               0x04
#define B_TMR_FLAGS_JOB_FOLLOW        0x08

#define B_EVT_RTC_SECOND        0x0001
#define B_EVT_RTC_MINUTE        0x0002
#define B_EVT_RTC_HOUR          0x0004
#define B_EVT_RTC_DAY           0x0008
#define B_EVT_CRON_JOB          0x0010
#define B_EVT_TIMEVALUE_CHANGE  0x0020
#define B_EVT_PULSE_CHANGE      0x0040
#define B_EVT_TEMPERATURE       0x0080
#define B_EVT_CUSTOM_0          0x1000
#define B_EVT_CUSTOM_1          0x2000
#define B_EVT_CUSTOM_2          0x4000
#define B_EVT_CUSTOM_3          0x8000


/** Timer to be used as system timer inside BlockSystem.
 * Number of timers in not limited. They are added into system in a 'linked list' manner.\n
 * A timer can work in two different modes: \b singleshot or \b continuous.\n
 * If timer is lisened by a Block (first constructor), it can be one of three types (see \c BlockTimer()).\n
 * Timer can be used also for any purpose (second constructor). In this case the \c callback() function will be called on every timeout.
 */
class BlockTimer
{
public:
	uint8_t flags;
    BlockBase* listener = NULL;
	void (* callback)(void);
	unsigned long millis_start;
	unsigned long millis_interval;
    BlockTimer* next; //connect timers in a chain
	
    /** Constructor.
     * @param [in] listener Provides a pointer to listener Block.
     * @param [in] type Select timer type:\n
     * \b B_TMR_TYPE_ONTIMER will call listener Block's \c onTimer() method on every timeout\n
     * \b B_TMR_TYPE_JOB will call listener Block's \c trigger() method on every timeout\n
     * \b B_TMR_TYPE_JOB_FOLLOW will send the \c BlockSpider() to listener Block on every timeout
     */
	BlockTimer(BlockBase* listener, uint8_t type = B_TMR_TYPE_ONTIMER);

    /** Constructor for custom timer usage.
     * @param [in] callback Function to be called on every timeout.*/
	BlockTimer(void (* callback)(void) = NULL);
    
    /** Start the timer.
     * @param [in] interval_ms Set timer interval in milliseconds.
     * @param [in] mode Select \c B_TMR_MODE_SINGLESHOT (default) or \c B_TMR_MODE_CONTINUOUS mode.*/
    void start(unsigned long interval_ms, uint8_t mode = B_TMR_MODE_SINGLESHOT);
    /** Stop the timer. */
	void stop(void);
    inline bool isJob(void);
    inline bool isJobFollow(void);
};

/**********************************************************************/
#define CLOCK_1S_PERIOD        1000U

/** Providing common functionalities: \b timers, \b events, \b reboot.
 * A new \b TIMER can be created and associated with a Block by \c newTimer() method.\n
 * An \b EVENT listener Block can be added by \c addEventListener() method. Blocks are signaling the events by \c setEvent() method.\n
 */

class BlockSystem
{
    /* System TIMERS */
    BlockTimer* timers_llst; //chain of BlockTimer instances

    /* System EVENTS */
    event_t event_flags;
    event_t event_flags_waiting;
    BlockBase* event_listener_first;

	unsigned long millis_last;

public:
    /** Constructor for custom timer usage.*/
    BlockSystem(void);
    //virtual ~BlockSystem(void);
    
    /** Add a new event listener Block.
     * Blocks are added to a 'chain' of event listeners in 'linked list' manner.
     * During the \c update() cycle the \c BlockBase::onEvent() method of each listener Block is called. The listeners are called in the same order as they were added to the BlockSystem.
     * @param [in] listener Pointer to listener Block.
     * @param [in] catching_events The event flag(s) to be caught by the listener Block. This value is stored in \c BlockBase::catching_events of the listener.
     * */

    void addEventListener(BlockBase* listener, event_t catching_events);
    /** Create a new timer and associate it with a Block.
     * Timer are added to a 'chain' in a 'linked list' manner.
     * During the \c update() cycle the corresponding method (depending on BlockTimer type) of each listener Block is called. The timers downcounters are calculated in same order as they were added to the BlockSystem.
     * @param [in] listener Pointer to listener (owner) Block.
     * @param [in] type Type of the timer. @see BlockTimer::BlockTimer()
     * */
    BlockTimer* newTimer(BlockBase* listener, uint8_t type = B_TMR_TYPE_ONTIMER);  //create new BlockTimer and connect it to belonging Block

    /** Create a general purpose timer.
     * @param [in] callback Pointer to the callback function to be called on timeout.
     * */
    BlockTimer* newTimer(void (* callback)(void));  //create new BlockTimer and give him the callback function
    
    /** The heart of BlockSystem.
     * This method must be called periodically in order to run timers and call event listeners.\n
     * Put it in the \c loop() function in Arduino project.
     * */
    void update(void); //mandatory; put it in infinte loop in order to keep Timer-s and Clock up to date and trigger events onTimer,onClock
    
    /** This method is used by Blocks to signal a specific event. */
    void setEvent(event_t event_flag);
    
    /** Reboot the adruino.*/
    static void reboot(void);
};
/**********************************************************************/

#endif
