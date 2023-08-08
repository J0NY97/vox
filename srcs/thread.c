#include "thread.h"
#include "time.h"

void	thread_manager_new(t_thread_manager *manager, unsigned int max_threads)
{
	manager->max_thread_amount = max_threads;
	manager->thread_amount = 0;
	manager->threads = malloc(sizeof(pthread_t) * manager->max_thread_amount);
	manager->info = malloc(sizeof(t_thread_info) * manager->max_thread_amount);
	pthread_mutex_init(&manager->mutex, NULL);
	for (unsigned int i = 0; i < manager->max_thread_amount; i++)
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
//	LG_INFO("Thread finished. (ID : %d)", info->id);
	return (NULL);
}

/*
 * Returns 0 if couldnt add thread; else 1;
*/
int	thread_manager_new_thread(t_thread_manager *manager, void *func, void *args, void *(*startFunc)(void*), void *(*endFunc)(void*))
{
	if (manager->thread_amount >= manager->max_thread_amount)
		return (0);
	for (unsigned int i = 0; i < manager->max_thread_amount; i++)
	{
		if (manager->info[i].started == 0)
		{
			manager->info[i].startFunc = startFunc;
			manager->info[i].endFunc = endFunc;
			manager->info[i].func = func;
			manager->info[i].args = args;
			manager->info[i].finished = 0;
			manager->info[i].started = 1;
			if (startFunc)
				startFunc(args);
			if (pthread_create(&manager->threads[i], NULL, thread_func, &manager->info[i]))
				LG_ERROR("Couldnt create thread. (ID : %d)", manager->info[i].id);
			//LG_INFO("New Thread Created (ID : %d (%d))", manager->info[i].id, manager->thread_amount);
			manager->thread_amount++;
			return (1);
		}
	}
	return (0);
}

int	create_new_detached_thread(void *func, void *args)
{
	pthread_t thread;

	if (pthread_create(&thread, NULL, func, args))
		LG_ERROR("Couldnt create thread.");
	pthread_detach(thread);
	return (1);
}

/*
 * Aka thread readiness;
 *
 * This should be called before anything in the args are being used;
*/
void	tm_check_threadiness(t_thread_manager *manager)
{
//	time_t _startTime = clock();
	int _maxThreadsReadyPerFrame = 64;
	int _threadsReady = 0;
	for (unsigned int i = 0; i < manager->max_thread_amount; i++)
	{
		if (manager->info[i].started &&
			manager->info[i].finished)
		{
			if (pthread_join(manager->threads[i], NULL))
				LG_ERROR("Couldnt join thread. (ID : %d)", manager->info[i].id);
			if (manager->info[i].endFunc)
				manager->info[i].endFunc(manager->info[i].args);
			manager->info[i].started = 0;
			manager->info[i].finished = 0;
			manager->thread_amount--;
			_threadsReady++;
		}
		if (_threadsReady >= _maxThreadsReadyPerFrame)
			break;
	}
	/* Takes avg. 0.001 with 20 threads
	if (_threadsReady > 0)
	{
		LG_INFO("%d Threads were ready this frame", _threadsReady);
		LG_INFO("Time taken : %f", (float)(clock() - _startTime) / CLOCKS_PER_SEC);
	}
	*/
}