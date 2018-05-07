#include "sched.h"

/* Global multiRR quantum parameter */
int multirr_quantum=3;
const int global_max_slice = 6;

/* Structure to store RR thread-specific fields */
struct multirr_data {
	int remaining_ticks_slice;
	int current_slice;
	int quantum_exhausted; /*1=cuanto agotado, 0=cuanto no agotado, -1=recien creada*/
	int max_slice;
};

static int task_new_multirr(task_t* t)
{
	struct multirr_data* cs_data=malloc(sizeof(struct multirr_data));

	if (!cs_data)
		return 1;  /* Cannot reserve memory */

	/* initialize the quantum */
	cs_data->remaining_ticks_slice=multirr_quantum;
	cs_data->current_slice=multirr_quantum;
	cs_data->quantum_exhausted= -1;
	cs_data->max_slice=global_max_slice;
	t->tcs_data=cs_data;
	return 0;
}

static void task_free_multirr(task_t* t)
{
	if (t->tcs_data) {
		free(t->tcs_data);
		t->tcs_data=NULL;
	}
}

static task_t* pick_next_task_multirr(runqueue_t* rq)
{
	task_t* t=head_slist(&rq->tasks);

	/* Current is not on the rq -> let's remove it */
	if (t) 
		remove_slist(&rq->tasks,t);

	return t;
}

static void enqueue_task_multirr(task_t* t,runqueue_t* rq, int preempted)
{
	struct multirr_data* cs_data=(struct multirr_data*) t->tcs_data;

	if (t->on_rq || is_idle_task(t))
		return;

	if (cs_data->quantum_exhausted == 1){
		if(cs_data->current_slice > 1){
			cs_data->current_slice = cs_data->current_slice - 1;
		}
	}
	else if (cs_data->quantum_exhausted == 0){
		if(cs_data->current_slice < cs_data->max_slice){
			cs_data->current_slice = cs_data->current_slice + 1;
		}
	}

	insert_slist(&rq->tasks,t); //Push task
	cs_data->remaining_ticks_slice= cs_data->current_slice; // Reset slice
}

static void task_tick_multirr(runqueue_t* rq)
{
	task_t* current=rq->cur_task;
	struct multirr_data* cs_data=(struct multirr_data*) current->tcs_data;

	if (is_idle_task(current))
		return;

	cs_data->remaining_ticks_slice--; /* Charge tick */
	
	if (cs_data->remaining_ticks_slice<=0){
		rq->need_resched=TRUE; //Force a resched !!

		if(current->runnable_ticks_left > 1){
			cs_data->quantum_exhausted = 1;
		}
		else{
			cs_data->quantum_exhausted = 0;		
		}
	}
	else{
		cs_data->quantum_exhausted = 0;
	}
}

static task_t* steal_task_multirr(runqueue_t* rq)
{
	task_t* t=tail_slist(&rq->tasks);

	if (t) 
		remove_slist(&rq->tasks,t);

	return t;
}

sched_class_t multirr_sched= {
	.task_new=task_new_multirr,
	.task_free=task_free_multirr,
	.pick_next_task=pick_next_task_multirr,
	.enqueue_task=enqueue_task_multirr,
	.task_tick=task_tick_multirr,
	.steal_task=steal_task_multirr
};
