#include "liblg.h"
#include "pthread.h"

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
	pthread_t		*threads;
	t_thread_info	*info;
	unsigned int	thread_amount;
	unsigned int	max_thread_amount;
}	t_thread_manager;

void	thread_manager_new(t_thread_manager *manager, unsigned int max_threads);
int		thread_manager_new_thread(t_thread_manager *manager, void *func, void *args);
void	thread_manager_check_threadiness(t_thread_manager *manager);