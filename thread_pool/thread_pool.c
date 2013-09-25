/*
 * =====================================================================================
 *
 *       Filename:  thread_pool.c
 *
 *    Description:  线程池的实现
 *
 *        Version:  1.0
 *        Created:  2013年09月25日 16时16分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Hurley (LiuHuan), liuhuan1992@gmail.com
 *        Company:  Class 1107 of Computer Science and Technology
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "thread_pool.h"

#define DEBUG

// 初始化线程池
thread_pool_t *thread_pool_init(int thread_num, int queue_max_num)
{
	thread_pool_t *thread_pool = NULL;

	do {
		thread_pool = (thread_pool_t *)malloc(sizeof(thread_pool_t));
		if (thread_pool == NULL) {
			break;
		}
		thread_pool->thread_num = thread_num;
		thread_pool->free_thread_num = thread_num;
		thread_pool->queue_max_num = queue_max_num;
		thread_pool->queue_curr_num = 0;
		thread_pool->head = thread_pool->tail = NULL;

		if (pthread_mutex_init(&(thread_pool->mutex), NULL)) {
			break;
		}
		if (pthread_cond_init(&(thread_pool->queue_empty), NULL)) {
			break;
		}
		if (pthread_cond_init(&(thread_pool->queue_not_empty), NULL)) {
			break;
		}
		if (pthread_cond_init(&(thread_pool->queue_not_full), NULL)) {
			break;
		}
		thread_pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
		if (thread_pool->threads == NULL) {
			break;
		}
		thread_pool->queue_close = 0;
		thread_pool->pool_close = 0;
		int i;
		for (i = 0; i < thread_pool->thread_num; i++) {
			if (pthread_create(&(thread_pool->threads[i]), NULL, 
							thread_function, (void *)thread_pool)) {
				free(thread_pool->threads);
				break;
			}
		}
		return thread_pool;
	} while (0);

	free(thread_pool);

	return NULL;
}

// 销毁线程池
int thread_pool_destroy(thread_pool_t *thread_pool)
{
	pthread_mutex_lock(&(thread_pool->mutex));
	// 线程池已经退出了，就直接退出
	if (thread_pool->queue_close || thread_pool->pool_close) {
		pthread_mutex_unlock(&(thread_pool->mutex));
		return -1;
	}

	// 置队列关闭标记
	thread_pool->queue_close = 1;
	
	// 等待队列为空
	while (thread_pool->queue_curr_num != 0) {
		pthread_cond_wait(&(thread_pool->queue_empty), &(thread_pool->mutex));
	}

	// 置线程池关闭标记
	thread_pool->pool_close = 1;
	pthread_mutex_unlock(&(thread_pool->mutex));

	// 唤醒线程池中正在阻塞的线程都退出
	pthread_cond_broadcast(&(thread_pool->queue_not_empty));

	// 唤醒阻塞的添加任务函数 threadpool_add_job 退出
	pthread_cond_broadcast(&(thread_pool->queue_not_full));

	int i;
	for (i = 0; i < thread_pool->thread_num; i++) {
		pthread_join(thread_pool->threads[i], NULL);
	}
	
	// 清理资源
	pthread_mutex_destroy(&(thread_pool->mutex));
	pthread_cond_destroy(&(thread_pool->queue_empty));
	pthread_cond_destroy(&(thread_pool->queue_not_empty));
	pthread_cond_destroy(&(thread_pool->queue_not_full));
	free(thread_pool->threads);

	job_t *free_job;
	while (thread_pool->head != NULL) {
		free_job = thread_pool->head;
		thread_pool->head = thread_pool->head->next;
		free(free_job);	
	}

	free(thread_pool);

	return 0;
}

// 调整线程池大小
//  return -1 参数错误
//  return -2 申请内存错误
//  return -3 线程创建错误
int thread_pool_resize(thread_pool_t *thread_pool, int thread_num, int queue_max_num)
{
	// 不允许减小线程池规模
	if (thread_num < thread_pool->thread_num || queue_max_num < thread_pool->queue_max_num) {
		return -1;
	}
	pthread_mutex_lock(&(thread_pool->mutex));
	thread_pool->queue_max_num = queue_max_num;
	
	thread_pool->threads = (pthread_t *)realloc(thread_pool->threads, sizeof(pthread_t) * thread_num);
	if (thread_pool->threads == NULL) {
		return -2;
	}
	int i;
	for (i = thread_pool->thread_num; i < thread_num; i++) {
		if (pthread_create(&(thread_pool->threads[i]), NULL, 
						thread_function, (void *)thread_pool)) {
			return -3;
		}
	}
	thread_pool->free_thread_num += (thread_num - thread_pool->thread_num);
	thread_pool->thread_num = thread_num;
	pthread_mutex_unlock(&(thread_pool->mutex));

	return 0;
}

// 向线程池添加任务
// @ is_block 表示是否阻塞式添加
// 	0 表示不阻塞，缓冲区满立即返回 1
// 	1 表示阻塞添加， 缓冲区满就等待，直到添加成功返回
//
//  return  0 表示正常
//  return  1 表示队列已满或所有线程忙碌(仅当非阻塞模式)
//  return -1 表示不可抗拒错误
int thread_pool_add_job(thread_pool_t *thread_pool, void *(job_callback)(void *), void *arg, int is_block)
{
	pthread_mutex_lock(&(thread_pool->mutex));
	// 队列满的时候
	while ((thread_pool->queue_curr_num == thread_pool->queue_max_num) && 
		!(thread_pool->queue_close || thread_pool->pool_close)) {
		if (is_block == 1) {
			pthread_cond_wait(&(thread_pool->queue_not_full), &(thread_pool->mutex));
		} else {
			pthread_mutex_unlock(&(thread_pool->mutex));
			return 1;
		}
	}

	// 所有线程都在忙的时候，不再添加任务
	if (thread_pool->free_thread_num == 0 && is_block == 0) {
		pthread_mutex_unlock(&(thread_pool->mutex));
		return 1;
	}

	// 队列关闭或者线程池关闭则退出
	if (thread_pool->queue_close || thread_pool->pool_close) {
		pthread_mutex_unlock(&(thread_pool->mutex));
		return -1;
	}

	job_t *new_job = (job_t *)malloc(sizeof(job_t));
	if (new_job == NULL) {
		pthread_mutex_unlock(&(thread_pool->mutex));
		return -1;
	}
	new_job->job_callback = job_callback;
	new_job->arg = arg;
	new_job->next = NULL;
	
	thread_pool->queue_curr_num++;
	if (thread_pool->head == NULL) {
		thread_pool->head = thread_pool->tail = new_job;
	} else {
		thread_pool->tail->next = new_job;
		thread_pool->tail = new_job;
	}

	pthread_cond_signal(&(thread_pool->queue_not_empty));

	pthread_mutex_unlock(&(thread_pool->mutex));
	
	return 0;
}

// 线程的处理函数
void *thread_function(void *arg)
{
	thread_pool_t *thread_pool = (thread_pool_t *)arg;
	job_t *curr_job = NULL;

	while (1) {
		pthread_mutex_lock(&(thread_pool->mutex));
		while ((thread_pool->queue_curr_num == 0) && !thread_pool->pool_close) {

			#ifdef DEBUG
				printf("Thread %lu will be sleep and wait!\n", pthread_self());
			#endif

			// 等待队列不再是空的消息
			pthread_cond_wait(&(thread_pool->queue_not_empty), &(thread_pool->mutex));
		}
		// 线程池关闭的时候，线程退出
		if (thread_pool->pool_close) {
			pthread_mutex_unlock(&(thread_pool->mutex));
			
			#ifdef DEBUG
				printf("Thread %lu exit!\n", pthread_self());
			#endif

			pthread_exit(NULL);
		}

		// 拿取一个任务
		thread_pool->queue_curr_num--;
		curr_job = thread_pool->head;
		if (thread_pool->queue_curr_num == 0) {
			thread_pool->head = thread_pool->tail = NULL;
			// 发出队列为空的信号(销毁函数需要)
			pthread_cond_signal(&(thread_pool->queue_empty));
		} else {
			thread_pool->head = thread_pool->head->next;
		}
		
		// 刚刚从队满转换到非空，通知所有可能阻塞的任务添加函数工作
		if (thread_pool->queue_curr_num == thread_pool->queue_max_num - 1) {
			// 考虑到可能有多个生产者阻塞在这里，所以通知全体
			pthread_cond_broadcast(&(thread_pool->queue_not_full));
		}
		thread_pool->free_thread_num--;
		pthread_mutex_unlock(&(thread_pool->mutex));

		#ifdef DEBUG
			printf("Thread %lu get a job!\n", pthread_self());
		#endif

		// 执行任务里面的函数，此处不在临界区域
		curr_job->job_callback(curr_job->arg);
		free(curr_job);
		curr_job = NULL;

		pthread_mutex_lock(&(thread_pool->mutex));
		thread_pool->free_thread_num++;
		pthread_mutex_unlock(&(thread_pool->mutex));
	}
}

// 线程池任务执行代码，打印下执行者和任务编号再休眠一秒退出
void *work(void *arg)
{
	char *p = (char *)arg;
	printf("thread_pool callback fuction : %s. Run Thread ID : %lu.\n", p, pthread_self());
	sleep(1);
}

// 测试函数，这里只模拟 一个生产者和一群消费者（支持多对多，只要多个线程添加即可）
// 可以看到，30个任务，每个休眠一秒，执行时间远小于串行的至少30s执行时间
int main(int argc, int argv)
{
	char *args[] = {
		"1",  "2",  "3",  "4",  "5",
		"6",  "7",  "8",  "9",  "10",
		"11", "12", "13", "14", "15",
		"16", "17", "18", "19", "20",
		"21", "22", "23", "24", "25",
		"26", "27", "28", "29", "30"};

	// 刻意让线程池大小初始化为 1 个线程，2个任务，然后测试扩增
	thread_pool_t *thread_pool = thread_pool_init(1, 2);

	int i = 0, tag, value;
	for (i = 0; i < 30; i++) {
		do {
			// 最后一个参数为 1 则是阻塞的添加，当缓冲区满之后等待直到可添加，这样做函数可能阻塞在这里
			// 最后一个参数为 0 则是非阻塞添加，当缓冲区满了之后不等待立即返回处理，当前这里做法是扩大缓冲大小
			tag = thread_pool_add_job(thread_pool, work, args[i], 0);
			if (tag == 1) {
				// 无可用线程则动态扩增缓冲区大小为之前的 2 倍
				value = thread_pool_resize(thread_pool, 
						thread_pool->thread_num * 2, thread_pool->queue_max_num * 2);
				if (value == -1) {
					printf("参数错误!\n");
					exit(-1);
				} else if (value == -2) {
					printf("申请内存错误!\n");
					exit(-1);
				} else if (value == -3) {
					printf("线程创建错误!\n");
					exit(-1);
				}
			}
		}while (tag != 0);
	}

	// 销毁线程池
	thread_pool_destroy(thread_pool);

	return 0;
}
