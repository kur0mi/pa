#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool()
{
	int i;
	for (i = 0; i < NR_WP; i++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp(char *str, int value)
{
	if (free_ != NULL) {
		WP *t = free_;
		free_ = free_->next;
		t->next = NULL;

		if (strlen(str) > 31)
			panic("expression too long");

		strcpy(t->str, str);
		t->value = value;

		if (head == NULL) {
			head = t;
			t->next = NULL;
		} else {
			t->next = head;
			head = t;
		}

		return t;
	} else {
		assert(0);
	}
}

void free_wp(int id)
{
	WP *t = head;
	WP *r = NULL;
	if (t->NO == id) {
		r = t;
		head = t->next;
	} else {
		while (t->next != NULL) {
			if (t->next->NO == id) {
				r = t->next;
				t->next = t->next->next;
				break;
			}
			t = t->next;
		}
	}

	if (r == NULL)
		panic("couldnt found wp %d", id);

	r->value = 0;
	if (free_ == NULL) {
		free_ = r;
		r->next = NULL;
	} else {
		r->next = free_;
		free_ = r;
	}
}

void show_wp()
{
	WP *t = head;
	while (t != NULL) {
		printf("wp [%d], %s: %d\n", t->NO, t->str, t->value);
		t = t->next;
	}
}

void show_free()
{
	WP *t = free_;
	while (t != NULL) {
		printf("wp [%d], %s: %d\n", t->NO, t->str, t->value);
		t = t->next;
	}
}

WP *check_wp(WP * w)
{
	if (w == NULL) {
		w = head;
	}

	while (w != NULL) {
		int res = expr(w->str);
		if (res != w->value) {
			w->value = res;
			return w;
		}
		w = w->next;
	}

	return NULL;
}
