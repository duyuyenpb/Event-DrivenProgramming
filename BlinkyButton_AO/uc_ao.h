#ifndef UC_AO_H
#define UC_AO_H

#include "ucos_ii.h" /* uC/OS-II API, port and compile-time configuration */

/*---------------------------------------------------------------------------*/
/* Event facilities... */

typedef uint16_t Signal; /* event signal */

enum ReservedSignals
{
    INIT_SIG, /* dispatched to AO before entering event-loop */
    USER_SIG  /* first signal available to the users */
};

/* Event base class */
typedef struct
{
    Signal sig; /* event signal */
    /* event parameters added in subclasses of Event */
} Event;

/*---------------------------------------------------------------------------*/
/* Actvie Object facilities... */

typedef struct Active Active; /* forward declaration */

typedef void (*DispatchHandler)(Active *const me, Event const *const e);

/* Active Object base class */
struct Active {
    INT8U thread;    /* private thread (the unique uC/OS-II task priority) */
    OS_EVENT *queue; /* private message queue */

    DispatchHandler dispatch; /* pointer to the dispatch() function */

    /* active object data added in subclasses of Active */
};

void Active_ctor(Active *const me, DispatchHandler dispatch);
void Active_start(Active *const me,
                  uint8_t prio, /* priority (1-based) */
                  Event **queueSto,
                  uint32_t queueLen,
                  void *stackSto,
                  uint32_t stackSize,
                  uint16_t opt);
void Active_post(Active *const me, Event const *const e);

/*---------------------------------------------------------------------------*/
/* Time Event facilities... */

/* Time Event class */
typedef struct {
    Event super;       /* inherit Event */
    Active *act;       /* the AO that requested this TimeEvent */
    uint32_t timeout;  /* timeout counter; 0 means not armed */
    uint32_t interval; /* interval for periodic TimeEvent, 0 means one-shot */
} TimeEvent;

void TimeEvent_ctor(TimeEvent *const me, Signal sig, Active *act);
void TimeEvent_arm(TimeEvent *const me, uint32_t timeout, uint32_t interval);
void TimeEvent_disarm(TimeEvent *const me);

/* static (i.e., class-wide) operation */
void TimeEvent_tick(void);

/*---------------------------------------------------------------------------*/
/* Assertion facilities... */

#define Q_ASSERT(check_)                   \
    if (!(check_))                         \
    {                                      \
        Q_onAssert(this_module, __LINE__); \
    }                                      \
    else                                   \
        (void)0

void Q_onAssert(char const *module, int loc);

#endif /* UC_AO_H */
