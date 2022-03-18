#include "liblg.h"
#include "pthread.h"

#define MAX_THREADS 4

typedef struct s_thread_info
{
	unsigned int	id;
	unsigned int	started;
	unsigned int	finished;
	void			(*func)(void *);
	void			*args;
}	t_thread_info;

typedef struct s_thread_manager
{
	pthread_t		threads[MAX_THREADS];
	t_thread_info	info[MAX_THREADS];
	unsigned int	thread_amount;
}	t_thread_manager;

void	thread_manager_new(t_thread_manager *manager);
int		thread_manager_new_thread(t_thread_manager *manager, void *func, void *args);
void	thread_manager_check_threadiness(t_thread_manager *manager);