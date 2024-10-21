#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "vector.h"
#include "logger.h"

//============================================================================================

vector_t* new_vector() {
    vector_t* vector = (vector_t*) calloc(sizeof(vector_t), sizeof(char));
	if (vector == nullptr) {
		LOG(ERROR, "Memory allocation error\n" STRERROR(errno));
		return nullptr;
	}

	if (vector_ctor(vector) == OK) {
		return vector;
	}

	free(vector);
	vector = nullptr;
    return nullptr;
}

vec_err_t vector_ctor(vector_t* vector) {
	assert(vector != nullptr);

	vector->data = calloc(1, sizeof(char));
	if (vector->data == nullptr) {
		LOG(ERROR, "Memory allocation error\n" STRERROR(errno));
		return MEMORY_ALLOCATION_ERROR;
	}

	vector->capacity = 1;
	vector->size = 0;
	return OK;
}

void vector_delete(vector_t* vector) {
	assert(vector != nullptr);

	vector_dtor(vector);

	free(vector);
	vector = nullptr;
}

void vector_dtor(vector_t* vector) {
	assert(vector != nullptr);

	free(vector->data);
	vector->data = nullptr;

	vector->size = 0;
	vector->capacity = 0;
}
//============================================================================================

vec_err_t vector_push_back_(vector_t* vector, void* src, size_t elm_width) {
	assert(vector != nullptr);
	assert(src != nullptr);
	assert(elm_width != 0);

	vec_err_t memory_add_status = memory_add_to_fit_(vector, elm_width);
	if (memory_add_status != OK) {
		return memory_add_status;
	}

	memcpy((char*) vector->data + vector->size, src, elm_width);
	vector->size += elm_width;
	return OK;
}

vec_err_t vector_pop_back_(vector_t* vec, void* dst, size_t elm_width) {
	assert(vec != nullptr);
	assert(dst != nullptr);
	assert(elm_width != 0);

	if (vec->size < elm_width) {
		return EMPTY_VECTOR;
	}

	memcpy(dst, (char*) vec->data + vec->size - elm_width, elm_width);
	memset((char*) vec->data + vec->size - elm_width, 0, elm_width);
	vec->size -= elm_width;
	return OK;
}

//============================================================================================

bool vector_has_space_(vector_t* vec, size_t len) {
	assert(vec != nullptr);

	return vec->capacity >= (vec->size + len);
}

vec_err_t memory_add_to_fit_(vector_t* vec, size_t len) {
	assert(vec != nullptr);

	if (vector_has_space_(vec, len)) {
		return OK;
	}

	do {
		vec->capacity *= 2;
	} while (!vector_has_space_(vec, len));

	void* new_data = realloc(vec->data, vec->capacity);
	if (new_data == nullptr) {
		LOG(ERROR, "Memory allocation error\n" STRERROR(errno));
		return MEMORY_ALLOCATION_ERROR;
	}

	vec->data = new_data;
	return OK;
}

vec_err_t memory_add_(vector_t* vec, size_t new_capacity) {
	assert(vec != nullptr);
	assert(new_capacity != 0);

	void* new_data = realloc(vec->data, new_capacity);
	if (new_data == nullptr) {
		LOG(ERROR, "Memory allocation error\n" STRERROR(errno));
		return MEMORY_ALLOCATION_ERROR;
	}

	vec->data = new_data;
	vec->capacity = new_capacity;
	return OK;
}

vec_err_t vector_reserve_(vector_t* vector, size_t new_capacity) {
	assert(vector != nullptr);

	if (vector->capacity >= new_capacity) {
		return OK;
	}

    return memory_add_(vector, new_capacity);
}

vec_err_t vector_shrink_to_fit_(vector_t* vector) {
	assert(vector != nullptr);

	return memory_add_(vector, vector->size);
}

//============================================================================================

void vector_insert_(vector_t* vec, size_t pos, void* elm, size_t elm_width) {
	assert(vec != nullptr);
	assert(elm != nullptr);
	assert(elm_width != 0);

	memcpy((char*) vec->data + (pos - 1) * elm_width, elm, vec->size - pos * elm_width);
}

void vector_erase_elm_(vector_t* vec, size_t pos, size_t elm_width) {
	assert(vec != nullptr);
	assert(elm_width != 0);

    memcpy((char*) vec->data + (pos - 1) * elm_width, vec->data, vec->size - pos * elm_width);
	vec->size -= elm_width;

	memset((char*) vec->data + vec->size, 0, elm_width);
}

void vector_erase_range_(vector_t* vec, size_t first, size_t last, size_t elm_width) {
	assert(vec != nullptr);
	assert(elm_width != 0);

	if (last == first) {
		return;
	}
	else if (last < first) {
		size_t new_last = first;
		first = last;
		last = new_last;
	}

	memcpy((char*) vec->data + (first - 1) * elm_width, (char*) vec->data + last * elm_width,
		    vec->size - last * elm_width);
	vec->size -= elm_width * (1 + last - first);

	memset((char*) vec->data + vec->size, 0, elm_width * (1 + last - first));
}

void vector_clear_(vector_t* vector) {
	assert(vector != nullptr);

	memset(vector->data, 0, vector->size);
	vector->size = 0;
}

void vector_at_(vector_t* vector, void* dst, size_t dst_width, size_t index) {
	assert(vector != nullptr);
	assert(dst != nullptr);
	assert(dst_width != 0);

	if (index > vector->size) {
		return;
	}

	memcpy(dst, vector->data, dst_width);
}

//============================================================================================

void* vector_element_ptr(vector_t* vec, size_t n, size_t elm_size) {
	assert(vec != nullptr); //LINK - if n > size

	return (char*) vec->data + (elm_size * n);
}

size_t vector_size(vector_t* vec) {
	assert(vec != nullptr);
	return vec->size;
}
