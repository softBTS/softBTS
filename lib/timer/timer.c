/*
 * (C) 2008,2009 by Holger Hans Peter Freyther <zecke@selfish.org>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <assert.h>
#include <string.h>
#include "../timer/timer.h"

static LLIST_HEAD(timer_list);
static struct timeval s_nearest_time;
static struct timeval s_select_time;

#define MICRO_SECONDS  1000000LL

#define TIME_SMALLER(left, right) \
        (left.tv_sec*MICRO_SECONDS+left.tv_usec) <= (right.tv_sec*MICRO_SECONDS+right.tv_usec)

void add_timer(struct timer_list *timer)
{
	struct timer_list *list_timer;

	/* TODO: Optimize and remember the closest item... */
	timer->active = 1;

	/* this might be called from within update_timers */
	llist_for_each_entry(list_timer, &timer_list, entry)
		if (timer == list_timer)
			return;

	timer->in_list = 1;
	llist_add(&timer->entry, &timer_list);
}

void schedule_timer(struct timer_list *timer, int seconds, int microseconds)
{
	struct timeval current_time;

	gettimeofday(&current_time, NULL);
	unsigned long long currentTime = current_time.tv_sec * MICRO_SECONDS + current_time.tv_usec;
	currentTime += seconds * MICRO_SECONDS + microseconds;
	timer->timeout.tv_sec = currentTime / MICRO_SECONDS;
	timer->timeout.tv_usec = currentTime % MICRO_SECONDS;
	add_timer(timer);
}

void del_timer(struct timer_list *timer)
{
	if (timer->in_list) {
		timer->active = 0;
		timer->in_list = 0;
		llist_del(&timer->entry);
	}
}

int timer_pending(struct timer_list *timer)
{
	return timer->active;
}

/*
 * if we have a nearest time return the delta between the current
 * time and the time of the nearest timer.
 * If the nearest timer timed out return NULL and then we will
 * dispatch everything after the select
 */
struct timeval *nearest_timer()
{
	struct timeval current_time;

	if (s_nearest_time.tv_sec == 0 && s_nearest_time.tv_usec == 0)
		return NULL;

	if (gettimeofday(&current_time, NULL) == -1)
		return NULL;

	unsigned long long nearestTime = s_nearest_time.tv_sec * MICRO_SECONDS + s_nearest_time.tv_usec;
	unsigned long long currentTime = current_time.tv_sec * MICRO_SECONDS + current_time.tv_usec;

	if (nearestTime < currentTime) {
		s_select_time.tv_sec = 0;
		s_select_time.tv_usec = 0;
	} else {
		s_select_time.tv_sec = (nearestTime - currentTime) / MICRO_SECONDS;
		s_select_time.tv_usec = (nearestTime - currentTime) % MICRO_SECONDS;
	}

	return &s_select_time;
}

/*
 * Find the nearest time and update s_nearest_time
 */
void prepare_timers()
{
	struct timer_list *timer, *nearest_timer = NULL;
	llist_for_each_entry(timer, &timer_list, entry) {
		if (!nearest_timer || TIME_SMALLER(timer->timeout, nearest_timer->timeout)) {
			nearest_timer = timer;
		}
	}

	if (nearest_timer) {
		s_nearest_time = nearest_timer->timeout;
	} else {
		memset(&s_nearest_time, 0, sizeof(struct timeval));
	}
}

/*
 * fire all timers... and remove them
 */
int update_timers()
{
	struct timeval current_time;
	struct timer_list *timer, *tmp;
	int work = 0;

	gettimeofday(&current_time, NULL);

	/*
	 * The callbacks might mess with our list and in this case
	 * even llist_for_each_entry_safe is not safe to use. To allow
	 * del_timer, add_timer, schedule_timer to be called from within
	 * the callback we jump through some loops.
	 *
	 * First we set the handled flag of each active timer to zero,
	 * then we iterate over the list and execute the callbacks. As the
	 * list might have been changed (specially the next) from within
	 * the callback we have to start over again. Once every callback
	 * is dispatched we will remove the non-active from the list.
	 *
	 * TODO: If this is a performance issue we can poison a global
	 * variable in add_timer and del_timer and only then restart.
	 */
	llist_for_each_entry(timer, &timer_list, entry) {
		timer->handled = 0;
	}

restart:
	llist_for_each_entry(timer, &timer_list, entry) {
		if (!timer->handled && TIME_SMALLER(timer->timeout, current_time)) {
			timer->handled = 1;
			timer->active = 0;
			(*timer->cb)(timer->data);
			work = 1;
			goto restart;
		}
	}

	llist_for_each_entry_safe(timer, tmp, &timer_list, entry) {
		timer->handled = 0;
		if (!timer->active) {
			del_timer(timer);
		}
	}

	return work;
}

int timer_check(void)
{
	struct timer_list *timer;
	int i = 0;

	llist_for_each_entry(timer, &timer_list, entry) {
		i++;
	}
	return i;
}
