#ifndef THREAD_H
# define THREAD_H

# include "liblg.h"
# include "pthread.h"

typedef struct s_thread_info
{
	unsigned int	id;
	unsigned int	started;
	unsigned int	finished;
	void			(*startFunc)(void*); // Which function gets called just before thread is created;
	void			(*func)(void *);
	void			(*endFunc)(void*); // Which function gets called just after thread is joined;
	void			*args;
}	t_thread_info;

typedef struct s_thread_manager
{
	pthread_t		*threads;
	t_thread_info	*info;
	unsigned int	thread_amount;
	unsigned int	max_thread_amount;
	pthread_mutex_t	mutex;
}	t_thread_manager;

void	thread_manager_new(t_thread_manager *manager, unsigned int max_threads);
int		thread_manager_new_thread(t_thread_manager *manager, void *func, void *args, void (*startFunc)(void*), void *endFunc);
void	thread_manager_check_threadiness(t_thread_manager *manager);
int		create_new_detached_thread(void *func, void *args);

#endif