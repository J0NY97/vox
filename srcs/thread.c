#include "thread.h"

void	thread_manager_new(t_thread_manager *manager, unsigned int max_threads)
{
	manager->max_thread_amount = max_threads;
	manager->thread_amount = 0;
	manager->threads = malloc(sizeof(pthread_t) * manager->max_thread_amount);
	manager->info = malloc(sizeof(t_thread_info) * manager->max_thread_amount);
	for (int i = 0; i < manager->max_thread_amount; i++)
	{
		manager->info[i].id = i;
		manager->info[i].started = 0;
		manager->info[i].finished = 0;
	}
	LG_INFO("New thread manager created.");
}

void	*thread_func(void *info_pointer)
{
	t_thread_info	*info;

	info = info_pointer;
	info->started = 1;
	info->func(info->args);
	info->finished = 1;
	LG_INFO("Thread finished. (ID : %d)", info->id);
	return (NULL);
}

/*
 * Returns 0 if couldnt add thread; else 1;
*/
int	thread_manager_new_thread(t_thread_manager *manager, void *func, void *args)
{
	if (manager->thread_amount >= manager->max_thread_amount)
		return (0);
	for (int i = 0; i < manager->max_thread_amount; i++)
	{
		if (manager->info[i].started == 0)
		{
			manager->info[i].func = func;
			manager->info[i].args = args;
			if (pthread_create(&manager->threads[i],
				NULL, thread_func, &manager->info[i]))
				LG_ERROR("Couldnt create thread. (ID : %d)", manager->info[i].id);
			LG_INFO("New Thread Created (ID : %d)", manager->info[i].id);
			manager->info[i].started = 1;
			manager->info[i].finished = 0;
			manager->thread_amount++;
			return (1);
		}
	}
	return (0);
}

/*
 * Aka thread readiness;
 *
 * This should be called before anything in the args are being used;
*/
void	thread_manager_check_threadiness(t_thread_manager *manager)
{
	for (int i = 0; i < manager->max_thread_amount; i++)
	{
		if (manager->info[i].started &&
			manager->info[i].finished)
		{
			if (pthread_join(manager->threads[i], NULL))
				LG_ERROR("Couldnt join thread. (ID : %d)", manager->info[i].id);
			manager->info[i].started = 0;
			manager->info[i].finished = 0;
			manager->thread_amount--;
			LG_INFO("Thread Joined. (ID : %d)", manager->info[i].id);
			LG_INFO("Threads Left : %d", manager->thread_amount);
			return ; // maximum one per frame
		}
	}
}