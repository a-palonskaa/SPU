#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

typedef struct {
	void* data;
	size_t size;
	size_t capacity;
} vector_t;

typedef enum {
	OK                      = 0,
	MEMORY_ALLOCATION_ERROR = 1,
	EMPTY_VECTOR            = 2,
} vec_err_t;


//============================================================================================

#define vector_insert(vec, pos, val_addr)                                         \
    do {                                                                          \
		vector_insert_((vector_t*)(vec_addr), (pos), (val), sizeof(*(val_addr))); \
    } while (0)

#define vector_push_back(vec_addr, src_addr)                                  \
	do {                                                                      \
	    vector_push_back_((vector_t*)(vec_addr), src_addr, sizeof(*src_addr)); \
    } while (0)

#define vector_pop_back(vec_addr, dst_addr)                                  \
	do {														             \
		vector_pop_back_((vector_t*)(vec_addr), dst_addr, sizeof(*dst_addr)); \
	} while(0)

//============================================================================================

#define vector_capacity(vec)                            \
	(vec) ? ((vector_t*)(vec))->capacity : (ssize_t) -1

#define vector_is_empty(vec)                           \
	(vec) ? !(((vector_t*)(vec))->size) : (ssize_t) -1

#define vector_tail_ptr(vec)                   \
    (vec) ? ((vector_t*)(vec))->data : nullptr

#define vector_head_ptr(vec)                                                                     \
	(vec_addr) ? ((void*)((char*)(vec) + ((vector_t*)(vec))->size)) : nullptr

#define vector_at(vec, dst, index)                                   \
	do {										                     \
		vector_at_((vector_t*) (vec), (dst), sizeof(*dst), (index)); \
	} while (0)

#define vector_front(vec, dst)                                 \
	do {							                           \
		vector_at_((vector_t*) (vec), (dst), sizeof(*dst), 0); \
	} while (0)

#define vector_back(vec)                                                                   \
	do {							                                                       \
		vector_at_((vector_t*) (vec), (dst), sizeof(*dst), ((vector_t*) (vec))->size - 1); \
	} while (0)

//============================================================================================

#define vector_reserve(vec, n)                                 \
    do {                                                       \                                            \
        vec_err_t err = vector_reserve_((vector_t*) (vec), n); \
		(void) err;                                            \
    } while (0)

#define vector_shrink_to_fit(vec)                  \
    do {                                           \
		vector_shrink_to_fit_((vector_t*) (vec)) ; \                                          			      \
    } while (0)

#define vector_clear(vec)                 \
    do {                                  \
		vector_clear_((vector_t*) (vec)); \
    } while (0)

//============================================================================================

vector_t* new_vector();
vec_err_t vector_ctor(vector_t* vector);
void vector_delete(vector_t* vector);
void vector_dtor(vector_t* vector);

//============================================================================================

bool vector_has_space_(vector_t* vec, size_t len);
vec_err_t memory_add_to_fit_(vector_t* vec, size_t len);
vec_err_t memory_add_(vector_t* vec, size_t new_capacity);
vec_err_t vector_reserve_(vector_t* vector, size_t new_capacity) ;
vec_err_t vector_shrink_to_fit_(vector_t* vector);

//============================================================================================
vec_err_t vector_push_back_(vector_t* vec, void* src, size_t elm_width);
vec_err_t vector_pop_back_(vector_t* vec, void* dst, size_t elm_width);

void vector_at_(vector_t* vector, void* dst, size_t dst_width, size_t index);

void vector_insert_(vector_t* vec, size_t pos, void* elm, size_t elm_width);
void vector_erase_elm_(vector_t* vec, size_t pos, size_t elm_width);
void vector_erase_range_(vector_t* vec, size_t first, size_t last, size_t elm_width);
void vector_clear_(vector_t* vector);

void* vector_element_ptr(vector_t* vec, size_t n, size_t elm_size);
size_t vector_size(vector_t* vec);

#endif /* VECTOR_H */
