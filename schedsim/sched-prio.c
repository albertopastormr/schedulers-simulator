#include "sched.h"

static task_t* pick_next_task_prio(runqueue_t* rq)
{
	task_t* t=head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */
	if (t) 
		remove_slist(&rq->tasks,t);

	return t;
}

static int compare_tasks_prio(void *t1,void *t2)
{
	task_t* tsk1=(task_t*)t1;
	task_t* tsk2=(task_t*)t2;
	return tsk1->prio - tsk2->prio;
}

static void enqueue_task_prio(task_t* t,runqueue_t* rq, int preempted)
{

	if (t->on_rq || is_idle_task(t))
		return;

	sorted_insert_slist(&rq->tasks, t, 1, compare_tasks_prio);  //Push task

    /* If the task was not using the current CPU, check whether a preemption is in order or not */
	if (!preempted) {
		task_t* current=rq->cur_task;

		/* Trigger a preemption if this task has a shorter priority than current */
		if (preemptive_scheduler && t->runnable_ticks_left < current->runnable_ticks_left) {
			rq->need_resched=TRUE;
			current->flags|=TF_INSERT_FRONT; /* To avoid unfair situations in the event
                                            	   another task with the same length wakes up as well*/
		}
	}
}

static task_t* steal_task_prio(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t) 
		remove_slist(&rq->tasks,t);
	
	return t;
}


sched_class_t prio_sched= {
	.pick_next_task=pick_next_task_prio,
	.enqueue_task=enqueue_task_prio,
	.steal_task=steal_task_prio
};
