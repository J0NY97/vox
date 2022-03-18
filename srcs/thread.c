#include "thread.h"

void	thread_manager_new(t_thread_manager *manager)
{
	manager->thread_amount = 0;

	for (int i = 0; i < MAX_THREADS; i++)
	{
		manager->info[i].id = i;
		manager->info[i].started = 0;
		manager->info[i].finished = 0;
	}
	LG_INFO("New thread manager created.\n");
}

void	*thread_func(void *info_pointer)
{
	t_thread_info	*info;

	info = info_pointer;
	info->started = 1;
	info->func(info->args);
	info->finished = 1;
	LG_INFO("Thread finished. (ID : %d)\n", info->id);
	return (NULL);
}

/*
 * Returns 0 if couldnt add thread;
*/
int	thread_manager_new_thread(t_thread_manager *manager, void *func, void *args)
{
	if (manager->thread_amount > MAX_THREADS)
		return (0);
	manager->info[manager->thread_amount].func = func;
	manager->info[manager->thread_amount].args = args;
	if (pthread_create(&manager->threads[manager->thread_amount],
		NULL, thread_func, &manager->info[manager->thread_amount]))
		LG_ERROR("Couldnt create thread.");
	manager->info[manager->thread_amount].started = 1;
	manager->thread_amount++;
	return (1);
}

/*
 * Aka thread readiness;
*/
void	thread_manager_check_threadiness(t_thread_manager *manager)
{
	for (int i = 0; i < manager->thread_amount; i++)
	{
		if (manager->info[i].started &&
			manager->info[i].finished)
		{
			pthread_join(manager->threads[i], NULL);
			manager->info[i].started = 0;
			manager->info[i].finished = 0;
			manager->thread_amount--;
		}
	}
}