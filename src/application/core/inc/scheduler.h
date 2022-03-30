#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

typedef struct scheduler_diagnostics
{
    int handlers_called_max;
    int event_queue_full;
} scheduler_diagnostics;


void scheduler_init(void);
void scheduler_loop(void);
int scheduler_check_events(void);
int scheduler_space(void);
const scheduler_diagnostics *scheduler_get_diagnostics(void);

#endif /* _SCHEDULER_H_ */
